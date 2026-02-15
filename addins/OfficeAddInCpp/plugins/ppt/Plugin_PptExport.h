#pragma once

#include "plugin_system/IPlugin.h"

namespace OfficeAddIn
{
    class Plugin_PptExport : public IPlugin
    {
    public:
        bool SupportsApp(AppKind app) const override;
        HRESULT OnLoad(IDispatch* pApplication, AppKind app) override;
        void OnUnload() override;
    };
}
