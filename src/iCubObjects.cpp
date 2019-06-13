//
// Created by jonas on 12/06/19.
//

#include "iCub/iCubObjects.h"

const std::vector<double> &iCubObjects::getMPosition() const {
    return m_position;
}

void iCubObjects::setPosition(const std::vector<double> &mPosition) {
    m_position = mPosition;
}

const std::string &iCubObjects::getColorLabel() const {
    return m_colorLabel;
}

void iCubObjects::setColorLabel(const std::string &mColorLabel) {
    m_colorLabel = mColorLabel;
}

yarp::os::Bottle iCubObjects::toBottle() {


    yarp::os::Bottle colorBottle;
    colorBottle.addString("color");
    colorBottle.addString(this->m_colorLabel);


    yarp::os::Bottle positionBottle;
    positionBottle.addString("position");
    positionBottle.addDouble(this->m_position.at(0));
    positionBottle.addDouble(this->m_position.at(1));
    positionBottle.addDouble(this->m_position.at(2));


    yarp::os::Bottle objectBottle;
    objectBottle.addList() = colorBottle;
    objectBottle.addList() = positionBottle;

    return objectBottle;
}

iCubObjects iCubObjects::iCubOjects() {
    return iCubObjects();
}
