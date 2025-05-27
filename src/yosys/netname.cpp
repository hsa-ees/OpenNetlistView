#include <QStringList>
#include <QList>
#include <QString>

#include <utility>
#include <vector>
#include <cstdint>

#include "netname.h"

namespace OpenNetlistView::Yosys {

Netname::Netname(QString name, QStringList bits, bool isHidden)
    : name(std::move(name))
    , bits(std::move(bits))
    , isHidden(isHidden)
{
}

Netname::~Netname() = default;

QString Netname::getName() const
{
    return this->name;
}

QStringList Netname::getBits() const
{
    return this->bits;
}

bool Netname::getIsHidden() const
{
    return this->isHidden;
}

uint64_t Netname::getBitWidth() const
{
    return this->bits.size();
}

void Netname::addAlternativeName(const QString& alternativeName)
{
    this->alternativeNetnames.push_back(alternativeName);
}

std::vector<QString> Netname::getAlternativeNetnames() const
{
    return this->alternativeNetnames;
}

} // namespace OpenNetlistView::Yosys
