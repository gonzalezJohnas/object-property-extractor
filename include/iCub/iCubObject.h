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

private:

    Color m_color;
    std::vector<double> m_anglePosition;
    std::vector<double> m_cartesianPosition;

};


#endif //OBJECTPROPERTYEXTRACTOR_ICUBOBJECTS_H
