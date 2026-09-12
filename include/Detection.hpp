#pragma once

#include <opencv2/opencv.hpp>
#include <string>

struct Detection
{
    int classId;
    std::string className;
    float confidence;

    cv::Rect box;
    cv::Point center;

    float relativeDepth = -1.0f;
};