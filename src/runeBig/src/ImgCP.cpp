#include "RMVideoCapture.hpp"
#include "ImgCP.hpp"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>  
#include <cmath>
#include "findRect.hpp"
#include "MnistRecognizer.h"
#include "DigitRecognizer.h"
#include "Settings.h"
#include "angleSol.h"
#include "master.h"
#include "LRBlock.h"
#include "ros/ros.h"
#include "geometry_msgs/Point.h"
using namespace std;
using namespace cv;
volatile unsigned int pIdx = 0;
volatile unsigned int cIdx = 0;
volatile bool failure = false;
struct ImageData {
	Mat img;
	unsigned int frame;
};
ImageData data[BUFFER_SIZE];
void preprocessRGB(Mat img)
    {
        vector<Mat> channels;
        split(img,channels); 
        Mat Red = channels.at(2);
        Mat Blue = channels.at(0);
        Mat Green = channels.at(1);
        Mat B_R = Blue - Red;
        Mat B_G = Blue - Green;
        Mat result;
        imshow("R-B",B_R);
        waitKey(1);
        imshow("R-G",B_R);
        waitKey(1);
        result = B_R & B_G;
        imshow("result",result);
        // result = R_B & R_G;
        // threshold(result,result,90,255,THRESH_BINARY);
        imshow("R",Red);
        waitKey(1);
        imshow("G",Green);
        waitKey(1);
        imshow("B",Blue);
        waitKey(1);
        result.copyTo(binary);
        return ;
    }
void DigitThread(Mat img,vector<int>& ans)
{
    //cout<<"thread 3 running"<<endl;
    DigitRecognizer dt;
    dt.preprocessRGB(img,img);
    if(!dt.findDigits())
    {
        //cout<<"thread 3 end 1"<<endl
        cout<<" no digit found "<<endl;
        return;
    }
    if(dt.getAns())
    {
       // cout<<"thread 3 end 2"<<endl;
        for(int i = 0; i<5;i++)
        {
            cout<< "  "<< dt.ans[i];
            ans.push_back(ans[i]);
        }
        //dt.recordResults(cIdx);
        return;
    }
    
    //dt.recordResults(cIdx);
    return;
}
void ImgCP::ImageProducer()
{
	if (isVideoMode)
	{
		if (videoPath == NULL)
		{
			cout << "excuse me?" << endl;
			return;
		}
		VideoCapture cap(videoPath);
		if (!cap.isOpened())
		{
			cout << "not open" << endl;
			return;
		}
		cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
		while(1)
		{
			while (pIdx - cIdx >= BUFFER_SIZE);
			Mat temp;
			cap >> temp; 
			resize(temp, temp, Size(640, 480), 0, 0, INTER_CUBIC);
			temp.copyTo(data[pIdx % BUFFER_SIZE].img);
			data[pIdx % BUFFER_SIZE].frame++;
			++pIdx;
		}
	}
	else
	{
		std::string cameraPath = "/dev/video";
        //const char* cp =  cameraPath + cameraNumber;
        RMVideoCapture cap("/dev/video1", 3); 
		cap.setVideoFormat(640, 480, 1);
		//cap.setExposureTime(0, settings->cameraSetting.ExposureTime);//settings->exposure_time);
		cap.startStream();
		cap.info();
		while(1)
		{
            //cout<<"image producer running"<<endl;
        	while (pIdx - cIdx >= BUFFER_SIZE);
			cap >> data[pIdx % BUFFER_SIZE].img;
			data[pIdx % BUFFER_SIZE].frame = cap.getFrameCount();
			++pIdx;
		}
	}
}

void ImgCP::ImageConsumer(int argc, char** argv)
{
    cout<<"start"<<endl;
    while(pIdx == 0);
    Settings s("setting.xml","1.yml");
    if(!s.load())
    {
	    cout<<"where is my setting file?"<<endl;        
	    return ;
    }
    while(true)
    {
    
            
                //waitKey(20);
     }
}
