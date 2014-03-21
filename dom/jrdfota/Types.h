/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_jrdfota_Types_h
#define mozilla_dom_jrdfota_Types_h

namespace mozilla {
namespace dom {
namespace jrdfota {
enum eJrdFotaAction {
  eNoAction,
  eGetNewPackage,
  eDownload,
  eCheckFirmwarm,
  ePause,
  eDelete,
  eInstall,
  eCheckInstallResult,
  eActionStatus_End
};

enum eFileHandleType {
  eCreate,
  eCheckExist,
  eRemove,
  eFileHandleType_End
};

#define FOTA_DEBUG true
#undef LOG
#if FOTA_DEBUG == true
  #ifdef MOZ_WIDGET_GONK
   #include <android/log.h>
   #define LOG(...) \
        __android_log_print(ANDROID_LOG_INFO, "JrdFotaGecko", ":--(%s:%d) %s: ",__FILE__, __LINE__, __func__); \
        __android_log_print(ANDROID_LOG_INFO, "JrdFotaGecko", __VA_ARGS__)
  #else
   #define LOG(...) \
        printf("JrdFotaGecko: --(%s:%d) %s:  ",__FILE__, __LINE__, __func__); \
        printf(__VA_ARGS__)
  #endif /*MOZ_WIDGET_GONK*/
#else
  #define LOG(...)
#endif /*DEBUG*/

} // namespace jrdfota
} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_jrdfota_Types_h
