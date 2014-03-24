#ifndef _OTU_OP_H_
#define _OTU_OP_H_

/// OTU Operation
///
/// This enum lists all the available OTU operations.
///
/// CAUTION: DO NOT MODIFY THE VALUE OF ENUM ALREADY SET.
///          IN ORDER TO INSURE BACKWARD COMPATIBILITY,
///           ONLY NEW VALUES INSERTION SHOULD BE DONE.
typedef enum {
  eOO_Unknown=0,                       /*     0 */
  eOO_Initialization=10,               /*    10 */
  eOO_VersionCheck=50,                 /*    50 */
  eOO_VersionReq = 100,                /*    100 */
  eOO_Download=1000,                   /* 1000 */
  eOO_Upgrade=2000,                    /*  2000 */
  eOO_ClientToolKeyCheck=3000,         /*  3000 */
  eOO_Shutdown=100000                  /* 100000 */
} tOTUOperation;

#endif /* _OTU_OP_H_  */
