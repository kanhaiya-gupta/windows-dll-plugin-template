#include "plugins/common/Plugin_Common.h"

namespace OfficeAddIn
{
    bool Plugin_Common::SupportsApp(AppKind app) const
    {
        (void)app;
        return true;
    }

    HRESULT Plugin_Common::OnLoad(IDispatch* /*pApplication*/, AppKind /*app*/)
    {
        return S_OK;
    }

    void Plugin_Common::OnUnload()
    {
    }
}
