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

#include <iCub/objectpropertyextractorRatethread.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;

#define THRATE 100 //ms

//********************interactionEngineRatethread******************************************************

objectpropertyextractorRatethread::objectpropertyextractorRatethread() : RateThread(THRATE) {
    robot = "icub";
    inputImage          = new ImageOf<PixelRgb>;

}

objectpropertyextractorRatethread::objectpropertyextractorRatethread(string _robot, string _configFile) : RateThread(
        THRATE) {
    robot = _robot;
    configFile = _configFile;
    inputImage          = new ImageOf<PixelRgb>;

}

objectpropertyextractorRatethread::~objectpropertyextractorRatethread() {
    // do nothing
}

bool objectpropertyextractorRatethread::threadInit() {


    yInfo("Initialization of the processing thread correctly ended");

    if (!imagePortIn.open(getName("/imageRGB:i").c_str())) {
        cout <<": unable to open port /imageRGB:i "  << endl;
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!featuresPortOut.open(getName("/features:o").c_str())) {
        cout <<": unable to open port /features:o"  << endl;
        return false;  // unable to open; let RFModule know so that it won't run
    }

    return true;
}

void objectpropertyextractorRatethread::setName(string str) {
    this->name = str;
}


std::string objectpropertyextractorRatethread::getName(const char *p) {
    string str(name);
    str.append(p);
    return str;
}

void objectpropertyextractorRatethread::setInputPortName(string InpPort) {

}

void objectpropertyextractorRatethread::run() {
    inputImage  = imagePortIn.read(true);

    if(inputImage != NULL){

        inputImageMat = cvarrToMat(inputImage->getIplImage());
        cvtColor(inputImageMat, inputImageMat, CV_RGB2BGR);
    }
}



void objectpropertyextractorRatethread::threadRelease() {
    yDebug("Release thread");

    this->featuresPortOut.interrupt();
    this->imagePortIn.interrupt();

    this->featuresPortOut.close();
    this->imagePortIn.close();
}



//*********************************************************************************************************************
//Core functions
cv::Point3d objectpropertyextractorRatethread::getCoordinateWorld(Point2f centerPoint) {
    return cv::Point3d(0, 0, 0);
}

std::string objectpropertyextractorRatethread::getDominantColor(Mat t_src) {


    Scalar avgValue = mean(t_src);

    double max, min;
    Point minIndex, maxIndex;
    minMaxLoc(avgValue, &min, &max, &minIndex, &maxIndex);

    const double red = avgValue[0, 2];
    const double green = avgValue[0, 1];
    const double blue = avgValue[0, 0];

    //Blue max value
    if (maxIndex.y == 0) {
        if (red > 150 && blue > 150) {
            return "purple";
        }
        else if (red > 150 && blue < 150) {
            return "pink";
        }
        else{
            return "blue";
        }
    }

    //Green max value
    else if (maxIndex.y == 1) {
           return "green";
        }


    //Red max value
    else if (maxIndex.y == 2){
        if(green > 200 ){
            return "yellow";
        }

        else if (green > 90) {
            return "orange";
        }

        else {
            return "red";
        }

    }

    return "unknown color";

}

cv::Point2f objectpropertyextractorRatethread::getCenter2DPosition(Mat t_src) {
    return cv::Point2f(t_src.cols/2, t_src.rows/2);
}

const int objectpropertyextractorRatethread::getPixelSize(Mat t_src) {


    return (int) round(sqrt(pow(t_src.cols,2) + pow(t_src.rows,2)));
}

void objectpropertyextractorRatethread::extractFeatures(Mat t_inputImage) {
    Bottle &features = featuresPortOut.prepare();
    features.clear();


    const string color = "( color "+this->getDominantColor(t_inputImage)+" )";
    const Point2f centerPoint = this->getCenter2DPosition(t_inputImage);
    const string pos2D = "(2D-pos "+std::to_string(centerPoint.x)+" "+std::to_string(centerPoint.y)+")";
    const string size = "( size "+ to_string(this->getPixelSize(t_inputImage)) +")";

    const Point3f worldPoint = this->getCoordinateWorld(centerPoint);
    const string pos3D = "(3D-pos "+std::to_string(worldPoint.x)+" "+std::to_string(worldPoint.y) +" "+std::to_string(worldPoint.z)+" )";

    features.addString(color);
    features.addString(pos2D);
    features.addString(size);
    features.addString(pos3D);


    featuresPortOut.write();

}


