/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_jrdfota_JrdFotaParent_h
#define mozilla_dom_jrdfota_JrdFotaParent_h

#include "mozilla/dom/jrdfota/PJrdFotaParent.h"

#ifdef MOZ_WIDGET_GONK
#include "JrdFotaNative.h"
#else
#include "Types.h"
#endif

namespace mozilla {
namespace dom {
namespace jrdfota {

class JrdFotaParent : public PJrdFotaParent
                   #ifdef MOZ_WIDGET_GONK
                    , public JrdFotaNative
                   #endif
                    , public nsISupports
{
public:
  NS_DECL_ISUPPORTS

  JrdFotaParent();
  virtual ~JrdFotaParent();

protected:
  virtual bool RecvGetNewPackage() MOZ_OVERRIDE;
  virtual bool RecvDownload() MOZ_OVERRIDE;
  virtual bool RecvPause() MOZ_OVERRIDE;
  virtual bool RecvDelete() MOZ_OVERRIDE;
  virtual bool RecvInstall() MOZ_OVERRIDE;
  virtual bool RecvCheckInstallResult() MOZ_OVERRIDE;

  virtual void SendNewPackageInfoReponse_Int(const char* pVersionName, int size, const char* pDescription);
  virtual void SendDownloadCompleteRateReponse_Int(int progressRate);
  virtual void SendCommonStatusReponse_Int(const char* pActionType, bool isSuccess, const char* pErrorType);
  virtual void SendResetActionStatus_Int();
};

} // namespace jrdfota
} // namespace dom
} // namespace mozilla

#endif /*mozilla_dom_jrdfota_JrdFotaParent_h*/
