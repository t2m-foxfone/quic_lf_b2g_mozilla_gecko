/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/ContentChild.h"
#include "MozJrdFotaManager.h"
#include "mozilla/dom/MozJrdFotaManagerBinding.h"
#include "Types.h"

using namespace mozilla;
using namespace mozilla::dom;
using namespace mozilla::dom::jrdfota;

nsString gJrdFotaActionStatus;

//Get new package callback
nsCOMPtr<IDOMJrdFotaGetNewPackageCb> gOnNewPkgInfoCb;

//Download progress
nsCOMPtr<IDOMJrdFotaDownloadProgressCb> gOnDownloadProgressCb;

//Common callback
nsCOMPtr<IDOMJrdFotaCommonCb> gOnCommonCb;

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(MozJrdFotaManager)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
  NS_WRAPPERCACHE_INTERFACE_MAP_ENTRY
NS_INTERFACE_MAP_END

NS_IMPL_CYCLE_COLLECTING_ADDREF(MozJrdFotaManager)
NS_IMPL_CYCLE_COLLECTING_RELEASE(MozJrdFotaManager)

NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE_1(MozJrdFotaManager, mWindow)

MozJrdFotaManager::MozJrdFotaManager(nsPIDOMWindow* aWindow)
    : mWindow(aWindow)
{
  /* member initializers and constructor code */
  SetIsDOMBinding();

  mJrdFotaChild = NULL;
  _SetJrdFotaActionStatus(eNoAction);
}

MozJrdFotaManager::~MozJrdFotaManager()
{
  /* destructor code */
  mJrdFotaChild = NULL;
  _SetJrdFotaActionStatus(eNoAction);
}

NS_IMETHODIMP
MozJrdFotaManager::GetJrdFotaActionStatus(nsAString & aJrdFotaActionStatus) {
  aJrdFotaActionStatus = gJrdFotaActionStatus;
  return NS_OK;
}

JSObject*
MozJrdFotaManager::WrapObject(JSContext* aCx, JS::Handle<JSObject*> aScope)
{
  return MozJrdFotaManagerBinding::Wrap(aCx, aScope, this);
}

NS_IMETHODIMP
MozJrdFotaManager::GetNewPackage(IDOMJrdFotaGetNewPackageCb &getCallback,
             IDOMJrdFotaCommonCb &statusCallback)
{
  LOG("enter GetNewPackage\n");

  //Set callback
  gOnNewPkgInfoCb = &getCallback;
  gOnCommonCb = &statusCallback;

  //Set action
  _SetJrdFotaActionStatus(eGetNewPackage);

  if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
       GetNewPackage_Int();
     #endif
  }
  else {
     LOG("child process\n");

     //Execute parent function
     #ifdef MOZ_WIDGET_GONK
       _GetJrdFotaChild()->SendGetNewPackage();
     #endif
  }

  return NS_OK;
}

NS_IMETHODIMP
MozJrdFotaManager::Download(IDOMJrdFotaDownloadProgressCb &downloadProgressCallback,
             IDOMJrdFotaCommonCb &statusCallback){
  LOG("enter Download\n");

  //Set callback
  gOnDownloadProgressCb = &downloadProgressCallback;
  gOnCommonCb = &statusCallback;

  //Set action
  _SetJrdFotaActionStatus(eDownload);
  /*Modified by tcl_baijian 2014-03-04 Download will be excuted in the main process begin*/
  if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
       Download_Int();
     #endif
  }
  else {
      LOG("child process\n");
     //Execute parent function
     #ifdef MOZ_WIDGET_GONK
      _GetJrdFotaChild()->SendDownload();
     #endif
  }
  /*Modified by tcl_baijian 2014-03-04 Download will be excuted in the main process end*/
  return NS_OK;
}

