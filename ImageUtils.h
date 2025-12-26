#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <QImage>
#include <opencv2/opencv.hpp>
#include <vector>

class ImageUtils {
public:
    // Convert an OpenCV Mat to a Qt QImage suitable for display. We copy
    // the data to ensure the QImage owns its pixels and the source Mat
    // can be modified or freed safely afterwards.
    static QImage matToQImage(const cv::Mat& src) {
        if (src.empty()) return QImage();
        if (src.type() == CV_8UC1) {
            return QImage(src.data, src.cols, src.rows, src.step, QImage::Format_Grayscale8).copy();
        }
        if (src.type() == CV_8UC3) {
            cv::Mat rgb;
            cv::cvtColor(src, rgb, cv::COLOR_BGR2RGB);
            return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
        }
        return QImage();
    }

    // Transforms
    static cv::Mat adjustBrightnessContrast(const cv::Mat& src, double alpha, int beta) {
        cv::Mat dst;
        src.convertTo(dst, -1, alpha, beta);
        return dst;
    }

    static cv::Mat adjustSaturation(const cv::Mat& src, int saturationVal) {
        if (src.type() != CV_8UC3) return src.clone();
        cv::Mat hsv;
        cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);
        cv::add(channels[1], cv::Scalar(saturationVal), channels[1]);
        cv::merge(channels, hsv);
        cv::Mat dst;
        cv::cvtColor(hsv, dst, cv::COLOR_HSV2BGR);
        return dst;
    }

    static cv::Mat rotate90(const cv::Mat& src) {
        cv::Mat dst;
        cv::rotate(src, dst, cv::ROTATE_90_CLOCKWISE);
        return dst;
    }


    static cv::Mat applyBlur(const cv::Mat& src, int kernelSize) {
        if (kernelSize <= 0) return src.clone();
        int k = (kernelSize % 2 == 0) ? kernelSize + 1 : kernelSize;
        cv::Mat dst;
        cv::GaussianBlur(src, dst, cv::Size(k, k), 0);
        return dst;
    }


    static cv::Mat toGrayscale(const cv::Mat& src) {
        if (src.channels() == 1) return src.clone();
        cv::Mat gray, result;
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);
        return result;
    }
    static cv::Mat sharpen(const cv::Mat& src) {
        cv::Mat dst;
        cv::Mat kernel = (cv::Mat_<float>(3,3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
        cv::filter2D(src, dst, -1, kernel);
        return dst;
    }
    
    static cv::Mat crop(const cv::Mat& src, cv::Rect rect) {
        // Ensure rect is within bounds
        rect = rect & cv::Rect(0, 0, src.cols, src.rows);
        return src(rect).clone();
    }
};

#endif //image utilis