#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

std::string identifyShape(const std::vector<cv::Point>& approx, double area, double perimeter) {
    int vertices = approx.size();

    double circularity = 4 * CV_PI * area / (perimeter * perimeter);

    if (vertices == 3) return "Triangle";
    else if (vertices == 4) {
        std::vector<double> angles;
        for (int i = 0; i < 4; i++) {
            cv::Point p1 = approx[i];
            cv::Point p2 = approx[(i + 1) % 4];
            cv::Point p3 = approx[(i + 2) % 4];

            cv::Point v1 = p2 - p1;
            cv::Point v2 = p3 - p2;

            double angle = std::abs(std::atan2(v1.y, v1.x) - std::atan2(v2.y, v2.x));
            angle = angle * 180 / CV_PI;
            if (angle > 180) angle = 360 - angle;
            angles.push_back(angle);
        }

        int rightAngleCount = 0;
        for (double angle : angles) {
            if (std::abs(angle - 90) < 15) {
                rightAngleCount++;
            }
        }

        if (rightAngleCount >= 2) {
            cv::Rect rect = cv::boundingRect(approx);
            double aspectRatio = (double)rect.width / rect.height;

            if (std::abs(aspectRatio - 1.0) < 0.15) {
                return "Square";
            }
            else {
                return "Rectangle";
            }
        }
        else {
            return "Rhombus";
        }
    }
    else if (vertices == 5) return "Pentagon";
    else if (vertices == 6) return "Hexagon";
    else if (vertices >= 8) {
        if (circularity > 0.89) {
            return "Circle";
        }
        else {
            return "Oval";
        }
    }
    else {
        return "Shape";
    }
}

int main() {
    setlocale(LC_ALL, "rus");

    cv::Mat img = cv::imread("shapes.jpg");
    if (img.empty()) {
        std::cout << "Не удалось открыть изображение!" << std::endl;
        return -1;
    }

    cv::Mat result = img.clone();

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::cout << "\nРЕЗУЛЬТАТЫ РАСПОЗНАВАНИЯ \n" << std::endl;

    int figureNumber = 0;

    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < 500) continue;

        figureNumber++;

        double perimeter = cv::arcLength(contours[i], true);

        std::vector<cv::Point> approx;
        cv::approxPolyDP(contours[i], approx, 0.02 * perimeter, true);

        std::string shapeName = identifyShape(approx, area, perimeter);

        cv::Moments m = cv::moments(contours[i]);
        cv::Point2f center(m.m10 / m.m00, m.m01 / m.m00);

        cv::drawContours(result, std::vector<std::vector<cv::Point>>{contours[i]}, -1, cv::Scalar(0, 255, 0), 2);
        cv::circle(result, center, 5, cv::Scalar(0, 0, 255), -1);

        std::string label = shapeName + " P: " + std::to_string((int)perimeter);
        cv::putText(result, label, cv::Point(center.x - 30, center.y - 10),
            cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0), 1);

        std::cout << "Фигура " << figureNumber << ":" << std::endl;
        std::cout << "  Название: " << shapeName << std::endl;
        std::cout << "  Периметр: " << (int)perimeter << std::endl;
        std::cout << "  Центр масс: (" << center.x << ", " << center.y << ")" << std::endl;
        std::cout << "  Количество вершин: " << approx.size() << std::endl;
        std::cout << "------------------------" << std::endl;
    }

    cv::imshow("Границы Canny", edges);
    cv::imshow("Распознанные фигуры", result);

    cv::imwrite("result_with_labels.jpg", result);
    std::cout << "\nРезультат сохранен как 'result_with_labels.jpg'" << std::endl;

    cv::waitKey(0);
    return 0;
}