NS_IMETHODIMP
MozJrdFotaManager::Pause(IDOMJrdFotaCommonCb &stopCallback){
  LOG("enter Pause\n");

  //Set callback
  gOnCommonCb = &stopCallback;

  //Set action
  _SetJrdFotaActionStatus(ePause);
  /*Modified by tcl_baijian 2014-03-04 pause will be excuted in the main process begin*/
  if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
       Pause_Int();
     #endif
  }
  else {
      LOG("child process\n");
      //Execute parent function
     _GetJrdFotaChild()->SendPause();
  }
  /*Modified by tcl_baijian 2014-03-04 pause will be excuted in the main process end*/
  return NS_OK;
}

NS_IMETHODIMP
MozJrdFotaManager::Delete(IDOMJrdFotaCommonCb &deleteCallback){
  LOG("enter Delete\n");

  //Set callback
  gOnCommonCb = &deleteCallback;

  //Set action
  _SetJrdFotaActionStatus(eDelete);
  /*Modified by tcl_baijian 2014-03-04 Delete will be excuted in the main process begin*/
  if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
       Delete_Int();
     #endif
  }
  else {
      LOG("child process\n");
     //Execute parent function
     _GetJrdFotaChild()->SendDelete();
  }
  /*Modified by tcl_baijian 2014-03-04 Delete will be excuted in the main process end*/
  return NS_OK;
}

NS_IMETHODIMP
MozJrdFotaManager::Install(IDOMJrdFotaCommonCb &installCallback)
{
  LOG("enter Install\n");

  //Set callback
  gOnCommonCb = &installCallback;

  //Set action
  _SetJrdFotaActionStatus(eInstall);
  /*Modified by tcl_baijian 2014-03-04 install will be excuted in the main process begin*/
  if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
       Install_Int();
     #endif
  }
  else {
      LOG("child process\n");
      //Execute parent function
      _GetJrdFotaChild()->SendInstall();
  }
  /*Modified by tcl_baijian 2014-03-04 install will be excuted in the main process end*/
  return NS_OK;
}

NS_IMETHODIMP
MozJrdFotaManager::CheckInstallResult(IDOMJrdFotaCommonCb &checkIRCallback)
{
  LOG("enter CheckInstallResult\n");

  //Set callback
  gOnCommonCb = &checkIRCallback;

  //Set action
  _SetJrdFotaActionStatus(eCheckInstallResult);

  if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
       CheckInstallResult_Int();
     #endif
  }
  else {
     LOG("child process\n");

     //Execute parent functio
     _GetJrdFotaChild()->SendCheckInstallResult();
  }

  return NS_OK;
}
/*Added by tcl_baijian 2014-03-17 gaia select the storage devices only in main process begin*/
NS_IMETHODIMP
MozJrdFotaManager::SelectSdcard(unsigned long type,bool immediately)
{
   LOG("enter SelectSdcard\n");
   if(GeckoProcessType_Default == XRE_GetProcessType()) {
     LOG("main process\n");
     #ifdef MOZ_WIDGET_GONK
      SelectSdcard_Int(type,immediately);
     #endif
  }
  else {
     LOG("child process\n");

     //Execute parent functio

  }

  return NS_OK;
}
/*Added by tcl_baijian 2014-03-17 gaia select the storage devices only in main process end*/
void
MozJrdFotaManager::SendNewPackageInfoReponse_Int(const char* pVersionName, int size, const char* pDescription)
{
   LOG("versionName: %s, size: %d, description: %s", pVersionName, size, pDescription);

   nsString versionName;
   nsString description;

   if(NULL != pVersionName) {
     versionName.AssignASCII(pVersionName);
   }
   if(NULL != pDescription) {
     description.AssignASCII(pDescription);
   }
   ErrorResult aRv;
   gOnNewPkgInfoCb->HandleEvent(versionName, size, description, aRv);
}

