#include "plugin_system/PluginHost.h"

namespace OfficeAddIn
{
    PluginHost& PluginHost::Instance()
    {
        static PluginHost s_host;
        return s_host;
    }

    void PluginHost::RegisterPlugin(IPlugin* plugin)
    {
        if (plugin)
            m_plugins.push_back(plugin);
    }

    void PluginHost::UnregisterPlugin(IPlugin* plugin)
    {
        for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if (*it == plugin)
            {
                m_plugins.erase(it);
                break;
            }
        }
    }

    HRESULT PluginHost::LoadAll(IDispatch* pApplication, AppKind app)
    {
        for (IPlugin* p : m_plugins)
        {
            if (p->SupportsApp(app))
                p->OnLoad(pApplication, app);
        }
        return (HRESULT)0; /* S_OK */
    }

    void PluginHost::UnloadAll()
    {
        for (IPlugin* p : m_plugins)
            p->OnUnload();
    }
}
