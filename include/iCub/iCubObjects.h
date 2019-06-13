//
// Created by jonas on 12/06/19.
//

#ifndef OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
#define OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H


#include <vector>
#include <string>
#include <yarp/os/Bottle.h>

class iCubObjects {

public:
    iCubObjects iCubOjects();

private:
    std::vector<double> m_position;
public:
    const std::vector<double> &getMPosition() const;

    void setPosition(const std::vector<double> &mPosition);

    const std::string &getColorLabel() const;

    void setColorLabel(const std::string &mColorLbale);

    yarp::os::Bottle toBottle();

private:
    std::string m_colorLabel;


};


#endif //OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
