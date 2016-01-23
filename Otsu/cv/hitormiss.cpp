#include "cv.h"
#include "highgui.h"

int main(){
    IplImage *img= cvLoadImage("pic.jpg");//读取图片
    cvNamedWindow("Example1",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Example2",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Example3",CV_WINDOW_AUTOSIZE);

    cvShowImage("Example1",img);//在Example1显示图片
    //    cvCopy(img,temp);
    IplImage* temp=cvCreateImage( //创建一个size为image,三通道8位的彩色图
        cvGetSize(img),
        IPL_DEPTH_8U,
        3
        );

    cvErode(img,temp,0,1);//腐蚀
    cvShowImage("Example2",temp);

    cvDilate(img,temp,0,1);//膨胀
    cvShowImage("Example3",temp);


    cvWaitKey(0);//暂停用于显示图片


    cvReleaseImage(&img);//释放img所指向的内存空间并且
    cvDestroyWindow("Example1");
    cvDestroyWindow("Example2");
    cvDestroyWindow("Example3");
    
    return 0;
}