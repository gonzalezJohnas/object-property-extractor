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


#include <yarp/dev/all.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Log.h>
#include <yarp/os/Bottle.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using  cv::Mat;
class ObjectpropertyExtractorRatethread : public yarp::os::RateThread {
private:
    bool result;                    //result of the processing


    std::string robot;              // name of the robot
    std::string name;               // rootname of all the ports opened by this thread



    // Image objects
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* inputImage;


    // Yarp port of the Thread
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > templateImagePort;
    yarp::os::BufferedPort<yarp::os::Bottle> input2DPosition;
    yarp::os::BufferedPort<yarp::os::Bottle> featuresPortOut;
    yarp::os::Port get3DPosition;

    // Local variables for processing
    cv::Mat  inputImageMat;




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


    Mat getInputImage(){ return this->inputImageMat;}


//************************************************************************************************************************
// Processing functions

    /**
     * Function that extract all the features
     */

    void extractFeatures(Mat inputImage);


    /**
     * Function that return 3D world coordinate from 2D points
     */

    cv::Point3d getCoordinateWorld(cv::Point2f centerPoint);

    /**
     * Function to get the name of the Dominant color of the input image
     */

    std::string getDominantColor(Mat inputImage);


    /**
     * Function to get the dominant color susing KMeans algorithm
     * @param inputImage
     * @param numberClusters
     * @return Matrix of the computed culster centers in BGR format
     */

    cv::Mat getDominantColorKMeans( Mat inputImage,  int numberClusters);

    /**
     * Function to get the center position in 2D referencial of input Image
     */

    cv::Point2f getCenter2DPosition(Mat inputImage);

    /**
     * Function to get the pixel size  in 2D referencial of input Image
     */

    const int getPixelSize(Mat inputImage);




};

#endif  //_objectpropertyextractor_RATETHREAD_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------

