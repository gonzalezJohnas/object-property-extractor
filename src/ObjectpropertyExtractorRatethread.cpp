// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
  * Copyright (C)2017  Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
  * Author: Jonas Gonzalez
  * email: 
  * Permission is granted to copy, distribute, and/or modify this program
  * under the terms of the GNU General Public License, version 2 or any
  * later version published by the Free Software Foundation.
  *
  * A copy of the license can be found at
  * http://www.robotcub.org/icub/license/gpl.txt
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
  * Public License for more details
*/

/**
 * @file objectpropertyextractorRatethreadRatethread.cpp
 * @brief Implementation of the eventDriven thread (see objectpropertyextractorRatethreadRatethread.h).
 */

#include <utility>
#include <yarp/os/Network.h>
#include <yarp/cv/Cv.h>
#include "iCub/ObjectpropertyExtractorRatethread.h"


using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;

#define THRATE 100 //ms




//********************interactionEngineRatethread******************************************************

ObjectpropertyExtractorRatethread::ObjectpropertyExtractorRatethread(yarp::os::ResourceFinder &rf)
        : RateThread(THRATE) {
    robot = "icub";
    inputImage = new ImageOf<PixelRgb>;
    cannyThreshold = rf.check("cannyThreshold",
                              Value(80),
                              "canny Threshold parameter ").asInt();
    widthInput = rf.check("widthInput",
                          Value(320),
                          "width of input image (int) ").asInt();


    heightInput = rf.check("hieghtInput",
                           Value(240),
                           "width of input image (int) ").asInt();

}

ObjectpropertyExtractorRatethread::ObjectpropertyExtractorRatethread(string _robot, yarp::os::ResourceFinder &rf)
        : RateThread(
        THRATE) {
    robot = std::move(_robot);
    inputImage = new ImageOf<PixelRgb>;

    cannyThreshold = rf.check("cannyThreshold",
                              Value(80),
                              "canny Threshold parameter ").asInt();
}

ObjectpropertyExtractorRatethread::~ObjectpropertyExtractorRatethread() = default;

bool ObjectpropertyExtractorRatethread::threadInit() {

    yInfo("Initialization of the processing thread correctly ended");

    if (!templateImagePort.open(getName("/imageRGB:i"))) {
        yInfo(" Unable to open port /imageRGB:i ");
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!featuresPortOut.open(getName("/features:o"))) {
        yInfo(" Unable to open port /features:o");
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!input2DPosition.open(getName("/position2D:i"))) {
        yInfo("Unable to open port /position2D:i");
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!anglePositionPort.open(getName("/anglePositionPort"))) {
        yInfo("Unable to open port /anglePositionPort");
        return false;  // unable to open; let RFModule know so that it won't run
    }


    if (!RpcIkinGazeCtrl.open(getName("/iKinGazeCtrl/rpc:o"))) {
        yInfo("Unable to open port /iKinGazeCtrl/rpc:o");
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!NetworkBase::connect("/iKinGazeCtrl/angles:o", anglePositionPort.getName())) {
        yInfo("Unable to connect to iKinGazeCtrl/angles:o check that IkInGazeCtrl is running");

    }

    if (!NetworkBase::connect(RpcIkinGazeCtrl.getName(), "/ikinGazeCtrl/rpc")) {
        yInfo("Unable to connect to iKinGazeCtrl/angles:o check that IkInGazeCtrl is running");

    }


    return true;
}

void ObjectpropertyExtractorRatethread::setName(string str) {
    this->name = std::move(str);
}

std::string ObjectpropertyExtractorRatethread::getName(const char *p) {
    string str(name);
    str.append(p);
    return str;
}

void ObjectpropertyExtractorRatethread::setInputPortName(string InpPort) {

}

void ObjectpropertyExtractorRatethread::run() {


    inputImage = templateImagePort.read(false);

    if (inputImage != nullptr) {

        inputImageMat = yarp::cv::toCvMat(*inputImage);
//      cvtColor(inputImageMat, inputImageMat, CV_RGB2BGR);

    }


}

void ObjectpropertyExtractorRatethread::threadRelease() {
    yInfo("Releasing thread");

    this->input2DPosition.close();
    this->anglePositionPort.close();
    this->featuresPortOut.close();
    this->templateImagePort.close();

}

