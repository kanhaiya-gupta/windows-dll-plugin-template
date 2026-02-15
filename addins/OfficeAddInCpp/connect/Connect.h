#pragma once

#include "connect/IDTExtensibility2.h"

class Connect : public IDTExtensibility2
{
public:
    Connect();
    virtual ~Connect();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    // IDispatch
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

    // IDTExtensibility2
    STDMETHODIMP OnConnection(IDispatch* Application, ext_ConnectMode ConnectMode, IDispatch* AddInInst, SAFEARRAY** custom) override;
    STDMETHODIMP OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY** custom) override;
    STDMETHODIMP OnAddInsUpdate(SAFEARRAY** custom) override;
    STDMETHODIMP OnStartupComplete(SAFEARRAY** custom) override;
    STDMETHODIMP OnBeginShutdown(SAFEARRAY** custom) override;

private:
    ULONG m_refCount;
    HMODULE m_hEngine;
    typedef int (__cdecl* PFN_Initialize)(void);
    typedef void (__cdecl* PFN_Shutdown)(void);
    PFN_Initialize m_pfnInitialize;
    PFN_Shutdown m_pfnShutdown;
};
