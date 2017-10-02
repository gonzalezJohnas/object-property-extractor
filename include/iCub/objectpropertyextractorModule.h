//
/*
  * Copyright (C)2017  Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
  * Author: Jonas Gonzalez
  * email: francesco.rea@iit.it
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
//

/**
 * @file objectpropertyextractorModuleModule.h
 * @brief Simple module as objectpropertyextractorModule.
 */

#ifndef objectpropertyextractorModule_H
#define objectpropertyextractorModule_H


/**
 *
 * \defgroup icub_objectpropertyextractorModuleThread icub_objectpropertyextractorModuleThread
 * @ingroup icub_morphoGen
 *
 * This is a module that load a script and provide rpc connection to execute actions
 *
 *
 *
 * \section lib_sec Libraries
 *
 * YARP.
 *
 * \section parameters_sec Parameters
 *
 * <b>Command-line Parameters</b>
 *
 * The following key-value pairs can be specified as command-line parameters by prefixing \c -- to the key
 * (e.g. \c --from file.ini. The value part can be changed to suit your needs; the default values are shown below.
 *
 * - \c from \c icub_objectpropertyextractorModule.ini \n
 *   specifies the configuration file
 *
 * - \c context \c icub_objectpropertyextractorModule/conf \n
 *   specifies the sub-path from \c ICUB_ROOT/icub/app to the configuration file
 *
 * - \c name \c icub_interactionInterface \n
 *   specifies the name of the objectpropertyextractorModuleThread (used to form the stem of objectpropertyextractorModuleRateThread port names)
 *
 * - \c robot \c icub \n
 *   specifies the name of the robot (used to form the root of robot port names)
 *
 * - \c config  \n
 *   specifies the name of the script that will be used
 *
 *
 * <b>Configuration File Parameters</b>
 *
 * The following key-value pairs can be specified as parameters in the configuration file
 * (they can also be specified as command-line parameters if you so wish).
 * The value part can be changed to suit your needs; the default values are shown below.
 *
 *
 *
 * \section portsa_sec Ports Accessed
 *
 * - None
 *
 * \section portsc_sec Ports Created
 *
 *  <b>Input ports</b>
 *
 *  - \c /objectpropertyextractorModule \n
 *    This port is used to send actions to execute to the specified script. \n
 *    The following commands are available
 *
 *  -  \c help \n
 *  -  \c quit \n
 *  -  \c exe  \n
 *
 *    Note that the name of this port mirrors whatever is provided by the \c --name parameter value
 *    The port is attached to the terminal so that you can type in commands and receive replies.
 *    The port can be used by other objectpropertyextractorModuleRateThreads but also interactively by a user through the yarp rpc directive, viz.: \c yarp \c rpc \c /objectpropertyextractorModule
 *    This opens a connection from a terminal to the port and allows the user to then type in commands and receive replies.
 *
 *
 * <b>Output ports</b>
 *
 *  - None
 *
 * \section in_files_sec Input Data Files
 *
 * None
 *
 * \section out_data_sec Output Data Files
 *
 * None
 *
 * \section conf_file_sec Configuration Files
 *
 * \c interactionInterface.ini  in \c ICUB_ROOT/app/objectpropertyextractorModule/conf \n
 *
 * \section tested_os_sec Tested OS
 *
 * Linux
 *
 * \section example_sec Example Instantiation of the Module
 *
 * <tt>objectpropertyextractorModule --name objectpropertyextractorModule </tt>
 *
 * \author Jonas Gonzalez
 *
 * Copyright (C) 2011 RobotCub Consortium\n
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.\n
 * This file can be edited at \c $/src/
 *
 */



#include <iostream>
#include <string>

#include <yarp/sig/all.h>
#include <yarp/os/all.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <iCub/objectpropertyextractorRatethread.h>
#include <opencv2/opencv.hpp>


// general command vocab's
#define COMMAND_VOCAB_OK                 VOCAB2('o','k')

#define COMMAND_VOCAB_SET                VOCAB3('s','e','t')
#define COMMAND_VOCAB_GET                VOCAB3('g','e','t')
#define COMMAND_VOCAB_SUSPEND            VOCAB3('s','u','s')
#define COMMAND_VOCAB_RES                VOCAB3('r','e','s')

#define COMMAND_VOCAB_HELP               VOCAB4('h','e','l','p')
#define COMMAND_VOCAB_FAILED             VOCAB4('f','a','i','l')


class objectpropertyextractorModule:public yarp::os::RFModule {


    std::string moduleName;                  // name of the module
    std::string robotName;                   // name of the robot
    std::string handlerPortName;             // name of handler port
    std::string configFile;                  // name of the configFile that the resource Finder will seek

    yarp::os::Port handlerPort;              // a port to handle messages
    yarp::os::Semaphore mutex;                  // semaphore for the respond function

    objectpropertyextractorRatethread *rThread;             // pointer to a new thread to be created and started in configure() and stopped in close()

public:
    /**
    *  configure all the objectpropertyextractorModuleModule parameters and return true if successful
    * @param rf reference to the resource finder
    * @return flag for the success
    */
    bool configure(yarp::os::ResourceFinder &rf);
    /**
    *  interrupt, e.g., the ports
    */
    bool interruptModule();

    /**
    *  close and shut down 
    */
    bool close();

    /**
    *  to respond through rpc port
    * @param command reference to bottle given to rpc port of module, alongwith parameters
    * @param reply reference to bottle returned by the rpc port in response to command
    * @return bool flag for the success of response else termination of module
    */
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);

    /**
    *  implemented to define the periodicity of the module
    */
    double getPeriod();

    /**
    *  unimplemented
    */
    bool updateModule();
};


#endif //objectpropertyextractorModule_H
