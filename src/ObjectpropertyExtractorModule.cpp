//
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
//
#include <iCub/ObjectpropertyExtractorModule.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

/*
 * Configure method. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * If you are migrating from the old Module, this is the
 *  equivalent of the "open" method.
 */

bool ObjectpropertyExtractorModule::configure(yarp::os::ResourceFinder &rf) {

    if (rf.check("help")) {
        printf("HELP \n");
        printf("====== \n");
        printf("--name           : changes the rootname of the module ports \n");
        printf("--robot          : changes the name of the robot where the module interfaces to  \n");
        printf("--name           : rootname for all the connection of the module \n");
        printf("--config       : path of the script to execute \n");
        printf(" \n");
        printf("press CTRL-C to stop... \n");
        return true;
    }

    /* get the module name which will form the stem of all module port names */
    moduleName = rf.check("name",
                          Value("/objectpropertyextractor"),
                          "module name (string)").asString();
    /*
    * before continuing, set the module name before getting any other parameters,
    * specifically the port names which are dependent on the module name
    */
    setName(moduleName.c_str());

    /*
    * get the robot name which will form the stem of the robot ports names
    * and append the specific part and device required
    */
    robotName = rf.check("robot",
                         Value("icub"),
                         "Robot name (string)").asString();



    /*
    * attach a port of the same name as the module (prefixed with a /) to the module
    * so that messages received from the port are redirected to the respond method
    */
    handlerPortName = "";
    handlerPortName += getName();         // use getName() rather than a literal

    if (!handlerPort.open(handlerPortName)) {
        cout << getName() << ": Unable to open port " << handlerPortName << endl;
        return false;
    }

    attach(handlerPort);                  // attach to port



    rThread = new ObjectpropertyExtractorRatethread(rf);


    /*pass the name of the module in order to create ports*/
    rThread->setName(moduleName);
    rThread->start();


    return true;       // let the RFModule know everything went well
    // so that it will then run the module
}

bool ObjectpropertyExtractorModule::close() {
    handlerPort.close();
    /* stop the thread */
    yInfo("stopping the thread \n");
    rThread->threadRelease();
    rThread->stop();
    return true;
}

bool ObjectpropertyExtractorModule::interruptModule() {
    yInfo("Interrupting \n");
    handlerPort.interrupt();
    rThread->interruptThread();
    return true;
}


bool ObjectpropertyExtractorModule::respond(const Bottle &command, Bottle &reply) {
    vector<string> replyScript;
    string helpMessage = string(getName().c_str()) +
                         " commands are: \n" +
                         "help \n" +
                         "quit \n";
    reply.clear();

    if (command.get(0).asString() == "quit") {
        reply.addString("quitting");
        return false;
    }


    bool ok = false;
    bool rec = false; // is the command recognized?

    mutex.wait();

    switch (command.get(0).asVocab()) {
        case COMMAND_VOCAB_HELP:
            rec = true;
            {
                reply.addVocab(Vocab::encode("many"));

                ok = true;
            }
            break;

        case COMMAND_VOCAB_SET:
            rec = true;
            {
                switch (command.get(1).asVocab()) {

                    default:
                        cout << "received an unknown request after SET" << endl;
                        break;
                }
            }
            break;

        case COMMAND_VOCAB_GET:
            rec = true;
            {
                switch (command.get(1).asVocab()) {

                    case COMMAND_VOCAB_FEATURES: {
                        rThread->extractFeatures();
                        ok = true;
                        break;
                    }

                    case COMMAND_VOCAB_COLOR : {

                        ok = true ;
                        break;
                    }

                    default:
                        cout << "received an unknown request after a GET" << endl;
                        break;
                }
            }
            break;

        case COMMAND_VOCAB_SUSPEND:
            rec = true;
            {
                rThread->suspend();
                ok = true;
            }
            break;

        case COMMAND_VOCAB_RES:
            rec = true;
            {
                rThread->suspend();
                ok = true;
            }
            break;

        case COMMAND_VOCAB_TEST:
            rec = true;
            {
                rThread->testOPC();
                ok = true;
            }
            break;


        default:
            break;

    }
    mutex.post();

    if (!rec)
        ok = RFModule::respond(command, reply);

    if (!ok) {
        reply.clear();
        reply.addVocab(COMMAND_VOCAB_FAILED);
    } else
        reply.addVocab(COMMAND_VOCAB_OK);

    return ok;


}

/* Called periodically every getPeriod() seconds */
bool ObjectpropertyExtractorModule::updateModule() {
    return true;
}

double ObjectpropertyExtractorModule::getPeriod() {
    /* module periodicity (seconds), called implicitly by myModule */
    return 1.0;
}

ObjectpropertyExtractorModule::~ObjectpropertyExtractorModule() {
    delete rThread;

}

