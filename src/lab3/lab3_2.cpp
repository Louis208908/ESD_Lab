#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main ( int argc, const char *argv[] )
{
    // variable to store the frame get from video stream
    cv::Mat frame;
    cv::Size size=cv::Size(640,480);
    int count=0;
    int fuck = atoi(argv[1]);
    int final_count = fuck*10;
    // open video stream device
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a5d5f5dacb77bbebdcbfb341e3d4355c1
    cv::VideoCapture camera ( 2 );
    // camera.set(CV_CAP_PROP_FRAME_WIDTH, 600);
    // camera.set(CV_CAP_PROP_FRAME_HEIGHT, 400);
    cv::VideoWriter video("/root/lab3/out.avi",CV_FOURCC('M','J','P','G'),10,size);
    // check if video stream device is opened success or not
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a9d2ca36789e7fcfe7a7be3b328038585
    if( !camera.isOpened() )
    {
        std::cerr << "Could not open video device." << std::endl;
        return 1;
    }

    // set propety of the frame
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a8c6d8c2d37505b5ca61ffd4bb54e9a7c
    // https://docs.opencv.org/3.4.7/d4/d15/group__videoio__flags__base.html#gaeb8dd9c89c10a5c63c139bf7c4f5704d

    while ( camera.read(frame) )
    {
        video.write(frame);
        count ++;
        if (count == final_count) break;
    }

    // closing video stream
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#afb4ab689e553ba2c8f0fec41b9344ae6
    camera.release ( );

    return 0;
}