#include "plugins/ppt/Plugin_PptExport.h"

namespace OfficeAddIn
{
    bool Plugin_PptExport::SupportsApp(AppKind app) const
    {
        return app == AppKind::PowerPoint;
    }

    HRESULT Plugin_PptExport::OnLoad(IDispatch* /*pApplication*/, AppKind /*app*/)
    {
        return S_OK;
    }

    void Plugin_PptExport::OnUnload()
    {
    }
}
