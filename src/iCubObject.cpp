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

    yarp::os::Bottle topLeftBottle;
    topLeftBottle.addString("rect_topLeft");
    topLeftBottle.addInt(this->m_rectangleTopLeft.x);
    topLeftBottle.addInt(this->m_rectangleTopLeft.y);

    yarp::os::Bottle bottomRight;
    bottomRight.addString("rect_bottomRight");
    bottomRight.addInt(this->m_rectangleBottomRight.x);
    bottomRight.addInt(this->m_rectangleBottomRight.y);


    yarp::os::Bottle objectBottle;
    objectBottle.addList() = colorBottle;
    objectBottle.addList() = cartesianPositionBottle;
    objectBottle.addList() = anglePositionBottle;
    objectBottle.addList() = topLeftBottle;
    objectBottle.addList() = bottomRight;

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

const iCub::Point2d &iCubObject::get_rectangleTopLeft() const {
    return m_rectangleTopLeft;
}

void iCubObject::set_rectangleTopLeft(const iCub::Point2d &m_topLeft) {
    m_rectangleTopLeft = m_topLeft;
}

const iCub::Point2d &iCubObject::get_rectangleBottomRight() const {
    return m_rectangleBottomRight;
}

void iCubObject::set_rectangleBottomRight(const iCub::Point2d &m_rectangleBottomRight) {
    iCubObject::m_rectangleBottomRight = m_rectangleBottomRight;
}



