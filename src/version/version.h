
/**
 * @file version.h
 * @author Lukas Bauer
 * @brief Header file for retrieving the current version of the OpenNetlistView application.
 */

#ifndef __VERSION_H__
#define __VERSION_H__

#include <QString>

namespace OpenNetlistView {

/**
 * @brief Retrieves the current version of the OpenNetlistView application.
 *
 * @return A QString containing the version string.
 */
QString getDiagViewerVersion();

} // namespace OpenNetlistView

#endif // __VERSION_H__
