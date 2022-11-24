# Lab3

**Request:**

1. Show the content of camera capture on the Screen by I2C
2. Save a picture of camera capture after pressing c without pressing enter
3. Save the content of camera as a video after booting

## Sol for request 1

1. connect to camera by camera capture api in openCV

    ``` CPP
        cv::VideoCapture camera ( 2 );
        camera.read(frame);
    ```

2. output to framebuffer, just like what you did in lab2

## Sol for request 2

1. control your terminal by **termios** api

    ``` CPP
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
    ```

2. save picture by openCV api

    ``` CPP
        char filename[100];
        sprintf(filename, "image_%d.png", count);
        // save image
        cv::imwrite(filename, frame);

    ```

## Sol for request 3

1. Create a shell script for dynamic linking

    ```shell
    # !/bin/bash
    LD_LIBRARY_PATH=/root/lab3/ /root/lab3/lab3-2 5
    ```

2. Using systemd to create service after booting

    ``` systemd
    [Unit]

    Description=fuck

    [Service]
    ExecStart=/root/lab3/start_capture.sh
    Type=oneshot

    [Install]
    WantedBy=multi-user.target

    ```

3. Using openCV api to store video

    ``` CPP
    cv::VideoWriter video("/root/lab3/out.avi",CV_FOURCC('M','J','P','G'),10,size);

    video.write(frame);

    ```


## How to compile

> Change fuck.cpp and fuck to your file name and target file name

``` bash
    arm-linux-gnueabihf-g++ fuck.cpp -o fuck -I /opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/include/ -I /usr/local/arm-opencv/install/include/ -L /usr/local/arm-opencv/install/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/qt5.5_env/lib/ -Wl,-rpath-link=/opt/EmbedSky/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/qt5.5/rootfs_imx6q_V3_qt5.5_env/usr/lib/ -lpthread -lopencv_world
```
