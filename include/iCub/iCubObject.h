//
// Created by jonas on 12/06/19.
//

#ifndef OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
#define OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H


#include <vector>
#include <string>
#include <yarp/os/Bottle.h>

class iCubObject {

public:
    iCubObject iCubOjects();




public:
    const std::vector<double> &getMPosition() const;

    void setAnglePosition(const std::vector<double> &mPosition);

    const std::string &getColorLabel() const;

    void setColorLabel(const std::string &mColorLabel);

    const std::vector<double> &getM_cartesianPosition() const;

    void setCartesianPosition(const std::vector<double> &m_cartesianPosition);

    yarp::os::Bottle toBottle();

private:
    std::string m_colorLabel;
    std::vector<double> m_anglePosition;
    std::vector<double> m_cartesianPosition;

};


#endif //OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
