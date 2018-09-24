#include <iostream>
#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include<time.h>
#define pi 3.141592653
using namespace std;
#define _CRT_SECURE_NO_WARNINGS
/**
 * Image
 **/
Image::Image (int width_, int height_){

    assert(width_ > 0);
    assert(height_ > 0);

    width           = width_;
    height          = height_;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
	int b = 0; //which byte to write to
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
			data.raw[b++] = 0;
		}
	}

    assert(data.raw != NULL);
}

Image::Image (const Image& src){
	
	width           = src.width;
    height          = src.height;
    num_pixels      = width * height;
    sampling_method = IMAGE_SAMPLING_POINT;
    
    data.raw = new uint8_t[num_pixels*4];
    
    //memcpy(data.raw, src.data.raw, num_pixels);
    *data.raw = *src.data.raw;
}

Image::Image (char* fname){

	int numComponents; //(e.g., Y, YA, RGB, or RGBA)
	data.raw = stbi_load(fname, &width, &height, &numComponents, 4);
	
	if (data.raw == NULL){
		printf("Error loading image: %s", fname);
		exit(-1);
	}
	

	num_pixels = width * height;
	sampling_method = IMAGE_SAMPLING_POINT;
	
}

Image::~Image (){
    delete data.raw;
    data.raw = NULL;
}

void Image::Write(char* fname){
	
	int lastc = strlen(fname);

	switch (fname[lastc-1]){
	   case 'g': //jpeg (or jpg) or png
	     if (fname[lastc-2] == 'p' || fname[lastc-2] == 'e') //jpeg or jpg
	        stbi_write_jpg(fname, width, height, 4, data.raw, 95);  //95% jpeg quality
	     else //png
	        stbi_write_png(fname, width, height, 4, data.raw, width*4);
	     break;
	   case 'a': //tga (targa)
	     stbi_write_tga(fname, width, height, 4, data.raw);
	     break;
	   case 'p': //bmp
	   default:
	     stbi_write_bmp(fname, width, height, 4, data.raw);
	}
}

void Image::AddNoise (double factor)
{
	/* WORK HERE */
	srand((unsigned)time(NULL));
	//sigma represents variance of Gaussian distribution; mu prepresents the mean value
	float sigma = factor; 
	int x, y;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			float n = sqrt(-2.0 * log(rand()*1.0/ (RAND_MAX + 1.0)))
				*cos(2 * pi*rand()*1.0 / (RAND_MAX + 1.0));
			Pixel p = GetPixel(x, y);
			//Pixel noise(r*sigma + mu,g*sigma + mu,b*sigma + mu,p.a);
			Pixel noised_p(ComponentClamp(n*factor+p.r), ComponentClamp(n*factor+p.g),
				ComponentClamp(n*factor+p.b),p.a);
			GetPixel(x, y) = noised_p;
		}
	}
}

void Image::Brighten (double factor)
{
	int x,y;
	for (x = 0 ; x < Width() ; x++)
	{
		for (y = 0 ; y < Height() ; y++)
		{
			Pixel p = GetPixel(x, y); 
			Pixel scaled_p = p*factor;
			GetPixel(x,y) = scaled_p;
		}
	}
}


