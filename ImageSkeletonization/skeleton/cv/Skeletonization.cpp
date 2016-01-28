#include<cv.h>  
#include <highgui.h>  
#include <iostream>

using namespace std;

IplImage *result, *imagehit;
IplImage *image_cmp, *image_cmp_erode, *image_erode;

int se1[8][9];
int se2[8][9];
//创建8个方向的结构元素
void create_se()
{
	memset(se1, 0, sizeof(se1));
	memset(se2, 0, sizeof(se2));
	se1[0][4] = 255;
	se1[0][6] = 255;
	se1[0][7] = 255;
	se1[0][8] = 255;
	se2[0][0] = 255;
	se2[0][1] = 255;
	se2[0][2] = 255;

	se1[1][4] = 255;
	se1[1][6] = 255;
	se1[1][7] = 255;
	se1[1][3] = 255;
	se2[1][5] = 255;
	se2[1][1] = 255;
	se2[1][2] = 255;

	se1[2][4] = 255;
	se1[2][6] = 255;
	se1[2][0] = 255;
	se1[2][3] = 255;
	se2[2][2] = 255;
	se2[2][5] = 255;
	se2[2][8] = 255;

	se1[3][4] = 255;
	se1[3][3] = 255;
	se1[3][1] = 255;
	se1[3][0] = 255;
	se2[3][5] = 255;
	se2[3][7] = 255;
	se2[3][8] = 255;

	se1[4][4] = 255;
	se1[4][1] = 255;
	se1[4][2] = 255;
	se1[4][0] = 255;
	se2[4][8] = 255;
	se2[4][6] = 255;
	se2[4][7] = 255;

	se1[5][4] = 255;
	se1[5][5] = 255;
	se1[5][2] = 255;
	se1[5][1] = 255;
	se2[5][7] = 255;
	se2[5][3] = 255;
	se2[5][6] = 255;

	se1[6][2] = 255;
	se1[6][4] = 255;
	se1[6][5] = 255;
	se1[6][8] = 255;
	se2[6][0] = 255;
	se2[6][6] = 255;
	se2[6][3] = 255;

	se1[7][4] = 255;
	se1[7][5] = 255;
	se1[7][7] = 255;
	se1[7][8] = 255;
	se2[7][0] = 255;
	se2[7][1] = 255;
	se2[7][3] = 255;
}