void ObjectpropertyExtractorRatethread::interruptThread() {

    yInfo("Inteprupting thread");

    this->input2DPosition.interrupt();
    this->featuresPortOut.interrupt();
    this->templateImagePort.interrupt();
    this->anglePositionPort.interrupt();
}

void ObjectpropertyExtractorRatethread::extractFeatures() {


    iCubObject receivedObject;
    const Mat t_inputImage = inputImageMat;
    Bottle &features = featuresPortOut.prepare();
    features.clear();


    iCub::Point2d topLeft, bottomRight;
    getRectanglePoints(t_inputImage, topLeft, bottomRight);
    const Color color = this->getDominantColor(t_inputImage);


    const std::string contour_name = this->getContours(t_inputImage);
    const vector<double> anglesPosition = this->getCoordinateWorldAngles();
    const vector<double> cartesianPosition = this->getCoordinateWorld3D(topLeft, bottomRight);


    receivedObject.set_color(color);
    receivedObject.setAnglePosition(anglesPosition);
    receivedObject.setCartesianPosition(cartesianPosition);
    receivedObject.set_rectangleTopLeft(topLeft);
    receivedObject.set_rectangleBottomRight(bottomRight);


    features = receivedObject.toBottle();


    featuresPortOut.write();

}


//********************************************* Core functions *********************************************************

std::vector<double> ObjectpropertyExtractorRatethread::getCoordinateWorld3D(iCub::Point2d &topLeft, iCub::Point2d &bottomRight) {

    std::vector<double> cartesianPosition;
    int centerOfMassX = bottomRight.x / 2;
    int centerOfMassY = bottomRight.y / 2;
    Bottle cmd, reply;

    cmd.addVocab(COMMAND_VOCAB_GET);
    cmd.addVocab(COMMAND_VOCAB_3D);
    cmd.addVocab(COMMAND_VOCAB_MONO);

    Bottle sub_cmd;
    sub_cmd.addString("left");
    sub_cmd.addInt(centerOfMassX);
    sub_cmd.addInt(centerOfMassY);
    sub_cmd.addInt(1);

    cmd.addList() = sub_cmd;

    RpcIkinGazeCtrl.write(cmd,reply);

    if(cmd.get(0).asVocab() == COMMAND_VOCAB_ACK){
        Bottle *positionBottle = cmd.get(1).asList();
        cartesianPosition.push_back(positionBottle->get(0).asDouble());
        cartesianPosition.push_back(positionBottle->get(1).asDouble());
        cartesianPosition.push_back(positionBottle->get(2).asDouble());
    }

    else{
        yError("unable to compute 3D position with 2D point %d %d", centerOfMassX, centerOfMassY);
        cartesianPosition.push_back(-1);
        cartesianPosition.push_back(-1);
        cartesianPosition.push_back(-1);
    }

}

Color ObjectpropertyExtractorRatethread::getDominantColor(const Mat t_src) {

    const Mat center = getDominantColorKMeans(t_src, 3);


    auto colorValue = center.ptr<float>();

    const Mat color(1, 3, CV_8UC3, cvScalar(colorValue[0], colorValue[1], colorValue[2]));
    const double dist_black = norm(blackColor, color, NORM_L2);


    Mat hsv_value;
    cvtColor(color, hsv_value, COLOR_BGR2HSV_FULL);
    const int hue = hsv_value.at<Vec3b>(0, 0).val[0];
    const int sat = hsv_value.at<Vec3b>(0, 0).val[1];
    const int value = hsv_value.at<Vec3b>(0, 0).val[2];

    if (sat < 30 && value > 160) {
        Color white("white", 255, 255, 255);
        return white;
    }

    for (const auto &it : colorMap) {
        if (hue < it.first) {
            return it.second;

        }
    }

    Color default_color("unknown", 0, 0, 0);
    return default_color;

}


