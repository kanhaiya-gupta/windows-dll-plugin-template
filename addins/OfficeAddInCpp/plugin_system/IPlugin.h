#pragma once

#include <oaidl.h>

namespace OfficeAddIn
{
    enum class AppKind
    {
        None = 0,
        PowerPoint,
        Excel,
        Word
    };

    struct IPlugin
    {
        virtual ~IPlugin() = default;
        virtual bool SupportsApp(AppKind app) const = 0;
        virtual HRESULT OnLoad(IDispatch* pApplication, AppKind app) = 0;
        virtual void OnUnload() = 0;
    };
}
