#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    setlocale(LC_ALL, "rus");
    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyeCascade;
    cv::CascadeClassifier smileCascade;

    if (!faceCascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml")) {
        std::cerr << "Ошибка загрузки каскада лица" << std::endl;
        return -1;
    }
    if (!eyeCascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml")) {
        eyeCascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye.xml");
    }
    if (!smileCascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_smile.xml")) {
        std::cerr << "Ошибка загрузки каскада улыбки" << std::endl;
        return -1;
    }

    cv::VideoCapture cap("ZUA.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Ошибка открытия видеофайла ZUA.mp4" << std::endl;
        return -1;
    }

    cv::Mat frame, gray;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(30, 30));

        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);

            cv::Mat faceROI = gray(face);

            std::vector<cv::Rect> eyes;
            eyeCascade.detectMultiScale(faceROI, eyes, 1.1, 3, 0, cv::Size(10, 10));

            std::vector<cv::Rect> filteredEyes;
            int faceCenterY = face.height / 2;

            for (const auto& eye : eyes) {
                if (eye.y + eye.height < faceCenterY) {
                    if (eye.width < face.width / 2 && eye.height < face.height / 3) {
                        filteredEyes.push_back(eye);
                    }
                }
            }

            if (filteredEyes.size() > 2) {
                int faceCenterX = face.width / 2;
                std::sort(filteredEyes.begin(), filteredEyes.end(),
                    [faceCenterX](const cv::Rect& a, const cv::Rect& b) {
                        int centerA = a.x + a.width / 2;
                        int centerB = b.x + b.width / 2;
                        return std::abs(centerA - faceCenterX) < std::abs(centerB - faceCenterX);
                    });
                filteredEyes.resize(2);
            }

            for (const auto& eye : filteredEyes) {
                cv::Point center(face.x + eye.x + eye.width / 2,
                    face.y + eye.y + eye.height / 2);
                int radius = std::max(eye.width, eye.height) / 2;
                cv::circle(frame, center, radius, cv::Scalar(255, 0, 0), 2);
            }

            std::vector<cv::Rect> smiles;
            smileCascade.detectMultiScale(faceROI, smiles, 1.1, 25, 0, cv::Size(25, 25));

            for (const auto& smile : smiles) {
                if (smile.y + smile.height > face.height / 2) {
                    cv::rectangle(frame,
                        cv::Point(face.x + smile.x, face.y + smile.y),
                        cv::Point(face.x + smile.x + smile.width, face.y + smile.y + smile.height),
                        cv::Scalar(0, 0, 255), 2);
                }
            }
        }

        cv::imshow("Детекция лица, глаз и улыбки", frame);
        if (cv::waitKey(30) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}