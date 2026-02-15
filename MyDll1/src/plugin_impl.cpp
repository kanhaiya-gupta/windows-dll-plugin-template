#include "pch.h"
#include "MyDll1_API.h"
#include <opencv2/opencv.hpp>

unsigned int MyDll1_GetVersion(void)
{
    return (MYDLL1_VERSION_MAJOR << 16) |
           (MYDLL1_VERSION_MINOR << 8) |
           MYDLL1_VERSION_PATCH;
}

int MyDll1_Initialize(void)
{
    return 0;
}

void MyDll1_Shutdown(void)
{
}

int MyDll1_GetThirdPartyCheck(void)
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
