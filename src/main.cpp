#include <iostream>
#include "iCub/ObjectpropertyExtractorModule.h"


using namespace yarp::os;
using namespace yarp::sig;


int main(int argc, char *argv[]) {


    Network yarp;
    ObjectpropertyExtractorModule module;
    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("objectPropertyExtractor.ini");      //overridden by --from parameter
    rf.setDefaultContext("objectpropertyextractor");              //overridden by --context parameter
    rf.configure(argc, argv);


    yInfo("resourceFinder: %s", rf.toString().c_str());

    module.runModule(rf);

    return 0;
}
