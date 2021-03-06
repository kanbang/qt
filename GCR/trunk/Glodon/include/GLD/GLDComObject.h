/*************************************************************************
*                                                                        *
*   广联达COM对象基类GObject定义单元 H                                   *
*                                                                        *
*   设计：Zhangsk 2012.05.23                                             *
*   备注：                                                               *
*   审核：                                                               *
*                                                                        *
*   Copyright (c) 2012-2013 Glodon Corporation                           *
*                                                                        *
*************************************************************************/


#ifndef GLDCOMOBJECT_H
#define GLDCOMOBJECT_H

#pragma warning(disable:4786 )
#include <assert.h>
#include <map>
#include <string>

#ifdef WIN32
#    include <unknwn.h>
#else
#    include "GLDObjBase.h"
#    include "GLDUnknwn.h"
#    include "GLDGuidDef.h"
#endif

using namespace std;


interface IGNonDelegatingUnknown
{
    virtual LONG STDMETHODCALLTYPE NonDelegatingQueryInterface(REFIID, void* *) = 0;
    virtual ULONG STDMETHODCALLTYPE NonDelegatingAddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE NonDelegatingRelease(void) = 0;
};

typedef IGNonDelegatingUnknown *PGNDUNKNOWN;

class GLDCOMMONSHARED_EXPORT GObject : public IGNonDelegatingUnknown
{
private:
    const LPUNKNOWN pUnknown_; /* Owner of this object */

protected:                      /* So we can override NonDelegatingRelease() */
    volatile LONG cRef_;       /* Number of reference counts */

public:
    GObject();
    virtual ~GObject() {}

    /* Return the owner of this object */
    LPUNKNOWN GetOwner() const 
    {
        return pUnknown_;
    };

    /* Non delegating unknown implementation */
    virtual LONG STDMETHODCALLTYPE NonDelegatingQueryInterface(REFIID, void **);
    virtual ULONG STDMETHODCALLTYPE NonDelegatingAddRef();
    virtual ULONG STDMETHODCALLTYPE NonDelegatingRelease();

#ifdef GDEBUG
    LONG getRefCount() { return cRef_; }
    void checkVaild() { delete new GObject(); }
#endif
};

/* The standard InterlockedXXX functions won't take volatiles */
static inline LONG WINAPI XInterlockedIncrement( volatile LONG * plong )
{
#ifndef WIN32
    return *plong = *plong + 1;
#else
    return InterlockedIncrement( const_cast<LONG*>( plong ) ); 
#endif
}

static inline LONG WINAPI XInterlockedDecrement( volatile LONG * plong )
{
#ifndef WIN32
    return *plong = *plong - 1;
#else
    return InterlockedDecrement( const_cast<LONG*>( plong ) ); 
#endif
}

#define DECLARE_IUNKNOWN                                                        \
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) {            \
        return GetOwner()->QueryInterface(riid,ppv);                            \
    };                                                                            \
    ULONG STDMETHODCALLTYPE AddRef() {                                            \
        return GetOwner()->AddRef();                                            \
    };                                                                            \
    ULONG STDMETHODCALLTYPE Release() {                                            \
        return GetOwner()->Release();                                            \
    };

/* Return an interface pointer to a requesting client
   performing a thread safe AddRef as necessary */

STDAPI GetGInterface(LPUNKNOWN pUnk, void **ppv);
#endif  // GLDCOMOBJECT_H
