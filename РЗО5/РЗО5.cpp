#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

string detectShape(const vector<Point>& contour) {
    double perimeter = arcLength(contour, true);

    vector<Point> approx;
    approxPolyDP(contour, approx, 0.02 * perimeter, true);

    int v = approx.size();

    Rect r = boundingRect(contour);
    double ar = (double)r.width / r.height;

    double area = contourArea(contour);
    double rectArea = r.width * r.height;
    double fill = area / rectArea;

    if (v == 4) {
        if (ar > 0.9 && ar < 1.1 && fill > 0.75)
            return "Square";
    }

    if (v == 3 || v == 4) {
        if (fill < 0.6)
            return "Triangle";
    }

    if (v > 7) {
        double perim = arcLength(contour, true);
        double circularity = 4 * 3.1415 * area / (perim * perim);

        if (circularity > 0.85)
            return "Circle";
    }

    return "Unknown";
}

int main() {
    VideoCapture cap("video.mkv");

    double fps = cap.get(CAP_PROP_FPS);
    int width = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);

    VideoWriter writer("output_video.avi",
        VideoWriter::fourcc('M', 'J', 'P', 'G'),
        fps,
        Size(width, height));

    if (!writer.isOpened()) {
        cout << "Ошибка создания видео для записи\n";
        return -1;
    }

    Mat frame, gray, edges, contoursDraw;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        GaussianBlur(gray, gray, Size(5, 5), 0);
        Canny(gray, edges, 30, 100);
        dilate(edges, edges, Mat(), Point(-1, -1), 2);

        vector<vector<Point>> contours;
        findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        contoursDraw = Mat::zeros(frame.size(), CV_8UC3);
        drawContours(contoursDraw, contours, -1, Scalar(0, 255, 255), 2);

        for (auto& contour : contours) {
            double area = contourArea(contour);

            if (area < 500) continue;

            string shape = detectShape(contour);
            if (shape == "Unknown") continue;

            Rect box = boundingRect(contour);
            rectangle(frame, box, Scalar(0, 255, 0), 2);

            Point center(box.x + box.width / 2,
                box.y + box.height / 2);

            putText(frame, shape,
                Point(center.x - 40, center.y),
                FONT_HERSHEY_SIMPLEX, 0.6,
                Scalar(0, 0, 255), 2);
        }

        writer.write(frame);

        imshow("Result", frame);
        imshow("Contours after Canny", contoursDraw);

        if (waitKey(30) == 27) break;
    }

    cap.release();
    writer.release();
    destroyAllWindows();

    return 0;
}