int otsu(IplImage * image)
{
	int width = image->width, height = image->height;
	double hist[256];
	int countA[256];
	double avgValue = 0;  
	uchar* ptr, *ptr2;
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

//腐蚀
void erode(IplImage * srcimage, IplImage * dstimage, int senum, int k)
{ 	
	int flag;  
	bool jud;
	unsigned char *ptr, *dst;  
	for (int i = 0 ; i< dstimage->height; i++)  
	{  
		for (int j = 0 ; j< dstimage->width; j++)  
		{  
			dst = (unsigned char*)dstimage->imageData + i*dstimage->widthStep +j;  
		
			flag = 1;  
			dst = (unsigned char*)dstimage->imageData + i*dstimage->widthStep +j;  
			if (( i == 0) || (j == 0) || (i == dstimage->height-1) || (j == dstimage->width -1 ))  
			{  
				*dst = 0;  
			}  
			else{  
				for (int r = i -1 ; r<= i+1; r++)  
				{  
					for (int s = j-1 ; s <= j +1 ; s++)  
					{  
						ptr = (unsigned char*)srcimage->imageData + r * srcimage->widthStep+ s;  
						if (k == 0)
							jud = ((*ptr != se1[senum][3*(r-i+1) + s-j+1]) && (se1[senum][3*(r-i+1) + s-j+1] == 255));
						else 
							jud = ((*ptr != se2[senum][3*(r-i+1) + s-j+1]) && (se2[senum][3*(r-i+1) + s-j+1] == 255));
						if (jud)
						{  						
							flag = 0;  
							break;  
						}  
					}  
				}  
				if (flag == 1)  
				{  		 		  
					*dst = 255;  					
				}  
				else{ 
					*dst = 0;  
				}  
			}
		}  
	} 
}

//击中或不击中变换
void hit_or_miss(IplImage * srcimage, IplImage * dstimage, int senum)
{
	image_cmp = cvCreateImage(cvSize(srcimage->width, srcimage->height),srcimage->depth ,1);  
	image_erode = cvCreateImage(cvSize(srcimage->width, srcimage->height),srcimage->depth ,1);  
	image_cmp_erode = cvCreateImage(cvSize(srcimage->width, srcimage->height),srcimage->depth ,1);   
	int flag;  
	unsigned char *ptr, *dst,*ptr2;  

	for (int i = 0 ; i< image_cmp->height; i++)  
	{  
		for (int j = 0 ; j< image_cmp->width; j++)  
		{  
			ptr = (unsigned char*)srcimage->imageData + i * srcimage->widthStep+ j;  
			dst = (unsigned char*)image_cmp->imageData + i*image_cmp->widthStep + j;  
			*dst = 255- (*ptr);  

		}  
	}  	
	erode(srcimage, image_erode, senum, 0);
	erode(image_cmp, image_cmp_erode, senum, 1);
	//二者求交集  
	for (int i = 0 ; i< dstimage->height; i++)  
	{  
		for (int j = 0 ; j< dstimage->width; j++)  
		{  
			ptr = (unsigned char *)image_erode->imageData + image_erode->widthStep * i + j;  
			ptr2 = (unsigned char *)image_cmp_erode->imageData + image_cmp_erode->widthStep * i + j;  
			dst = (unsigned char *)dstimage->imageData +  dstimage->widthStep*i + j;  
			if (((*ptr) == 255) && ((*ptr2) == 255))  
			{  
				*dst = 255;   			
			}  
			else{  			
				*dst = 0;  
			}  			
		} 
	}     
}

//细化操作
void thinning(IplImage * srcimage, IplImage * dstimage, int senum)
{
	unsigned char *ptr, *dst,*ptr2; 
	hit_or_miss(srcimage, imagehit, senum);

	for (int i = 0 ; i< srcimage->height; i++)  
	{  
		for (int j = 0 ; j< srcimage->width; j++)  
		{  
			ptr = (unsigned char*)srcimage->imageData + i * srcimage->widthStep+ j;  
			ptr2 = (unsigned char*)imagehit->imageData + i*imagehit->widthStep + j;  
			dst = (unsigned char*) dstimage->imageData + i*dstimage->widthStep + j;
			if (((*ptr2)  == 0) && ((*ptr) == 255))  
			{  
				*dst = 255;
			}  
			else
			{
				*dst = 0;
			}

		}  
	}
}

//粗话操作
void thickening(IplImage * srcimage, IplImage * dstimage, int senum)
{
	unsigned char *ptr, *dst,*ptr2; 
	hit_or_miss(srcimage, imagehit, senum);

	for (int i = 0 ; i< srcimage->height; i++)  
	{  
		for (int j = 0 ; j< srcimage->width; j++)  
		{  
			ptr = (unsigned char*)srcimage->imageData + i * srcimage->widthStep+ j;  
			ptr2 = (unsigned char*)imagehit->imageData + i*imagehit->widthStep + j;  
			dst = (unsigned char*) dstimage->imageData + i*dstimage->widthStep + j;
			if (((*ptr2)  == 255) || ((*ptr) == 255))  
			{  
				*dst = 255;
			}  
			else
			{
				*dst = 0;
			}

		}  
	}
}
int main(){  
	IplImage * imagegray = cvLoadImage("potato.jpg", 0);  
	IplImage * image = cvCreateImage(cvSize(imagegray->width, imagegray->height),imagegray->depth ,1);    
	int thres = otsu(imagegray);
	cvThreshold(imagegray, image, thres, 255, CV_THRESH_BINARY);
	result = cvCreateImage(cvSize(image->width, image->height), image->depth, 1);
	imagehit = cvCreateImage(cvSize(image->width, image->height), image->depth, 1);
	create_se();
	cvNamedWindow("skeleton1", 1);  
	cvShowImage("skeleton1", image);
	for (int i = 0; i < 50; i++) 
	{
		for (int j = 0; j < 8; j++)
		{
			thinning(image, result, j);
			image = result;			
			cvShowImage("skeleton",image);
			cvWaitKey(10);
		}
	}	
	cvWaitKey(0);
	cvDestroyAllWindows();
	return 0 ;  
}  