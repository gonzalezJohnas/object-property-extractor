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

#include "iCub/ObjectpropertyExtractorRatethread.h"

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace std;
using namespace cv;

#define THRATE 100 //ms

//********************interactionEngineRatethread******************************************************

ObjectpropertyExtractorRatethread::ObjectpropertyExtractorRatethread(yarp::os::ResourceFinder &rf) : RateThread(THRATE) {
    robot = "icub";
    inputImage = new ImageOf<PixelRgb>;
    cannyThreshold = 10;

}

ObjectpropertyExtractorRatethread::ObjectpropertyExtractorRatethread(string _robot, yarp::os::ResourceFinder &rf) : RateThread(
        THRATE) {
    robot = std::move(_robot);
    inputImage = new ImageOf<PixelRgb>;

    cannyThreshold = 10;

}

ObjectpropertyExtractorRatethread::~ObjectpropertyExtractorRatethread() = default;

bool ObjectpropertyExtractorRatethread::threadInit() {


    yInfo("Initialization of the processing thread correctly ended");

    if (!templateImagePort.open(getName("/imageRGB:i").c_str())) {
        cout <<": unable to open port /imageRGB:i "  << endl;
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!featuresPortOut.open(getName("/features:o").c_str())) {
        cout <<": unable to open port /features:o"  << endl;
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!input2DPosition.open(getName("/position2D:i").c_str())) {
        cout <<": unable to open port /position2D:i"  << endl;
        return false;  // unable to open; let RFModule know so that it won't run
    }

    if (!get3DPosition.open(getName("/getPosition3D").c_str())) {
        cout <<": unable to open port /getPosition3D"  << endl;
        return false;  // unable to open; let RFModule know so that it won't run
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
    inputImage  = templateImagePort.read();


    if(inputImage != nullptr){

        inputImageMat = cvarrToMat(inputImage->getIplImage());
//      cvtColor(inputImageMat, inputImageMat, CV_RGB2BGR);

    }
    
    


}



void ObjectpropertyExtractorRatethread::threadRelease() {
    yInfo("Releasing thread");

    this->featuresPortOut.interrupt();
    this->templateImagePort.interrupt();

    this->featuresPortOut.close();
    this->templateImagePort.close();
}


void ObjectpropertyExtractorRatethread::extractFeatures() {

    const Mat t_inputImage = this ->getInputImage();
    Bottle &features = featuresPortOut.prepare();
    features.clear();


    const string color = "( color "+this->getDominantColor(t_inputImage)+" )";

    cout << "Color " <<  color << endl;

    const Point2f centerPoint = this->getCenter2DPosition();
    const string pos2D = "(2D-pos "+std::to_string(centerPoint.x)+" "+std::to_string(centerPoint.y)+")";
    const vector<cv::Point>  contour = this->getContours()[0];

    cout << "POS 2D " << centerPoint.x << " " << centerPoint.y << endl;

    const Point3f worldPoint = this->getCoordinateWorld(centerPoint);
    const string pos3D = "(3D-pos "+std::to_string(worldPoint.x)+" "+std::to_string(worldPoint.y) +" "+std::to_string(worldPoint.z)+" )";

    cout << " 3D position" << pos3D <<  std::endl;

    features.addString(color);
    features.addString(pos2D);
    //features.addString(size);
    features.addString(pos3D);


    featuresPortOut.write();

}


//*********************************************************************************************************************
//Core functions
cv::Point3d ObjectpropertyExtractorRatethread::getCoordinateWorld(Point2f centerPoint) {



    yarp::os::Bottle cmd, reply;
    cv::Point3d PositionWorld;
    cmd.clear();
    cmd.addString("Root");
    cmd.addInt(static_cast<int>(centerPoint.x));
    cmd.addInt(static_cast<int>(centerPoint.y));

    if(get3DPosition.getOutputCount()){
        get3DPosition.write(cmd, reply);

        PositionWorld.x = reply.get(0).asDouble();
        PositionWorld.y = reply.get(1).asDouble();
        PositionWorld.z = reply.get(2).asDouble();

    }

    return PositionWorld;
}






std::string ObjectpropertyExtractorRatethread::getDominantColor(const Mat t_src) {


    Mat centers = getDominantColorKMeans(t_src, 2);
    auto colorValue = centers.ptr<float>(0);

    
    if( ((int)colorValue[0]) == 0 || ((int)colorValue[1]) == 0 || ((int)colorValue[2]) == 0  ){
        cout << "Change centers " << colorValue[0] << " " << colorValue[1] << " " << colorValue[2] << endl;
        colorValue = centers.ptr<float>(1);
    }

    double max, min;
    Point minIndex, maxIndex;
    minMaxLoc(centers, &min, &max, &minIndex, &maxIndex);

    const int red = colorValue[0];
    const int green = colorValue[1];
    const int blue = colorValue[2];

    //cout << red << " " << green << " " << blue << "Max index " << maxIndex.x << endl;


     //Blue max value
    if (maxIndex.x == 0) {
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

    //Green max value
    else if (maxIndex.x == 1) {
           return "green";
        }


    //Red max value
    else if (maxIndex.x == 2){
        if (red > 140 && blue > 150) {
            return "purple";
        }
        else if (red > 150 && blue < 150) {
            return "pink";
        }
        else{
            return "blue";
        }

    }


    return "white";

}

cv::Point2f ObjectpropertyExtractorRatethread::getCenter2DPosition() {

    Bottle *inputBottle2DPosition = input2DPosition.read();
    if( inputBottle2DPosition != nullptr){
        const int xPositionTopLeft = inputBottle2DPosition->get(0).asInt();
        const int yPositionTopLeft = inputBottle2DPosition->get(1).asInt();

        const int xPositionBottomRight = inputBottle2DPosition->get(2).asInt();
        const int yPositionBottomRight = inputBottle2DPosition->get(3).asInt();

        const int width = xPositionBottomRight - xPositionTopLeft;
        const int height = yPositionBottomRight - yPositionTopLeft;


        return cv::Point2f(xPositionTopLeft + (width / 2), yPositionTopLeft + (height /  2));

    }

    return cv::Point2f(0, 0);
}

cv::Mat ObjectpropertyExtractorRatethread::getDominantColorKMeans(const Mat inputImage, const int numberOfClusters) {

    // Parameters for Kmeans
    const int numberOfAttempts = 20;
    const int numberOfSteps = 1000;
    const float stopCriterion = 0.0001;
    Mat centers, labels, samples;
    TermCriteria kmeansCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, numberOfSteps, stopCriterion);

    // Process the inputImage to match the requirement of Kmeans input
    inputImage.convertTo(samples, CV_32F);
    samples = samples.reshape(3, inputImage.rows * inputImage.cols);

    kmeans(samples, numberOfClusters, labels, kmeansCriteria, numberOfAttempts, KMEANS_PP_CENTERS, centers );

    return centers;
}

std::vector<std::vector<cv::Point> > ObjectpropertyExtractorRatethread::getContours() {
    using namespace cv;


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    Mat cannyImage, grayImage;

    cvtColor(inputImageMat, grayImage, CV_RGB2GRAY);

   
    /// Detect edges using canny
    Canny( grayImage, cannyImage,  cannyThreshold , cannyThreshold *2, 3 );

    
    findContours( cannyImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// Draw contours
    Mat drawing = Mat::zeros( cannyImage.size(), CV_8UC3 );



  
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( 255, 255, 255);
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }

    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    waitKey(-1);
    return contours;
}


