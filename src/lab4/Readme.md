# Lab4

**Request:**

1. Show the content of camera on the HDMI Screen
2. Implement a scroll picture on the HDMI Screen

## Sol for request 1

1. Change the framebuffer interface by setting in uboot

2. run your lab3 again ouob

## Sol for request 2

1. Concat your picture together by cv::hconcat

    ``` CPP
        cv::hconcat(pic1, pic2, dst);
        cv::hconcat(dst, pic3, dst);
        cv::cvtColor(dst, dst, cv::COLOR_BGR2BGR565);
    ```

2. add a offset to when your finding the starter of your output

    ``` CPP
        ofs.write(reinterpret_cast<char *>(dst.ptr(y, cnt)), fb_info.xres_virtual * 2);

    ```

## How to compile

> Change fuck.cpp and fuck to your file name and target file name

``` bash
    arm-linux-gnueabihf-g++ fuck.cpp -o fuck -I /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/include/ -I /usr/local/arm-opencv/install/include/ -L /usr/local/arm-opencv/install/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/qt5.5_env/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/usr/lib/ -lpthread -lopencv_world
```