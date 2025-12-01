#include "PreferenceSystem.h"

#include "ipreferencesystem.h"

#include "module/StaticModule.h"

namespace settings
{

IPreferencePage& PreferenceSystem::getPage(const std::string& path)
{
    return getRootPage().createOrFindPage(path);
}

void PreferenceSystem::foreachPage(const std::function<void(IPreferencePage&)>& functor)
{
    return getRootPage().foreachChildPage(functor);
}

PreferencePage& PreferenceSystem::getRootPage()
{
    if (!_rootPage)
    {
        _rootPage = std::make_shared<PreferencePage>("");
    }
    return *_rootPage;
}

// RegisterableModule implementation
const std::string& PreferenceSystem::getName() const
{
    static std::string _name(MODULE_PREFERENCESYSTEM);
    return _name;
}

const StringSet& PreferenceSystem::getDependencies() const
{
    static StringSet _dependencies;
    return _dependencies;
}

// Define the static PreferenceSystem module
module::StaticModuleRegistration<PreferenceSystem> preferenceSystemModule;

}
