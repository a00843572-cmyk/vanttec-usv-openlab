#include "DepthEstimator.hpp"

#include <cmath>

float DepthEstimator::estimateRelativeDepth(
    const cv::Rect& box,
    const cv::Size& imageSize
) const
{
    float imageArea =
        static_cast<float>(
            imageSize.width * imageSize.height
        );

    float boxArea =
        static_cast<float>(
            box.width * box.height
        );

    if (imageArea <= 0.0f || boxArea <= 0.0f)
    {
        return -1.0f;
    }

    float normalizedArea =
        boxArea / imageArea;

    // Smaller objects in the image receive a larger
    // relative-depth value (interpreted as farther away).
    return 1.0f / std::sqrt(normalizedArea);
}