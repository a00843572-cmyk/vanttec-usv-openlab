#include "DepthEstimator.hpp"
#include "ObjectDetector.hpp"

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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

    int textY = std::max(
        20,
        detection.box.y - 10
    );

    cv::putText(
        frame,
        label.str(),
        cv::Point(
            detection.box.x,
            textY
        ),
        cv::FONT_HERSHEY_SIMPLEX,
        0.45,
        cv::Scalar(0, 255, 0),
        1
    );
}

void processFrame(
    cv::Mat& frame,
    ObjectDetector& detector,
    const DepthEstimator& depthEstimator
)
{
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

        std::cout
            << "Class: "
            << detection.className
            << " | Confidence: "
            << std::fixed
            << std::setprecision(2)
            << detection.confidence
            << " | Center: ("
            << detection.center.x
            << ", "
            << detection.center.y
            << ") | Relative depth: "
            << detection.relativeDepth
            << std::endl;
    }

    std::cout
        << "Detections: "
        << detections.size()
        << std::endl;
}

int main(
    int argc,
    char** argv
)
{
    try
    {
        ObjectDetector detector(
            "models/yolo.onnx",
            "models/classes.txt",
            0.25f,
            0.45f
        );

        DepthEstimator depthEstimator;

        /*
         * IMAGE MODE
         *
         * Example:
         * ./build/usv_vision --image assets/test.jpg
         */
        if (
            argc >= 3 &&
            std::string(argv[1]) == "--image"
        )
        {
            std::string imagePath =
                argv[2];

            cv::Mat frame =
                cv::imread(imagePath);

            if (frame.empty())
            {
                std::cerr
                    << "Error: could not load image: "
                    << imagePath
                    << std::endl;

                return 1;
            }

            processFrame(
                frame,
                detector,
                depthEstimator
            );

            cv::imwrite(
                "assets/result.jpg",
                frame
            );

            std::cout
                << "Result saved to assets/result.jpg"
                << std::endl;

            return 0;
        }

        /*
         * CAMERA MODE
         *
         * Example:
         * ./build/usv_vision --camera 0
         */
        if (
            argc >= 3 &&
            std::string(argv[1]) == "--camera"
        )
        {
            int cameraIndex =
                std::stoi(argv[2]);

            cv::VideoCapture camera(
                cameraIndex
            );

            if (!camera.isOpened())
            {
                std::cerr
                    << "Error: could not open camera "
                    << cameraIndex
                    << std::endl;

                return 1;
            }

            std::cout
                << "Camera opened successfully."
                << std::endl;

            std::cout
                << "Press Q or ESC to exit."
                << std::endl;

            cv::Mat frame;

            while (true)
            {
                camera >> frame;

                if (frame.empty())
                {
                    std::cerr
                        << "Error: empty camera frame."
                        << std::endl;

                    break;
                }

                processFrame(
                    frame,
                    detector,
                    depthEstimator
                );

                cv::imshow(
                    "VantTec USV Vision",
                    frame
                );

                int key =
                    cv::waitKey(1);

                if (
                    key == 27 ||
                    key == 'q' ||
                    key == 'Q'
                )
                {
                    break;
                }
            }

            camera.release();
            cv::destroyAllWindows();

            return 0;
        }

        /*
         * DEFAULT MODE
         *
         * If no arguments are provided,
         * process assets/test.jpg.
         */

        cv::Mat frame =
            cv::imread(
                "assets/test.jpg"
            );

        if (frame.empty())
        {
            std::cerr
                << "Error: could not load assets/test.jpg"
                << std::endl;

            return 1;
        }

        processFrame(
            frame,
            detector,
            depthEstimator
        );

        cv::imwrite(
            "assets/result.jpg",
            frame
        );

        std::cout
            << "Result saved to assets/result.jpg"
            << std::endl;

        std::cout
            << std::endl
            << "Usage:"
            << std::endl
            << "Image:  ./build/usv_vision --image assets/test.jpg"
            << std::endl
            << "Camera: ./build/usv_vision --camera 0"
            << std::endl;

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "Error: "
            << error.what()
            << std::endl;

        return 1;
    }
}