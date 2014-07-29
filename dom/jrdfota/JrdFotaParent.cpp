/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "JrdFotaParent.h"

namespace mozilla {
namespace dom {
namespace jrdfota {

  NS_INTERFACE_MAP_BEGIN(JrdFotaParent)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
  NS_INTERFACE_MAP_END
  NS_IMPL_ADDREF(JrdFotaParent)
  NS_IMPL_RELEASE(JrdFotaParent)

JrdFotaParent::JrdFotaParent() {
  LOG("enter\n");
  MOZ_COUNT_CTOR(JrdFotaParent);
}

JrdFotaParent::~JrdFotaParent() {
  LOG("enter\n");
  MOZ_COUNT_DTOR(JrdFotaParent);
}

bool
JrdFotaParent::RecvGetNewPackage() {
  LOG("enter\n");

#ifdef MOZ_WIDGET_GONK
  GetNewPackage_Int();
#endif

  return true;
}

bool
JrdFotaParent::RecvDownload() {
  LOG("enter\n");

#ifdef MOZ_WIDGET_GONK
  Download_Int();
#endif

  return true;
}

bool
JrdFotaParent::RecvPause() {
  LOG("enter\n");

#ifdef MOZ_WIDGET_GONK
  Pause_Int();
#endif

  return true;
}

bool
JrdFotaParent::RecvDelete() {
  LOG("enter\n");

#ifdef MOZ_WIDGET_GONK
  Delete_Int();
#endif

  return true;
}

bool
JrdFotaParent::RecvInstall() {
  LOG("enter\n");

#ifdef MOZ_WIDGET_GONK
  Install_Int();
#endif

  return true;
}

bool
JrdFotaParent::RecvCheckInstallResult() {
  LOG("enter\n");

#ifdef MOZ_WIDGET_GONK
  CheckInstallResult_Int();
#endif

  return true;
}

void
JrdFotaParent::SendNewPackageInfoReponse_Int(const char* pVersionName, int size, const char* pDescription) {
   LOG("versionName: %s, size: %d, description: %s", pVersionName, size, pDescription);

   nsString versionName;
   nsString description;

   if(NULL != pVersionName) {
     versionName.AssignASCII(pVersionName);
   }
   if(NULL != pDescription) {
     description.AssignASCII(pDescription);
   }

   NewPackageInfoResponse pkgInfo(versionName, size, description);
   PJrdFotaParent::SendResponseValue(pkgInfo);
}
void
JrdFotaParent::SendDownloadCompleteRateReponse_Int(int progressRate) {
   LOG("progressRate = %d", progressRate);

   DownloadCompletedRateResponse  sProgressRate(progressRate);
   PJrdFotaParent::SendResponseValue(sProgressRate);
}
void
JrdFotaParent::SendCommonStatusReponse_Int(const char* pActionType, bool isSuccess, const char* pErrorType) {
   LOG("actionType: %s, isSuccess: %d, errorType: %s", pActionType, isSuccess, pErrorType);

   nsString actionType;
   nsString errorType;

   if(NULL != pActionType) {
     actionType.AssignASCII(pActionType);
   }

   if(NULL != pErrorType) {
     errorType.AssignASCII(pErrorType);
   }
   CommonStatusResponse commonStatus(actionType, isSuccess, errorType);
   PJrdFotaParent::SendResponseValue(commonStatus);
}

void
JrdFotaParent::SendResetActionStatus_Int() {
   LOG("enter\n");

   PJrdFotaParent::SendResetActionStatus();
}

} // namespace jrdfota
} // namespace dom
} // namespace mozilla mozilla
