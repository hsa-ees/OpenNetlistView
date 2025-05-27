#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

#include "module.h"

#include "diagram.h"

namespace OpenNetlistView::Yosys {

Diagram::Diagram() = default;

Diagram::~Diagram() = default;

void Diagram::addModule(const std::shared_ptr<Module>& module)
{
    if(module != nullptr)
    {
        modules.emplace_back(module);
    }
}

void Diagram::setTopModule(const std::shared_ptr<Module>& module)
{
    if(module != nullptr)
    {
        topModule = module;
    }
}

void Diagram::addTopModule(const std::shared_ptr<Module>& module)
{
    if(module != nullptr)
    {
        addModule(module);
        topModule = module;
    }
}

std::shared_ptr<Module> Diagram::getModuleByName(const QString& name) const
{
    const auto foundModule = std::find_if(modules.begin(), modules.end(), [&name](const std::shared_ptr<Module>& module) {
        return module->getType() == name;
    });

    if(foundModule != modules.end())
    {
        return *foundModule;
    }

    return nullptr;
}

std::shared_ptr<Module> Diagram::getTopModule() const
{
    return topModule;
}

std::unique_ptr<std::vector<std::shared_ptr<Module>>> Diagram::getModules() const
{
    return std::make_unique<std::vector<std::shared_ptr<Module>>>(this->modules);
}

// NOLINTBEGIN(misc-no-recursion)
void Diagram::linkSubModules(const std::shared_ptr<Module>& module)
{
    // start with the top module

    // check if the type of a node matches the name of a module
    // if so add the module to the node

    if(module == nullptr)
    {
        return;
    }

    // get all the nodes of the module
    auto nodes = module->getNodes();

    for(auto& node : *nodes)
    {
        // get the type of the node
        const auto type = node->getType();

        // get the module with the same name
        const auto subModule = getModuleByName(type);

        // if the module is found add it to the node
        if(subModule != nullptr)
        {
            module->addSubModule(node->getName(), subModule);
            linkSubModules(subModule);
        }
    }
}

void Diagram::printSubModuleHierarchy(const std::shared_ptr<Module>& module, const int depth)
{

    if(module == nullptr)
    {
        return;
    }
    // print the name of the module and its submodules
    for(int i = 0; i < depth; i++)
    {
        std::cout << "  ";
    }

    std::cout << module->getType().toStdString() << std::endl;

    // get the submodules of the module
    const auto subModules = module->getSubModules();

    // print the submodules
    for(const auto& [type, subModule] : subModules)
    {
        printSubModuleHierarchy(subModule, depth + 1);
    }
}
// NOLINTEND(misc-no-recursion)

} // namespace OpenNetlistView::Yosys
