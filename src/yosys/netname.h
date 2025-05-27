/**
 * @file netname.h
 * @brief Header file for the Netname class in the OpenNetlistView::Yosys namespace.
 *
 * This file contains the declaration of the Netname class, which represents a net name
 * with associated bits and alternative names in the Yosys part of the OpenNetlistView project.
 *
 * @author Lukas Bauer
 */

#ifndef __NETNAME_H__
#define __NETNAME_H__

#include <QStringList>
#include <QString>
#include <cstdint>

namespace OpenNetlistView::Yosys {

/**
 * @class Netname
 * @brief Represents a net name in the OpenNetlistView Yosys module.
 *
 * The Netname class encapsulates the details of a net name, including its name, bits, visibility,
 * and alternative names. It provides methods to access these details and manipulate the net name.
 */
class Netname
{
public:
    /**
     * @brief Constructs a Netname object with the specified name, bits, and visibility.
     *
     * @param name The net name.
     * @param bits A list containing the bits of the net name.
     * @param isHidden A boolean indicating whether the net name is hidden.
     */
    Netname(QString name, QStringList bits, bool isHidden = false);

    /**
     * @brief Destructor for the Netname class.
     */
    ~Netname();

    /**
     * @brief Gets the  net name.
     *
     * @return The net name
     */
    QString getName() const;

    /**
     * @brief Gets the bits of the net name.
     *
     * @return A list containing the bits of the net name.
     */
    QStringList getBits() const;

    /**
     * @brief Gets the visibility of the net name.
     *
     * @return true if the net name is hidden, false otherwise
     */
    bool getIsHidden() const;

    /**
     * @brief Gets the bit width of the net name.
     *
     * @return The number of bits of the net name.
     */
    uint64_t getBitWidth() const;

    /**
     * @brief Adds an alternative name to the net name.
     *
     * @param alternativeName The alternative name to be added.
     */
    void addAlternativeName(const QString& alternativeName);

    /**
     * @brief Gets the alternative names of the net name.
     *
     * @return A list containing the alternative names of the net name.
     */
    std::vector<QString> getAlternativeNetnames() const;

private:
    QString name;                             ///< the net name
    QStringList bits;                         ///< the bits of the net name
    bool isHidden;                            ///< indicates whether the net name is hidden
    std::vector<QString> alternativeNetnames; ///< alternative names for the net name
};

} // namespace OpenNetlistView::Yosys

#endif // __NETNAME_H__