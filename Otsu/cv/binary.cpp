#include"cv.h"
#include"highgui.h"
#include <iostream>
#include <math.h>
using namespace std;

IplImage* srcImage;
IplImage* grayImage; 
IplImage* desImage; 


int width;//图片宽度
int height;//图片高度

int countA[256];
double hist[256];
int sCA = 0, sFCA = 0;
int maxA = 0, minA = 255, minT, maxT;
int intensity;
double minB, maxB, b, p; 
double miu0, miu1;
uchar* ptr, *ptr2;
double con;

void conmiu(int t)
{
	double CA = 0;
	double FCA = 0;	
	for (int f = 0; f <= t; f++)
	{
		CA += countA[f];
		FCA += countA[f] * f;
	}

	miu0 = FCA / CA;
	miu1 = (sFCA - FCA) / (sCA - CA);
}

double miu(int a, int threshold)
{
	if (a > threshold)
		return exp(-con * abs(a - miu1));
	else
		return exp(-con * abs(a - miu0));
}

double min(double x, double y)
{
	if (x > y)
		return y;
	else 
		return x;
}

void otsu()
{
    double avgValue = 0;  
	int numPixel = width * height;  
    for(int i=0; i < 256; i++)  
    {  
        hist[i] = double(countA[i]) / numPixel;  
        avgValue += i * hist[i];       
    }  
	 
    int threshold = 0;    
    double gmax = 0;  
    double wk = 0, uk = 0;      
    for(int i = 0; i < 256; i++) {  
        wk += hist[i];  
        uk += i * hist[i];       

        double ut = avgValue*wk-uk;  
        double g = ut * ut / (wk*(1-wk));    
          
        if(g > gmax)  
        {  
            gmax = g;  
            threshold = i;  
        }         
    }  
	cout<<"Otsu: "<<threshold<<endl; 
}

void liner_index()
{
	minB = height * width;
	for (int t = 0; t < 256; t++)
	{
		b = 0;
		conmiu(t);
		for (int row = 0; row < height; row++)
		{
			ptr = (uchar*) grayImage->imageData + row * width;
			for (int cols=0; cols < width; cols++)
			{
				intensity = ptr[cols];	
				b = b + min(miu(intensity, t), (1 - miu(intensity, t)));				
			}
		}
		if (b < minB)
		{
			minB = b;
			minT = t;
		}
	}
	cout<<"Liner index: "<<minT<<endl;
}

void quadratic_index()
{
	minB = height * width;
	for (int t = 0; t < 256; t++)
	{
		b = 0;
		conmiu(t);
		for (int row = 0; row < height; row++)
		{
			ptr = (uchar*) grayImage->imageData + row * width;
			for (int cols=0; cols < width; cols++)
			{
				intensity = ptr[cols];	
				b = b + pow(min(miu(intensity, t), (1 - miu(intensity, t))), 2);				
			}
		}
		if (b < minB)
		{
			minB = b;
			minT = t;
		}
	}
	cout<<"Quadratic index: "<<minT<<endl;	
}

void similarity_measure()
{
	maxB = -1;
	for (int t = 0; t < 256; t++)
	{
		b = 0;
		conmiu(t);
		for (int row = 0; row < height; row++)
		{
			ptr = (uchar*) grayImage->imageData + row * width;
			for (int cols=0; cols < width; cols++)
			{
				intensity = ptr[cols];	
				b += miu(intensity, t);				
			}
		}

		if (b > maxB)
		{
			maxB = b;
			maxT = t;
		}
	}
	cout<<"Similarity measure: "<<maxT<<endl;	
}

void compactness()
{
	maxB = -1;
	for (int t = 0; t < 256; t++)
	{
		b = 0;
		p = 0;
		conmiu(t);
		for (int row = 1; row < height; row++)
		{
			ptr = (uchar*) grayImage->imageData + row * width;
			for (int cols = 1; cols < width; cols++)
			{
				intensity = ptr[cols];	
				b += miu(intensity, t);				
			}
		}
		for (int row = 1; row < height; row++)
		{
			ptr = (uchar*) grayImage->imageData + row * width;
			for (int cols = 1; cols < width - 1; cols++)
			{
				intensity = ptr[cols];	
				p += abs(miu(intensity, t) - miu(ptr[cols-1], t));				
			}
		}
		for (int row = 1; row < height; row++)
		{
			for (int cols = 1; cols < width - 1; cols++)
			{
				ptr = (uchar*) grayImage->imageData + cols * width;
				intensity = ptr[row];
				ptr2 = (uchar*) grayImage->imageData + (cols + 1)* width;
				p += abs(miu(intensity, t) - miu(ptr2[row], t));				
			}
		}
		b = b / p / p;
		if (b > maxB)
		{
			maxB = b;
			maxT = t;
		}
	}
	cout<<"Compactness: "<<maxT<<endl;	
}


int main(int argc, char** argv)
{
	srcImage = cvLoadImage( "pic.jpg", CV_LOAD_IMAGE_UNCHANGED); //导入图片	
	grayImage =  cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 1); 
	desImage =  cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(srcImage, grayImage, CV_RGB2GRAY);		//转化为灰度值

	width = grayImage->width;	//图片宽度
	height =grayImage->height;	//图片高度

	memset(countA, 0, sizeof(countA));
	for (int row = 0; row < height; row++)
	{
		ptr = (uchar*) grayImage->imageData + row * width; //获得灰度值数据指针
		for (int cols=0; cols < width; cols++)
		{
			intensity = ptr[cols];
			countA[intensity]++;
			if (intensity > maxA)
				maxA = intensity;
			if (intensity < minA)
				minA = intensity;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		sCA += countA[i];
		sFCA += countA[i] * i;
	}
	con = 1.0 / (maxA - minA);

	otsu();
	liner_index();
	quadratic_index();
	similarity_measure();
	compactness();

	system("pause");
	return 0;  
}

