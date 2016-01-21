#include<cv.h>  
#include <highgui.h>  
#include <iostream>

unsigned char* ptrx;
unsigned char* ptry;
unsigned char* ptr;
double maxG = 0;
using namespace std;
 

int otsu(IplImage * image)
{
	int width = image->width, height = image->height;
	double hist[256];
	int countA[256];
	double avgValue = 0;  
	uchar* ptr;
	int numPixel = width * height;
	int intensity;
	memset(countA, 0, sizeof(countA));
	for (int row = 0; row < height; row++)
	{
		ptr = (uchar*) image->imageData + row * width; //获得灰度值数据指针
		for (int cols=0; cols < width; cols++)
		{
			intensity = ptr[cols];
			countA[intensity]++;
		}
	}
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
	return threshold;
}

void rec(IplImage *ipl, int y, int x, int low, int dep) {
	//对8邻域像素进行查询  
	if (dep > 1000 || y >= ipl->width || x >= ipl->height || x < 1 || y < 1)
		return;
	int xNum[8] = {1,1,0,-1,-1,-1,0,1};  
	int yNum[8] = {0,1,1,1,0,-1,-1,-1};  
	int yy, xx;  
	for(int i = 0; i < 8; i++)  
	{  
		yy = y + yNum[i];  
		xx = x + xNum[i];
		unsigned char* ptr = (unsigned char*)ipl->imageData + yy * ipl->widthStep + xx;
		if (*ptr >= low && *ptr != 255)
		{
			*ptr = 255;
			rec(ipl, yy, xx, low, dep + 1);
		}
	}
}
int main(){  
	IplImage * imagegray = cvLoadImage("image.png", 0);  
	IplImage * ix = cvCreateImage(cvSize(imagegray->width, imagegray->height), imagegray->depth, 1);
	IplImage * iy = cvCreateImage(cvSize(imagegray->width, imagegray->height), imagegray->depth, 1);
	IplImage * result = cvCreateImage(cvSize(imagegray->width, imagegray->height), imagegray->depth, 1);
	IplImage * result1 = cvCreateImage(cvSize(imagegray->width, imagegray->height), imagegray->depth, 1);
	int h = imagegray->height;
	int w = imagegray->width;
	double *magMat = new double[h * w]; 
	double *theta = new double[h * w]; 
	int *orients = new int[h * w];
	//Step 1. 高斯滤波去除噪音
	cvSmooth(imagegray, result, CV_GAUSSIAN, 5, 5);

	//Step 2. 计算图像梯度及其方向
	cvSobel(result, ix, 1, 0, 3 );  //x方向梯度强度 
	cvSobel(result, iy, 0, 1, 3 ); //y方向梯度强度 

	int count =0;
	for(int i = 0; i < h; i++) 
	{ 
		for(int j = 0; j < w; j++ ) 
		{ 
			ptrx = (unsigned char*)ix->imageData + i * ix->widthStep + j;
			ptry = (unsigned char*)iy->imageData + i * iy->widthStep + j;
			ptr = (unsigned char*)result->imageData + i * result->widthStep + j;
			double dx = *ptrx; 
			double dy = *ptry;  
		
			double MagG = sqrt((float)(dx * dx) + (float)(dy * dy)); 
		
			double direction = atan2((float) dy,(float) dx) * 57.3; 
 
			magMat[i * w + j] = MagG; 
			*ptr = (unsigned char)MagG;
			if(MagG > maxG) 
				maxG = MagG; 
			if (direction < 0)
				direction += 360;
			theta[i * w + j] = direction;
		
			if ( (direction>0 && direction < 22.5) || (direction >157.5 && direction < 202.5) || (direction>337.5 && direction<360) ) 
				direction = 0;
			else if ( (direction>22.5 && direction < 67.5) || (direction >202.5 && direction <247.5) )
				direction = 45; 
			else if ( (direction >67.5 && direction < 112.5)|| (direction>247.5 && direction<292.5) )
				direction = 90; 
			else if ( (direction >112.5 && direction < 157.5)|| (direction>292.5 && direction<337.5) ) 
				direction = 135; 
			else 
				direction = 0; 
			
			orients[count] = (int)direction; 
			count++; 
		} 
	} 
	count=0;
	double leftPixel, rightPixel;
	for(int  i = 1; i < h-1; i++ )
	{
		for(int j = 1; j < w-1; j++ )
		{
			ptr = (unsigned char*)result->imageData + i * result->widthStep + j;
			switch ( orients[count] )
			{
			case 0:
				leftPixel  = magMat[i*w+j-1];
				rightPixel = magMat[i*w+j+1];
				break;
			case 45:
				leftPixel  = magMat[i*w-w+j+1];
				rightPixel = magMat[i*w+w+j-1];
				break;
			case 90:
				leftPixel  = magMat[i*w-w+j];
				rightPixel = magMat[i*w+w+j];
				break;
			case 135:
				leftPixel  = magMat[i*w-w+j-1];
				rightPixel = magMat[i*w+w+j+1];
				break;
			}
			
			if (( magMat[i*w+j] < leftPixel ) || (magMat[i*w+j] < rightPixel ) )
				*ptr=0;
			else
				*ptr = (unsigned char)(magMat[i*w+j]/ maxG*255);
			count++;
		}
	}

	//Step 4. 双阈值检测
	int maxT = otsu(result);
	int minT = maxT / 2;
	for (int i = 1; i < h - 1; i++)
		for (int j = 1; j < w - 1; j++)
		{
			ptr = (unsigned char*)result->imageData + i * result->widthStep + j;
			if (*ptr < minT)
				*ptr = 0;
			if (*ptr > maxT && *ptr != 255)
			{
				*ptr = 255;
				rec(result, i, j, minT, 0);
			}			
		}
	for (int i = 1; i < h - 1; i++)
		for (int j = 1; j < w - 1; j++)
		{
			ptr = (unsigned char*)result->imageData + i * result->widthStep + j;
			if (*ptr != 255)
			
				*ptr = 0;		
		}
	cvCanny(imagegray, result1, maxT, minT,3);
	cvNamedWindow("image", 1);  
	cvShowImage("image", result1);
	cvWaitKey(0);
	cvDestroyAllWindows();
	return 0 ;  
}  