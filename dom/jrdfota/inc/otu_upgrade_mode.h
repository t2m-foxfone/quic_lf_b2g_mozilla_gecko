#ifndef _OTU_UPGRADE_MODE_H_
#define _OTU_UPGRADE_MODE_H_

/// OTU Upgrade Mode
///
/// This enum lists all the available OTU Upgrade mode.
///
/// CAUTION: DO NOT MODIFY THE VALUE OF ENUM ALREADY SET.
///          IN ORDER TO INSURE BACKWARD COMPATIBILITY,
///           ONLY NEW VALUES INSERTION SHOULD BE DONE.
typedef enum {
  eOUM_Unknown=0,               /*     0 */
  eOUM_Full=1,                  /*     1 */
  eOUM_Incremental=2,           /*     2 */
  eOUM_Teleweb=3,               /*     3 */
  eOUM_Sugar=4                  /*     4 */
} tOTUUpgradeMode;

#endif /* _OTU_UPGRADE_MODE_H_  */
