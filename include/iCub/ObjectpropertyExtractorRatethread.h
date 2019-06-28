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
 * @file objectpropertyextractorRatethread.h
 * @brief Definition of a thread that receives an data from input port and sends it to the output port.
 */


#ifndef _objectpropertyextractor_RATETHREAD_H_
#define _objectpropertyextractor_RATETHREAD_H_


#include <yarp/conf/system.h>
#include <yarp/os/Log.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Portable.h>
#include <yarp/sig/Image.h>

#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yarp/os/RateThread.h>
#include <opencv2/core/types_c.h>

#include "iCubObject.h"



const static cv::Mat whiteColor(1, 3, CV_8UC3, cvScalar(255, 255, 255));
const static cv::Mat blackColor(1, 3, CV_8UC3, cvScalar(0, 0, 0));


const static Color red("red", 255, 0,  0);
const static Color yellow("yellow", 255, 255,  0);
const static Color blue("blue", 0, 0,  255);
const static Color green("green", 0, 255,  0);

const static std::map<unsigned int, Color> colorMap = {
        {20,  red },
        {75, yellow},
        {138, green},
        {250, blue},
        {360, red},

};




class ObjectpropertyExtractorRatethread : public yarp::os::RateThread {
public:

    /**
    * constructor default
    */
    explicit ObjectpropertyExtractorRatethread(yarp::os::ResourceFinder &rf);

    /**
    * constructor
    * @param robotname name of the robot
    */
    ObjectpropertyExtractorRatethread(std::string robotname, yarp::os::ResourceFinder &rf);

    /**
     * destructor
     */
    ~ObjectpropertyExtractorRatethread() override;

    /**
    *  initialises the thread
    */
    bool threadInit() override;

    /**
    *  correctly releases the thread
    */
    void threadRelease() override;

    /**
    *  active part of the thread
    */
    void run() override;

    void interruptThread();

    /**
    * function that sets the rootname of all the ports that are going to be created by the thread
    * @param str rootnma
    */
    void setName(std::string str);

    /**
    * function that returns the original root name and appends another string iff passed as parameter
    * @param p pointer to the string that has to be added
    * @return rootname
    */
    std::string getName(const char *p);

    /**
    * function that sets the inputPort name
    */
    void setInputPortName(std::string inpPrtName);

    cv::Mat getInputImage() { return this->inputImageMat; }

//************************************************************************************************************************


    /**
     * Function that extract all the features
     */

    void extractFeatures();


    void testOPC();

    std::string testColor(cv::Mat img);

private:

    std::string robot;              // name of the robot
    std::string name;               // rootname of all the ports opened by this thread


    int cannyThreshold;

    // Image objects
    yarp::sig::ImageOf<yarp::sig::PixelRgb> *inputImage;

    // Yarp port of the Thread
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > templateImagePort;
    yarp::os::BufferedPort<yarp::os::Bottle> input2DPosition;
    yarp::os::BufferedPort<yarp::os::Bottle> featuresPortOut;
    yarp::os::BufferedPort<yarp::os::Bottle> anglePositionPort;
    yarp::os::BufferedPort<yarp::os::Bottle> cartesianPositionPort;
    // Local variables for processing
    cv::Mat inputImageMat;


    void sendFeatures(iCubObject t_object);

    /**
    * Function that return 3D world coordinate from 2D points
    */

    std::vector<double> getCoordinateWorld3D();

    /**
    * Function that return the  world coordinate express in angles
    */

    std::vector<double> getCoordinateWorldAngles();


    /**
     * Function to get the name of the Dominant color of the input image
     */

    Color getDominantColor(cv::Mat inputImage);

    /**
     * Function to get the dominant color susing KMeans algorithm
     * @param inputImage
     * @param numberClusters
     * @return Matrix of the computed culster centers in BGR format
     */

    cv::Mat getDominantColorKMeans(cv::Mat inputImage, int numberClusters);

    /**
     * Function to get the center position in 2D referencial of input Image
     */

    void getRectanglePoints(cv::Mat inputImage, iCub::Point2d &topLeft, iCub::Point2d &bottomRight);

    /**
     * Function to detect contours
     * @return vector of point forming the contour
     */
    std::string getContours(cv::Mat inputImage);





};

#endif  //_objectpropertyextractor_RATETHREAD_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------

