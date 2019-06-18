//
// Created by jonas on 12/06/19.
//

#include "iCub/iCubObject.h"

const std::vector<double> &iCubObject::getMPosition() const {
    return m_anglePosition;
}

void iCubObject::setAnglePosition(const std::vector<double> &mPosition) {
    m_anglePosition = mPosition;
}

const std::string &iCubObject::getColorLabel() const {
    return m_colorLabel;
}

void iCubObject::setColorLabel(const std::string &mColorLabel) {
    m_colorLabel = mColorLabel;
}

yarp::os::Bottle iCubObject::toBottle() {


    yarp::os::Bottle colorBottle;
    colorBottle.addString("color");
    colorBottle.addString(this->m_colorLabel);


    yarp::os::Bottle anglePositionBottle;
    anglePositionBottle.addString("angle_position");
    anglePositionBottle.addDouble(this->m_anglePosition.at(0));
    anglePositionBottle.addDouble(this->m_anglePosition.at(1));
    anglePositionBottle.addDouble(this->m_anglePosition.at(2));


    yarp::os::Bottle cartesianPositionBottle;
    cartesianPositionBottle.addString("cartesian_position");
    cartesianPositionBottle.addDouble(this->m_cartesianPosition.at(0));
    cartesianPositionBottle.addDouble(this->m_cartesianPosition.at(1));
    cartesianPositionBottle.addDouble(this->m_cartesianPosition.at(2));


    yarp::os::Bottle objectBottle;
    objectBottle.addList() = colorBottle;
    objectBottle.addList() = cartesianPositionBottle;
    objectBottle.addList() = anglePositionBottle;

    return objectBottle;
}

iCubObject iCubObject::iCubOjects() {
    return iCubObject();
}

const std::vector<double> &iCubObject::getM_cartesianPosition() const {
    return m_cartesianPosition;
}

void iCubObject::setCartesianPosition(const std::vector<double> &m_cartesianPosition) {
    iCubObject::m_cartesianPosition = m_cartesianPosition;
}


