#include "cv.h"
#include "highgui.h"

int main(){
    IplImage *img= cvLoadImage("pic.jpg");//��ȡͼƬ
    cvNamedWindow("Example1",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Example2",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Example3",CV_WINDOW_AUTOSIZE);

    cvShowImage("Example1",img);//��Example1��ʾͼƬ
    //    cvCopy(img,temp);
    IplImage* temp=cvCreateImage( //����һ��sizeΪimage,��ͨ��8λ�Ĳ�ɫͼ
        cvGetSize(img),
        IPL_DEPTH_8U,
        3
        );

    cvErode(img,temp,0,1);//��ʴ
    cvShowImage("Example2",temp);

    cvDilate(img,temp,0,1);//����
    cvShowImage("Example3",temp);


    cvWaitKey(0);//��ͣ������ʾͼƬ


    cvReleaseImage(&img);//�ͷ�img��ָ����ڴ�ռ䲢��
    cvDestroyWindow("Example1");
    cvDestroyWindow("Example2");
    cvDestroyWindow("Example3");
    
    return 0;
}