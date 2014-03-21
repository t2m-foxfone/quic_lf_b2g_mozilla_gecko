/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
* WuChengLin add for webidl bindings on 2013-12-4
*/
#ifndef mozilla_dom_jrdfota_MozJrdFotaManager_h
#define mozilla_dom_jrdfota_MozJrdFotaManager_h

#include "mozilla/dom/jrdfota/PJrdFotaChild.h"
#include "domstubs.h"
#include "nsXULAppAPI.h"
#include "nsWrapperCache.h"
#include "mozilla/Attributes.h"
#include "nsPIDOMWindow.h"
#ifdef MOZ_WIDGET_GONK
#include "JrdFotaNative.h"
#else
#include "Types.h"
#endif

namespace mozilla {
namespace dom {

class IDOMJrdFotaCommonCb;
class IDOMJrdFotaDownloadProgressCb;
class IDOMJrdFotaGetNewPackageCb;

namespace jrdfota {


class JrdFotaChild : public PJrdFotaChild
{
public:
  JrdFotaChild();
  virtual ~JrdFotaChild();

protected:
  virtual bool RecvResponseValue(const JrdFotaResponseValue& response) MOZ_OVERRIDE;
  virtual bool RecvResetActionStatus() MOZ_OVERRIDE;
};

class MozJrdFotaManager MOZ_FINAL
                        : public nsISupports
                        , public nsWrapperCache
                      #ifdef MOZ_WIDGET_GONK
                        , public JrdFotaNative
                      #endif
{
public:
  NS_DECL_CYCLE_COLLECTING_ISUPPORTS
  NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_CLASS(MozJrdFotaManager)

  MozJrdFotaManager(nsPIDOMWindow* aWindow);
  virtual ~MozJrdFotaManager();

  nsPIDOMWindow* GetParentObject() const { return mWindow; }
  virtual JSObject* WrapObject(JSContext* aCx, JS::Handle<JSObject*> aScope) MOZ_OVERRIDE;

  // WebIDL
  NS_IMETHODIMP GetNewPackage(IDOMJrdFotaGetNewPackageCb &getCallback,
                IDOMJrdFotaCommonCb &statusCallback);

  NS_IMETHODIMP Download(IDOMJrdFotaDownloadProgressCb &downloadProgressCallback,
             IDOMJrdFotaCommonCb &statusCallback);

  NS_IMETHODIMP Pause(IDOMJrdFotaCommonCb &stopCallback);
  NS_IMETHODIMP Delete(IDOMJrdFotaCommonCb &deleteCallback);
  NS_IMETHODIMP Install(IDOMJrdFotaCommonCb &installCallback);
  NS_IMETHODIMP CheckInstallResult(IDOMJrdFotaCommonCb &checkIRCallback);
  NS_IMETHODIMP GetJrdFotaActionStatus(nsAString & aJrdFotaActionStatus);
  /*Added by tcl_baijian 2014-03-17 gaia select the storage devices begin*/
  NS_IMETHODIMP SelectSdcard(unsigned long type,bool immediately);
  /*Added by tcl_baijian 2014-03-17 gaia select the storage devices end*/

protected:
  virtual void SendNewPackageInfoReponse_Int(const char* pVersionName, int size, const char* pDescription);
  virtual void SendDownloadCompleteRateReponse_Int(int progressRate);
  virtual void SendCommonStatusReponse_Int(const char* pActionType, bool isSuccess, const char* pErrorType);
  virtual void SendResetActionStatus_Int();

private:
  PJrdFotaChild* _GetJrdFotaChild();
  void _SetJrdFotaActionStatus(eJrdFotaAction eAction);
  PJrdFotaChild* mJrdFotaChild;
protected:
  nsCOMPtr<nsPIDOMWindow> mWindow;

};

} // namespace jrdfota
} // namespace dom
} // namespace mozilla


#endif