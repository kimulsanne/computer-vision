#include<cv.h>  
#include <highgui.h>  
#include <iostream>

using namespace std;

 
void histogramEqualization(IplImage * srcimage, IplImage * dstimage) {
	unsigned char *ptr, *dst;
	int count[256] = {0};		//灰度值个数
	double p[256];				//灰度的比例 
	double c[256] = {0};		//累计灰度比例
	//统计灰度个数
	for (int i = 0 ; i < srcimage->height; i++) {  
		for (int j = 0 ; j< srcimage->width; j++) {  
			ptr = (unsigned char*)srcimage->imageData + i * srcimage->widthStep + j;
			count[*ptr]++;
		}
	}
	//计算灰度分布密度
	for(int i = 0; i < 256; i++) {
		p[i] = double(count[i])/(srcimage->width * srcimage->height);
		if (i == 0)
			c[i] = p[i];
		else 
			c[i] = c[i-1] + p[i];
	}
	//累计分布取整，保存计算出来的灰度映射关系
	for (int i = 0; i < 256; i++) {
		c[i] = (int)(255.0 * c[i] + 0.5);
	}
	for(int i = 0; i < srcimage->height; i++) {
		for(int j = 0; j < srcimage->width; j++) {
			ptr = (unsigned char*)srcimage->imageData + i * srcimage->widthStep + j;
			dst = (unsigned char*)dstimage->imageData + i * dstimage->widthStep + j;
			*dst = c[(*ptr)];
		}
	}
}

int main(){  
	IplImage * imagegray = cvLoadImage("image.png", 0);  
	IplImage * result = cvCreateImage(cvSize(imagegray->width, imagegray->height), imagegray->depth, 1);
	histogramEqualization(imagegray, result);
	cvNamedWindow("image", 1);  
	cvShowImage("image", result);
	cvNamedWindow("originimage", 1);  
	cvShowImage("originimage", imagegray);
	cvWaitKey(0);
	cvDestroyAllWindows();
	return 0 ;  
}  