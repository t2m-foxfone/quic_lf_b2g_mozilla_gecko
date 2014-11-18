#ifndef _OTU_OPTIONS_H_
#define _OTU_OPTIONS_H_

#include "mytypes.h"


#define OTU_MAX_OPTION_NAME_LEN    30
#define OTU_MAX_OPTION_VALUE_LEN   255

#define OTU_MAX_INFO_FLAG_LEN    30
#define OTU_MAX_INFO_VALUE_LEN   30

#define OTU_REPORT_INFO_VALUE_1 "1"
#define OTU_REPORT_INFO_VALUE_2 "2"

#define OTU_REPORT_INFO_VALUE_ERROR "100"


#define OTU_ROOT_FLAG "root_flag"
#define OTU_ROOT_FLAG_VALUE_NO  OTU_REPORT_INFO_VALUE_1
#define OTU_ROOT_FLAG_VALUE_YES OTU_REPORT_INFO_VALUE_2

#define OTU_CONNECT_TYPE "connect_type"
#define OTU_CONNECT_TYPE_VALUE_3G    OTU_REPORT_INFO_VALUE_1
#define OTU_CONNECT_TYPE_VALUE_WIFI  OTU_REPORT_INFO_VALUE_2

#define OTU_CHECK_TYPE "check_type"
#define OTU_CHECK_TYPE_VALUE_AUTO     OTU_REPORT_INFO_VALUE_1
#define OTU_CHECK_TYPE_VALUE_MANUAL   OTU_REPORT_INFO_VALUE_2

#define OTU_SESSION_BACKUP_FILE_NAME  "session_backup.txt"

typedef struct otu_options
{
  char *option_name;
  char *option_value;
} tOTUOption;


typedef struct otu_report_info
{
  char *info_flag;
  char *info_value;
} tOTUReport_info;

#endif /* _OTU_OPTIONS_H_ */

