#include "ObjectDetector.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

ObjectDetector::ObjectDetector(
    const std::string& modelPath,
    const std::string& classesPath,
    float confidenceThreshold,
    float nmsThreshold
)
    : confidenceThreshold(confidenceThreshold),
      nmsThreshold(nmsThreshold)
{
    net = cv::dnn::readNetFromONNX(modelPath);

    if (net.empty())
    {
        throw std::runtime_error(
            "Could not load detection model: " + modelPath
        );
    }

    loadClassNames(classesPath);

    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

void ObjectDetector::loadClassNames(
    const std::string& path
)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Could not open classes file: " + path
        );
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            classNames.push_back(line);
        }
    }
}

std::vector<Detection> ObjectDetector::detect(
    const cv::Mat& frame
)
{
    if (frame.empty())
    {
        throw std::runtime_error(
            "Empty frame received."
        );
    }

    cv::Mat blob = cv::dnn::blobFromImage(
        frame,
        1.0 / 255.0,
        cv::Size(inputWidth, inputHeight),
        cv::Scalar(),
        true,
        false
    );

    net.setInput(blob);

    std::vector<cv::Mat> outputs;

    net.forward(
        outputs,
        net.getUnconnectedOutLayersNames()
    );

    if (outputs.empty())
    {
        throw std::runtime_error(
            "YOLO returned no output."
        );
    }

    cv::Mat output = outputs[0];

    std::cout
        << "YOLO output dims: "
        << output.dims
        << std::endl;

    for (int i = 0; i < output.dims; ++i)
    {
        std::cout
            << "dim[" << i << "] = "
            << output.size[i]
            << std::endl;
    }

    if (output.dims != 3)
    {
        throw std::runtime_error(
            "Unexpected YOLOv5 output dimensions."
        );
    }

    int rows = output.size[1];
    int dimensions = output.size[2];

    cv::Mat detectionsMat(
        rows,
        dimensions,
        CV_32F,
        output.ptr<float>()
    );

    float xFactor =
        static_cast<float>(frame.cols)
        / inputWidth;

    float yFactor =
        static_cast<float>(frame.rows)
        / inputHeight;

    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;

    float maxObjectnessFound = 0.0f;
    float maxConfidenceFound = 0.0f;

    for (int i = 0; i < rows; ++i)
    {
        float* data =
            detectionsMat.ptr<float>(i);

        float objectness =
            data[4];

        if (objectness > maxObjectnessFound)
        {
            maxObjectnessFound =
                objectness;
        }

        cv::Mat scores(
            1,
            dimensions - 5,
            CV_32FC1,
            data + 5
        );

        cv::Point classIdPoint;
        double maxClassScore;

        cv::minMaxLoc(
            scores,
            nullptr,
            &maxClassScore,
            nullptr,
            &classIdPoint
        );

        float confidence =
            objectness *
            static_cast<float>(
                maxClassScore
            );

        if (confidence > maxConfidenceFound)
        {
            maxConfidenceFound =
                confidence;
        }

        if (objectness < confidenceThreshold)
        {
            continue;
        }

        if (confidence < confidenceThreshold)
        {
            continue;
        }

        float cx = data[0];
        float cy = data[1];
        float width = data[2];
        float height = data[3];

        int left =
            static_cast<int>(
                (cx - width / 2.0f)
                * xFactor
            );

        int top =
            static_cast<int>(
                (cy - height / 2.0f)
                * yFactor
            );

        int boxWidth =
            static_cast<int>(
                width * xFactor
            );

        int boxHeight =
            static_cast<int>(
                height * yFactor
            );

        boxes.emplace_back(
            left,
            top,
            boxWidth,
            boxHeight
        );

        confidences.push_back(
            confidence
        );

        classIds.push_back(
            classIdPoint.x
        );
    }

    std::cout
        << "Max objectness: "
        << maxObjectnessFound
        << std::endl;

    std::cout
        << "Max final confidence: "
        << maxConfidenceFound
        << std::endl;

    std::vector<int> indices;

    cv::dnn::NMSBoxes(
        boxes,
        confidences,
        confidenceThreshold,
        nmsThreshold,
        indices
    );

    std::vector<Detection> results;

    for (int index : indices)
    {
        cv::Rect box =
            boxes[index]
            &
            cv::Rect(
                0,
                0,
                frame.cols,
                frame.rows
            );

        if (
            box.width <= 0 ||
            box.height <= 0
        )
        {
            continue;
        }

        Detection detection;

        detection.classId =
            classIds[index];

        if (
            detection.classId >= 0 &&
            detection.classId <
            static_cast<int>(
                classNames.size()
            )
        )
        {
            detection.className =
                classNames[
                    detection.classId
                ];
        }
        else
        {
            detection.className =
                "unknown";
        }

        detection.confidence =
            confidences[index];

        detection.box =
            box;

        detection.center =
            cv::Point(
                box.x +
                    box.width / 2,
                box.y +
                    box.height / 2
            );

        results.push_back(
            detection
        );
    }

    return results;
}