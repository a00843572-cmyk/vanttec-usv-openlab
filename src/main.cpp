#include "DepthEstimator.hpp"
#include "ObjectDetector.hpp"

#include <opencv2/opencv.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

void drawDetection(
    cv::Mat& frame,
    const Detection& detection
)
{
    cv::rectangle(
        frame,
        detection.box,
        cv::Scalar(0, 255, 0),
        2
    );

    cv::circle(
        frame,
        detection.center,
        4,
        cv::Scalar(0, 0, 255),
        -1
    );

    std::ostringstream label;

    label
        << detection.className
        << " conf="
        << std::fixed
        << std::setprecision(2)
        << detection.confidence
        << " center=("
        << detection.center.x
        << ","
        << detection.center.y
        << ") rel_depth="
        << std::setprecision(2)
        << detection.relativeDepth;

    cv::putText(
        frame,
        label.str(),
        cv::Point(
            detection.box.x,
            std::max(20, detection.box.y - 10)
        ),
        cv::FONT_HERSHEY_SIMPLEX,
        0.45,
        cv::Scalar(0, 255, 0),
        1
    );
}

int main()
{
    try
    {
        ObjectDetector detector(
            "models/yolo.onnx",
            "models/classes.txt",
            0.25f,   // minimum confidence
            0.45f    // NMS threshold
        );

        DepthEstimator depthEstimator;

        cv::Mat frame =
            cv::imread("assets/test.jpg");

        if (frame.empty())
        {
            std::cerr
                << "Error: could not load assets/test.jpg"
                << std::endl;

            return 1;
        }

        auto detections =
            detector.detect(frame);

        for (auto& detection : detections)
        {
            detection.relativeDepth =
                depthEstimator.estimateRelativeDepth(
                    detection.box,
                    frame.size()
                );

            drawDetection(
                frame,
                detection
            );
        }

        cv::imwrite(
            "assets/result.jpg",
            frame
        );

        std::cout
            << "Processing complete."
            << std::endl;

        std::cout
            << "Detections: "
            << detections.size()
            << std::endl;

        std::cout
            << "Result saved to assets/result.jpg"
            << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Error: "
            << error.what()
            << std::endl;

        return 1;
    }

    return 0;
}