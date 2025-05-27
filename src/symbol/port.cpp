#include <QString>
#include <third_party/libcola/cola.h>
#include <third_party/libvpsc/rectangle.h>

#include <utility>
#include <ostream>
#include <sstream>
#include <vector>

#include "port.h"

namespace OpenNetlistView::Symbol {

Port::Port(QString name, double xPos, double yPos)
    : name(std::move(name))
    , xPos(xPos)
    , yPos(yPos)
{
}

Port::~Port() = default;

const QString& Port::getName() const
{
    return name;
}

double Port::getXPos() const
{
    return xPos;
}

double Port::getYPos() const
{
    return yPos;
}

std::pair<QString, int> Port::generateColaRep(std::vector<vpsc::Rectangle*>& rectangles,
    vpsc::Rectangle* bodyRect)
{

    vpsc::Rectangle* rect = nullptr;

    // Check if the port is on the left, right, top, or bottom of the body rectangle
    // left
    if(xPos < bodyRect->getCentreX() &&
        yPos >= bodyRect->getMinY() &&
        yPos <= bodyRect->getMaxY())
    {
        rect = new vpsc::Rectangle(xPos - portRectWidth,
            xPos,
            yPos - (portRectHeight / 2),
            yPos + (portRectHeight / 2));
    }
    // right
    else if(xPos > bodyRect->getCentreX() &&
            yPos >= bodyRect->getMinY() &&
            yPos <= bodyRect->getMaxY())
    {
        rect = new vpsc::Rectangle(xPos,
            xPos + portRectWidth,
            yPos - (portRectHeight / 2),
            yPos + (portRectHeight / 2));
    }
    // top
    else if(yPos > bodyRect->getCentreY() &&
            xPos >= bodyRect->getMinX() && xPos <= bodyRect->getMaxX())
    {
        rect = new vpsc::Rectangle(xPos - (portRectWidth / 2),
            xPos + (portRectWidth / 2),
            yPos,
            yPos + portRectHeight);
    }
    // bottom
    else if(yPos < bodyRect->getCentreY() &&
            xPos >= bodyRect->getMinX() &&
            xPos <= bodyRect->getMaxX())
    {
        rect = new vpsc::Rectangle(xPos - (portRectWidth / 2),
            xPos + (portRectWidth / 2),
            yPos - portRectHeight,
            yPos);
    }
    rectangles.push_back(rect);

    return std::make_pair(name, rectangles.size() - 1);
}

std::ostream& operator<<(std::ostream& outStream, const Port& port)
{

    std::stringstream sStream = std::stringstream();

    sStream << "Port: " << port.name.toStdString()
            << " (X: " << port.xPos << ", Y: "
            << port.yPos << ")";

    return outStream << sStream.str();
}

} // namespace OpenNetlistView::Symbol