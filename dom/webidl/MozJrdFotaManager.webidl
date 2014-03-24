/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.w3.org/TR/battery-status/
 *
 * Copyright © 2012 W3C® (MIT, ERCIM, Keio), All Rights Reserved. W3C
 * liability, trademark and document use rules apply.
 */

 callback interface IDOMJrdFotaGetNewPackageCb
 {
   /**
    * PARAMETERS:
    *    versionName (IN):  the update package name, example: 294ZZ
    *    size (IN):  Total update file size in bytes
    *    description(IN):  new package description
   */
   void handleEvent( DOMString versionName,
                     unsigned long size,
                     DOMString description);
 };

 callback interface IDOMJrdFotaDownloadProgressCb
 {
   /**
    * PARAMETERS:
    *    progressRate (IN):  A value between 0 and 100 is returned.
   */
   void handleEvent( unsigned long progressRate);
 };

 callback interface IDOMJrdFotaCommonCb
 {
   /**
    * PARAMETERS:
    *    actionType(IN):  GetNewPackage, Download, Pause, Delete, CheckInstallResult
    *    isSuccessed(IN):  common action success or fail
    *    errorType(IN):  error_type
   */
   void handleEvent( DOMString actionType,
                     boolean isSuccessed,
                     DOMString errorType);
 };

interface MozJrdFotaManager
{
  /**
   * Get the package info of the latest updated package.
  */
  void getNewPackage( IDOMJrdFotaGetNewPackageCb getCallback,
                                                IDOMJrdFotaCommonCb statusCallback);

  /**
   * Download the update package or resume the last download
   * PARAMETERS:
   *    statusCallback (IN):  100% of request has been completed, we will check the fireware(eg, check file size).
   *          Set the download status with  success  or not according to the checking result.
  */
  void download( IDOMJrdFotaDownloadProgressCb  downloadProgressCallback,
                                                IDOMJrdFotaCommonCb statusCallback);
  /**
   * Stop downloading process
  */
  void pause( IDOMJrdFotaCommonCb pauseCallback);

  /**
   * After downloaded the update package, we will save the update file to hard disk
   * App can call this interface to deleting the update file
  */
  void delete( IDOMJrdFotaCommonCb deleteCallback);

  /**
   * Upgrade handset
   * The phone will restart for installation when call this interface.
   * So app should note the user to save their data.
  */
  void install( IDOMJrdFotaCommonCb installCallback);

  /**
   * After installed, system will write the install result into a file.
   * This interface can read the result and notify the app.
  */
  void checkInstallResult( IDOMJrdFotaCommonCb checkIRCallback);

 /**
  *Fota Notify gecko to select external or internal sdcard.
  *
 */
  void selectSdcard(unsigned long type,boolean immediately);

  readonly attribute DOMString JrdFotaActionStatus;
};
