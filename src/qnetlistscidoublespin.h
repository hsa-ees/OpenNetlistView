/**
 * @file qnetlistscidoublespin.h
 * @brief Declaration of the QNetlistSciDoubleSpin class, a custom QDoubleSpinBox
 *        with scientific notation support for OpenNetlistView.
 *
 * This file contains the definition of the QNetlistSciDoubleSpin class, which
 * extends QDoubleSpinBox to provide custom behavior for displaying and parsing
 * double values in scientific notation.
 *
 * @author Lukas Bauer
 */
#ifndef __Q_NETLIST_SCI_DOUBLE_SPIN_H__
#define __Q_NETLIST_SCI_DOUBLE_SPIN_H__

#include <QDoubleSpinBox>
#include <QObject>

/**
 * @class QNetlistSciDoubleSpin
 * @brief A specialized QDoubleSpinBox that provides scientific notation support.
 *
 * This class extends QDoubleSpinBox to allow for custom handling of text
 * representation and value parsing, enabling the use of scientific notation
 * for displaying and editing double values.
 *
 */
namespace OpenNetlistView {

class QNetlistSciDoubleSpin : public QDoubleSpinBox
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for QNetlistSciDoubleSpin.
     * @param parent The parent widget.
     */
    QNetlistSciDoubleSpin(QWidget* parent = 0);

    /**
     * @brief Destructor for QNetlistSciDoubleSpin.
     */
    ~QNetlistSciDoubleSpin();

protected:
    /**
     * @brief Override the textFromValue method to provide scientific notation.
     * @param value The double value to convert to text.
     * @return The text representation of the value in scientific notation.
     */
    QString textFromValue(double value) const override;

    /**
     * @brief Override the valueFromText method to parse scientific notation.
     * @param text The text representation of the value.
     * @return The parsed double value.
     */
    double valueFromText(const QString& text) const override;
};

} // namespace OpenNetlistView

#endif // __Q_NETLIST_SCI_DOUBLE_SPIN_H__