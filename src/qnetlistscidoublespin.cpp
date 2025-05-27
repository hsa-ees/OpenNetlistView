#include <QDoubleSpinBox>
#include <QWidget>
#include <QLocale>
#include <QString>

#include "qnetlistscidoublespin.h"

namespace OpenNetlistView {

QNetlistSciDoubleSpin::QNetlistSciDoubleSpin(QWidget* parent)
    : QDoubleSpinBox(parent)
{
}

QNetlistSciDoubleSpin::~QNetlistSciDoubleSpin() = default;

QString QNetlistSciDoubleSpin::textFromValue(double value) const
{
    return QString::number(value, 'e', 2);
}

double QNetlistSciDoubleSpin::valueFromText(const QString& text) const
{
    bool ok = false;
    double value = QLocale().c().toDouble(text, &ok);

    if(!ok)
    {
        value = 0.0F;
    }

    return value;
}

} // namespace OpenNetlistView