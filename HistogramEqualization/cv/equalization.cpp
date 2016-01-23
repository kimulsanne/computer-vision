#include<cv.h>  
#include <highgui.h>  
#include <iostream>

using namespace std;

 
void histogramEqualization(IplImage * srcimage, IplImage * dstimage) {
	unsigned char *ptr, *dst;
	int count[256] = {0};		//�Ҷ�ֵ����
	double p[256];				//�Ҷȵı��� 
	double c[256] = {0};		//�ۼƻҶȱ���
	//ͳ�ƻҶȸ���
	for (int i = 0 ; i < srcimage->height; i++) {  
		for (int j = 0 ; j< srcimage->width; j++) {  
			ptr = (unsigned char*)srcimage->imageData + i * srcimage->widthStep + j;
			count[*ptr]++;
		}
	}
	//����Ҷȷֲ��ܶ�
	for(int i = 0; i < 256; i++) {
		p[i] = double(count[i])/(srcimage->width * srcimage->height);
		if (i == 0)
			c[i] = p[i];
		else 
			c[i] = c[i-1] + p[i];
	}
	//�ۼƷֲ�ȡ���������������ĻҶ�ӳ���ϵ
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