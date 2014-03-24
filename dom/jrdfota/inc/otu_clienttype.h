#ifndef _COMMON_INC_OTU_CLIENTTYPE_H_
#define _COMMON_INC_OTU_CLIENTTYPE_H_

/// OTU Client type
///
/// This enum lists all the client types of OTU.
///
/// CAUTION: DO NOT MODIFY THE VALUE OF ENUM ALREADY SET.
///          IN ORDER TO INSURE BACKWARD COMPATIBILITY,
///           ONLY NEW VALUES INSERTION SHOULD BE DONE.

typedef enum
{
    /*
       No keys are associated with this one. Just to make sure
       that 0 is used and cannot be used by someone else. We might
       need a 0 someday.
    */
    eCT_No_Type = 0,

    /*
       For SUApp application which is used by FOTA/OTU IT team for tests.
    */
    eCT_TCT_Suapp = 1,

     /*
      FOTA in Android 3G Mobile.
     */
    eCT_TCT_Android_3G_Mobile_Build_In_Fota = 10,

     /*
      FOTA in Android Tablet.
     */
    eCT_TCT_Android_Tablet_Build_In_Fota = 20,
    eCT_TCT_Android_Tablet_Fota = 21,
     /*
      FOTA in Android CDMA Mobile.
     */
    eCT_TCT_Android_CDMA_Mobile_Build_In_Fota = 30,

     /*
      OTU Client for PC
     */
    eCT_TCT_Windows_OTU = 1000,

     /*
      OTU Client for Mac
     */
    eCT_TCT_Mac_OTU = 1001,

     /*
      OTU Client for Linux
     */
    eCT_TCT_Linux_OTU = 1002,

     /*
      OTU DataCard Client for PC
     */
    eCT_TCT_Windows_OTU_DataCard = 1003,

    /*
      OTU DataCard Client for Mac
     */
    eCT_TCT_Mac_OTU_DataCard = 1004,

    /*
      OTU DataCard Client for Linux
     */
    eCT_TCT_Linux_OTU_DataCard = 1005,

    /*
      FOTA for MIFI devices.
    */
    eCT_TCT_Mifi_FOTA = 1006,
    /*
      Tool used by repair centers.
     */
    eCT_TCT_Sugar = 1010,

    /*
      Tool used by CTS and operators during product certification/qualification.
     */
    eCT_TCT_External_TeleWeb = 1020,

    /*
      Pc suit used by user.
    */
    eCT_TCT_PCsuit = 1030,
    /*
       OTU like Tool developed by M7 (third party company).
    */
    eCT_M7_Fota_For_Android_Mobile = 10000

}tOTUClientType;

#endif

