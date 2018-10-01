#include <iostream>
#include "image.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include<time.h>
#define pi 3.141592653589793238462643383279502884197169399375
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
			Pixel brighten_p = p*factor;
			GetPixel(x,y) = brighten_p;
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
	if (factor > 1)
	{
		factor = 1.0;
	}
	else if (factor < 0)
	{
		factor = 0;
	}
	////RGB->HSI
	const int height = Height();
	const int width = Width();
	//float **H = new float*[width];
	//float **S = new float*[width];
	//float **I = new float*[width];
	//for (int i = 0; i < width; ++i)
	//{
	//	H[i] = new float[height];
	//	S[i] = new float[height];
	//	I[i] = new float[height];
	//}
	//for (int x = 0; x < Width(); x++)
	//{
	//	for (int y = 0; y < Height(); y++)
	//	{
	//		Pixel p = GetPixel(x, y);
	//		float r = p.r / 255.0;
	//		float g = p.g / 255.0;
	//		float b = p.b / 255.0;
	//		I[x][y] = (r + g + b) / 3.0;
	//		float R = r / (3 * I[x][y]);
	//		float G = g / (3 * I[x][y]);
	//		float B = b / (3 * I[x][y]);
	//		if ((r == g) && (g ==b))
	//		{
	//			S[x][y] = 0;
	//			H[x][y] = 0;
	//		}
	//		else
	//		{
	//			float w = 0.5*((r -g + r - b) / (1e-7+sqrt((r - g)*(r - g) + (r - b)*(g - b))));
	//			if (w > 1)w = 1;
	//			if (w < -1)w = -1;
	//			H[x][y] = acos(w);
	//			if (H[x][y] < 0)
	//			{
	//				/*cerr << "H<0" << endl;
	//				system("pause>nul");*/
	//				H[x][y] += 2 * pi;
	//			}
	//			else if (H[x][y] > 2 * pi)
	//			{
	//				H[x][y] -= 2 * pi;
	//			}
	//			else if (b > g)
	//				H[x][y] = 2 * pi - H[x][y];
	//			if (r <= g&&r <= b)
	//				S[x][y] = 1.0 - 3*R;
	//			else if(g <= r&&g <= b)
	//				S[x][y] = 1.0 - 3*G;
	//			else
	//				S[x][y] = 1.0 - 3*B;
	//		}
	//		//change S			
	//		S[x][y] = S[x][y] * factor;
	//		//S[x][y] = 1 / (1 + exp(-S[x][y]));
	//	}
	//}
	////HSI->RGB
	//for (int x = 0; x < Width(); x++)
	//{
	//	for (int y = 0; y < Height(); y++)
	//	{
	//		float s = S[x][y];
	//		float h = H[x][y];
	//		float i = I[x][y];
	//		float R, G, B;
	//		int r, g, b;
	//		if (s > 1)
	//			s = 1;
	//		if (i > 1)
	//			i = 1;
	//		if (s == 0)
	//			r = g = b = i;
	//		else
	//		{
	//			if ((h >= 0) && (h < 2 * pi / 3))
	//			{
	//				B = (1 - s) / 3;
	//				R = (1 + s * cos(h) / cos(pi / 3 - h)) / 3;
	//				G = 1 - R - B;
	//			}
	//			else if((h >= 2 * pi / 3)&&(h< 4 * pi / 3))
	//			{
	//				h = h - 2 * pi / 3;
	//				R = (1 - s) / 3;
	//				G=(1 + s * cos(h) / cos(pi / 3 - h)) / 3;
	//				B = 1 - R - G;
	//			}
	//			else
	//			{
	//				h = h - 4 * pi / 3;
	//				G = (1 - s) / 3;
	//				B = (1 + s * cos(h) / cos(pi / 3 - h)) / 3;
	//				R = 1 - B - G;
	//			}
	//		}
	//		if (R < 0)R = 0;
	//		if (G < 0)G = 0;
	//		if (B < 0)B = 0;
	//		r = 3 * i*R;
	//		g = 3 * i*G;
	//		b = 3 * i*B;
	//		if (r > 1)r -= 1;
	//		if (g > 1)g -= 1;
	//		if (b > 1)b -= 1;
	//		Pixel p = GetPixel(x, y);
	//		Pixel saturated_p(r*255,g*255,b*255,p.a);
	//		GetPixel(x, y) = saturated_p;
	//	}
	//}
	//for (int i = 0; i < width; ++i)
	//{
	//	delete[] H[i];
	//	delete[] S[i];
	//	delete[] I[i];
	//}
	//delete[] H;
	//delete[] S;
	//delete[] I;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Pixel p = GetPixel(x, y);
			int L = p.Luminance();
			Pixel gray(L, L, L, p.a);
			p = PixelLerp(gray, p, factor);
			GetPixel(x, y) = p;

		}
	}
	

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
	int x, y;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel quant_p(PixelQuant(p, nbits));
			GetPixel(x, y) = quant_p;
		}
	}
}

