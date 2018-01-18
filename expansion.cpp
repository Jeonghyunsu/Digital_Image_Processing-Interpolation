#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define BASESIZE 256
#define BASESIZE_WIDTH 256
#define BASESIZE_HEIGHT 256//lena256.raw의 size

enum InterpolationType
{
	BILINEAR=1,
	BICUBIC=2
};

unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nInitVal);
void MemFree2D(unsigned char **Mem, int nHeight);
bool isInsideBoundary(int nHeight, int nWidth, double h, double w);

unsigned char BicubicInterpolation(unsigned char** In, int nHeight_Ori, int nWidth_Ori, double h_Cvt, double w_Cvt);
unsigned char BilinearInterpolation(unsigned char** In, int nHeight_Ori, int nWidth_Ori, double h_Cvt, double w_Cvt);

int main()
{
	int nInterpolationType;
	int expansionSize;
	double dScaleRate;
	
	FILE *Input, *Output;

	cout << "Input Interploation Type: (1): Bilinear (2): Bicubic : ";
	cin >> nInterpolationType ;

	cout << "Expansion size : 436 or 512 : ";
	cin >> expansionSize;
	dScaleRate = (double)expansionSize/BASESIZE;

	int nHeight_Scale = BASESIZE_HEIGHT * dScaleRate;
	int nWidth_Scale = BASESIZE_WIDTH * dScaleRate;

	unsigned char** ch_in_gray = MemAlloc2D(BASESIZE_HEIGHT, BASESIZE_WIDTH, 0);
	unsigned char** Out = MemAlloc2D(nHeight_Scale, nWidth_Scale, 0);

	fopen_s(&Input,"lena256.raw", "rb"); 
	if (Input == NULL)
		cout<<"Input File open failed";


	for (int h = 0; h < BASESIZE_HEIGHT; h++)
		fread(ch_in_gray[h], sizeof(unsigned char), BASESIZE_WIDTH, Input);
	fclose(Input); 

	for (int h = 0; h < nHeight_Scale; h++)
	{
		for (int w = 0; w < nWidth_Scale; w++)
		{
			double h_Ori = h / dScaleRate;
			double w_Ori = w / dScaleRate;
					
			if (isInsideBoundary(nHeight_Scale, nWidth_Scale, h_Ori, w_Ori))
			{
				if (nInterpolationType == BILINEAR) {
					Out[h][w] = BilinearInterpolation(ch_in_gray, BASESIZE_HEIGHT, BASESIZE_WIDTH, h_Ori, w_Ori);
				}
				else {// Bicubic interpolation
					Out[h][w] = BicubicInterpolation(ch_in_gray, BASESIZE_HEIGHT, BASESIZE_WIDTH, h_Ori, w_Ori);
				}

			}
		}
	}
	
	Output = NULL;

	switch (nInterpolationType)
	{
	case BILINEAR:
		switch (expansionSize)
		{
		case 436:fopen_s(&Output, "BILINAER436.raw", "wb"); break;
		case 512:fopen_s(&Output, "BILINEAR512.raw", "wb"); break;

		default:fopen_s(&Output, "BILINEAR_ANYTHING_ELSE1.raw", "wb"); break;
		}
		break;
	case BICUBIC:
		switch (expansionSize)
		{
		case 436 :fopen_s(&Output, "BICUBIC436.raw", "wb"); break;
		case 512 :fopen_s(&Output, "BICUBIC512.raw", "wb"); break;

		default:fopen_s(&Output, "BICUBIC_ANYTHING_ELSE2.raw", "wb"); break;
		}
		break;

	}

	if (Output == NULL) {
		cout << "Output File open failed";
		return 0;
	}
	
	for (int i = 0; i < expansionSize; i++)
		fwrite(Out[i], sizeof(unsigned char), expansionSize, Output); 
	fclose(Output);

	switch (nInterpolationType)
	{
	case BILINEAR:
		switch (expansionSize)
		{
		case 436:cout << "BILINEAR TYPE , SIZE 436 파일이 만들어졌습니다."; break;
		case 512:cout << "BILINEAR TYPE , SIZE 512 파일이 만들어졌습니다."; break;

		default: cout << "기타 사이즈로 만들어졌습니다."; break;
		}
		break;
	case BICUBIC:
		switch (expansionSize)
		{
		case 436:cout << "BICUBIC TYPE , SIZE 436 파일이 만들어졌습니다."; break;
		case 512:cout << "BICUBIC TYPE , SIZE 512 파일이 만들어졌습니다."; break;
		
		default: cout << "기타 사이즈로 만들어졌습니다."; break;
		}
		break;
	}
	return 0;
}

unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nInitVal)
{
	unsigned char** rtn = new unsigned char*[nHeight];
	for (int n = 0; n < nHeight; n++)
	{
		rtn[n] = new unsigned char[nWidth];
		memset(rtn[n], nInitVal, sizeof(unsigned char) * nWidth);
	}
	return rtn;
}

void MemFree2D(unsigned char **Mem, int nHeight)
{
	for (int n = 0; n < nHeight; n++)
	{
		delete[] Mem[n];
	}
	delete[] Mem;
}

bool isInsideBoundary(int nHeight, int nWidth, double h, double w)
{
	if (h >= 0 && w >= 0 && h < nHeight && w < nWidth)
	{
		return true;
	}
	else return false;
}

unsigned char BilinearInterpolation(unsigned char** In, int nHeight_Ori, int nWidth_Ori, double h_Cvt, double w_Cvt)
{
	unsigned char left_up, left_down, right_up, right_down, result_pixel;

	int h1, w1;
	h1 = (int)h_Cvt;//해당 (y,x)의 (0,0)의 기준이 되는 좌표
	w1 = (int)w_Cvt;//

	double h_length_up = h_Cvt - h1;
	double h_length_down = h1 + 1 - h_Cvt;
	double w_length_left = w_Cvt - w1;
	double w_length_right = w1 + 1 - w_Cvt;

	if (h1 == nHeight_Ori - 1 && w1 == nWidth_Ori - 1)
	{
		return 0;
	}
	else if (h1 == nHeight_Ori - 1 && w1 != nWidth_Ori - 1)
	{
		return 0;
	}
	else if (h1 != nHeight_Ori - 1 && w1 == nWidth_Ori - 1)
	{
		return 0;
	}
	else {

		left_up = In[h1][w1];
		left_down = In[h1 + 1][w1];
		right_up = In[h1][w1 + 1];
		right_down = In[h1 + 1][w1 + 1];

		result_pixel = (h_length_down*(w_length_right*left_up + w_length_left*right_up)) + (h_length_up*(w_length_right*left_down + w_length_left*right_down));
		return result_pixel;
	}

}
unsigned char BicubicInterpolation(unsigned char** In, int nHeight_Ori, int nWidth_Ori, double h_Cvt, double w_Cvt)
{
	int h1, w1;
	h1 = (int)h_Cvt;//해당 (y,x)의 (0,0)의 기준이 되는 좌표
	w1 = (int)w_Cvt;//

	if (h1 == 0) {
		return 0;
	}
	else if (w1 == 0) {
		return 0;
	}
	else if (w1 > nWidth_Ori - 1 - 3) {
		return 0;
	}
	else if (h1 > nHeight_Ori - 1 - 3) {
		return 0;
	}

	else {
		double inverse_matrix[4][4] = { { -0.166667, 0.5, -0.5, 0.166667 },
		{ 0.5, -1, 0.5,0 },
		{ -0.333333, -0.5, 1, -0.166667 },
		{ 0,	1,	0,	0 }
		};
		//In[h1][w1] = f(0,0)
		double first_cal[4];
		double mid_cal[4];
		double last_cal[4];
		int result;

		double n_w = w_Cvt - w1;
		double n_h = h_Cvt - h1;

		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				first_cal[i] = inverse_matrix[i][0] * In[(h1 - 1) + j][w1 - 1] + inverse_matrix[i][1] * In[(h1 - 1) + j][w1] +
					inverse_matrix[i][2] * In[(h1 - 1) + j][w1 + 1] + inverse_matrix[i][3] * In[(h1 - 1) + j][w1 + 2];
				//역행렬 * f(-1)~f(3)의 연산
			}
			mid_cal[j] = first_cal[0]*(n_w*n_w*n_w) + first_cal[1]*(n_w*n_w) + first_cal[2]*(n_w) + first_cal[3];
			//an^3 + bn^2 + cn + d
		}

		for (int i = 0; i < 4; i++)
		{
			last_cal[i] = inverse_matrix[i][0] * mid_cal[0] + inverse_matrix[i][1] * mid_cal[1] +
				inverse_matrix[i][2] * mid_cal[2] + inverse_matrix[i][3] * mid_cal[3];
			//역행렬 * f(-1)~f(3)의 연산
		}
		result = last_cal[0]*(n_h*n_h*n_h) + last_cal[1]*(n_h*n_h) + last_cal[2]*(n_h)+last_cal[3];
		//an^3 + bn^2 + cn + d

		return (unsigned char)result;
	}

}
