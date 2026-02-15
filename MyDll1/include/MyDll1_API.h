#pragma once

#ifdef MYDLL1_EXPORTS
#define MYDLL1_API __declspec(dllexport)
#else
#define MYDLL1_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MYDLL1_VERSION_MAJOR 1
#define MYDLL1_VERSION_MINOR 0
#define MYDLL1_VERSION_PATCH 0

MYDLL1_API unsigned int MyDll1_GetVersion(void);
MYDLL1_API int MyDll1_Initialize(void);
MYDLL1_API void MyDll1_Shutdown(void);
/* Returns 1 if OpenCV (third_party) is working; 0 on failure. Use to verify framework + third_party. */
MYDLL1_API int MyDll1_GetThirdPartyCheck(void);

#ifdef __cplusplus
}
#endif
