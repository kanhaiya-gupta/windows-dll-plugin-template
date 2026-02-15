#pragma once

#ifdef PLUGINENGINE_EXPORTS
#define PLUGINENGINE_API __declspec(dllexport)
#else
#define PLUGINENGINE_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PLUGINENGINE_VERSION_MAJOR 1
#define PLUGINENGINE_VERSION_MINOR 0
#define PLUGINENGINE_VERSION_PATCH 0

PLUGINENGINE_API unsigned int PluginEngine_GetVersion(void);
PLUGINENGINE_API int PluginEngine_Initialize(void);
PLUGINENGINE_API void PluginEngine_Shutdown(void);
/* Returns 1 if OpenCV (third_party) is working; 0 on failure. Use to verify framework + third_party. */
PLUGINENGINE_API int PluginEngine_GetThirdPartyCheck(void);

#ifdef __cplusplus
}
#endif
