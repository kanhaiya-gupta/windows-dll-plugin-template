#include <windows.h>
#include <stdio.h>
#include <new>
#include "connect/Connect.h"

static ULONG g_lockCount = 0;

#define CLSID_OfficeAddInCpp_Connect __uuidof(Connect)
static const CLSID CLSID_Connect = { 0xE5555555, 0x5555, 0x5555, { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 } };

class ClassFactory : public IClassFactory
{
public:
    ClassFactory() : m_refCount(1) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IClassFactory)
        {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() override
    {
        ULONG n = InterlockedDecrement(&m_refCount);
        if (n == 0) delete this;
        return n;
    }
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) override
    {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        Connect* p = new (std::nothrow) Connect();
        if (!p) return E_OUTOFMEMORY;
        HRESULT hr = p->QueryInterface(riid, ppv);
        p->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL fLock) override
    {
        if (fLock) InterlockedIncrement(&g_lockCount);
        else InterlockedDecrement(&g_lockCount);
        return S_OK;
    }
private:
    ULONG m_refCount;
};

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    if (rclsid != CLSID_Connect) return CLASS_E_CLASSNOTAVAILABLE;
    ClassFactory* p = new (std::nothrow) ClassFactory();
    if (!p) return E_OUTOFMEMORY;
    HRESULT hr = p->QueryInterface(riid, ppv);
    p->Release();
    return hr;
}

STDAPI DllCanUnloadNow(void)
{
    return (g_lockCount == 0) ? S_OK : S_FALSE;
}

static void RegisterClsid(HKEY hkeyRoot, const wchar_t* subkey, const CLSID& clsid, const wchar_t* desc, const wchar_t* path)
{
    wchar_t clsidStr[40];
    StringFromGUID2(clsid, clsidStr, 40);
    wchar_t keyPath[256];
    swprintf_s(keyPath, L"%s\\%s", subkey, clsidStr);
    HKEY hkey;
    if (RegCreateKeyExW(hkeyRoot, keyPath, 0, nullptr, 0, KEY_WRITE, nullptr, &hkey, nullptr) == 0)
    {
        RegSetValueExW(hkey, nullptr, 0, REG_SZ, (const BYTE*)desc, (DWORD)((wcslen(desc) + 1) * sizeof(wchar_t)));
        RegSetValueExW(hkey, L"InprocServer32", 0, REG_SZ, (const BYTE*)path, (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
        RegCloseKey(hkey);
    }
}

static void UnregisterClsid(HKEY hkeyRoot, const wchar_t* subkey, const CLSID& clsid)
{
    wchar_t clsidStr[40];
    StringFromGUID2(clsid, clsidStr, 40);
    wchar_t keyPath[256];
    swprintf_s(keyPath, L"%s\\%s", subkey, clsidStr);
    RegDeleteTreeW(hkeyRoot, keyPath);
}

STDAPI DllRegisterServer(void)
{
    wchar_t path[MAX_PATH];
    HMODULE hMod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&DllRegisterServer, &hMod) || GetModuleFileNameW(hMod, path, MAX_PATH) == 0)
        return E_FAIL;
    RegisterClsid(HKEY_CLASSES_ROOT, L"CLSID", CLSID_Connect, L"OfficeAddInCpp Connect", path);
    RegisterClsid(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID", CLSID_Connect, L"OfficeAddInCpp Connect", path);
    wchar_t clsidStr[40];
    StringFromGUID2(CLSID_Connect, clsidStr, 40);
    HKEY hkey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Classes\\OfficeAddInCpp.Connect", 0, nullptr, 0, KEY_WRITE, nullptr, &hkey, nullptr) == 0)
    {
        RegSetValueExW(hkey, nullptr, 0, REG_SZ, (const BYTE*)L"OfficeAddInCpp Connect", (DWORD)46);
        RegSetValueExW(hkey, L"Clsid", 0, REG_SZ, (const BYTE*)clsidStr, (DWORD)((wcslen(clsidStr) + 1) * sizeof(wchar_t)));
        RegCloseKey(hkey);
    }
    return S_OK;
}

STDAPI DllUnregisterServer(void)
{
    UnregisterClsid(HKEY_CLASSES_ROOT, L"CLSID", CLSID_Connect);
    UnregisterClsid(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID", CLSID_Connect);
    RegDeleteTreeW(HKEY_CURRENT_USER, L"Software\\Classes\\OfficeAddInCpp.Connect");
    return S_OK;
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID)
{
    (void)hInstance;
    (void)reason;
    return TRUE;
}
