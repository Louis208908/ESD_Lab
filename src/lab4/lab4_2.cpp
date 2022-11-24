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

struct framebuffer_info
{
    uint32_t bits_per_pixel; // depth of framebuffer
    uint32_t xres_virtual;   // how many pixel in a row in virtual screen
    uint32_t yres_virtual;   // how many pixel in a column in virtual screen
};

struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path);

int main(int argc, const char *argv[])
{
    // variable to store the frame get from video stream
    cv::Mat pic1, pic2, pic3, dst;
    cv::Size2f frame_size;
    int ratio1 = atoi(argv[1]);
    int ratio2 = atoi(argv[2]);
    // get info of the framebuffer
    // fb_info = ......
    framebuffer_info fb_info = get_framebuffer_info("/dev/fb0");
    // open the framebuffer device
    // http://www.cplusplus.com/reference/fstream/ofstream/ofstream/
    // ofs = ......
    std::ofstream ofs("/dev/fb0");

    // get frame size from frame buffer
    // frame_size = ......
    frame_size = cv::Size2f(fb_info.xres_virtual, fb_info.yres_virtual);

    // open image from path
    // https://docs.opencv.org/3.4.7/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
    pic1 = cv::imread("1.jpg", 1);
    pic2 = cv::imread("2.jpg", 1);
    pic3 = cv::imread("3.jpg", 1);

    // resize the image to fit the screen
    // https://docs.opencv.org/3.4.7/da/d54/group__imgproc__transform.html#ga47a974309e9102f5f08231edc7e7529d
    // pic1 = ......
    // pic2 = ......
    // pic3 = ......
    cv::resize(pic1, pic1, frame_size);
    cv::resize(pic2, pic2, frame_size);
    cv::resize(pic3, pic3, frame_size);

    // concat three images horizontally
    //  https://docs.opencv.org/3.4.7/d2/de8/group__core__array.html#ga0b430e5beb1e900466aec780a1d3d041
    //  dst = ......
    cv::hconcat(pic1, pic2, dst);
    cv::hconcat(dst, pic3, dst);
    cv::cvtColor(dst, dst, cv::COLOR_BGR2BGR565);
    int cnt = 0;
    while (cnt < 10000)
    {
        // output the video frame to framebufer row by row
        for (int y = 0; y < dst.size().height; y++)
        {
            // move to the next written position of output device framebuffer by "std::ostream::seekp()"
            // http://www.cplusplus.com/reference/ostream/ostream/seekp/
            ofs.seekp(y * fb_info.xres_virtual * 2);
            // write to the framebuffer by "std::ostream::write()"
            // you could use "cv::Mat::ptr()" to get the pointer of the corresponding row.
            // you also need to cacluate how many bytes required to write to the buffer
            // http://www.cplusplus.com/reference/ostream/ostream/write/
            // https://docs.opencv.org/3.4.7/d3/d63/classcv_1_1Mat.html#a13acd320291229615ef15f96ff1ff738
            ofs.write(reinterpret_cast<char *>(dst.ptr(y, cnt)), fb_info.xres_virtual * 2);
        }
        cnt += 50;
    }
    return 0;
}
struct framebuffer_info get_framebuffer_info(const char *framebuffer_device_path)
{
    struct framebuffer_info fb_info;      // Used to return the required attrs.
    struct fb_var_screeninfo screen_info; // Used to get attributes of the device from OS kernel.

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
    fb_info.yres_virtual = screen_info.yres_virtual;
    fb_info.bits_per_pixel = screen_info.bits_per_pixel;
    return fb_info;
};