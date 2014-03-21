#ifndef _OTU_STATUS_H_
#define _OTU_STATUS_H_

/// OTU Status
///
/// This enum lists all the possible return values of
/// most of the current OTU functions.
///
// WARNING: DO NOT CHANGE THE VALUE BELOW ELSE YOU WILL
//          MAKE THE DLL INCOMPATIBLE. HOWEVER, YOU
//          CAN INSERT VALUE AS LONG AS THEY DON'T SHIFT
//          THE ALREADY EXISTING VALUES.
typedef enum {
  eOS_Unknown=0,                          /*     0 */
  eOS_Initialized=1,                      /*     1 */
  eOS_Starting=10,                        /*    10 */
  eOS_Starting_Auth=15,                   /*    15 */
  eOS_Connecting=20,                      /*    20 */
  eOS_Connecting_Auth=25,                 /*    25 */
  eOS_Connected=30,                       /*    30 */
  eOS_Connected_Auth=35,                  /*    35 */
  eOS_Sending=40,                         /*    40 */
  eOS_Sent_Nothing=41,                    /*    41 */
  eOS_Sending_Auth=45,                    /*    45 */
  eOS_Decoding=46,                        /*    46 */
  eOS_Skipping=47,                        /*    47 */
  eOS_Received_Nothing=48,                /*    48 */
  eOS_Waiting=50,                         /*    50 */
  eOS_Waiting_Auth=55,                    /*    55 */
  eOS_SessionInstanciation=60,            /*    60 */
  eOS_Idle=70,                            /*    70 */
  eOS_Paused=80,                          /*    80 */
  eOS_TemporaryPaused=85,                 /*    85 */
  eOS_Downloading=90,                     /*    90 */
  eOS_Processing=95,                      /*    95 */
  eOS_Authorized=99,                      /*    99 */

  ////////////////////////////
  eOS_Completed=100, /// Dummy status to separate incompleted status above with completed status below.
  ////////////////////////////


  eOS_Msg_In_Queue,                       /*   101 */
  eOS_Msg_Sent,                           /*   102 */
  eOS_No,                                 /*   103 */
  /* Everything below eOS_Ok value, means incompleted and
     still on going processing. */

  eOS_Ok=999,                             /*   999  - Means successful
                           and finished processing. */

  /*
     Everything above eOS_Ok value, means error!

     In most of the cases, when an error occurs and if possible,
     the download will be paused. If the end user is able to
     correct the problems, then it should be possible to
     resume the download.

     There are 2 kinds of errors:

     - Recoverable errors with value less than eOS_FatalError=10000
       meaning that such errors doesn't involve abortion of processing.
       It mays however request a pause.

     - Unrecoverable errors with value higher than eOS_FatalError=10000
       meaning that such errors may involve abortion of processing
       if the end user doesn't take any actions (for example, for a
       disk full error the end user is expected to delete some files
       to make some space).
   */

  eOS_Error=1000,                         /*  1000 */
  eOS_TimeOut=1001,                       /*  1001 */
  eOS_Failed,                             /*  1002 */
  eOS_BadUsage,                           /*  1003 */
  eOS_ConnectionFailed,                   /*  1004 */
  eOS_UpgradeRequired,                    /*  1005 */
  eOS_NoSoftwareAvailableError,           /*  1006 */
  eOS_NetworkQualityTooBad,               /*  1007 */
  eOS_ServerTooBusy,                      /*  1008 */
  eOS_InternalError,                      /*  1009 */
  eOS_ConfFileError,                      /*  1010 */
  eOS_UnknownInitError,                   /*  1011 */
  eOS_AnotherRequestAlreadyOnGoingError,  /*  1012 */
  eOS_FileNotFoundError,                  /*  1013 */
  eOS_ThreadNotReadyError,                /*  1014 */
  eOS_NoServerAvailableError,             /*  1015 */
  eOS_MaxRepetitionLimitReachedError,     /*  1016 */
  eOS_RemotePeerDisconnectedError,        /*  1017 */
  eOS_BadCheckSumError,                   /*  1018 */
  eOS_CorruptedFileError,                 /*  1019 */
  eOS_SendingQueueFullError,              /*  1020 */
  eOS_FunctionParameterError,             /*  1021 */
  eOS_IncompleteMsgError,                 /*  1022 */
  eOS_UnrecognizedFieldCodeError,         /*  1023 */
  eOS_MsgFieldNotFoundError,              /*  1024 */
  eOS_MsgFieldTooBigError,                /*  1025 */
  eOS_InvalidMsgSeqIndexError,            /*  1026 */
  eOS_MalformedMsgStartingTagError,       /*  1027 */
  eOS_MalformedMsgEndingTagError,         /*  1028 */
  eOS_InvalidFieldSizeError,              /*  1029 */
  eOS_InsufficientBufferSizeError,        /*  1030 */
  eOS_InQueue_Full_Error,                 /*  1031 */
  eOS_OutQueue_Full_Error,                /*  1032 */
  eOS_Payload_Size_Too_Big_Error,         /*  1033 */
  eOS_Msg_Size_Too_Big_Error,             /*  1034 */
  eOS_Network_Error,                      /*  1035 */
  eOS_Bad_Header_Error,                   /*  1036 */
  eOS_Bad_Tail_Error,                     /*  1037 */
  eOS_No_Msg_Error,                       /*  1038 */
  eOS_Not_Authorized,                     /*  1039 */
  eOS_DllCallBackFunctionError,           /*  1040 */
  eOS_SessionNotExistingError,            /*  1041 */

  eOS_FatalError=10000,                   /* 10000 */
  eOS_DirectoryAuthorizationError,        /* 10001 */
  eOS_OutOfMemoryError,                   /* 10002 */
  eOS_FileSystemFullError,                /* 10003 */
  eOS_FileNameTooLongError,               /* 10004 */
  eOS_ReadOnlyFileSystemError,            /* 10005 */
  eOS_MaxFileSystemLimitError,            /* 10006 */
  eOS_HardDiskLowLevelError,              /* 10007 */
  eOS_OtherFileSystemError,               /* 10008 */
  eOS_DirectoryNotFoundError,             /* 10009 */
  eOS_Encryption_Key_Not_Found_Error,     /* 10010 */
  eOS_Encryption_Key_Changed_Error,       /* 10011 */
  eOS_App_Logic_Error,                    /* 10012 */
  eOS_Bad_Msg_Index_Error,                /* 10013 */
  eOS_Protocol_Error,                     /* 10014 */
  eOS_Challenge_Error,                    /* 10015 */
  eOS_AbnormalNoSoftwareAvailableError,   /* 10016 */
  eOS_SoftwareDescriptionTooLong,         /* 10017 */
  eOS_ClientToolKeyError,                 /* 10018 */
  eOS_SessionCreationError,               /* 10019 */
  eOS_SessionManager_CreationError        /* 10020 */

} tOTUStatus;

#endif /* _OTU_STATUS_H_  */
