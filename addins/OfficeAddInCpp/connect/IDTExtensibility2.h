#pragma once

#include <oaidl.h>

enum ext_ConnectMode
{
    ext_cm_AfterStartup = 0,
    ext_cm_Startup = 1,
    ext_cm_External = 2,
    ext_cm_CommandLine = 3,
    ext_cm_Solution = 4,
    ext_cm_UISetup = 5
};

enum ext_DisconnectMode
{
    ext_dm_HostShutdown = 0,
    ext_dm_UserClosed = 1,
    ext_dm_UISetupComplete = 2,
    ext_dm_SolutionClosed = 3
};

struct __declspec(uuid("B65AD801-ABAF-11D0-BB8B-00A0C90F2744")) IDTExtensibility2 : public IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE OnConnection(
        IDispatch* Application,
        ext_ConnectMode ConnectMode,
        IDispatch* AddInInst,
        SAFEARRAY** custom) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnDisconnection(
        ext_DisconnectMode RemoveMode,
        SAFEARRAY** custom) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnAddInsUpdate(SAFEARRAY** custom) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnStartupComplete(SAFEARRAY** custom) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnBeginShutdown(SAFEARRAY** custom) = 0;
};
