#ifndef _OTU_STRUCT_H_
#define _OTU_STRUCT_H_

/* Below are the maximum limit */
#define OTU_MAX_DEVICE_ID_LEN            30
#define OTU_MAX_FILE_SET_TYPE_NAME_LEN   30
#define OTU_MAX_COM_REF_NAME_LEN         30
#define OTU_MAX_VERSION_LEN              30
#define OTU_MAX_FILENAME_LEN             50
#define OTU_MAX_FILE_CHECKSUM_LEN        41
#define OTU_MAX_SESSION_NAME_LEN         (OTU_MAX_FILE_SET_TYPE_NAME_LEN+OTU_MAX_COM_REF_NAME_LEN+OTU_MAX_VERSION_LEN+OTU_MAX_VERSION_LEN+10)
#define OTU_MAX_DESCRIPTION_LEN          65537 // 64KB + 1
#define OTU_MAX_FILE_NB_IN_FILE_SET      100
#define OTU_MAX_SPECIAL_OP_DATA_LEN      1024

#include "otu_upgrade_mode.h"
#include "otu_clienttype.h"
#include "otu_options.h"
typedef struct otu_device_info
{
    /**
       This field uniquely identifies the device.
       For GSM phone, it should be IMEI.
       For CDMA, it should be ESN.
       etc
    */
    char device_id[OTU_MAX_DEVICE_ID_LEN+1];

    /**
       This field identifies which file set to look at.
       For GSM handset firmware, it is the handset model name
       like 'OT-800' for example. But it can also be other type
       like 'OT-800/DRIVER' or 'OT-800/PC SUITE' which identifies
       the PC drivers or PC application for the OT-800 handset.
    */
    char file_set_type[OTU_MAX_FILE_SET_TYPE_NAME_LEN+1];

    /**
       Commercial reference of the device.

       Examples:
       C825X-2ARGES1
       980X-2BALFR1-S90
    */
    char commercial_reference[OTU_MAX_COM_REF_NAME_LEN+1];

    /**
       Current version code of the device to update.

       Example:
       28AFR
    */
    char version_code[OTU_MAX_VERSION_LEN+1];


    /**
       Currently supported upgrade modes. This parameter should
       reflect the effective modes that is supported by the
       device.

       If the device supports mode than one mode, then this below
       parameter contain a ORed value from the basic values defined
       by the tOTUUpgradeMode type.
    */
    tOTUUpgradeMode upgrade_mode;

    /**
       The type of client.
    */
    tOTUClientType client_type;
    /**
       The report info from client.
    */
    tOTUReport_info *report_info;
} tOTUDeviceInfo;



typedef struct otu_file
{
    /**
       File unique identifier into the whole OTU system.

       This ID allows the system to precisely identify a given file.
    */
    tUInt32 file_id;

    /**
       File name.
    */
    char file_name[OTU_MAX_FILENAME_LEN+1];

    /**
       Total file size in bytes.
    */
    unsigned int size;

    /**
        Total number of chunks.
    */
    unsigned int nb_chunk;

    /**
       Current file version code.

    */
    char version[OTU_MAX_VERSION_LEN+1];

    /**
       SHA1 Checksum of the current file under binary format.
       (Size is 20 bytes for SHA1).
    */
    char checksum[OTU_MAX_FILE_CHECKSUM_LEN+1];
}  tOTUFile;

typedef struct otu_file_set
{
    /**
       Total number of files contained into the current fileset.
    */
    unsigned int file_nb;

    /**
       Description of the current fileset.
    */
    char description[OTU_MAX_DESCRIPTION_LEN+1];

    /**
       Target (or to) version code. This field MUST always be specified.
       It contains the resulting version that the device will have once the
       update contained in the current file set is applied.

       Example:
       294ZZ
    */

    char to_version_code[OTU_MAX_VERSION_LEN+1];

    /**
       Upgrade mode (whether FULL or INCREMENTAL).

       If INCREMENTAL, then the current file set only contains
       the difference between a starting version (specified in
       the 'from_version_code' field below) and a terget
       version (specified in the 'to_version_code' field above).
    */
    tOTUUpgradeMode upgrade_mode;

    /**
       Current (or from) version code.

       This field is required/mandatory when applying an INCREMENTAL
       update as it helps insuring that the INCREMENTAL update is
       applied from the right starting version.

       Example:
       28BZZ
    */
    char from_version_code[OTU_MAX_VERSION_LEN+1];

    /**
       File list of the current fileset.

       The below pointer will point on an array of tOTUFile object.
       The field 'file_nb' contains the number of objects that can
       be found in this array.

    */
    tOTUFile *file_list;

    /**
       Mask of files to be downloaded. This byte array contains
       as much byte as files in the file set (file_nb).

       By default, the value is 1 for each file meaning they
       need to be downloaded. If set to 0, then the corresponding
       file won't be downloaded when used with 'otu_request_download_start'.
    */
    char *file_mask;

    /**
       System internal version.
    */
    tUInt32 internal_version;

} tOTUFileSet;


