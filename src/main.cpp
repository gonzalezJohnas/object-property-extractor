#include <iostream>
#include <iCub/objectpropertyextractorModule.h>


using namespace yarp::os;
using namespace yarp::sig;


int main(int argc, char *argv[]) {

    Network yarp;
    objectpropertyextractorModule module;

    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("interactionInterface.ini");      //overridden by --from parameter
    rf.setDefaultContext("interactionInterface");              //overridden by --context parameter
    rf.configure(argc, argv);


    yInfo("resourceFinder: %s", rf.toString().c_str());

    module.runModule(rf);
    return 0;
}
