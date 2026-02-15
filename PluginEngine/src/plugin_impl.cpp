#include "pch.h"
#include "PluginEngine_API.h"
#include <opencv2/opencv.hpp>

unsigned int PluginEngine_GetVersion(void)
{
    return (PLUGINENGINE_VERSION_MAJOR << 16) |
           (PLUGINENGINE_VERSION_MINOR << 8) |
           PLUGINENGINE_VERSION_PATCH;
}

int PluginEngine_Initialize(void)
{
    return 0;
}

void PluginEngine_Shutdown(void)
{
}

int PluginEngine_GetThirdPartyCheck(void)
{
    try
    {
        cv::Mat m = cv::Mat::zeros(2, 2, CV_8UC1);
        if (m.rows == 2 && m.cols == 2)
            return 1;
        return 0;
    }
    catch (...)
    {
        return 0;
    }
}
