#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    Mat img = imread("kartinka.png");
    if (img.empty())
    {
        cout << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    imshow("1. Original (BGR)", img);

    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    imshow("2. HSV", hsv);
    imwrite("HSV.png", hsv);
    vector<Mat> hsvChannels;
    split(hsv, hsvChannels);
    imshow("3. H channel", hsvChannels[0]);

    Mat mask;
    inRange(hsv, Scalar(20, 100, 100), Scalar(35, 255, 255), mask);
    imshow("4. Yellow mask", mask);
    imwrite("yellow_mask.png", mask);

    Mat result;
    img.copyTo(result, mask);
    imshow("5. Yellow objects", result);
    imwrite("yellow_objects.png", result);

    waitKey(0);
    return 0;
}