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
#include "iCub/ObjectpropertyExtractorRatethread.h"
#include <opencv2/core.hpp>


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
                            Value(10),
                            "canny Threshold parameter ").asInt();

}

ObjectpropertyExtractorRatethread::ObjectpropertyExtractorRatethread(string _robot, yarp::os::ResourceFinder &rf)
    : RateThread(
    THRATE) {
  robot = std::move(_robot);
  inputImage = new ImageOf<PixelRgb>;

  cannyThreshold = rf.check("cannyThreshold",
                            Value(10),
                            "canny Threshold parameter ").asInt();
}

ObjectpropertyExtractorRatethread::~ObjectpropertyExtractorRatethread() = default;

bool ObjectpropertyExtractorRatethread::threadInit() {

  yInfo("Initialization of the processing thread correctly ended");

  if (!templateImagePort.open(getName("/imageRGB:i").c_str())) {
    yInfo(" Unable to open port /imageRGB:i ");
    return false;  // unable to open; let RFModule know so that it won't run
  }

  if (!featuresPortOut.open(getName("/features:o").c_str())) {
    yInfo(" Unable to open port /features:o");
    return false;  // unable to open; let RFModule know so that it won't run
  }

  if (!input2DPosition.open(getName("/position2D:i").c_str())) {
    yInfo("Unable to open port /position2D:i");
    return false;  // unable to open; let RFModule know so that it won't run
  }

  if (!anglePositionPort.open(getName("/anglePositionPort").c_str())) {
    yInfo("Unable to open port /anglePositionPort");
    return false;  // unable to open; let RFModule know so that it won't run
  }

  if (!NetworkBase::connect("/iKinGazeCtrl/angles:o", anglePositionPort.getName())) {
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

      inputImageMat = cvarrToMat(inputImage->getIplImage());
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

  const Mat t_inputImage = this->getInputImage();
  Bottle &features = featuresPortOut.prepare();
  features.clear();

  const string color = this->getDominantColor(t_inputImage);

  yInfo("Dominant color found %s", color.c_str());

  const Point2f centerPoint(0,0);
  const vector<cv::Point> contour = this->getContours(t_inputImage)[0];
  const vector<double> worldPointAngle = this->getCoordinateWorldAngles();

  yInfo("POS 2D %f %f",  centerPoint.x, centerPoint.y);

  yInfo("Angles coordinate are : %f, %f, %f", worldPointAngle[0], worldPointAngle[1], worldPointAngle[2]);

  features.addString(color);

  Bottle listPos;
  listPos.addDouble(centerPoint.x);
  listPos.addDouble(centerPoint.y);
  listPos.addDouble( worldPointAngle[0]);
  listPos.addDouble( worldPointAngle[1]);
  listPos.addDouble( worldPointAngle[2]);

  features.addList() = listPos;


  featuresPortOut.write();

}


//********************************************* Core functions *********************************************************

cv::Point3d ObjectpropertyExtractorRatethread::getCoordinateWorld3D(Point2f centerPoint) {

  yarp::os::Bottle cmd, reply;
  cv::Point3d PositionWorld;
  cmd.clear();
  cmd.addString("Root");
  cmd.addInt(static_cast<int>(centerPoint.x));
  cmd.addInt(static_cast<int>(centerPoint.y));

  if (anglePositionPort.getOutputCount()) {
    anglePositionPort.write(cmd, reply);

    PositionWorld.x = reply.get(0).asDouble();
    PositionWorld.y = reply.get(1).asDouble();
    PositionWorld.z = reply.get(2).asDouble();

  }

  return PositionWorld;
}

std::string ObjectpropertyExtractorRatethread::getDominantColor(const Mat t_src) {

  Mat centers = getDominantColorKMeans(t_src, 2);
  auto colorValue = centers.ptr<float>(0);

  if (((int) colorValue[0]) == 0 || ((int) colorValue[1]) == 0 || ((int) colorValue[2]) == 0) {
    cout << "Change centers " << colorValue[0] << " " << colorValue[1] << " " << colorValue[2] << endl;
    colorValue = centers.ptr<float>(1);
  }

  double max, min;
  Point minIndex, maxIndex;
  minMaxLoc(centers, &min, &max, &minIndex, &maxIndex);

  const int red = static_cast<const int>(colorValue[0]);
  const int green = static_cast<const int>(colorValue[1]);
  const int blue = static_cast<const int>(colorValue[2]);

  //cout << red << " " << green << " " << blue << "Max index " << maxIndex.x << endl;


  //Red max value

  if ((red + blue + green) / 3 > 240) {
    return "white";
  } else if (maxIndex.x == 0) {
    if (green > 200) {
      return "yellow";
    } else if (green > 90) {
      return "orange";
    } else {
      return "red";
    }

  }

    //Green max value
  else if (maxIndex.x == 1) {
    return "green";
  }


    //Blue max value
  else if (maxIndex.x == 2) {
    if (red > 140 && blue > 140) {
      return "purple";
    } else if (red > 140 && blue < 140) {
      return "pink";
    } else {
      return "blue";
    }

  }

  return "none";

}

cv::Point2f ObjectpropertyExtractorRatethread::getCenter2DPosition() {

  Bottle *inputBottle2DPosition = input2DPosition.read();
  if (inputBottle2DPosition != nullptr) {
    const int xPositionTopLeft = inputBottle2DPosition->get(0).asInt();
    const int yPositionTopLeft = inputBottle2DPosition->get(1).asInt();

    const int xPositionBottomRight = inputBottle2DPosition->get(2).asInt();
    const int yPositionBottomRight = inputBottle2DPosition->get(3).asInt();

    const int width = xPositionBottomRight - xPositionTopLeft;
    const int height = yPositionBottomRight - yPositionTopLeft;

    return cv::Point2f(xPositionTopLeft + (width / 2), yPositionTopLeft + (height / 2));

  }

  return cv::Point2f(0, 0);
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

  return centers;
}

std::vector<std::vector<cv::Point> > ObjectpropertyExtractorRatethread::getContours(const Mat inputImage) {
  using namespace cv;


  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;

  Mat cannyImage, grayImage;

  cvtColor(inputImageMat, grayImage, CV_RGB2GRAY);


  /// Detect edges using canny
  Canny(grayImage, cannyImage, cannyThreshold, cannyThreshold * 2, 3);

  findContours(cannyImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  /// Draw contours
  Mat drawing = Mat::zeros(cannyImage.size(), CV_8UC3);

  for (int i = 0; i < contours.size(); i++) {
    Scalar color = Scalar(255, 255, 255);
    drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
  }

  /// Show in a window
//  namedWindow("Contours", CV_WINDOW_AUTOSIZE);
//  imshow("Contours", drawing);
//  waitKey(-1);
  return contours;
}


vector<double> ObjectpropertyExtractorRatethread::getCoordinateWorldAngles() {

  vector<double> angles;

  if(anglePositionPort.getInputCount()){
    Bottle anglesBottle;
    anglePositionPort.read(anglesBottle);

    angles.push_back(anglesBottle.get(0).asDouble());
    angles.push_back(anglesBottle.get(1).asDouble());
    angles.push_back(anglesBottle.get(2).asDouble());

  }


  return angles;
}

void ObjectpropertyExtractorRatethread::sendFeatures(iCubObjects t_object) {
    Bottle &features = featuresPortOut.prepare();
    features.clear();
    features = t_object.toBottle();

    this->featuresPortOut.write();


}

void ObjectpropertyExtractorRatethread::testOPC() {
    iCubObjects o = iCubObjects();
    o.setColorLabel("blue");
    o.setPosition(std::vector<double>{1,2,3});

    this->sendFeatures(o);

}

