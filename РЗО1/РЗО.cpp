#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    Mat image = imread("image.jpg");

    if (image.empty())
    {
        cout << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    imshow("Original Image", image);

    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);
    imshow("Grayscale", grayImage);

    Mat resizedImage;
    resize(image, resizedImage, Size(), 0.5, 0.5);
    imshow("Resized", resizedImage);

    Mat rotatedImage;
    rotate(image, rotatedImage, ROTATE_90_CLOCKWISE);
    imshow("Rotated", rotatedImage);

    Mat blurredImage;
    GaussianBlur(image, blurredImage, Size(15, 15), 0);
    imshow("Gaussian Blur", blurredImage);

    Mat edges;
    Canny(grayImage, edges, 100, 200);
    imshow("Canny Edges", edges);

    imwrite("gray_output.jpg", grayImage);
    imwrite("resized_output.jpg", resizedImage);
    imwrite("rotated_output.jpg", rotatedImage);
    imwrite("blurred_output.jpg", blurredImage);
    imwrite("edges_output.jpg", edges);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