/**
   Session object simply links an instance of device information
   object with a file set.
*/
typedef struct session_info
{
    char name[OTU_MAX_SESSION_NAME_LEN+1];
    tOTUDeviceInfo *device_info;
    tOTUFileSet *file_set;
} tOTUSessionInfo;


/**
   OTU Special Operation Type.

   It defines all the possible special operation type a device may receive from the server.

   See tGOTUSpecialOperation definition below for details.

 */
typedef enum
{
    eGSOT_Unknown=0,

    /**
       NAME: DisableAutoCheck

       FUNCTION SUMMARY:
       Disable the automatic and periodic FOTA checking. This will insure our capacity to prevent
       devices to use our FOTA servers resources.

       DETAILS:
       When a device receive such SpecialOp, then it should modify the settings of the FOTA client
       to 'Manual Check' so that Automatic and Periodic FOTA checking is stopped. In case a timer
       has been armed or an event has been scheduled in order to perform the next FOTA check, then
       it should be deleted in order to prevent any further checking.

       DATA:
       None.

       CAUTION: Use this message carefully as it is not possible to reenable the auto check since
       the devices will stop accessing the FOTA servers.

    */
    eGSOT_DisableAutoCheck=1,

    /**
       NAME: ChangeAutoCheckPeriod

       FUNCTION SUMMARY:
       Enable and/or change the period of the automatic FOTA checking.

       DETAILS:
       When a device receive such SpecialOp, then it should modify the settings of the FOTA client
       to 'Automatic Check' with the period found into the data of the operation. If the device
       had this feature previously disabled (manual check), then an alarm should be armed or an
       event should be created in order for the device to perform the automatic check in the
       given new period.
       If the device doesn't support the given period, then it should select the closest period
       value of the oneprovided. If there is no closest period value or if no data is found, then
       the device should use the first period found into the list that is not 'Manual Check'.

       DATA:

       Period of the automatic check to set.

       Format of the data is as follow:

       "{'format version':1,'data':[{'period':604800}]}"

       where
         * 'format version' is the format version of the data. Format may vary with version for future
     expansion.

     * 'data' is a list of items. For EnableAutoCheck, only one item should be available. If more
     than one is provided, then only the first one should be taken into consideration.

     * 'period': Period of automatic check in seconds.

       NOTE: As the device MUST contact the server to receive such Special Op, it is not possible
       to use this message to repair a previous mistake that disabled the automatic checking of
       a whole operator or country. This message is more used to modify the frequency/period of
       checking of currently active devices.

     */
    eGSOT_ChangeAutoCheckPeriod=2,

     /**
       NAME: ForceUpdate

       FUNCTION SUMMARY:


       DETAILS:
       When a device receives such SpecialOp together with an available version,
       then the device should SILENTLY download the package via the authorized
       connection type and automatically upgrade the phone firmware WITHOUT end user action and
       notification / information. Once the firmware is entirely downloaded, then the device should
       initiate an upgrade operation.It is the FOTA client responsibility to select the time when to perform the reboot (we can think
       that the FOTA client will initiate the reboot only when the phone is in idle mode for more than
       2 min for example or during the night while the end user is sleeping).
       The data consists of a list of connection type/flag record as detailed below:
       <connection type name> field contains the name of the connection type.
       The possible connection types are:
       3g : This type identifies the FOTA automatic check of the standard data connection.
       wifi : This type identifies the FOTA automatic check of the WIFI connection.
       Devices should recognize these strings and behaves accordingly.
       If the connection type name is not recognized by the device, then it should be ignored.
       <download flag> is a Boolean. If set to ¡®false¡¯, then the device should NOT try to download the update
       package from the corresponding connection. If set to ¡®true¡¯, then the device should start downloading the
       update package once such corresponding connection becomes available.

       DATA:

       Example:
       {
            'format version':1,
            'data':
            [
              {'3g': false}
              {'wifi¡¯: true},
            ]
      }
       In previous example, a device receiving such SpecialOp should try to silently download the update package each time
       WIFI connection is available. But if the WIFI connection is lost during the download and the 3G network becomes available,
       then the download process should be stopped as the flag is set to false for 3G network. The download will resume once a WIFI
       network becomes available again.
     */
    eGSOT_ForceUpdate=3,

     /**
       NAME: ChangeAPN

       FUNCTION SUMMARY:


       DETAILS:
       When a device receives such SpecialOp, then the device should SILENTLY clear the current APN list and replace it with the one
       provided in the data payload. Additionally, the device should activate the one designated with the activation flag.
       The data is composed of list of APN/flag record as detailed below:
       <apn name> is the name of the APN to be displayed.
       <activation flag> is a flag informing whether this APN should be the currently active APN. If set to ¡®true¡¯, then the
       corresponding APN should be added into the APN list and set as the currently active APN. If set to ¡®false¡¯, then the
       corresponding APN must just be added into the list.

    DATA:
    {
       'format version':1,
       'data':
       [
       {¡®<apn name>¡¯: <activation flag>}
       ...
       ...
       ...
       ]
       }

    Example:
     {
       'format version':1,
       'data':
        [
         {¡®orange3g¡¯ : true},
         {¡®orangenet¡¯ : false},
         {¡®orangemms¡¯ : false}
        ]
     }
     */
    eGSOT_ChangeAPN=4,

    /*
       NAME: UpgradeAPK
       FUNCTION SUMMARY:

       When a device receives such operation, then it should download the APK via the authorized network type and install it.
       Note that we may need to update the FOTA Client APK itself with this function.
       In order to download such APK, the FOTA client should use exactly the same standard process as to download the
       firmware.        But instead of using ¡¥FIRMWARE¡¦ as FileSet Type, it should use ¡¥APK¡¦ instead.
       The data consists of a list of connection type/flag record as detailed below:
       <connection type name> field contains the name of the connection type.
       The possible connection types are:
       3g : This type identifies the FOTA automatic check of the standard data connection.
       wifi : This type identifies the FOTA automatic check of the WIFI connection.
       Devices should recognize these strings and behaves accordingly.
       If the connection type name is not recognized by the device, then it should be ignored.
       <download flag> is a Boolean. If set to ¡¥false¡¦, then the device should NOT try to download the APK from the
       corresponding connection. If set to ¡¥true¡¦, then the device should start downloading the APK once such corresponding
       connection becomes available.
       DATA:
       {
         'format version':1,
         'data':
         [
           {¡®<connection type name>¡¯: <download flag>}
           ...
           ...
           ...
         ]
      }


       Example:
       {
         'format version':1,
         'data':
         [
           {'3g': true}
           {'wifi¡¯: true},
         ]
       }

       In previous example, a device receiving such SpecialOp should try to download the APK whether on WIFI or 3G network
       whatever is active.
    */
    eGSOT_UpgradeAPK=5



  } tGOTUSpecialOpType;