void Image::ChangeContrast (double factor)
{
	/* WORK HERE */
	int x, y;
	float Luminance = 0.0;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			Luminance += 0.30*p.r + 0.59*p.g + 0.11*p.b;
		}
	}
	Luminance =(int)(Luminance / (Width()*Height()));
	//int minr=INT_MAX, maxr=INT_MIN;
	//int ming = INT_MAX, maxg = INT_MIN;
	//int minb = INT_MAX, maxb = INT_MIN;
	//for (x = 0; x < Width(); x++)
	//{
	//	for (y = 0; y < Height(); y++)
	//	{
	//		Pixel p = GetPixel(x, y);
	//		int r = factor * (p.r - Luminance) ;//+ Luminance
	//		int g = factor * (p.g - Luminance) ;//+ Luminance
	//		int b = factor * (p.b - Luminance) ;//+ Luminance
	//		if (r > maxr)
	//			maxr = r;
	//		else if (r < minr)
	//			minr = r;
	//		if (g > maxg)
	//			maxg = g;
	//		else if (g < ming)
	//			ming = g;
	//		if (b > maxb)
	//			maxb = b;
	//		else if (b < minb)
	//			minb = b;
	//	}
	//}
	//int scoper = maxr - minr;
	//int scopeg = maxg - ming;
	//int scopeb = maxb - minb;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel contrast_p;
			float r = int(factor*(p.r - Luminance) + Luminance) ;//- float(minr)) / scoper * 255.0
			float g = int(factor*(p.g - Luminance) + Luminance);// - float(ming)) / scopeg * 255.0
			float b = int(factor*(p.b - Luminance) + Luminance) ;//- float(minb)) / scopeb * 255.0
			contrast_p.SetClamp((int)r, (int)g, (int)b, p.a);
			GetPixel(x, y) = contrast_p;
		}
	}
}


void Image::ChangeSaturation(double factor)
{
	/* WORK HERE */
	
	//RGB->HSI
	const int height = Height();
	const int width = Width();
	float **H = new float*[width];
	float **S = new float*[width];
	float **I = new float*[width];
	for (int i = 0; i < width; ++i)
	{
		H[i] = new float[height];
		S[i] = new float[height];
		I[i] = new float[height];

	}
	for (int x = 0; x < Width(); x++)
	{
		for (int y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			float r = p.r / 255.0;
			float g = p.g / 255.0;
			float b = p.b / 255.0;
			I[x][y] = (r + g + b) / 3.0;
			float R = r / (3 * I[x][y]);
			float G = g / (3 * I[x][y]);
			float B = b / (3 * I[x][y]);
			if ((r == g) && (g ==b))
			{
				S[x][y] = 0;
				H[x][y] = 0;
			}
			else
			{
				float w = 0.5*((2 * r - g - b) / sqrt((r - g)*(r - g) + (r - b)*(g - b)));
				if (w > 1)w = 1;
				if (w < -1)w = -1;
				H[x][y] = acos(w);
				if (H[x][y] < 0)
				{
					cerr << "H<0" << endl;
					system("pause>nul");
				}
				else if (b > g)
					H[x][y] = 2 * pi - H[x][y];
				if (r <= g&&r <= b)
					S[x][y] = 1 - 3*R;
				else if(g <= r&&g <= b)
					S[x][y] = 1 - 3*G;
				else
					S[x][y] = 1 - 3*B;
			}
			//change S			
			S[x][y] = S[x][y] * factor;
			//S[x][y] = 1 / (1 + exp(-S[x][y]));
		}
	}
	//HSI->RGB
	for (int x = 0; x < Width(); x++)
	{
		for (int y = 0; y < Height(); y++)
		{
			float s = S[x][y];
			float h = H[x][y];
			float i = I[x][y];
			float R, G, B;
			int r, g, b;
			if (s > 1)
				s = 1;
			if (i > 1)
				i = 1;
			if (s == 0)
				r = g = b = i;
			else
			{
				if ((h >= 0) && (h < 2 * pi / 3))
				{
					B = (1 - s) / 3;
					R = (1 + s * cos(h) / cos(pi / 3 - h)) / 3;
					G = 1 - R - B;
				}
				else if((h >= 2 * pi / 3)&&(h< 4 * pi / 3))
				{
					h = h - 2 * pi / 3;
					R = (1 - s) / 3;
					G=(1 + s * cos(h) / cos(pi / 3 - h)) / 3;
					B = 1 - R - G;
				}
				else
				{
					h = h - 4 * pi / 3;
					G = (1 - s) / 3;
					B = (1 + s * cos(h) / cos(pi / 3 - h)) / 3;
					R = 1 - B - G;
				}
			}
			if (R < 0)R = 0;
			if (G < 0)G = 0;
			if (B < 0)B = 0;
			r = 3 * i*R;
			g = 3 * i*G;
			b = 3 * i*B;
			if (r > 1)r = 1;
			if (g > 1)g = 1;
			if (b > 1)b = 1;
			Pixel p = GetPixel(x, y);
			Pixel saturated_p(r*255,g*255,b*255,p.a);
			GetPixel(x, y) = saturated_p;
		}
	}


	for (int i = 0; i < width; ++i)
	{
		delete[] H[i];
		delete[] S[i];
		delete[] I[i];

	}
	delete[] H;
	delete[] S;
	delete[] I;


	

}


