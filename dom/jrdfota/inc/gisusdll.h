#ifndef _OTUDLL_H_
#define _OTUDLL_H_

#include "mydll.h"
#include "mytypes.h"

#if defined(SCRAMBLE_SYMBOLS)
#include "scrambled_gisus_symbols.h"
#endif

extern "C"
{
// If using C to compile a file including the current header file,
// then the definition of bool is necessary!
#ifndef __cplusplus
    // Define bool type.
    typedef enum { false=0, true=1 } bool;
#endif

#include "otu_status.h"
#include "otu_struct.h"
#include "otu_op.h"
#include "otu_options.h"
#include "otu_upgrade_mode.h"
#include "otu_clienttype.h"
#include "otu_dll_encrypt.h"


    //#############################################################
    // Step 1: Initialization functions.
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_init
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Initialize the OTU library.
    ///
    /// DESCRIPTION:
    ///   This function must be used before any use of the below API.
    ///
    /// PARAMETERS:
    ///   func_arr: Call back function array.
    ///   arr_size: The size of call back function array.
    ///   log_mode: Flag to either activate or deactivate logging.
    ///             If true, then logging is activated and the log file
    ///             will be generated. If false, then logging is deactivated
    ///             and no log file will be generated.
    ///   debug_mode: Flag to activate or deactivate debugging output into
    ///             the log file. If true, then debug log are output into
    ///             the log file together with normal logging output. You
    ///             should be aware that this may slow down the execution of
    ///             the DLL and the generated log may take several hundreds
    ///             of MB. The log will grow indefinitely. If the flag is set
    ///             to false, then no DEBUG info will be output into the log.
    ///
    ///             NOTE: Some modules may be compiled with LOW_LEVEL_DEBUG flag.
    ///             If so, and if the flag is set to true, then much higher level
    ///             of debug information will be output. Each received/sent network
    ///             message will be output. Each important buffer content will be
    ///             output. Log may easily reached size of GB.
    ///
    ///   default_options: This parameter let the programmer specifies some default
    ///             option value. It consists of an array of tOTUOptions objects
    ///             terminated by a NULL value. For example, it is possible to set
    ///             the addresses of the SUMaster servers or the directory where to
    ///             store the chunks. Each tOTUOptions object consist of a pair of
    ///             option_name and option_value. The important option_name are:
    ///
    ///             "SessionRepository": Directory that will contain all the downloaded
    ///                chunks and the session information. Example: "/sdcard/.downloaded".
    ///
    ///                NOTE: If the directory doesn't exist, then the library will attempt
    ///                      to create it automatically when needed.
    ///
    ///             "LogFile": Fullpath of the log file that will be created if log_mode
    ///                is set to true. Example: '/data/fota/fotalib.log'.
    ///
    ///             "SUMaster": Comma separated list of SUMaster to use. Only these servers
    ///                will be used. Examples: 'otu.tctmobile.com;otu01.tctmobile.com;otu02.tctmobile.com;otu03.tctmobile.com;otu04.tctmobile.com'
    ///
    ///             "SessionBackupFile": This option specifies where to store/read the session
    ///                backup file when using the 'otu_backup_session_info' or 'otu_restore_session_info'
    ///                function. Example: '/data/downloaded/session_info.sav'
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Initialization succeed.
    ///
    ///   * eOS_OutOfMemoryError: Initialization failed due to lack
    ///       of memory. Application should stop here as it is
    ///       impossible to continue.
    ///
    ///   * eOS_TimeOut: Time out error when waiting for a thread to
    ///       start. One thread took too much time to start. This
    ///       lead to a fatal error. A error message in the log file
    ///       should be available about the failing thread.
    ///
    ///   * eOS_UnknownInitError: If an unknown error occured (this
    ///       mainly should happen if the underlying implementation of
    ///       OTU DLL (gisusdllimpl.cpp) has been changed by returning
    ///       new types of errors that haven't been reported accordingly
    ///       to the delegate class (gisusdll.cpp).
    **/
    EXPORT tOTUStatus otu_init(OTU_DLL_KEY_CBF_T *func_arr, tUInt32 arr_size, bool log_mode=true, bool debug_mode=false,tOTUClientType client_type=eCT_No_Type,const tOTUOption *default_options=NULL);//zzma


    //#############################################################
    // Step 2: Log and Debug Mode setting/getting functions.
    //#############################################################


    /**
    /// FUNCTION NAME:
    ///   otu_remove_log
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Remove (or delete) the log file from the file system.
    ///
    /// DESCRIPTION:
    ///    The application may need to delete the log file from time
    ///    to time in order to avoid the log file to becomes too big.
    ///    A new interface has been added in order to achieve this
    ///    function.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   None.
    **/
    EXPORT void otu_remove_log();

    /**
    /// FUNCTION NAME:
    ///   otu_set_log_mode
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Activate or deactivate logging into the log file.
    ///
    /// DESCRIPTION:
    ///    Set the log mode accordingly to the given mode parameter.
    ///    If mode is set to 'true' and no log file exists, then it is
    ///    created. Else if log file already exist, then logs are just
    ///    appended to the existing log file. If mode is set to 'false',
    ///    then the log file is not modified or created at all.
    ///
    ///   By default, log mode is activated when otu_init is called.
    ///   Default log filename is 'otudll.log' and is created in the
    ///   current working directory.
    ///
    /// PARAMETERS:
    ///   * mode (IN):  Value of the log mode to be set to.
    ///       - true: Activate log mode into log file. All messages
    ///         concerning [INFO], [WARNING], [ERROR] and [FATAL]
    ///         are logged into the log file.
    ///
    ///       - false: Deactivate debug mode for log file. Only normal
    ///         messages are printed (usually info, error or warnings)
    ///
    /// RETURN VALUE:
    ///   None.
    **/
    EXPORT void otu_set_log_mode(bool mode);


    /**
    /// FUNCTION NAME:
    ///   otu_get_log_mode
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Returns the current log mode.
    ///
    /// DESCRIPTION:
    ///   None.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   * true: Log mode is currently activated.
    ///
    ///   * false: Log mode is currently deactivated.
    **/
    EXPORT bool otu_get_log_mode();


    /**
    /// FUNCTION NAME:
    ///   otu_set_debug_mode
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Activate or deactivate the debug verbosity in log file.
    ///
    /// DESCRIPTION:
    ///    Set the debug mode accordingly to the given mode parameter.
    ///    This function can be called at any time during the
    ///    application life cycle accordingly to debugging needs.
    ///
    ///    By default, debug mode is NOT activated when 'otu_init' is
    ///    called.
    ///
    /// PARAMETERS:
    ///   * mode (IN): Value of the debug mode to be set to.
    ///       - true: Activate debug mode for log file. All messages
    ///         related to debug are output into the log file.
    ///
    ///       - false: Deactivate debug mode for log file.
    ///
    /// RETURN VALUE:
    ///   None.
    **/
    EXPORT void otu_set_debug_mode(bool mode);


    /**
    /// FUNCTION NAME:
    ///   otu_get_debug_mode
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Returns the current debug mode.
    ///
    /// DESCRIPTION:
    ///   None.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   * true: Debug mode is currently activated.
    ///
    ///   * false: Debug mode is currently deactivated.
    **/
    EXPORT bool otu_get_debug_mode();


    //#############################################################
    // Step 3: Request function implying communication with SUMaster
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_create_device_info
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Instanciate a tOTUDeviceInfo object into the OTU library.
    ///
    /// DESCRIPTION:
    ///   Before using any of the other functions, you must instanciate
    ///   such object by providing all the parameters of the current
    ///   device. This will insure proper memory management within
    ///   the OTU library.
    ///
    /// IMPORTANT NOTE: This object will be used/referenced in other
    ///                 object later.
    ///
    ///
    /// PARAMETERS:
    ///   * device_id (IN): Unique identifier of the device. For GSM, it will
    ///       be the IMEI. For CDMA, it should be the ESN. Etc ...
    ///
    ///   * model (IN): String identifying file set. For GSM handset firmware,
    ///       it will be the handset model name like 'OT-800'. For the PC
    ///       driver or PC application, it can be 'OT-800/DRIVER' or 'OT-800/PC SUITE'.
    ///
    ///   * commercial_reference (IN): Commercial reference of the current device
    ///       being update.
    ///
    ///   * current_version (IN): Current version string of the current device (necessary
    ///          for Incremental update mode - see below).
    ///
    ///   * upgrade_mode (IN): Requested update mode.
    ///          Full mode is ALWAYS possible. Incremental mode is not
    ///          supported by all phones.
    ///
    ///   * client_type (IN): Requested client type.
    ///
    /// RETURN VALUE:
    ///   * 0 or NULL if no more memory available.
    ///
    ///   * The instanciated object with all it parameter updated accordingly
    ///     to the given input.
    ///
    **/
    EXPORT tOTUDeviceInfo *otu_create_device_info(
        const char *device_id=0,  /* or IMEI for GSM */
        const char *model=0,
        const char *commercial_reference=0,
        const char *current_version=0,
        tOTUUpgradeMode upgrade_mode=eOUM_Full,
        tOTUClientType client_type=eCT_No_Type,
        tOTUReport_info* report_info=NULL
        );

    /**
    /// FUNCTION NAME:
    ///   otu_request_fileset_info
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   Result of this function can be obtained by using
    ///   'otu_get_file_list' function.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   Request to download latest available information about the
    ///   file set related to the given file set name and commercial
    ///   reference from the SUMaster server.
    ///
    ///   NOTE: In order to succeed, the computer executing this
    ///         function should be connected to internet.
    ///
    /// DESCRIPTION:
    ///   AMSU main functionality is to manage file set. The smallest
    ///   file set is the one that contains one file.
    ///   AMSU is independent of file types. It can hold any kind of
    ///   files of any size (with a limit of 4GB).
    ///
    ///   The latest available information are: number of files in
    ///   the file set, the file names list, their individual
    ///   version (may not be used and may always be 0) and the file
    ///   set version (global version). By comparing the file set
    ///   version (or global version) with the version present in
    ///   the device to update, the calling application is able to
    ///   know whether there is an update available.
    ///
    ///   NOTE: Decision to update or not is given to the calling
    ///         application.
    ///
    /// PARAMETERS:
    ///   * device_info (IN): Device information allowing the OTU
    ///       service to identify the device.
    ///
    ///   * upgrade_mode: (IN): Requested update mode.
    ///          Full mode is ALWAYS possible. Incremental mode is not
    ///          supported by all phones.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Request accepted.
    ///
    ///   * eOS_FunctionParameterError: Request not accepted due
    ///     to missing mandatory parameter. A diff mode upgrade
    ///     has been requested but the device_version_code is NULL.
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT
    ///     accepted because another request is already ongoing
    ///     and being processed.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Starting (Active State): Newly started request.
    ///
    ///  * eOS_Connecting (Active State): Connection to SUMaster is
    ///    ongoing. This includes Socket connection, StartCom
    ///    message exchange and Challenge checking.
    ///
    ///  * eOS_Connected (Active State): Physical and logical and
    ///    secured connection established with SUMaster server
    ///    but no request sent yet. Very short life time state.
    ///
    ///  * eOS_Sending (Active State): Request has just been sent.
    ///    Very short life time state.
    ///
    ///  * eOS_Waiting (Active State): Request has been successfully
    ///    sent and now waiting for the answer from the server.
    ///
    ///  * eOS_Ok (Idle State): Answer from server received and
    ///    request succeeded.
    ///    It is now possible to call otu_get_file_list to get all
    ///    the information related to the file set.
    ///
    ///  * eOS_Failed (Idle State): Request failed.
    ///    Whether application failed to send the request or an improper
    ///    answer has been received from the server.
    ///    After such status, otu_get_file_list should not be called
    ///    else it will return incoherent value.
    ///
    ///  * eOS_TimeOut (Idle State): Request failed due to time out.
    ///    Request has been sent but no answer has been received
    ///    within the allowed time.
    ///    After such status, otu_get_file_list should not be called
    ///    else it will return incoherent value.
    ///
    ///  * eOS_ConnectionFailed (Idle State): Physical or logical
    ///    connection failed or couldn't be established after
    ///    MAXALLOWEDCONNECTRETRIES connection retries.
    ///    This can be due to the following reasons:
    ///      - Client Internet connection limitation
    ///      - Very bad Internet connection quality between the client
    ///        and the AMSU server.
    ///      - AMSU Server not available
    ///    After such status, otu_get_file_list should not be called
    ///    else it will return incoherent value.
    ///
    ///  * eOS_UpgradeRequired (Idle State): This status is due to
    ///    the inability to verify or match the (security) challenge
    ///    performed just after the connection. In fact, for each new
    ///    version of the protocol involving incompatible changes with
    ///    previous version, a new key is generated rendering the old
    ///    version of the tool useless and forcing all the clients
    ///    using the old protocol to upgrade.
    ///    After such status, otu_get_file_list should not be called
    ///    else it will return incoherent value.
    ///
    ///  * eOS_InternalError (Idle State): System resource exhausted or
    ///    application logic error.
    ///    After such status, otu_get_file_list should not be called
    ///    else it will return incoherent value.
    ///
    **/
    EXPORT tOTUStatus otu_request_fileset_info(
        tOTUDeviceInfo *device_info
        );


    /**
    /// FUNCTION NAME:
    ///   otu_get_request_status
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Returns the status of a previous (asynchronous) request.
    ///   This function should be used after a call to one of the
    ///   following asynchronous functions in order to know whether
    ///   they completed their processing successfully or not:
    ///   - otu_request_sw_info
    ///   - otu_request_session_resume
    ///   - otu_request_session_delete
    ///   - otu_request_session_purge_all
    ///   - otu_request_download_start
    ///   - otu_request_download_stop
    ///
    /// DESCRIPTION:
    ///   This function allows the calling application to precisely
    ///   follow the execution of an asynchronous function.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///    The return value depends on the previously called request
    ///    function. Please see the section 'OTU_REQUEST_STATUS RETURN
    ///    VALUE' in the comments of the corresponding request function
    ///    for details about the possible return value.
    ///
    **/
    EXPORT tOTUStatus otu_get_request_status();


    /**
    /// FUNCTION NAME:
    ///   otu_get_file_list
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   This function should be called after a successful
    ///   execution completion of otu_request_sw_info (caution:
    ///   this is an asynchronous function. Use otu_request_status
    ///   to check whether this function completed successfully).
    ///
    /// DESCRIPTION:
    ///   This function returns the number of files for
    ///   the current mobile phone with their individual name and
    ///   size in bytes.
    ///   No file order is actually defined in the returned list. So
    ///   the calling application should not make any assumption on
    ///   the file order.
    ///
    ///   Returned arrays should not be freed by the calling application.
    ///   They will be handled by the OTU DLL instead.
    ///
    /// PARAMETERS:
    ///
    ///   * session_info: (OUT): This parameter will contain the associated
    ///       session information allocated and returned by the function on successful
    ///       execution. It can be used later to call other function or
    ///       can be updated by the client if necessary.
    ///       Memory management is handled by the library and should not
    ///       be attempted by the client itself.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Function execution success. Returned values should
    ///      be coherent.
    ///
    ///   * eOS_BadUsage: Function execution failed because not called
    ///      after a successfull execution of otu_request_sw_info.
    ///      Returned parameters should be incoherent or meaningless.
    ///
    **/
    EXPORT tOTUStatus otu_get_file_list(
        tOTUSessionInfo **session_info
        );


    /**
    /// FUNCTION NAME:
    ///   otu_get_special_op
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   This function should be called after a successful
    ///   execution completion of otu_request_sw_info (caution:
    ///   this is an asynchronous function. Use otu_request_status
    ///   to check whether this function completed successfully).
    ///
    /// DESCRIPTION:
    ///   This function returns an eventual special operation provided
    ///   by the GOTU server together with the version check message.
    ///
    ///   Client has the responsibility to understand, analyze and execute
    ///   the returned special operation.
    ///
    ///   Several Special Operation may be returned by a message from the server.
    ///   So this function should be called several times until it returns
    ///   the error code eOS_Failed.
    ///
    ///
    /// PARAMETERS:
    ///   * special_op (OUT): This parameter should contain a pointer to a
    ///       pre-allocated object so that after execution of this function,
    ///       its attributes will be updated. Note that it is the responsibility
    ///       of the caller to properly allocate the object before calling this
    ///       function and deallocate this object once finished.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Function execution success. The special_op object has been
    ///      updated.
    ///
    ///   * eOS_Failed: No Special Object found.
    ///
    **/
        EXPORT tOTUStatus otu_get_special_op(tGOTUSpecialOperation *special_op);


    //#############################################################
    // Step 4: Session Management functions.
    //
    //    Sessions are created automatically by the OTU DLL when
    //    otu_request_download_start is called. Sessions is a mean
    //    to implement previously downloaded data persistence in
    //    case the computer reboot or the application crash.
    //    Sessions are identified by the commercial reference of the
    //    mobile phone (for example 'C825X-2ARGES1').
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_get_session_availability
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   This function allows the caller to know whether a session
    ///   for a given commercial reference has already been created
    ///   on end user computer and is actually available for being
    ///   resumed.
    ///
    /// DESCRIPTION:
    ///   The implementation of this function is very simple and just
    ///   check whether the master file can be found (the master file
    ///   contains the list of all other files that composes the
    ///   current file set).
    ///   Even if the master file is found, this doesn't mean that
    ///   some data of a previous download are available. The only
    ///   way to know that is to resume the session and call
    ///   'otu_download_status' function.
    ///
    /// PARAMETERS:
    ///   * device_info (IN): device_info representing the device for which
    ///    the session availability needs to be checked.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Session for the given device_info has been found.
    ///
    ///   * eOS_No: Session for the given device_info has NOT
    ///     been found.
    ///
    **/
    EXPORT tOTUStatus otu_get_session_availability(
        tOTUSessionInfo *session_info
        );

    /**
    /// FUNCTION NAME:
    ///   otu_get_session_list
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   This function allows the caller to get the list of available
    ///   sessions found on the hard disk. Any returned session should
    ///   be resumable although not necessary completed.
    ///
    /// DESCRIPTION:
    ///   The implementation of this function is very simple and just
    ///   check whether the OTU master file can be found (the OTU
    ///   master file contains the list of all sessions registered
    ///   previously).
    ///
    /// PARAMETERS:
    ///   * nb_session (OUT): This parameter will contain the number of
    ///       sessions found in the array of sessions returned by the next
    ///       parameter.
    ///
    ///   * session_info: (OUT): This parameter will contain an array
    ///       of session information objects. These objects are allocated
    ///       and returned by the function on successful execution.
    ///       It can be used later as parameter of other functions or can be
    ///       updated by the client if necessary.
    ///       Memory management is handled by the library and should not
    ///       be attempted by the client itself.
    ///
    ///       NOTE: If no OTU master file is found then nb_session is zero
    ///             and session_info is NULL.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Always returns ok.
    ///
    **/
    EXPORT tOTUStatus otu_get_session_list(
        tUInt32 *nb_session,
        tOTUSessionInfo **session_info
        );

    /**
    /// FUNCTION NAME:
    ///   otu_request_session_resume
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   This function allows the caller to gather all consistent data
    ///   chunks of a previous attempt of a file set download. By doing
    ///   so, the caller avoid to download again some already downloaded
    ///   chunks and resume back the download at the point where it stopped
    ///   last time (if the download already completed last time, then no
    ///   download at all is required).
    ///
    /// DESCRIPTION:
    ///   This function will first look for the master file. If found,
    ///   then it will be used to identify the list of chunk files already
    ///   downloaded. Then each downloaded chunk files are strictly
    ///   checked to make sure their data are consistent and not corrupted.
    ///   If a chunk files is found to be corrupted, then it is silently
    ///   ignored so that it is forced to be downloaded again.
    ///
    ///   NOTE: Because corrupted chunks are silently ignored, this means
    ///         that this function will alway complete successfully. In the
    ///         worst case, the session will contain 0 chunks as if a new
    ///         session has been freshly created.
    ///
    /// PARAMETERS:
    ///   * session_info (IN): session_info representing the session to resume.
    ///        This can be either the object returned by 'otu_get_file_list' or
    ///        'otu_get_session_list'.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Session resume request has been accepted and has
    ///     started.
    ///
    ///   * eOS_OutOfMemoryError: Failed due to memory exhaustion (cannot
    ///     allocate enough space for a new session).
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT
    ///     accepted because another request is already ongoing
    ///     and being processed.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Starting (Active State): Newly started request.
    ///
    ///  * eOS_Progressing (Active State): Processing is ongoing. It is possible
    ///    to use 'otu_request_completion_status' to know the completion status
    ///    of the request.
    ///
    ///  * eOS_Ok (Idle State): The session has been successfully resumed.
    ///    It is now possible to use it by calling 'otu_request_download_start'
    ///
    **/
    EXPORT tOTUStatus otu_request_session_resume(
        tOTUSessionInfo *session_info
        );


    /**
    /// FUNCTION NAME:
    ///   otu_request_session_delete
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   This function deletes ALL files (master file and chunk files)
    ///   found into the repository related to the session for the given
    ///   commercial reference.
    ///
    /// DESCRIPTION:
    ///   This function will first look for the master file. If found,
    ///   then it will be used to identify the list of chunk files to
    ///   delete from the repository.
    ///
    /// PARAMETERS:
    ///   * device_info (IN): device_info object representing the session to delete.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Session delete request has been accepted and has
    ///     started.
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT
    ///     accepted because another request is already ongoing
    ///     and being processed.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Starting (Active State): Newly started request.
    ///
    ///  * eOS_Progressing (Active State): Processing is ongoing. It is possible
    ///    to use 'otu_request_completion_status' to know the completion status
    ///    of the request.
    ///
    ///  * eOS_Ok (Idle State): The session has been successfully deleted.
    ///
    ///  * eOS_FileNotFoundError (Idle State): The session couldn't be deleted
    ///    because the master file associated to the given commercial reference
    ///    couldn't be found into the repository.
    **/
    EXPORT tOTUStatus otu_request_session_delete(
        tOTUSessionInfo *session_info
        );

    /**
    /// FUNCTION NAME:
    ///   otu_request_session_purge_all
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   This function deletes ALL files found into the repository
    ///   whether they correspond to chunk files, master files or
    ///   even files put by end users.
    ///
    /// DESCRIPTION:
    ///   None.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Session delete request has been accepted and has
    ///     started.
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT
    ///     accepted because another request is already ongoing
    ///     and being processed.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Starting (Active State): Newly started request.
    ///
    ///  * eOS_Progressing (Active State): Processing is ongoing. It is possible
    ///    to use 'otu_request_completion_status' to know the completion status
    ///    of the request.
    ///
    ///  * eOS_Ok (Idle State): The purge operation has been successfully executed.
    ///
    ///  * eOS_DirectoryAuthorizationError (Idle State): The purge operation
    ///    failed due to lack of write authorization on the repository.
    ///
    **/
    EXPORT tOTUStatus otu_request_session_purge_all();


    //#############################################################
    // Step 5: Request function implying communication with SUDH
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_request_download_start
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   This functions starts the download process of the file
    ///   set corresponding to the given commercial reference and
    ///   model.
    ///
    /// DESCRIPTION:
    ///   This function has a complex internal behavior. A summary
    ///   is presented here.
    ///
    ///   1) There is no explicit function to create session. This is
    ///      because 'otu_request_download_start' will implicitely create
    ///      a new session if required (or use an existing one). Another
    ///      reason is for the design of security. A session is a way
    ///      to hide file set complexity and insure encryption transparency.
    ///      We don't want the calling application being easily able
    ///      to manipulate such objects.
    //
    ///   2) Once a Session has been created or an existing session
    ///      is being used, then all subsequent downloaded file chunks
    ///       are registered into and linked with this session.
    ///
    ///   3) Authorization to download is obtained automatically from
    ///      the SUMaster. Before the authorization has not been registered
    ///      to all the SUDH server, it won't be possible for the OTU DLL
    ///      to connect with them and obtain data.
    ///
    ///   4) File set is downloaded chunk by chunk. Progress can be seen
    ///      by using the 'otu_request_completion_status' function.
    ///
    /// PARAMETERS:
    ///   * session_info (IN).
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Download request has been accepted and has started.
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT accepted
    ///     because another request is already ongoing and being
    ///     processed.
    ///
    ///   * eOS_ThreadNotReadyError: Multidownloader object not in the idle
    ///     state when requesting to download file. This is usually a application
    ///     logic problem and should never happen.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Starting (Active State): Newly started request.
    ///
    ///  * eOS_AuthorizationSending (Active State): Sending authorization
    ///    request.
    ///
    ///  * eOS_AuthorizationWaiting (Active State): Waiting authorization
    ///    grant.
    ///
    ///  * eOS_Connecting (Active State): Connection to SUMaster is
    ///    ongoing. This includes Socket connection, StartCom
    ///    message exchange and Challenge checking.
    ///
    ///  * eOS_Connected (Active State): Physical and logical and
    ///    secured connection established with SUMaster server
    ///    but no request sent yet. Very short life time state.
    ///
    ///  * eOS_Sending (Active State): Request has just been sent.
    ///    Very short life time state.
    ///
    ///  * eOS_Waiting (Active State): Request has been successfully
    ///    sent and now waiting for the answer from the server.
    ///
    ///  * eOS_Ok (Idle State): The download operation has been successfully completed.
    ///    On success, it is possible to use the 'otu_get_first_sw_byte' and
    ///    'otu_get_next_sw_byte' functions to get the downloaded files content.
    ///
    ///  * eOS_Failed (Idle State): Request failed.
    ///    Whether application failed to send the request or an improper
    ///    answer has been received from the server.
    ///    After such status, neither 'otu_get_first_sw_byte' nor
    ///    'otu_get_next_sw_byte' functions should be called.
    ///
    ///  * eOS_TimeOut (Idle State): Request failed due to time out.
    ///    Request has been sent but no answer has been received
    ///    within the allowed time.
    ///    After such status, neither 'otu_get_first_sw_byte' nor
    ///    'otu_get_next_sw_byte' functions should be called.
    ///
    ///  * eOS_ConnectionFailed (Idle State): Physical or logical
    ///    connection failed or couldn't be established after
    ///    MAXALLOWEDCONNECTRETRIES connection retries.
    ///    This can be due to the following reasons:
    ///      - Client Internet connection limitation
    ///      - Very bad Internet connection quality between the client
    ///        and the AMSU server.
    ///      - AMSU Server not available
    ///    After such status, neither 'otu_get_first_sw_byte' nor
    ///    'otu_get_next_sw_byte' functions should be called.
    ///
    ///  * eOS_UpgradeRequired (Idle State): This status is due to
    ///    the inability to verify or match the (security) challenge
    ///    performed just after the connection. In fact, for each new
    ///    version of the protocol involving incompatible changes with
    ///    previous version, a new key is generated rendering the old
    ///    version of the tool useless and forcing all the clients
    ///    using the old protocol to upgrade.
    ///    After such status, neither 'otu_get_first_sw_byte' nor
    ///    'otu_get_next_sw_byte' functions should be called.
    ///
    ///  * eOS_InternalError (Idle State): System resource exhausted or
    ///    application logic error.
    ///    After such status, neither 'otu_get_first_sw_byte' nor
    ///    'otu_get_next_sw_byte' functions should be called.
    ///
    ///  * eOS_DirectoryAuthorizationError (Idle State): The download operation
    ///    failed due to lack of write authorization on the repository. Write
    ///    authorization is required to save chunk files and master file.
    ///    After such status, neither 'otu_get_first_sw_byte' nor
    ///    'otu_get_next_sw_byte' functions should be called.
    ///
    /// * eOS_ClientToolKeyError (Idle State): This status is due to
    ///    the key of the client has been blocked. The client needs to be
    ///    upgraded.
    ///
    **/
    EXPORT tOTUStatus otu_request_download_start(
        tOTUSessionInfo *session_info,tUInt32 customized_num=0
        );

    /**
    /// FUNCTION NAME:
    ///   otu_request_download_stop
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   Stop a download in such a way that it can be resumed
    ///   by a call to otu_request_download_start.
    ///
    /// DESCRIPTION:
    ///   This function pauses the download process. This function is
    ///   asynchronous because the running threads may be in such
    ///   state that doesn't allow them to respond quickly. So after
    ///   the call to this function, once 'otu_request_status' returns
    ///   'eOS_Ok' then the calling application can be sure that
    ///   the downloading threads are fully stopped and no more data
    ///   are sent or received over the network.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Download stop request has been accepted and has started.
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT accepted
    ///     because another request is already ongoing and being
    ///     processed.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Paused (Active State): Download successfully paused.
    ///
    ///
    **/
    EXPORT tOTUStatus otu_request_download_stop();

    /**
    /// FUNCTION NAME:
    ///   otu_get_request_completion_status
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Return the percentage of completion of a currently running
    ///   request.
    ///
    /// DESCRIPTION:
    ///   This function allows the calling application to implement a
    ///   progress bar showing the completion state of current request
    ///   process.
    ///
    ///   IMPORTANT NOTE: The calling application MUST NOT rely on the
    ///                   value returned by this function when evaluating
    ///                   the end of a request processing. Instead it should
    ///                   mainly monitor the return value of the
    ///                   'otu_request_status' function for accurate
    ///                   completion detection. This is because this function
    ///                   may suffer from rounding problems that may announce
    ///                   a completion when actually it is not fully completed yet.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   A value between 0 and 1000 is returned.
    ///   0 means 0% of request has been completed.
    ///   341 means 34.1% of request has been completed.
    ///   1000 means 100% of request has been completed.
    ///
    **/
    EXPORT int otu_get_request_completion_status();

    /**
    /// FUNCTION NAME:
    ///   otu_download_speed
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Return the current download speed in bytes per second.
    ///
    /// DESCRIPTION:
    ///   This function allows the calling application to implement a
    ///   download speed display showing the ongoing activities of the
    ///   download procesisng.
    ///
    ///   NOTE: Given speed is just an estimated speed and should not be
    ///         taken as a precise source of download speed.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   Bytes/second.
    ///
    **/
    EXPORT float otu_download_speed();

    /**
    /// FUNCTION NAME:
    ///   otu_downloaded_byte
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Return the aggregated amount of bytes already downloaded and
    ///   the total size of bytes to download.
    ///
    /// DESCRIPTION:
    ///   This function allows the calling application to implement a
    ///   downloaded byte counter during the download process.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///  * eOS_Ok: This function always success in current implementation.
    ///
    **/
    EXPORT tOTUStatus otu_downloaded_byte(tUInt32 *total_size, tUInt32 *downloaded_size);

    //#############################################################
    // Step 6: Software Data Access functions.
    //         (Usage of locally downloaded files)
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_get_first_sw_byte
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Returns the first byte of the given file id.
    ///
    /// DESCRIPTION:
    ///   The side effect of this function is to open the file
    ///   corresponding to the given index and set an internal READ pointer
    ///   on this file so that any subsequent call to 'otu_get_next_sw_byte'
    ///   will move an internal READ pointer within this file by one byte
    ///   at a time.
    ///
    ///   NOTE:
    ///       Before calling this function, a download should have been
    ///       completed first (otu_request_status returns eOS_Ok).
    ///
    ///   IMPORTANT NOTE:
    ///       This function has been designed in order to protect the access
    ///       to the software binaries. In fact, the files content won't be found
    ///       into memory in one memory block area only. Instead, the file
    ///       content will be widespread into different memory blocks at
    ///       different (random) location. So this function is the only way
    ///       to get the software binaries in the right order.
    ///
    /// PARAMETERS:
    ///   * file_index (IN): index of the file to point to. The value of
    ///       this parameter must range between 0 and file_nb-1 where
    ///       file_nb is obtained from a previous call to 'otu_get_file_list'.
    ///
    /// RETURN VALUE:
    ///   The value of the very first byte of the corresponding file
    ///   is returned. So returned value ranges from 0 to 255 included.
    ///   If the given file_index corresponds to an empty file (file size
    ///   is 0 byte), then 'UNDEFINED' is returned by this function.
    ///   If the given file_index is wrong then 'UNDEFINED' is returned
    ///   as well.
    ///
    ///   NOTE: The return value type is purposely oversized (32 bits instead
    ///         of 8 bits) in order to allow to return UNDEFINED.
    ///
    **/
    EXPORT unsigned int otu_get_first_sw_byte(unsigned int file_index);

    /**
    /// FUNCTION NAME:
    ///   otu_get_next_sw_byte
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Returns the next byte of the current file being .
    ///
    /// DESCRIPTION:
    ///   Return the current byte value pointed by the READ pointer and
    ///   increment the READ pointer on the next byte.
    ///   If there is no more byte to read, then this function returns
    ///   'Undefined'.
    ///
    ///   IMPORTANT NOTE:
    ///       This function has been designed in order to protect the access
    ///       to the software binaries. In fact, the files content won't be found
    ///       into memory in one memory block area only. Instead, the file
    ///       content will be widespread into different memory blocks at
    ///       different (random) location. So this function is the only way
    ///       to get the software binaries in the right order.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   The value of a byte of is returned. So returned value ranges
    ///   from 0 to 255 included.
    ///   If end of file has been reached, then 'UNDEFINED' is returned by
    ///   this function.
    ///
    ///   NOTE: The return value type is purposely oversized (32 bits instead
    ///         of 8 bits) in order to allow to return UNDEFINED.
    ///
    **/
    EXPORT unsigned int otu_get_next_sw_byte();


    //#############################################################
    // Step 7: Error message
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_get_status_msg
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Returns the text message corresponding to the given status.
    ///
    /// DESCRIPTION:
    ///   This function allows to get the error message associated to
    ///   a given status. It can be useful for logging facilities.
    ///
    /// PARAMETERS:
    ///   * status (IN): Value of a returned status code for which a
    ///      corresponding message is sought.
    ///
    ///   * msg (OUT): Message corresponding to the status code and
    ///      updated on successful execution. Returned buffer must NOT
    ///      be freed.
    ///
    /// RETURN VALUE:
    ///  * eOS_Ok: The corresponding message has been found and returned
    ///       into the msg parameter.
    ///
    ///  * eOS_Failed: No message has been found for the given status.
    ///
    **/
    EXPORT tOTUStatus otu_get_status_msg(tOTUStatus status, const char ** msg);

    //#############################################################
    // Step 8: Statistics function.
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_notify_status_for_statistics
    ///
    /// FUNCTION TYPE:
    ///   Asynchronous. Use 'otu_request_status' to know when the
    ///   function has finished its execution (eOS_Ok or an error
    ///   is returned). Such function cannot be called if another
    ///   asynchronous function has been already called and didn't
    ///   completed yet.
    ///
    ///   During the execution of this function, it is possible
    ///   to get the completion status of the processing by calling
    ///   the 'otu_request_completion_status' function. This can
    ///   be useful if the caller needs to show a progress bar.
    ///
    ///   See the 'OTU_REQUEST_STATUS RETURN VALUE' section below
    ///   to know about the possible returned value of this function
    ///   when calling 'otu_request_status'.
    ///
    /// USAGE:
    ///   Send data to master server to notify about a positive or
    ///   negative operation status with the possibility to send
    ///   additional and optional information/data (used for statistics
    ///   purpose only).
    ///
    ///   NOTE 1: In order to succeed, the computer executing this
    ///         function should be connected to internet.
    ///
    ///   NOTE 2: The usage of this function is optional.
    ///
    ///   IMPORTANT NOTE: In some countries, such information sending
    ///     may be subjected to the initial approval of the end user.
    ///     Make sure to get the authorization/agreement from the end
    ///     user before calling this function.
    ///
    /// DESCRIPTION:
    ///   After calling this function, a message is created into the
    ///   sending queue with all the given statistics information. In
    ///   order to properly send the information, the client should
    ///   insure to not shutdown the library until there is no more
    ///   message to send. This can be achieved by using the function
    ///   'otu_can_sending_queue_shutdown'.
    ///
    /// PARAMETERS:
    ///   * operation (IN): Operation type. For example: eOO_Download.
    ///
    ///   * status (IN): Status to report. For example: eOS_FileSystemFullError.
    ///
    ///   * session_info (IN): session_info representing the currently active session.
    ///       Can be NULL if not session info need to be sent.
    ///
    ///   * statistics_data_size (IN,OPTIONAL): Size of statistics data found in the
    ///         next parameter.
    ///
    ///   * statistics_data (IN,OPTIONAL): Statistics data to send. The format of
    ///        these data is unspecified in order being able to accept
    ///        any kind of data (and so being format independent). This parameter
    ///        can be NULL if not data need to be sent (then statistics_data_size
    ///        must be 0).
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Request accepted.
    ///
    ///   * eOS_AnotherRequestAlreadyOnGoingError: Request NOT
    ///     accepted because another request of a different type is already
    ///     ongoing and being processed.
    ///
    ///     NOTE: If another request for notification is currently active
    ///           and in process, then a new call to this function is semantically
    ///           accepted and possible at the condition the sending queue is
    ///           not full (see eOS_SendingQueueFullError error below).
    ///
    ///   * eOS_SendingQueueFullError: Request NOT accepted because the sending
    ///      queue is full and so new message cannot be queued for sending. This
    ///      can happen because several calls to 'otu_notify_status_for_statistics'
    ///      in a row is accepted and may fill up the sending queue.
    ///
    /// OTU_REQUEST_STATUS RETURN VALUE:
    ///
    ///  * eOS_Starting (Active State): Newly started request.
    ///
    ///  * eOS_Connecting (Active State): Connection to SUMaster is
    ///    ongoing. This includes Socket connection, StartCom
    ///    message exchange and Challenge checking.
    ///
    ///  * eOS_Connected (Active State): Physical and logical and
    ///    secured connection established with SUMaster server
    ///    but no request sent yet. Very short life time state.
    ///
    ///  * eOS_Sending (Active State): Request has just been sent.
    ///    Very short life time state.
    ///
    ///  * eOS_Waiting (Active State): Request has been successfully
    ///    sent and now waiting for the answer from the server.
    ///
    ///  * eOS_Ok (Idle State): Answer from server received and
    ///    request succeeded.
    ///    It is now possible to shutdown the library or make some
    ///    other request.
    ///
    ///  * eOS_Failed (Idle State): Request failed.
    ///    Whether application failed to send the request or an improper
    ///    answer has been received from the server.
    ///    After such returned status, the statistics data sent previously
    ///    are not garanteed of being properly sent. A new attempt
    ///    may be explicitely requested by the client.
    ///
    ///  * eOS_TimeOut (Idle State): Request failed due to time out.
    ///    Request has been sent but no answer has been received
    ///    within the allowed time.
    ///    After such returned status, the statistics data sent previously
    ///    are not garanteed of being properly sent. A new attempt
    ///    may/should be explicitely requested by the client.
    ///
    ///  * eOS_ConnectionFailed (Idle State): Physical or logical
    ///    connection failed or couldn't be established after
    ///    MAXALLOWEDCONNECTRETRIES connection retries
    ///    This can be due to the following reasons:
    ///      - Client Internet connection limitation
    ///      - Very bad Internet connection quality between the client
    ///        and the AMSU server.
    ///      - AMSU Server not available
    ///    After such returned status, the statistics data have not been
    ///    sent properly. A new attempt may/should be (explicitely) requested
    ///    by the client.
    ///
    ///  * eOS_UpgradeRequired (Idle State): This status is due to
    ///    the inability to verify or match the (security) challenge
    ///    performed just after the connection. In fact, for each new
    ///    version of the protocol involving incompatible changes with
    ///    previous version, a new key is generated rendering the old
    ///    version of the tool useless and forcing all the clients
    ///    using the old protocol to upgrade.
    ///    After such returned status, the statistics data have not been
    ///    sent properly. A new attempt is NOT necessary as the attempt
    ///    is sure to fail again.
    ///
    ///  * eOS_InternalError (Idle State): System resource exhausted or
    ///    application logic error.
    ///    After such returned status, the statistics data have not been
    ///    sent properly. The expected behaviour from the client should be
    ///    to shutdown the library.
    ///
    **/
    EXPORT tOTUStatus otu_notify_status_for_statistics(
        tOTUOperation operation,           /* eOO_Download */
        tOTUStatus status,                 /* eOS_FileSystemFullError */
        tOTUSessionInfo *session_info,
        tUInt32 statistics_data_size=0,    /* ... */
        void *statistics_data=0           /* ... */
        );

    /**
    /// FUNCTION NAME:
    ///   otu_backup_session_info
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   This function is mainly used for FOTA client that needs to
    ///   reboot the phone in order to start the upgrade process.
    ///   As the reboot operation will loose the session_info context,
    ///   it is necessary to save it on a file system.
    ///
    /// DESCRIPTION:
    ///   Backup the whole session_info object into a file so it can be
    ///   restored later by otu_restore_session_info.
    ///
    /// PARAMETERS:
    ///   * session_info (IN): Session info object to backup.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Backup succeeded.
    ///
    ///   * eOS_FileSystemFullError: Backup failed due to lack of space.
    ///
    ///   * eOS_HardDiskLowLevelError: Backup failed due to disk error.
    ///
    ///   * eOS_DirectoryNotFoundError: Backup failed due to non existing directory.
    ///
    **/
    EXPORT tOTUStatus otu_backup_session_info(tOTUSessionInfo *session_info);

    /**
    /// FUNCTION NAME:
    ///   otu_restore_session_info
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Restore the whole session_info object from a previously
    ///   backup file (otu_backup_session_info should have been
    ///   successfully called before).
    ///
    /// DESCRIPTION:
    ///   This function will recreate the session_info object from
    ///   the backup found on the file system.
    ///
    /// PARAMETERS:
    ///   * session_info (OUT): Restored session info object.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Restore succeeded. The session_info parameter contains
    ///        a valid session_info object.
    ///
    ///   * eOS_FileNotFoundError: Restore failed because the backup file
    ///        cannot be found. Maybe a previous call to otu_backup_session_info
    ///        failed. The returned session_info parameter is not valid.
    ///
    ///   * eOS_CorruptedFileError: Restored failed because the backup file
    ///        is corrupted. The returned session_info parameter is not valid.
    ///
    **/
    EXPORT tOTUStatus otu_restore_session_info(tOTUSessionInfo **session_info);

    /**
    /// FUNCTION NAME:
    ///   otu_remove_session_info
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Remove any existing session_info backup file.
    ///
    /// DESCRIPTION:
    ///
    /// PARAMETERS:
    ///    None.
    ///
    /// RETURN VALUE:
    ///    None.
    ///
    **/
    EXPORT void otu_remove_session_info();

    //#############################################################
    // Step 9: Library Shutdown function.
    //#############################################################

    /**
    /// FUNCTION NAME:
    ///   otu_shutdown
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Shutdown all the running thread and free all the resources.
    ///
    /// DESCRIPTION:
    ///   This function request all threads to stop their processing
    ///   as soon as possible and relinquish their resources.
    ///   It is to be noted that some threads may be unresponsive
    ///   during 45 seconds when for example they try to connect
    ///   to a server that takes time to answer.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Shutdown always return success.
    ///
    **/
    EXPORT tOTUStatus otu_shutdown();

    /**
    /// FUNCTION NAME:
    ///   otu_reset
    ///
    /// FUNCTION TYPE:
    ///   Synchronous.
    ///
    /// USAGE:
    ///   Reset the OTU library.
    ///
    /// DESCRIPTION:
    ///   This function can only be called when the library is
    ///   not currently downloading. It will reset some internal
    ///   variables in order to appear as if it just has been
    ///   newly instanciated.
    ///
    /// PARAMETERS:
    ///   None.
    ///
    /// RETURN VALUE:
    ///   * eOS_Ok: Reset succeed.
    ///
    ///   * eOS_Error: Reset failed because of some internal errors.
    ///
    ///   * eOS_OutOfMemoryError: Failed to instanciate new objects
    ///     due to memory exhaustion.
    ///
    ///   * eOS_TimeOut: Failed to start the thread on time.
    ///
    **/
    EXPORT tOTUStatus otu_reset();




  /**
     Below are some functions that can be used for version information.
   */
  EXPORT const char *otu_str_version();

  EXPORT const char *otu_str_build_id();

  EXPORT const char *otu_str_protocol_id();

  EXPORT tUInt32 otu_int32_version();

  EXPORT tUInt32 otu_int32_build_id();

  EXPORT tUInt32 otu_int32_protocol_id();

}

#endif /* _OTUDLL_H_ */

