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
#include<cstring>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

#define THRATE 100 //ms

//********************interactionEngineRatethread******************************************************

objectpropertyextractorRatethread::objectpropertyextractorRatethread() : RateThread(THRATE) {
    robot = "icub";
}

objectpropertyextractorRatethread::objectpropertyextractorRatethread(string _robot, string _configFile) : RateThread(
        THRATE) {
    robot = _robot;
    configFile = _configFile;
}

objectpropertyextractorRatethread::~objectpropertyextractorRatethread() {
    // do nothing
}

bool objectpropertyextractorRatethread::threadInit() {


    yInfo("Initialization of the processing thread correctly ended");

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

}


void objectpropertyextractorRatethread::threadRelease() {
    // nothing

}


