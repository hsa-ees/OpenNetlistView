/**
 * @file diagram.h
 * @author Lukas Bauer
 * @brief Header file for the Diagram class in the OpenNetlistView::Yosys namespace.
 */

#ifndef __YOSYS_DIAGRAM_H__
#define __YOSYS_DIAGRAM_H__

#include <memory>
#include <vector>

namespace OpenNetlistView::Yosys {

// forward declaration
class Module;

/**
 * @class Diagram
 * @brief Represents a diagram consisting of multiple modules.
 *
 * The Diagram class provides functionalities to manage and manipulate
 * a collection of modules. It allows adding modules, setting a top module,
 * and retrieving modules by name. The class also provides methods to get
 * the top module and all modules in the diagram.
 */
class Diagram
{

public:
    /**
     * @brief Construct a new Diagram object
     *
     */
    Diagram();

    /**
     * @brief Destroy the Diagram object
     *
     */
    ~Diagram();

    /**
     * @brief Add a module to the diagram
     *
     * @param module The module to be added.
     */
    void addModule(const std::shared_ptr<Module>& module);

    /**
     * @brief Set the top module of the diagram
     *
     * @param module The top module to be set.
     */
    void setTopModule(const std::shared_ptr<Module>& module);

    /**
     * @brief add the top module to the diagram
     *
     * adds the module to the diagram and sets it as the top module
     *
     * @param module the module to be added
     */
    void addTopModule(const std::shared_ptr<Module>& module);

    /**
     * @brief Get a module by name
     *
     * @param name The name of the module.
     * @return std::shared_ptr<Module> The module with the given name.
     */
    std::shared_ptr<Module> getModuleByName(const QString& name) const;

    /**
     * @brief Get the Top Module object
     *
     * @return the top module of the diagram
     */
    std::shared_ptr<Module> getTopModule() const;

    /**
     * @brief Get the Modules object
     *
     * @return the modules of the diagram
     */
    std::unique_ptr<std::vector<std::shared_ptr<Module>>> getModules() const;

    /**
     * @brief Link the sub modules of a module
     *
     * this finds the sub modules of the given module and adds them to the modules map
     * then it recursively calls itself for each sub module to find their sub modules
     *
     * @param module the module to link the sub modules of
     */
    void linkSubModules(const std::shared_ptr<Module>& module);

    /**
     * @brief Print the hierarchy of the sub modules
     *
     * prints the hierarchy of the sub modules of the given module
     *
     * @param module the module to print the hierarchy of
     * @param depth the depth of the hierarchy
     */
    void printSubModuleHierarchy(const std::shared_ptr<Module>& module, const int depth = 0);

private:
    std::vector<std::shared_ptr<Module>> modules; ///< Vector of shared pointers to Module objects.
    std::shared_ptr<Module> topModule;            ///< Shared pointer to the top Module object.
};

} // namespace OpenNetlistView::Yosys

#endif // __YOSYS_DIAGRAM_H__