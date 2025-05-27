#include <QGraphicsItem>
#include <QString>

#include <utility>

#include "component.h"

namespace OpenNetlistView::Yosys {

Component::Component(QString name)
    : name(std::move(name))
    , graphicsItem(nullptr)
{
}

Component::~Component() = default;

QString Component::getName() const
{
    return name;
}

void Component::setName(const QString& name)
{
    this->name = name;
}

void Component::setGraphicsItem(QGraphicsItem* item)
{
    this->graphicsItem = item;
}

QGraphicsItem* Component::getGraphicsItem() const
{
    return graphicsItem;
}

} // namespace OpenNetlistView::Yosys