void Image::RandomDither (int nbits)
{
	/* WORK HERE */
	/*Image *noise = new Image(*this);*/
	float segma = 0.5;
	int x, y;
	/*for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{

			Pixel p = GetPixel(x, y);
			noise->GetPixel(x, y).SetClamp(p.r, p.g, p.b, p.a);
		}
	}*/
	this->AddNoise(segma);
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			Pixel p = GetPixel(x, y);
			//Pixel q = noise->GetPixel(x, y);
			Pixel quant_p(PixelQuant(p+0.5, nbits));
			GetPixel(x, y) = quant_p;
		}
	}

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
	const int mheight = Height();
	const int mwidth = Width();
	float er,eg,eb;
	int x, y;
	for (x = 0; x < mwidth - 1; x++)
	{	
		for (y = 1; y < mheight - 1; y++)
		{
			Pixel p = GetPixel(x, y);
			Pixel p1 = GetPixel(x, y + 1);
			Pixel p2 = GetPixel(x + 1, y + 1);
			Pixel p3 = GetPixel(x + 1, y);
			Pixel p4 = GetPixel(x + 1, y - 1);
			Pixel quant_p(PixelQuant(p, nbits));
			
			er = p.r - quant_p.r;
			eg = p.g - quant_p.g;
			eb = p.b - quant_p.b;
			GetPixel(x + 0, y + 1).SetClamp(p1.r+er* ALPHA, p1.g + eg * ALPHA, p1.b + eb * ALPHA,p1.a);
			GetPixel(x + 1, y + 1).SetClamp(p2.r + er * DELTA, p2.g + eg * DELTA, p2.b + eb * DELTA, p2.a);
			GetPixel(x + 1, y + 0).SetClamp(p3.r + er * GAMMA, p3.g + eg * GAMMA, p3.b + eb * GAMMA, p3.a);
			GetPixel(x + 0, y + 1).SetClamp(p4.r + er * BETA, p4.g + eg * BETA, p4.b + eb * BETA, p4.a);
			GetPixel(x, y) = quant_p;
		}
	}
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
	Image *blur = new Image(*this);
	float alpha = 2.0 ;
	int x, y;
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{
			
			Pixel p = GetPixel(x, y);
			blur->GetPixel(x, y).SetClamp(p.r,p.g,p.b,p.a);
		}
	}
	blur->Blur(n);
	for (x = 0; x < Width(); x++)
	{
		for (y = 0; y < Height(); y++)
		{

			Pixel p = GetPixel(x, y);
			Pixel b = blur->GetPixel(x, y);
			Pixel sharpen_p(ComponentClamp(alpha*p.r+(1.0-alpha)*b.r), ComponentClamp(alpha*p.g + (1.0 - alpha)*b.g),
				ComponentClamp(alpha*p.b + (1.0 - alpha)*b.b), p.a);
			GetPixel(x, y) = sharpen_p;
		}
	}
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
	float r=max(1.0/sx,1.0/sy);
	int i, j;
	const int mheight = Height();
	const int mwidth = Width();
	int neww = sx * mwidth;
	int newh = sy * mheight;
	Image* scale_p=new Image(neww,newh);
	//new image's center
	int xpt = neww / 2;
	int ypt = newh / 2;
	//int **pixelr = new int*[mwidth];
	//int **pixelg = new int*[mwidth];
	//int **pixelb = new int*[mwidth];
	//for (i = 0; i < mwidth; i++)
	//{
	//	pixelr[i] = new int[mheight];
	//	pixelg[i] = new int[mheight];
	//	pixelb[i] = new int[mheight];
	//	for (j = 0; j < mheight; j++)
	//	{
	//		Pixel p = GetPixel(i, j);
	//		// keep the edge
	//		pixelr[i][j] = p.r;
	//		pixelg[i][j] = p.g;
	//		pixelb[i][j] = p.b;
	//	}
	//}
	int b = 0;//new image index
	for (j = 0; j < newh; j++)
	{
		double ty = (j - ypt) / sy + mheight / 2;
		int py = (int)ty;
		for (i = 0; i< neww; i++)
		{
			double tx = (i - xpt) / sx + mwidth / 2;
			int px = (int)tx;
			int p1_2, p3_4;
			//Find the value of the surrounding points
			if (px<0 || px> mwidth - 2 || py<0 || py>mheight - 2)
			{
				//The edge copy
				Pixel p = GetPixel(px, py);
				scale_p->data.raw[b++] = p.r;
				scale_p->data.raw[b++] = p.g;
				scale_p->data.raw[b++] = p.b;
				scale_p->data.raw[b++] = p.a;
				continue;
			}
			Pixel p1 = GetPixel(px, py);
			Pixel p2 = GetPixel(px + 1, py);
			Pixel p3 = GetPixel(px, py + 1);
			Pixel p4 = GetPixel(px + 1, py + 1);
			//int b = 4 * (x * neww + y);
			//r
			p1_2 = p1.r + (tx - px)*(p2.r - p1.r);
			p3_4 = p3.r + (tx - px)*(p4.r - p3.r);
			scale_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
			//g
			p1_2 = p1.g + (tx - px)*(p2.g - p1.g);
			p3_4 = p3.g + (tx - px)*(p4.g - p3.g);
			scale_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
			//b
			p1_2 = p1.b + (tx - px)*(p2.b - p1.b);
			p3_4 = p3.b + (tx - px)*(p4.b - p3.b);
			scale_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
			//a
			p1_2 = p1.a + (tx - px)*(p2.a - p1.a);
			p3_4 = p3.a + (tx - px)*(p4.a - p3.a);
			scale_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
		}
	}
	return scale_p;
}