void
MozJrdFotaManager::SendDownloadCompleteRateReponse_Int(int progressRate) {
    //Currently, no case use this function
    /*Modified by tcl_baijian 2014-03-04 SendDownloadCompleteRate will be excuted in the main process begin*/
    LOG("download progress rate:%d",progressRate);
    ErrorResult aRv;
    gOnDownloadProgressCb->HandleEvent(progressRate,aRv);
    /*Modified by tcl_baijian 2014-03-04 SendDownloadCompleteRate will be excuted in the main process end*/
}

void
MozJrdFotaManager::SendCommonStatusReponse_Int(const char* pActionType, bool isSuccess, const char* pErrorType)
{
   LOG("actionType: %s, isSuccess: %d, errorType: %s", pActionType, isSuccess, pErrorType);

   nsString actionType;
   nsString errorType;

   if(NULL != pActionType) {
     actionType.AssignASCII(pActionType);
   }

   if(NULL != pErrorType) {
     errorType.AssignASCII(pErrorType);
   }
   ErrorResult aRv;
   gOnCommonCb->HandleEvent(actionType, isSuccess, errorType, aRv);
}

void
MozJrdFotaManager::SendResetActionStatus_Int()
{
  LOG("enter\n");
  gJrdFotaActionStatus.AssignASCII("NoAction");
}

void
MozJrdFotaManager::_SetJrdFotaActionStatus(eJrdFotaAction eAction)
{
    const char* actionStr = NULL;

    switch(eAction) {
       case eNoAction:
          actionStr="NoAction";
          break;

       case eGetNewPackage:
          actionStr="GetNewPackage";
          break;

       case eDownload:
          actionStr="Download";
          break;

       case eDelete:
          actionStr="Delete";
          break;

       case eInstall:
          actionStr="Install";
          break;

       case ePause:
          actionStr="Pause";
          break;

       case eCheckInstallResult:
          actionStr="CheckInstallResult";
          break;

       default :
          MOZ_ASSERT(0);
          break;
    }
    LOG("actionStr: %s\n", actionStr);
    gJrdFotaActionStatus.AssignASCII(actionStr);
}

PJrdFotaChild*
MozJrdFotaManager::_GetJrdFotaChild() {
  if (!mJrdFotaChild) {
    mJrdFotaChild = ContentChild::GetSingleton()->SendPJrdFotaConstructor();
  }

  return mJrdFotaChild;
}

/** **********************************************************
  *                  JrdFotaChild Class                       *
 *************************************************************/
JrdFotaChild::JrdFotaChild() {
  LOG("JrdFotaChild create object\n");
  MOZ_COUNT_CTOR(JrdFotaChild);
}

JrdFotaChild::~JrdFotaChild() {
  LOG("JrdFotaChild release object\n");
  MOZ_COUNT_DTOR(JrdFotaChild);
}

bool
JrdFotaChild::RecvResponseValue(const JrdFotaResponseValue& aValue)
{
  LOG("RecvResponseValue\n");
  switch (aValue.type()) {

    case JrdFotaResponseValue::TNewPackageInfoResponse:
    {
      NewPackageInfoResponse r = aValue;
      ErrorResult aRv;
      gOnNewPkgInfoCb->HandleEvent(r.versionName(), r.size(), r.description(), aRv);
      break;
    }

    case JrdFotaResponseValue::TDownloadCompletedRateResponse:
    {
      DownloadCompletedRateResponse r = aValue;
      ErrorResult aRv;
      gOnDownloadProgressCb->HandleEvent(r.progressRate(), aRv);
      break;
    }

    case JrdFotaResponseValue::TCommonStatusResponse:
    {
      CommonStatusResponse r = aValue;
      ErrorResult aRv;
      gOnCommonCb->HandleEvent(r.actionType(), r.isSuccess(), r.errorType(), aRv);
      break;
    }
    default:
    {
      NS_RUNTIMEABORT("not reached");
      break;
    }
  }
  return true;
}

bool
JrdFotaChild::RecvResetActionStatus()
{
  LOG("enter\n");
  gJrdFotaActionStatus.AssignASCII("NoAction");
  return true;
}