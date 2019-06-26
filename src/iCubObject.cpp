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




yarp::os::Bottle iCubObject::toBottle() {


    yarp::os::Bottle colorBottle;
    colorBottle.addString("color");
    colorBottle.addString(this->m_color.name);
    colorBottle.addInt(this->m_color.red);
    colorBottle.addInt(this->m_color.green);
    colorBottle.addInt(this->m_color.blue);


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

    yarp::os::Bottle centerOfMassBottle;
    centerOfMassBottle.addString("center_mass");
    centerOfMassBottle.addInt(this->m_centerOfMass2D.x);
    centerOfMassBottle.addInt(this->m_centerOfMass2D.y);


    yarp::os::Bottle objectBottle;
    objectBottle.addList() = colorBottle;
    objectBottle.addList() = cartesianPositionBottle;
    objectBottle.addList() = anglePositionBottle;
    objectBottle.addList() = centerOfMassBottle;

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

const Color &iCubObject::getM_color() const {
    return m_color;
}

void iCubObject::set_color(const Color &m_color) {
    iCubObject::m_color = m_color;
}

const Point2d &iCubObject::getM_centerOfMass2D() const {
    return m_centerOfMass2D;
}

void iCubObject::setM_centerOfMass2D(const Point2d &m_centerOfMass2D) {
    iCubObject::m_centerOfMass2D = m_centerOfMass2D;
}