Image* Image::Rotate(double angle)
{
	/* WORK HERE */
	int x, y, px, py;
	double tx, ty, p1, p2, p3, p4, p1_2, p3_4;
	const int mheight = Height();
	const int mwidth = Width();
	while (angle>180)
	{
		angle -= 360;
	}
	while (angle <= -180)
	{
		angle += 360;
	}
	float theta = angle / 180.0  * pi;
	double SinTheta = sin(theta);
	double CosTheta = cos(theta);
	int neww, newh;
	if (angle >= -90 && angle <= 90)
	{
		neww = mwidth * cos(abs(theta)) + mheight * sin(abs(theta));
		newh = mheight * cos(abs(theta)) + mwidth * sin(abs(theta));
	}
	else 
	{
		neww = mwidth * cos(pi-abs(theta)) + mheight * sin(pi - abs(theta));
		newh = mheight * cos(pi - abs(theta)) + mwidth * sin(pi - abs(theta));
	}
	double xpt = neww / 2;
	double ypt = newh / 2;
	double xr = mwidth / 2;
	double yr = mheight / 2;
	Image* rotate_p = new Image(neww, newh);
	double ConstX = -xpt * cos(theta) + ypt * sin(theta) + xr;
	double ConstY = -ypt * cos(theta) - xpt * sin(theta) + yr;
	int b = 0;//new image index
	for (y = 0; y < newh; y++)
	{
		tx = -y * SinTheta - CosTheta + ConstX;
		ty = y * CosTheta - SinTheta + ConstY;
		for (x = 0; x < neww; x++)
		{
			tx += CosTheta; //x*CosTheta - y*SinTheta + ConstX; (x-xr)*CosTheta - (y-yr)*SinTheta + xr 
			ty += SinTheta; //y*CosTheta + x*SinTheta + ConstY; (y-yr)*CosTheta + (x-xr)*SinTheta + yr 
			px = (int)tx;
			py = (int)ty;
			if (px<0 || px> mwidth - 2 || py<0 || py>mheight - 2)
			{
				//The point in the non-original area is set to 255(white)
				for(int i=0;i<4;++i)
					rotate_p->data.raw[b++] = 255;
				continue;
			}
			//Find the value of the surrounding points
			Pixel p1 = GetPixel(px, py);
			Pixel p2 = GetPixel(px+1, py);
			Pixel p3 = GetPixel(px, py+1);
			Pixel p4 = GetPixel(px+1, py+1);
			//int b = 4 * (x * neww + y);
			//r
			p1_2 = p1.r + (tx - px)*(p2.r - p1.r);
			p3_4 = p3.r + (tx - px)*(p4.r - p3.r);
			rotate_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
			//g
			p1_2 = p1.g + (tx - px)*(p2.g - p1.g);
			p3_4 = p3.g + (tx - px)*(p4.g - p3.g);
			rotate_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
			//b
			p1_2 = p1.b + (tx - px)*(p2.b - p1.b);
			p3_4 = p3.b + (tx - px)*(p4.b - p3.b);
			rotate_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
			//a
			p1_2 = p1.a + (tx - px)*(p2.a - p1.a);
			p3_4 = p3.a + (tx - px)*(p4.a - p3.a);
			rotate_p->data.raw[b++] = p1_2 + (ty - py)*(p3_4 - p1_2);
		}
	}
	//for (i = xpt - neww / 2; i < xpt + neww / 2; i++) 
	//{
	//	for (j = ypt - newh / 2; j < ypt + newh / 2; j++) 
	//	{
	//		
	//		int b = 4 * (i * neww + j);
	//		k = int((i - xpt)*cos(theta) + (j - ypt)*sin(theta));
	//		m = int((j - ypt)*cos(theta) - (i - xpt)*sin(theta));
	//		k += mwidth / 2;
	//		m += mheight / 2;
	//		if (k >= 0 && k <  mwidth && m >= 0 && m < mheight)
	//		{
	//			Pixel p = GetPixel(k, m);
	//			scale_p->data.raw[b++] = p.r;
	//			scale_p->data.raw[b++] = p.g;
	//			scale_p->data.raw[b++] = p.b;
	//			scale_p->data.raw[b++] = p.a;
	//			
	//		}
	//		else//The point in the non-original area is set to 255(white)
	//		{
	//			scale_p->data.raw[b++] = 255;
	//			scale_p->data.raw[b++] = 255;
	//			scale_p->data.raw[b++] = 255;
	//			scale_p->data.raw[b++] = 255;
	//		}	
	//	}
	//}
	return rotate_p;
}