/**
   OTU Special Operation (Client side).

   The GOTU servers are able to execute some operations on the mobile device.

   For example, it can disable the FOTA automatic check function in order to reduce
   the load on servers.

   Other operation may be implemented in future.
 */
typedef struct gotu_special_op
{
  /** Contains the type id allowing the device to recognize the operation. */
  tGOTUSpecialOpType type;

  /** If the operation requires some data, then this field contains the data size. */
  tUInt32 data_size;

  /** If the operation requires some data, then they are stored in this field.

      Data should be represented as a NULL terminated string in order to be compatible
      with JNI interface with JAVA that only supports NON BINARY string.

      So if complex structured data should be passed, then either use JSON or XML
      string format.

      Examples:
         Suppose we need to pass objects like:

        struct COORD {
             float x[3];
             }

        struct DATA {
            int type;
        char label[250];
        COORD coord;
        float zoom;
            }

     Then you may either use XML string:
        "<DATA><type>40</type><label>Shanghai</label><COORD><x index=0>345.67</x><x index=1>-12.345</x><x index=2>2.3453</x></COORD><zoom>7.09</zoom></DATA>"

     or JSON string (which may be more efficient/compact):
        "{'data':{'type':40,'label':'Shanghai','coord':{'x':[345.67,-12.345,2.3453]},'zoom':7.09}}"


      NOTE: Currently maximum supported size is 1024 bytes (+1 char for '\0').
   */
  tByte data[OTU_MAX_SPECIAL_OP_DATA_LEN+1];
} tGOTUSpecialOperation;



#endif /* _OTU_STRUCT_H_  */
