#include "plugins/excel/Plugin_ExcelCharts.h"

namespace OfficeAddIn
{
    bool Plugin_ExcelCharts::SupportsApp(AppKind app) const
    {
        return app == AppKind::Excel;
    }

    HRESULT Plugin_ExcelCharts::OnLoad(IDispatch* /*pApplication*/, AppKind /*app*/)
    {
        return S_OK;
    }

    void Plugin_ExcelCharts::OnUnload()
    {
    }
}
