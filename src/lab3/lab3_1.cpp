#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 0;
        old.c_cc[VTIME] = 2;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}



struct framebuffer_info
{
    uint32_t bits_per_pixel;    // depth of framebuffer
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
};

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path );

int main ( int argc, const char *argv[] )
{
    // variable to store the frame get from video stream
    cv::Mat frame,frame2;
    cv::Size2f frame_size;
    int count=0;
    int count2=0;
    // open video stream device
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a5d5f5dacb77bbebdcbfb341e3d4355c1
    cv::VideoCapture camera ( 2 );
    camera.set(CV_CAP_PROP_FRAME_WIDTH, 600);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, 400);
    // get info of the framebuffer
    // fb_info = ......
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    // open the framebuffer device
    // http://www.cplusplus.com/reference/fstream/ofstream/ofstream/
    // ofs = ......
    std::ofstream ofs("/dev/fb0");
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

    while ( true )
    {
        // get video frame from stream
        // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a473055e77dd7faa4d26d686226b292c1
        // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a199844fb74226a28b3ce3a39d1ff6765
        // frame = ......
        camera.read(frame);
        // get size of the video frame
        // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a146f8e8dda07d1365a575ab83d9828d1
        // frame_size = ......
        frame_size = frame.size();
        // transfer color space from BGR to BGR565 (16-bit image) to fit the requirement of the LCD
        // https://docs.opencv.org/3.4.7/d8/d01/group__imgproc__color__conversions.html#ga397ae87e1288a81d2363b61574eb8cab
        // https://docs.opencv.org/3.4.7/d8/d01/group__imgproc__color__conversions.html#ga4e0972be5de079fed4e3a10e24ef5ef0
        cv::cvtColor(frame, frame2, 12);
        // output the video frame to framebufer row by row
        for ( int y = 0; y < frame_size.height; y++ )
        {
            // move to the next written position of output device framebuffer by "std::ostream::seekp()"
            // http://www.cplusplus.com/reference/ostream/ostream/seekp/
            ofs.seekp(y*fb_info.xres_virtual*2);
            // write to the framebuffer by "std::ostream::write()"
            // you could use "cv::Mat::ptr()" to get the pointer of the corresponding row.
            // you also need to cacluate how many bytes required to write to the buffer
            // http://www.cplusplus.com/reference/ostream/ostream/write/
            // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a13acd320291229615ef15f96ff1ff738
            ofs.write(reinterpret_cast<char*>(frame2.ptr(y)), fb_info.xres_virtual * 2);
        count2 ++;
        if(count2 == 10){
            if (getch() == 'c'){
                // create file name by sprintf
                char filename[100];
                sprintf(filename, "image_%d.png", count);
                // save image
                cv::imwrite(filename, frame);
                count +=1;
            }      
            count2 = 0;
        }
        
    }

    // closing video stream
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#afb4ab689e553ba2c8f0fec41b9344ae6
    camera.release ( );

    return 0;
}

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path )
{
    struct framebuffer_info fb_info;        // Used to return the required attrs.
    struct fb_var_screeninfo screen_info;   // Used to get attributes of the device from OS kernel.

    // open deive with linux system call "open( )"
    // https://man7.org/linux/man-pages/man2/open.2.html
    int fd = open(framebuffer_device_path, O_RDWR);
    // get attributes of the framebuffer device thorugh linux system call "ioctl()"
    // the command you would need is "FBIOGET_VSCREENINFO"
    // https://man7.org/linux/man-pages/man2/ioctl.2.html
    // https://www.kernel.org/doc/Documentation/fb/api.txt
    ioctl(fd, FBIOGET_VSCREENINFO, &screen_info);
    // put the required attributes in variable "fb_info" you found with "ioctl() and return it."
    // fb_info.xres_virtual = ......
    // fb_info.bits_per_pixel = ......
    fb_info.xres_virtual = screen_info.xres_virtual;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;
    return fb_info;
};