#pragma once

#include "plugin_system/IPlugin.h"
#include <vector>

namespace OfficeAddIn
{
    class PluginHost
    {
    public:
        static PluginHost& Instance();
        void RegisterPlugin(IPlugin* plugin);
        void UnregisterPlugin(IPlugin* plugin);
        HRESULT LoadAll(IDispatch* pApplication, AppKind app);
        void UnloadAll();

    private:
        PluginHost() = default;
        std::vector<IPlugin*> m_plugins;
    };
}
