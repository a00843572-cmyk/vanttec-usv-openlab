#pragma once

#include <opencv2/opencv.hpp>

class DepthEstimator
{
public:
    float estimateRelativeDepth(
        const cv::Rect& box,
        const cv::Size& imageSize
    ) const;
};