Image* Image::Crop(int x, int y, int w, int h)
{
	/* WORK HERE */
	return NULL;
}


void Image::ExtractChannel(int channel)
{
	/* WORK HERE */
	int x, y;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel extrac_c;
			switch (channel)
			{
			case 1:extrac_c.Set(p.r, 0, 0, p.a); break;
			case 2:extrac_c.Set(0, p.g, 0, p.a); break;
			case 3:extrac_c.Set(0, 0, p.b, p.a); break;
			default:cerr << "channel number error!" << endl; system("pause>nul");
			}
			GetPixel(x, y) = extrac_c;
		}
	}
}


void Image::Quantize (int nbits)
{
	/* WORK HERE */
}

void Image::RandomDither (int nbits)
{
	/* WORK HERE */
}


static int Bayer4[4][4] =
{
    {15,  7, 13,  5},
    { 3, 11,  1,  9},
    {12,  4, 14,  6},
    { 0,  8,  2, 10}
};


void Image::OrderedDither(int nbits)
{
	/* WORK HERE */
}

/* Error-diffusion parameters */
const double
    ALPHA = 7.0 / 16.0,
    BETA  = 3.0 / 16.0,
    GAMMA = 5.0 / 16.0,
    DELTA = 1.0 / 16.0;

void Image::FloydSteinbergDither(int nbits)
{
	/* WORK HERE */
}

void Image::Blur(int n)
{
	/* WORK HERE */
	/*using GaussianBlur*/
	int i, j, R, G, B;
	const int mheight = Height();
	const int mwidth = Width();
	int **gaur = new int*[mheight];
	int **gaug = new int*[mheight];
	int **gaub = new int*[mheight];
	int **pixelr = new int*[mheight];
	int **pixelg = new int*[mheight];
	int **pixelb = new int*[mheight];
	for (i = 0; i < mheight; i++)
	{
		gaur[i] = new int[mwidth];
		gaug[i] = new int[mwidth];
		gaub[i] = new int[mwidth];
		pixelr[i] = new int[mwidth];
		pixelg[i] = new int[mwidth];
		pixelb[i] = new int[mwidth];
		for (j = 0; j < mwidth; j++)
		{
			Pixel p = GetPixel(i, j);
			// keep the edge
			gaur[i][j] = pixelr[i][j] = p.r;
			gaug[i][j] = pixelg[i][j] = p.g;
			gaub[i][j] = pixelb[i][j] = p.b;
		}

	}
	// filter n times
	for (int k = 0; k < n; ++k)
	{
		for (i = 1; i < mheight - 1; i++)
		{
			for (j = 1; j < mwidth - 1; j++)
			{
				R = gaur[i - 1][j - 1] + 2 * gaur[i][j - 1] + gaur[i + 1][j - 1]
					+ 2 * gaur[i - 1][j] + 4 * gaur[i][j] + 2 * gaur[i + 1][j]
					+ gaur[i - 1][j + 1] + 2 * gaur[i][j + 1] + gaur[i + 1][j + 1];
				gaur[i][j] = R / 16;

				G = gaug[i - 1][j - 1] + 2 * gaug[i][j - 1] + gaug[i + 1][j - 1]
					+ 2 * gaug[i - 1][j] + 4 * gaug[i][j] + 2 * gaug[i + 1][j]
					+ gaug[i - 1][j + 1] + 2 * gaug[i][j + 1] + gaug[i + 1][j + 1];
				gaug[i][j] = G / 16;

				B = gaub[i - 1][j - 1] + 2 * gaub[i][j - 1] + gaub[i + 1][j - 1]
					+ 2 * gaub[i - 1][j] + 4 * gaub[i][j] + 2 * gaub[i + 1][j]
					+ gaub[i - 1][j + 1] + 2 * gaub[i][j + 1] + gaub[i + 1][j + 1];
				gaub[i][j] = B / 16;
			}
		}
	}
	int x, y;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel blur_p(ComponentClamp(gaur[x][y]), ComponentClamp(gaug[x][y]),
				ComponentClamp(gaub[x][y]), p.a);
			GetPixel(x, y) = blur_p;
		}
	}
}

