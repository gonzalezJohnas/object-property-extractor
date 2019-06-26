//
// Created by jonas on 12/06/19.
//

#ifndef OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
#define OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H


#include <vector>
#include <string>
#include <yarp/os/Bottle.h>

struct  Color{
    Color() : name(), red(), green(), blue() {}
    Color(std::string newName, int r, int g, int b)
            : name(std::move(newName)), red(r), green(g), blue(b) {}
    std::string name;
    int red, green, blue;
};

struct  Point2d{
    Point2d() : x(), y() {}
    Point2d( int t_x, int t_y)
            : x(t_x), y(t_y) {}
    int x, y;
};

class iCubObject {

public:
    iCubObject iCubOjects();




public:
    const std::vector<double> &getMPosition() const;

    void setAnglePosition(const std::vector<double> &mPosition);



    const std::vector<double> &getM_cartesianPosition() const;

    void setCartesianPosition(const std::vector<double> &m_cartesianPosition);

    yarp::os::Bottle toBottle();

    const Color &getM_color() const;
    void set_color(const Color &m_color);

    const Point2d &getM_centerOfMass2D() const;

    void setM_centerOfMass2D(const Point2d &m_centerOfMass2D);

private:

    Color m_color;
    std::vector<double> m_anglePosition;
    std::vector<double> m_cartesianPosition;
    Point2d m_centerOfMass2D;


};


#endif //OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
