#include "plugin_system/PluginHost.h"
#include "plugins/ppt/Plugin_PptExport.h"
#include "plugins/excel/Plugin_ExcelCharts.h"
#include "plugins/word/Plugin_WordTemplates.h"
#include "plugins/common/Plugin_Common.h"

namespace OfficeAddIn
{
    static Plugin_PptExport s_pptExport;
    static Plugin_ExcelCharts s_excelCharts;
    static Plugin_WordTemplates s_wordTemplates;
    static Plugin_Common s_common;

    void RegisterAllPlugins()
    {
        PluginHost& host = PluginHost::Instance();
        host.RegisterPlugin(&s_pptExport);
        host.RegisterPlugin(&s_excelCharts);
        host.RegisterPlugin(&s_wordTemplates);
        host.RegisterPlugin(&s_common);
    }
}