void Image::Sharpen(int n)
{
	/* WORK HERE */
	
}

void Image::EdgeDetect()
{
	/* WORK HERE */
	/*using Laplace operator*/
	int i, j, R, G, B;
	const int mheight = Height();
	const int mwidth = Width();
	int **lapr = new int*[mheight];
	int **lapg = new int*[mheight];
	int **lapb = new int*[mheight];
	int **pixelr = new int*[mheight];
	int **pixelg = new int*[mheight];
	int **pixelb = new int*[mheight];
	for (i = 0; i < mheight; i++)
	{
		lapr[i] = new int[mwidth];
		lapg[i] = new int[mwidth];
		lapb[i] = new int[mwidth];
		pixelr[i] = new int[mwidth];
		pixelg[i] = new int[mwidth];
		pixelb[i] = new int[mwidth];
		for (j = 0; j < mwidth; j++)
		{
			Pixel p = GetPixel(i, j);
			// keep the edge
			lapr[i][j] = pixelr[i][j] = p.r;
			lapg[i][j] = pixelg[i][j] = p.g;
			lapb[i][j] = pixelb[i][j] = p.b;
		}

	}
	for (i = 1; i < mheight - 1; i++)
	{
		for (j = 1; j < mwidth - 1; j++)
		{
			R = -pixelr[i - 1][j - 1] - pixelr[i][j - 1] - pixelr[i + 1][j - 1]
				- pixelr[i - 1][j] + 8 * pixelr[i][j] - pixelr[i + 1][j]
				- pixelr[i - 1][j + 1] - pixelr[i][j + 1] - pixelr[i + 1][j + 1];
			lapr[i][j] = abs(R);

			G = -pixelg[i - 1][j - 1] - pixelg[i][j - 1] - pixelg[i + 1][j - 1]
				- pixelg[i - 1][j] + 8 * pixelg[i][j] - pixelg[i + 1][j]
				- pixelg[i - 1][j + 1] - pixelg[i][j + 1] - pixelg[i + 1][j + 1];
			lapg[i][j] = abs(G);

			B = -pixelb[i - 1][j - 1] - pixelb[i][j - 1] - pixelb[i + 1][j - 1]
				- pixelb[i - 1][j] + 8 * pixelb[i][j] - pixelb[i + 1][j]
				- pixelb[i - 1][j + 1] - pixelb[i][j + 1] - pixelb[i + 1][j + 1];
			lapb[i][j] = abs(B);
		}
	}
	int x, y;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel sharpen_p(lapr[x][y], lapg[x][y], lapb[x][y], p.a);
			GetPixel(x, y) = sharpen_p;
		}
	}
}

Image* Image::Scale(double sx, double sy)
{
	/* WORK HERE */
	return NULL;
}

Image* Image::Rotate(double angle)
{
	/* WORK HERE */
	return NULL;
}

void Image::Fun()
{
	/* WORK HERE */
}

/**
 * Image Sample
 **/
void Image::SetSamplingMethod(int method)
{
    assert((method >= 0) && (method < IMAGE_N_SAMPLING_METHODS));
    sampling_method = method;
}


Pixel Image::Sample (double u, double v){
    /* WORK HERE */
	return Pixel();
}