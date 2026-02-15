#include "plugins/word/Plugin_WordTemplates.h"

namespace OfficeAddIn
{
    bool Plugin_WordTemplates::SupportsApp(AppKind app) const
    {
        return app == AppKind::Word;
    }

    HRESULT Plugin_WordTemplates::OnLoad(IDispatch* /*pApplication*/, AppKind /*app*/)
    {
        return S_OK;
    }

    void Plugin_WordTemplates::OnUnload()
    {
    }
}
