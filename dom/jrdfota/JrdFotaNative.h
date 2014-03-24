/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_jrdfota_JrdFotaNative_h
#define mozilla_dom_jrdfota_JrdFotaNative_h

//OTU header file
#include "inc/mytypes.h"
#include "inc/otu_struct.h"
#include "inc/otu_status.h"
#include "inc/otu_dll_encrypt.h"
#include "inc/otu_upgrade_mode.h"
#include "inc/otu_clienttype.h"
#include "inc/otu_op.h"
#include "inc/gisusdll.h"
#include "inc/otu_options.h"

#include "openssl/dsa.h"
#include "openssl/sha.h"

#include "nsITimer.h"
#include "Types.h"
#include <string>


using std::string;

namespace mozilla {
namespace dom {
namespace jrdfota {

class JrdFotaNative
{
public:
  JrdFotaNative();
  virtual ~JrdFotaNative();

protected:
  /**
   * For Child Recv function
  */
  void GetNewPackage_Int();
  void Download_Int();
  void Pause_Int();
  void Delete_Int();
  void Install_Int();
  void CheckInstallResult_Int();
  /*Added by tcl_baijian 2014-03-17 select the storage devices begin*/
  void SelectSdcard_Int(unsigned long type,bool immediately);
  /*Added by tcl_baijian 2014-03-17 select the storage devices end*/

  virtual void SendNewPackageInfoReponse_Int(const char* pVersionName, int size, const char* pDescription) = 0;
  virtual void SendDownloadCompleteRateReponse_Int(int progressRate) = 0;
  virtual void SendCommonStatusReponse_Int(const char* pActionType, bool isSuccess, const char* pErrorType) = 0;
  virtual void SendResetActionStatus_Int() = 0;

private:
  /**
   * Jrd Fota function
  */
  void _getTagInfoFromDescription(const char* pDescription, const char* pTagName, string& strTagInfo);
  bool _fileHandle(const char* pFilePath, eFileHandleType handleType);
  void _fota_send_common_status_response(const char* pActionType, bool isSuccess, tOTUStatus res);
  void _fota_send_new_package_info_response(tOTUSessionInfo* pSessionInfo);
  const char* _fota_get_error_type_str(tOTUStatus res);
  void _fota_act_after_report(void);
  static void _fota_report_timer_hdlr(nsITimer *aTimer, void *aClosure);
  tOTUStatus _fota_report_to_otu_server(tOTUOperation operation,
                         tOTUStatus status,
                         tOTUSessionInfo *session_info,
                         tUInt32 statistics_data_size,
                         void *statistics_data);
  tOTUStatus _fota_Init(void);
  void _fota_shut_down(void);
  /*Modified by tcl_baijian 2014-03-04 reset the lib begin*/
  void _fota_reset(void);
  /*Modified by tcl_baijian 2014-03-04 reset the lib end*/
  tOTUStatus _fota_get_device_current_params(char  *pSwVersion, char *pImei, char *pCmRefer);
  tOTUStatus _fota_create_device_info(void);
  static void _fota_get_new_package_timer_hdlr(nsITimer *aTimer, void *aClosure);
  bool _fota_delete_old_sessions(tOTUSessionInfo *pNewSessionInfo);
  static void _fota_download_timer_hdlr(nsITimer *aTimer, void *aClosure);
  void _fota_download_start(void);
  static void _fota_resume_session_timer_hdlr(nsITimer *aTimer, void *aClosure);
  void _fota_resume_session(void);
  static void _fota_get_session_info_timer_hdlr(nsITimer *aTimer, void *aClosure);
  void _fota_get_session_info(void);
  tOTUStatus _fota_store_downloaded_file(tOTUSessionInfo* pSessionInfo);
  void _fota_act_after_delete_session(tOTUStatus delete_status);
  static void _fota_delete_session_timer_hdlr(nsITimer *aTimer, void *aClosure);
  void _fota_delete_session(tOTUSessionInfo* session_info);
  void _fota_check_and_store(tOTUSessionInfo* session_info);
  static void _fota_pause_timer_hdlr(nsITimer *aTimer, void *aClosure);
  bool _fota_delete_local_update_file(void);

  /**
   * debug log function
  */
  void _fota_device_info_log(tOTUDeviceInfo* pDeviceInfo);
  void _fota_file_info_log(tOTUFile* pFileInfo);
  void _fota_file_set_log(tOTUFileSet* pFileSet);
  void _fota_session_info_log(tOTUSessionInfo* pSessionInfo, int nb_session);

  /**
   * Call back functions for otu_init
  */
  static int _encrypt_data(tByte *out_str, tUInt32 *out_str_len,
                tUInt32 max_out_len, const tByte *in_str, tUInt32 in_str_len);

  static int _get_key_id(tByte *out_str, tUInt32 *out_str_len,
                tUInt32 max_out_len, const tByte *in_str, tUInt32 in_str_len);

  static int _get_client_version_id(tByte *out_str, tUInt32 *out_str_len,
                tUInt32 max_out_len, const tByte *in_str, tUInt32 in_str_len);

  /**
   *Use the deviceinfo, sessioninfo pointer to access the update data.
   * Notes: the memory is managed by the otu lib.
   * But we should execute the otu_shutdown function to free these buffer.
  */
  tOTUDeviceInfo* pDeviceInfo;
  static tOTUSessionInfo *pSessionInfo;

  nsCOMPtr<nsITimer> mCommonTimer;
  nsCOMPtr<nsITimer> mPauseTimer;
  unsigned short commonTimeOut;
  unsigned short mPauseTimeOut;

  //For async handler
  static bool bGetNewPackaged;
  eJrdFotaAction eActionType;
  tOTUStatus deliverStatus;
  int previousRate;
  bool bDownloadStarted;
  /*Modified by tcl_baijian 2014-03-04 make sure the lib  olny once begin*/
  bool bInitial;
  /*Modified by tcl_baijian 2014-03-04 make sure the lib  olny once end*/
};

} // namespace jrdfota
} // namespace dom
} // namespace mozilla

#endif /*mozilla_dom_jrdfota_JrdFotaNative_h*/
