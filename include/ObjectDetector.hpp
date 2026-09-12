#pragma once

#include "Detection.hpp"

#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

class ObjectDetector
{
public:
    ObjectDetector(
        const std::string& modelPath,
        const std::string& classesPath,
        float confidenceThreshold = 0.45f,
        float nmsThreshold = 0.45f
    );

    std::vector<Detection> detect(const cv::Mat& frame);

private:
    cv::dnn::Net net;

    std::vector<std::string> classNames;

    float confidenceThreshold;
    float nmsThreshold;

    int inputWidth = 640;
    int inputHeight = 640;

    void loadClassNames(const std::string& path);
};