#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <linux/fb.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/ioctl.h>
#include <poll.h>
#include <termio.h>

struct framebuffer_info{
    uint32_t bits_per_pixel;    // depth of framebuffer
    uint32_t xres_virtual;      // how many pixel in a row in virtual screen
};

struct framebuffer_info get_framebuffer_info ( const char *framebuffer_device_path );

int main ( int argc, const char *argv[] )
{	
	bool capt;
	char img_name[15];
	int img_offset=0;
	bool video_in;
	char ch;
    struct pollfd fd[1] = {{fd:0, events:POLLIN, 0}};

    // variable to store the frame get from video stream
    cv::Mat frame, frame_buf;
	cv::Size2f frame_size;
    // open video stream device
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a5d5f5dacb77bbebdcbfb341e3d4355c1
    cv::VideoCapture camera ( 2 );
	framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    std::ofstream ofs("/dev/fb0");
    // get info of the framebuffer
    // fb_info = ......

    // open the framebuffer device
    // http://www.cplusplus.com/reference/fstream/ofstream/ofstream/
    // ofs = ......

    // check if video stream device is opened success or not
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a9d2ca36789e7fcfe7a7be3b328038585
    // if( !check )
    // {
    //     std::cerr << "Could not open video device." << std::endl;
    //     return 1;
    // }

    // set propety of the frame 
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a8c6d8c2d37505b5ca61ffd4bb54e9a7c
    // https://docs.opencv.org/3.4.7/d4/d15/group__videoio__flags__base.html#gaeb8dd9c89c10a5c63c139bf7c4f5704d
	// set to the acceptable size
	camera.set(CV_CAP_PROP_FPS, 24);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, 600);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, 450);
	
	// https://ithelp.ithome.com.tw/questions/10188261
	cv::Size videoSize = cv::Size((int)camera.get(CV_CAP_PROP_FRAME_WIDTH), (int)camera.get(CV_CAP_PROP_FRAME_HEIGHT));
	cv::VideoWriter writer;
	//https://docs.opencv.org/3.4/dd/d9e/classcv_1_1VideoWriter.html
	writer.open("record.avi", CV_FOURCC('M', 'J', 'P', 'G'), 24, videoSize);
//https://www.796t.com/content/1541838498.html
    struct termios t;
    tcgetattr(0, &t);
    t.c_lflag &= ~ICANON;
	//TCSANOW???  ?????????????????????????????????????????????
    tcsetattr(0, TCSANOW, &t);
    while ( true )
    {
        // get video frame from stream
        // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a473055e77dd7faa4d26d686226b292c1
        // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#a199844fb74226a28b3ce3a39d1ff6765
        // frame = ......
		camera >> frame;
        // get size of the video frame
        // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a146f8e8dda07d1365a575ab83d9828d1
        // frame_size = ......
		frame_size = frame.size();
        // transfer color space from BGR to BGR565 (16-bit image) to fit the requirement of the LCD
        // https://docs.opencv.org/3.4.7/d8/d01/group__imgproc__color__conversions.html#ga397ae87e1288a81d2363b61574eb8cab
        // https://docs.opencv.org/3.4.7/d8/d01/group__imgproc__color__conversions.html#ga4e0972be5de079fed4e3a10e24ef5ef0
	    cv::cvtColor(frame,frame_buf,cv::COLOR_BGR2BGR565);
    	int framebuffer_width = fb_info.xres_virtual;
        // output the video frame to framebufer row by row
        for ( int y = 0; y < frame_size.height; y++ )
        {
            // move to the next written position of output device framebuffer by "std::ostream::seekp()"
            // http://www.cplusplus.com/reference/ostream/ostream/seekp/
			ofs.seekp(y*framebuffer_width*2);
			ofs.write(reinterpret_cast<char*>(frame_buf.ptr(y)),frame_size.width*2);
            // write to the framebuffer by "std::ostream::write()"
            // you could use "cv::Mat::ptr()" to get the pointer of the corresponding row.
            // you also need to cacluate how many bytes required to write to the buffer
            // http://www.cplusplus.com/reference/ostream/ostream/write/
            // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a13acd320291229615ef15f96ff1ff738
        }

		if (!frame.empty()){
                writer.write(frame);
		}
		/*if (cv::pollKey(30)==('c'-'0')) {
			img_offset +=1;
			printf("-----\n");
            sprintf(img_name, "./%d.png", img_offset);
            cv::imwrite(img_name, frame);
		} 
		else if(cv::pollKey(30)==('q'-'0')) {
			break;	
		}
		else{
			continue;		
		} */
		capt = poll(fd, 1, 10);
		if(capt ==1)
            ch = std::cin.get();
            if (ch == 'c'){
				img_offset +=1;
                sprintf(img_name, "%d.png", img_offset);
                cv::imwrite(img_name, frame);
            }
            else if(ch=='q'){
                break;
            }
        

    }

    // closing video stream
    // https://docs.opencv.org/3.4.7/d8/dfe/classcv_1_1VideoCapture.html#afb4ab689e553ba2c8f0fec41b9344ae6
    camera.release ();

    return 0;
}

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;        // Used to return the required attrs.
    struct fb_var_screeninfo screen_info;   // Used to get attributes of the device from OS kernel.

    // open device with linux system call "open()"
    // https://man7.org/linux/man-pages/man2/open.2.html
    
    int fd = open(framebuffer_device_path, O_RDWR);
    // get attributes of the framebuffer device thorugh linux system call "ioctl()".
    // the command you would need is "FBIOGET_VSCREENINFO"
    // https://man7.org/linux/man-pages/man2/ioctl.2.html
    // https://www.kernel.org/doc/Documentation/fb/api.txt
    if (fd>=0) {
		if (!ioctl(fd, FBIOGET_VSCREENINFO, &screen_info)) {
		        fb_info.xres_virtual = screen_info.xres_virtual;
		        fb_info.bits_per_pixel = screen_info.bits_per_pixel;
		}
    }
    // put the required attributes in variable "fb_info" you found with "ioctl() and return it."
    // fb_info.xres_virtual =       // 8
    // fb_info.bits_per_pixel =     // 16

    return fb_info;
};
