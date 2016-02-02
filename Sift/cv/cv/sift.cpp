#include<cv.h>  
#include <highgui.h>  
#include <features2d/features2d.hpp>
#include <legacy/legacy.hpp>
#include <nonfree/nonfree.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(){  
	//Sift ����

	//��ȡͼ��
	Mat input1=imread("image.jpg",1);
	Mat input2=imread("image_p.jpg",1);
	Mat output1, output2;

	SiftFeatureDetector sift_detector;
	vector<KeyPoint> keypoint1,keypoint2;
	sift_detector.detect(input1,keypoint1);
	//��һ��ͼ��������	
	sift_detector.detect(input1,keypoint1);
	drawKeypoints(input1,keypoint1,output1);
	//�ڶ���ͼ��������
	sift_detector.detect(input2,keypoint2);
	drawKeypoints(input2,keypoint2,output2);

	SiftDescriptorExtractor extractor;
	Mat descriptor1,descriptor2;
	//������������
	extractor.compute(input1,keypoint1,descriptor1);
	extractor.compute(input2,keypoint2,descriptor2);
	
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> matches;
	Mat img_matches;
	//ƥ����������
	matcher.match(descriptor1,descriptor2,matches);
	nth_element(matches.begin(), matches.begin()+30,matches.end());
	matches.erase(matches.begin() + 30, matches.end());
	//�������� 
	drawMatches(input1,keypoint1,input2,keypoint2,matches,img_matches);
	imshow("sift",img_matches);
	imwrite("sift.jpg",img_matches);


	/*
	Surf����:

	Mat input1_surf=imread("lena.jpg",1);
	Mat input2_surf=imread("lena_r.jpg",1);
	SurfFeatureDetector surf_detector;
	surf_detector.detect(input1_surf,keypoint1);

	Mat output1_surf, output2_surf;
	//��һ��ͼ��������
	sift_detector.detect(input1_surf,keypoint1);
	drawKeypoints(input1_surf,keypoint1,output1_surf);
	//�ڶ���ͼ��������
	sift_detector.detect(input2_surf,keypoint2);
	drawKeypoints(input2_surf,keypoint2,output2_surf);

	SurfDescriptorExtractor extractor_surf;
	Mat descriptor1_surf,descriptor2_surf;
	Mat img_matches_surf;

	//������������
	extractor_surf.compute(input1_surf,keypoint1,descriptor1_surf);
	extractor_surf.compute(input2_surf,keypoint2,descriptor2_surf);
	//ƥ����������
	matcher.match(descriptor1_surf,descriptor2_surf,matches);
	//�������� 
	drawMatches(input1_surf,keypoint1,input2_surf,keypoint2,matches,img_matches_surf);
	imshow("surf",img_matches);
	imwrite("surf.jpg",img_matches);
	*/
	waitKey();


}  