void ObjectpropertyExtractorRatethread::getRectanglePoints(const Mat inputImage, iCub::Point2d &topLeft,
                                                           iCub::Point2d &bottomRight) {

    Mat gray_image;
    cvtColor(inputImage, gray_image, COLOR_RGB2GRAY);
    topLeft.x = -1;
    bottomRight.x = -1;


    for (int i = 0; i < inputImage.rows; ++i) {
        const int reverse_index_i = inputImage.rows - i - 1;

        for (int j = 0; j < inputImage.cols; ++j) {
            if ((int) gray_image.at<unsigned char>(i, j) > 0 && topLeft.x == -1) {
                topLeft.y = i;
                topLeft.x = j;
            }

            const int reverse_index_j = inputImage.cols - j - 1;
            if ((int) gray_image.at<unsigned char>(reverse_index_i, reverse_index_j) > 0 && bottomRight.x == -1) {
                bottomRight.y = reverse_index_i;
                bottomRight.x = reverse_index_j;
            }
        }
    }


}

cv::Mat ObjectpropertyExtractorRatethread::getDominantColorKMeans(const Mat inputImage, const int numberOfClusters) {

    // Parameters for Kmeans
    const int numberOfAttempts = 20;
    const int numberOfSteps = 1000;
    const float stopCriterion = 0.0001;
    Mat centers, labels, samples;
    TermCriteria kmeansCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, numberOfSteps, stopCriterion);

    // Process the inputImage to match the requirement of Kmeans input
    inputImage.convertTo(samples, CV_32F);
    samples = samples.reshape(3, inputImage.rows * inputImage.cols);

    kmeans(samples, numberOfClusters, labels, kmeansCriteria, numberOfAttempts, KMEANS_PP_CENTERS, centers);

    int max_label = -1;
    for (int i = 0; i < numberOfClusters; ++i) {


        auto colorValue = centers.ptr<float>(i);

        const Mat color(1, 3, CV_8UC3, cvScalar(colorValue[0], colorValue[1], colorValue[2]));
        const double dist_black = norm(blackColor, color, NORM_L2);

        if (dist_black > 50) {

            const long nb_occurence = count(labels.begin<int>(), labels.end<int>(), i);
            if (nb_occurence > max_label) {
                max_label = i;
            }
        }
    }


    return centers.row(max_label);
}

std::string ObjectpropertyExtractorRatethread::getContours(const Mat inputImage) {


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    Mat cannyImage, grayImage, approximated_poly;

    cvtColor(inputImage, grayImage, CV_BGR2GRAY);


    /// Detect edges using canny
    Canny(grayImage, cannyImage, cannyThreshold, cannyThreshold * 2, 3);


    findContours(cannyImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Draw contours
    Mat drawing = Mat::zeros(cannyImage.size(), CV_8UC3);


    for (int i = 0; i < contours.size(); ++i) {

        Scalar color = Scalar(255, 255, 255);
        drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
        approxPolyDP(contours[i], approximated_poly, arcLength(Mat(contours[i]), true) * 0.05, true);


        if (approximated_poly.rows == 3) {
            return "triangle";
        } else if (approximated_poly.rows == 4 &&
                   boundingRect(approximated_poly).width == boundingRect(approximated_poly).height) {
            return "square";
        } else {
            return "circle ";

        }
    }


    return "unknown";
}


vector<double> ObjectpropertyExtractorRatethread::getCoordinateWorldAngles() {

    vector<double> angles;

    if (anglePositionPort.getInputCount()) {
        Bottle *anglesBottle = anglePositionPort.read();

        angles.push_back(anglesBottle->get(0).asDouble());
        angles.push_back(anglesBottle->get(1).asDouble());
        angles.push_back(anglesBottle->get(2).asDouble());

    }


    return angles;
}

void ObjectpropertyExtractorRatethread::sendFeatures(iCubObject t_object) {
    Bottle &features = featuresPortOut.prepare();
    features.clear();
    features = t_object.toBottle();

    this->featuresPortOut.write();


}

void ObjectpropertyExtractorRatethread::testOPC() {
    iCubObject o = iCubObject();
    o.set_color(blue);
    o.setAnglePosition(std::vector<double>{1, 2, 3});
    o.setCartesianPosition(std::vector<double>{1, 2, 3});
    o.set_rectangleTopLeft(iCub::Point2d(100, 100));
    o.set_rectangleBottomRight(iCub::Point2d(200, 200));
    this->sendFeatures(o);

}

std::string ObjectpropertyExtractorRatethread::testColor(cv::Mat img) {
    getContours(img);
    return this->getDominantColor(std::move(img)).name;
}

