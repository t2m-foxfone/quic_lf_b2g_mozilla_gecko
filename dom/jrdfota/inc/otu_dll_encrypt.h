#ifndef __COMMON_INC_OTU_ENCRYPT_H__
#define __COMMON_INC_OTU_ENCRYPT_H__


#include <string.h> // RS - On some host, memset needs this include.

#include "mytypes.h"

typedef enum
{
    eECB_EncryptStringCB = 10,
    eECB_GetKeyIdCB = 20,
    eECB_GetVersionIdCB =30,

    // Add before this line.
    eECB_End
}eEncryptCallBack;


#define OTU_DLL_KEY_CBF_ARRAY_MAX_SIZE   (eECB_End + 10)
#define OTU_DLL_PLAIN_DATA_LEN           (512 + 1)
#define OTU_DLL_ENCRYPT_DATA_LEN         (512 + 1)
#define OTU_DLL_MAX_KEY_LEN              (1024 + 1)


struct tOTUDllEncryptInfo
{
    tOTUDllEncryptInfo():
    plain_data(),
    encrypt_data(),
    encrypt_data_len(0),
    key_id((tUInt32)-1),
    client_version_id(0)
    {
        memset(plain_data, 0, sizeof(plain_data));
        memset(encrypt_data, 0, sizeof(encrypt_data));
    }

    ~tOTUDllEncryptInfo() {}

    /**
      This field is plain data.
     */
    tByte   plain_data[OTU_DLL_PLAIN_DATA_LEN];

    /**
      This field is signature.
     */
    tByte   encrypt_data[OTU_DLL_ENCRYPT_DATA_LEN];

    /**
      This field is the length of _encrypt_data.
     */
    tUInt32 encrypt_data_len;

    /**
      This field is the DSA key id.
    */
    tUInt32 key_id;

    /**
      This field is the DSA key id.
    */
    tUInt32 client_version_id;
};



/**********************************************************
  * Return values:
  *  0 - Success
  *  others - Failed
  *
  * Parameters:
  *  out_str               [OUT] The output buffer address
  *  out_str_len          [OUT] The length of output string
  *  max_out_len        [IN]    The size of output buffer
  *  in_str                 [IN]    The input string address
  *  in_str_len            [IN]    The size of input string
  *
  * Note:
  *  The memory for out_str & out_str_len has been already allocated. The size
  *  of out_str is specified by max_out_len.
  **********************************************************/
typedef int (*OTU_DLL_KEY_CBF_T)(tByte *out_str,
                                     tUInt32 *out_str_len,
                                     tUInt32 max_out_len,
                                     const tByte *in_str,
                                     tUInt32 in_str_len
                                    );

#endif