void Image::Fun()
{
	/* WORK HERE */
	int x, y, px, py;
	double tx, ty, p1, p2, p3, p4, p1_2, p3_4;
	const int mheight = Height();
	const int mwidth = Width();
	double xr = (double)mwidth / 2.0f;
	double yr = (double)mheight / 2.0f;
	double factor = -0.01;
	Image* fun_p = new Image(mwidth, mheight);

	
	//copy original image
//   #pragma omp parallel for
	int b = 0;//new image index
	for (y = 0; y < mheight; y++)
	{
		for (x = 0; x < mwidth; x++)
		{
			fun_p->data.raw[b++] = GetPixel(x, y).r;
			fun_p->data.raw[b++] = GetPixel(x, y).g;
			fun_p->data.raw[b++] = GetPixel(x, y).b;
			fun_p->data.raw[b++] = GetPixel(x, y).a;
		}
	}
 //   #pragma omp parallel for
	for (y = 0; y < mheight; y++)
	{
		double relY = yr - y;	
		for (x = 0; x < mwidth; x++)
		{
			double relX = x - xr;
			double orgAngle;
			if (relX != 0)
			{
				orgAngle = atan(abs(relY) / abs(relX));
				if (relX > 0 && relY < 0) orgAngle = 2.0f*pi - orgAngle;
				else if (relX <= 0 && relY >= 0) orgAngle = pi - orgAngle;
				else if (relX <= 0 && relY < 0) orgAngle += pi;
			}
			else
			{
				// Take care of rare special case
				if (relY >= 0) orgAngle = 0.5f * pi;
				else orgAngle = 1.5f * pi;
			}
			double radius = sqrt(relX*relX + relY * relY);
			double newAngle = orgAngle + factor*radius;	// a progressive twist
			//double newAngle = orgAngle + 1 / (factor*radius + (4.0f / pi));
			double srcX = radius * cos(newAngle) ;
			double srcY = radius * sin(newAngle) ;
			srcX += xr;
			srcY += yr;
			srcY = mheight - srcY;
			px = (int)(floor(srcX));
			py = (int)(floor(srcY));
			// Clamp the source to legal image pixel
			/*if (px < 0) px = 0;
			else if (px >= mwidth) px = mwidth - 1;
			if (py < 0) py = 0;
			else if (py >= mheight) py = mheight - 1;*/
			if (px < 0)
			{
				if (py < 0)
				{
					GetPixel(x, y) = fun_p->GetPixel(0, 0);
					
				}
				else if (py > mheight - 2)
				{
					GetPixel(x, y) = fun_p->GetPixel(0, mheight - 1);
				}
				else
				{
					GetPixel(x, y) = fun_p->GetPixel(0, py);
				}
				continue;
			}
			else if (px > mwidth - 2)
			{
				if (py < 0)
				{
					GetPixel(x, y) = fun_p->GetPixel(mwidth - 1, 0);

				}
				else if (py > mheight - 2)
				{
					GetPixel(x, y) = fun_p->GetPixel(mwidth - 1, mheight - 1);
				}
				else
				{
					GetPixel(x, y) = fun_p->GetPixel(mwidth - 1, py);
				}
				continue;
			}
			else if (py >mheight -2)
			{
				if (px < 0)
				{
					GetPixel(x, y) = fun_p->GetPixel(0, mheight - 1);
				}
				else if (px > mwidth - 2)
				{
					GetPixel(x, y) = fun_p->GetPixel(mwidth - 1, mheight - 1);
				}
				else
				{
					GetPixel(x, y) = fun_p->GetPixel(px, mheight - 1);
				}
				continue;
			}
			else if(py < 0)
			{
				if (px < 0)
				{
					GetPixel(x, y) = fun_p->GetPixel(0, 0);
				}
				else if (px > mwidth - 2)
				{
					GetPixel(x, y) = fun_p->GetPixel(mwidth - 1, 0);
				}
				else
				{
					GetPixel(x, y) = fun_p->GetPixel(px, 0);
				}				
				continue;
			}
			//bilinear sampling
			//Find the value of the surrounding points
			Pixel p1 = fun_p->GetPixel(px, py);
			Pixel p2 = fun_p->GetPixel(px + 1, py);
			Pixel p3 = fun_p->GetPixel(px, py + 1);
			Pixel p4 = fun_p->GetPixel(px + 1, py + 1);
			//r
			p1_2 = p1.r + (srcX - px)*(p2.r - p1.r);
			p3_4 = p3.r + (srcX - px)*(p4.r - p3.r);
			GetPixel(x, y).r = p1_2 + (srcY - py)*(p3_4 - p1_2);
			
			//g
			p1_2 = p1.g + (srcX - px)*(p2.g - p1.g);
			p3_4 = p3.g + (srcX - px)*(p4.g - p3.g);
			GetPixel(x,y).g = p1_2 + (srcY - py)*(p3_4 - p1_2);
			//b
			p1_2 = p1.b + (srcX - px)*(p2.b - p1.b);
			p3_4 = p3.b + (srcX - px)*(p4.b - p3.b);
			GetPixel(x, y).b = p1_2 + (srcY - py)*(p3_4 - p1_2);
			//a
			p1_2 = p1.a + (srcX - px)*(p2.a - p1.a);
			p3_4 = p3.a + (srcX - px)*(p4.a - p3.a);
			GetPixel(x, y).a = p1_2 + (srcY - py)*(p3_4 - p1_2);
		}
	}
	
	return;
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
	if (sampling_method == IMAGE_SAMPLING_POINT)
	{

	}
	else if (sampling_method == IMAGE_SAMPLING_BILINEAR)
	{

	}
	else
	{

	}
	return Pixel();
}