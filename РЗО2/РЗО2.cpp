#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat canvas(600, 800, CV_8UC3, Scalar(255, 255, 255));

    line(canvas, Point(0, 300), Point(799, 300), Scalar(0, 0, 0), 3);

    rectangle(canvas, Point(50, 50), Point(250, 200), Scalar(255, 0, 0), 2);

    circle(canvas, Point(400, 450), 100, Scalar(0, 255, 255), -1);

    ellipse(canvas, Point(400, 250), Size(150, 70), 30, 0, 360, Scalar(0, 255, 0), 2);

    vector<Point> pts = { Point(650, 50), Point(750, 100), Point(780, 200),
                         Point(700, 250), Point(600, 150) };
    polylines(canvas, pts, true, Scalar(255, 255, 0), 2);

    String text = "Kotova S.A. Group 22-AM-1";
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 1.0;
    int thickness = 2;
    int baseline = 0;
    Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);

    int x = 10;
    int y = textSize.height + 10;
    int vx = 3;
    int vy = 2;

    namedWindow("Animation", WINDOW_NORMAL);

    while (true) {
        x += vx;
        y += vy;

        if (x < 0) {
            x = 0;
            vx = -vx;
        }
        if (x + textSize.width > canvas.cols) {
            x = canvas.cols - textSize.width;
            vx = -vx;
        }
        if (y < 0) {
            y = 0;
            vy = -vy;
        }
        if (y + textSize.height > canvas.rows) {
            y = canvas.rows - textSize.height;
            vy = -vy;
        }

        Mat frame = canvas.clone();
        putText(frame, text, Point(x, y), fontFace, fontScale, Scalar(0, 0, 0), thickness);
        imshow("Animation", frame);

        if (waitKey(30) == 27) break;
    }

    destroyAllWindows();

    Mat cornerFrame = canvas.clone();
    Point cornerPos(canvas.cols - textSize.width, canvas.rows - textSize.height);
    putText(cornerFrame, text, cornerPos, fontFace, fontScale, Scalar(0, 0, 0), thickness);

    imwrite("output.png", cornerFrame);

    namedWindow("Text", WINDOW_NORMAL);
    imshow("Text", cornerFrame);
    waitKey(0);

    return 0;
}