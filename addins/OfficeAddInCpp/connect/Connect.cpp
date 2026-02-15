#include <windows.h>
#include "connect/Connect.h"
#include "plugin_system/PluginHost.h"
#include "plugin_system/Plugins.h"

#define DISPID_OnConnection        1
#define DISPID_OnDisconnection     2
#define DISPID_OnAddInsUpdate      3
#define DISPID_OnStartupComplete   4
#define DISPID_OnBeginShutdown     5

Connect::Connect() : m_refCount(1), m_hEngine(nullptr), m_pfnInitialize(nullptr), m_pfnShutdown(nullptr) {}

Connect::~Connect()
{
    if (m_hEngine && m_pfnShutdown)
    {
        m_pfnShutdown();
        FreeLibrary(m_hEngine);
    }
}

STDMETHODIMP Connect::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    if (riid == IID_IUnknown || riid == IID_IDispatch || riid == __uuidof(IDTExtensibility2))
    {
        *ppv = static_cast<IDTExtensibility2*>(this);
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) Connect::AddRef() { return InterlockedIncrement(&m_refCount); }

STDMETHODIMP_(ULONG) Connect::Release()
{
    ULONG n = InterlockedDecrement(&m_refCount);
    if (n == 0) delete this;
    return n;
}

STDMETHODIMP Connect::GetTypeInfoCount(UINT* pctinfo)
{
    if (!pctinfo) return E_POINTER;
    *pctinfo = 0;
    return S_OK;
}

STDMETHODIMP Connect::GetTypeInfo(UINT, LCID, ITypeInfo**) { return E_NOTIMPL; }

STDMETHODIMP Connect::GetIDsOfNames(REFIID, LPOLESTR* rgszNames, UINT cNames, LCID, DISPID* rgDispId)
{
    if (cNames == 0 || !rgszNames || !rgDispId) return E_INVALIDARG;
    const wchar_t* name = rgszNames[0];
    if (wcscmp(name, L"OnConnection") == 0)       { *rgDispId = DISPID_OnConnection; return S_OK; }
    if (wcscmp(name, L"OnDisconnection") == 0)     { *rgDispId = DISPID_OnDisconnection; return S_OK; }
    if (wcscmp(name, L"OnAddInsUpdate") == 0)     { *rgDispId = DISPID_OnAddInsUpdate; return S_OK; }
    if (wcscmp(name, L"OnStartupComplete") == 0)  { *rgDispId = DISPID_OnStartupComplete; return S_OK; }
    if (wcscmp(name, L"OnBeginShutdown") == 0)    { *rgDispId = DISPID_OnBeginShutdown; return S_OK; }
    return DISP_E_UNKNOWNNAME;
}

STDMETHODIMP Connect::Invoke(DISPID dispIdMember, REFIID, LCID, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO*, UINT* puArgErr)
{
    if (wFlags & DISPATCH_METHOD && pVarResult) VariantInit(pVarResult);
    switch (dispIdMember)
    {
    case DISPID_OnConnection:
        if (pDispParams && pDispParams->cArgs >= 4)
            return OnConnection(
                pDispParams->rgvarg[3].pdispVal,
                (ext_ConnectMode)pDispParams->rgvarg[2].lVal,
                pDispParams->rgvarg[1].pdispVal,
                nullptr);
        return E_INVALIDARG;
    case DISPID_OnDisconnection:
        if (pDispParams && pDispParams->cArgs >= 2)
            return OnDisconnection((ext_DisconnectMode)pDispParams->rgvarg[1].lVal, nullptr);
        return E_INVALIDARG;
    case DISPID_OnAddInsUpdate:   return OnAddInsUpdate(nullptr);
    case DISPID_OnStartupComplete: return OnStartupComplete(nullptr);
    case DISPID_OnBeginShutdown:  return OnBeginShutdown(nullptr);
    default: return DISP_E_MEMBERNOTFOUND;
    }
}

STDMETHODIMP Connect::OnConnection(IDispatch* pApplication, ext_ConnectMode /*ConnectMode*/, IDispatch* /*AddInInst*/, SAFEARRAY** /*custom*/)
{
    static bool s_pluginsRegistered = false;
    if (!s_pluginsRegistered)
    {
        OfficeAddIn::RegisterAllPlugins();
        s_pluginsRegistered = true;
    }
    OfficeAddIn::PluginHost::Instance().LoadAll(pApplication, OfficeAddIn::AppKind::None);

    m_hEngine = LoadLibraryW(L"PluginEngine.dll");
    if (!m_hEngine) return S_OK;
    m_pfnInitialize = (PFN_Initialize)GetProcAddress(m_hEngine, "PluginEngine_Initialize");
    m_pfnShutdown = (PFN_Shutdown)GetProcAddress(m_hEngine, "PluginEngine_Shutdown");
    if (m_pfnInitialize) m_pfnInitialize();
    return S_OK;
}

STDMETHODIMP Connect::OnDisconnection(ext_DisconnectMode /*RemoveMode*/, SAFEARRAY** /*custom*/)
{
    OfficeAddIn::PluginHost::Instance().UnloadAll();
    if (m_hEngine && m_pfnShutdown)
    {
        m_pfnShutdown();
        m_pfnShutdown = nullptr;
        FreeLibrary(m_hEngine);
        m_hEngine = nullptr;
    }
    return S_OK;
}

STDMETHODIMP Connect::OnAddInsUpdate(SAFEARRAY** /*custom*/) { return S_OK; }
STDMETHODIMP Connect::OnStartupComplete(SAFEARRAY** /*custom*/) { return S_OK; }
STDMETHODIMP Connect::OnBeginShutdown(SAFEARRAY** /*custom*/) { return S_OK; }
