/*
  File Name: JrdNvAccess.cpp
  Author   : Xu Zhe <zhe.xu@tcl.com>
  Function : provide nvram access to gecko components.
  Requests : root privilege ( hardware access )
*/
#include "nsIDOMClassInfo.h"
#include "JrdNvAccess.h"
#include "Jrdmacro.h"
#include "dlfcn.h"

#if defined(MOZ_WIDGET_GONK)
/* nvram is only available on mobile phone. */
extern "C" {
  typedef  unsigned char      boolean;     /* Boolean value type. */
#if 1
#include "tracability.h"
#endif
}
#endif // MOZ_WIDGET_GONK

DOMCI_DATA(JrdNvAccess, mozilla::dom::jrd::JrdNvAccess)

namespace mozilla {
  namespace dom {
    namespace jrd {

      NS_INTERFACE_MAP_BEGIN(JrdNvAccess)
      NS_INTERFACE_MAP_ENTRY(nsIJrdNvAccess)
      NS_INTERFACE_MAP_ENTRY(nsISupports)
      NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(JrdNvAccess)
      NS_INTERFACE_MAP_END
      NS_IMPL_ADDREF(JrdNvAccess)
      NS_IMPL_RELEASE(JrdNvAccess)

      void* handle = NULL;
// liukun@tcl.com add 2014-03-20
      void* handleEx = NULL;
      uint8_t* pReadBuff = NULL;
#if defined(MOZ_WIDGET_GONK)
      int (*nv_operation_p)(NV_OP operation, uint16_t item_id, uint8_t* data, uint32_t* data_len) = NULL;
// end      
/* czb@tcl.com add for root check start*/
	  int (*tracability_read_root_flag_p)(tracability_item_type item, char* buf, uint16 size) = NULL;
/* czb@tcl.com add for root check end*/
      int (*tracability_read_p)(tracability_item_type item, char* buf, uint16 size) = NULL;
      int (*tracability_write_p)(tracability_item_type item, char* buf, uint16 size) = NULL;
      tracability_item_struct_t *tracability_data_map_p = NULL;
#endif

      JrdNvAccess::JrdNvAccess()
      {
            LOG("constructor\n");
#if defined(MOZ_WIDGET_GONK)
            handle = dlopen("/system/lib/libtraceability.so", RTLD_LAZY);
	    handleEx = dlopen("/system/lib/libnvrw.so", RTLD_LAZY);
            if (!handle) {
                LOG("maxh dlopen failure \n");
            } else {
			/*czb@tcl.com add for root check start*/
                tracability_read_root_flag_p = (int (*)(tracability_item_type, char*, uint16))dlsym(handle, "tracability_read_root_flag");
			/*czb@tcl.com add for root check end*/
                tracability_read_p = (int (*)(tracability_item_type, char*, uint16))dlsym(handle, "tracability_read");
                tracability_write_p = (int (*)(tracability_item_type, char*, uint16))dlsym(handle, "tracability_write");
                tracability_data_map_p = (tracability_item_struct_t *)dlsym(handle, "tracability_data_map");
		
            }
            // liukun add
	    if (!handleEx) {
                LOG("handleEx dlopen failure \n");
            } else {
                nv_operation_p = (int (*)(NV_OP, uint16_t, uint8_t*, uint32_t*))dlsym(handleEx, "t2m_qmi_nv_rw");
		// vendor/jrdcom/include/qmi_tct_api_v01.h:#define QMI_TCT_NV_ITEM_DATA_MAX_SIZE_V01 512
	        pReadBuff = (uint8_t*)NS_Alloc(512);
		if(!pReadBuff)
		  LOG("NS_Alloc pReadBuff failure \n");
            }
            // end
#endif
      }

      JrdNvAccess::~JrdNvAccess()
      {
            /* destructor code */
            LOG("destructor.\n");
#if defined(MOZ_WIDGET_GONK)
            if (handle) {
                dlclose(handle);
                handle = NULL;
			/*czb@tcl.com add for root check start*/
				tracability_read_root_flag_p = NULL;
			/*czb@tcl.com add for root check start*/
                tracability_read_p = NULL;
                tracability_write_p = NULL;
                tracability_data_map_p = NULL;
            }
            // liukun add
	    if (handleEx) {
                dlclose(handleEx);
                handleEx = NULL;
                nv_operation_p = NULL;
		if(pReadBuff){
		  NS_Free(pReadBuff);
		  pReadBuff = NULL;
		}
            }
            // end
#endif
      }

/*void readNvitemRoot (in long item, [optional] out unsigned long count, [retval, array, size_is (count)]  out octet value)*/

NS_IMETHODIMP JrdNvAccess::ReadNvitemRoot(int32_t item, uint32_t *count, uint8_t **value)
{
#ifdef MOZ_WIDGET_GONK
	LOG("enter\n");
	NULLPOINTER(count);
	NULLPOINTER(value);
	if (!tracability_read_root_flag_p || !tracability_data_map_p) {
		LOG("Error: tracability_read_root_flag_p is NULL!\n");
		return NS_ERROR_FAILURE;
	}
	item = 1;
	*count = 1;
	LOG("JrdNvAccess::ReadNvitemRoot item = %d\t | count1 = %d\n", item, *count);
	*value = (uint8_t*)NS_Alloc(*count + 10);
	NULLPOINTER(*value);
	int ret = tracability_read_root_flag_p((tracability_item_type)item, (char *)*value, *count);
	if (ret < 0) {
		LOG("tracability_read_root_flag() return error!\n");
		NS_Free(*value);
		*value = NULL;
		return NS_ERROR_FAILURE;
    }
#endif
	LOG("JrdNvAccess::ReadNvitem return\n");
	return NS_OK;
}

      /* void readNvItem (in long item, in unsigned long count, [array, size_is (count)] out octet value); */
      NS_IMETHODIMP JrdNvAccess::ReadNvitem(int32_t item, uint32_t *count, uint8_t **value)
      {
#ifdef MOZ_WIDGET_GONK
            LOG("enter\n");
            NULLPOINTER(count);
            NULLPOINTER(value);
            /*
            if (!item) {
                *count = sizeof(tracability_region_struct_t);
            } else if (item > 0){
                if (item == 45) {
                    *count = 1;
                } else {
                    *count = 4;
                }
            } else {
                *value = NULL;
                return NS_ERROR_FAILURE;
            }
            */
            if (!tracability_read_p || !tracability_data_map_p) {
                LOG("Error: tracability_read_p is NULL!\n");
                return NS_ERROR_FAILURE;
            }
            if (item < 0 || item > TRACABILITY_ITEM_TYPE_MAX) {
                LOG("Warning: input argument item is %d, should be > 0, use 0 instead and read whole region\n", item);
                *count = sizeof(tracability_region_struct_t);
                item = 0;
                //return NS_ERROR_FAILURE;
            } else {
                *count = tracability_data_map_p[item].size;
            }
            LOG("JrdNvAccess::ReadNvitem item = %d\t | count1 = %d\n", item, *count);
            *value = (uint8_t*)NS_Alloc(*count + 10);
            NULLPOINTER(*value);

            int ret = tracability_read_p((tracability_item_type)item, (char *)*value, *count);
            if (ret < 0) {
                LOG("tracability_read() return error!\n");
                NS_Free(*value);
                *value = NULL;
                return NS_ERROR_FAILURE;
            }
#endif
            LOG("JrdNvAccess::ReadNvitem return\n");

            return NS_OK;
      }

      /* void writeNvItem (in long item, in unsigned long count, [array, size_is (count)] in octet value); */
      NS_IMETHODIMP JrdNvAccess::WriteNvitem(int32_t item, uint32_t count, uint8_t *value)
      {
#ifdef MOZ_WIDGET_GONK
            LOG("enter WriteNvitem \n");
            NULLPOINTER(value);
            LOG("going to write nvram item %d.\n", item);
            if (sizeof (tracability_region_struct_t) < count) {
                return NS_ERROR_FAILURE;
            }

            if (!tracability_write_p) {
                LOG("Error: tracability_write_p is NULL!\n");
                return NS_ERROR_FAILURE;
            }

            int ret = tracability_write_p((tracability_item_type)item, (char *)value, count);
            if (ret < 0) {
                return NS_ERROR_FAILURE;
            }
#endif
            return NS_OK;
      }
      
      
      /* void readNvitemEx (in long item, [optional] out unsigned long count, [array, size_is (count), retval] out octet value); */
      NS_IMETHODIMP JrdNvAccess::ReadNvitemEx(int32_t item, uint32_t *count, uint8_t **value)
      {
#ifdef MOZ_WIDGET_GONK
	    uint32_t data_len = 0;
            LOG("enter\n");
            NULLPOINTER(count);
            NULLPOINTER(value);

            if (!nv_operation_p) {
                LOG("Error: nv_operation_p is NULL!\n");
                return NS_ERROR_FAILURE;
            }

            if (item < 0) {
                LOG("Error: input argument item is %d, should be > 0!!!\n", item);
                return NS_ERROR_FAILURE;
            }

            LOG("NvOperation::ReadNv item = %d\n", item);
	    NULLPOINTER(pReadBuff);

            int ret = nv_operation_p(NV_OP_READ, item, pReadBuff, &data_len);
            if (ret < 0) {
                LOG("NvOperation::ReadNv return error! --> ret:%d\n",ret);
                return NS_ERROR_FAILURE;
            }
            *count = data_len;
	    *value = pReadBuff;
#endif
            LOG("NvOperation::ReadNv sucess !!!\n");

            return NS_OK;
      }

      /* void writeNvitemEx (in long item, in unsigned long count, [array, size_is (count)] in octet value); */
      NS_IMETHODIMP JrdNvAccess::WriteNvitemEx(int32_t item, uint32_t count, uint8_t *value)
      {
#ifdef MOZ_WIDGET_GONK
            LOG("enter \n");
            NULLPOINTER(value);

            LOG("going to write nvram item %d.\n", item);

	    if (item < 0) {
                LOG("Error: input argument item is %d, should be > 0!!!\n", item);
                return NS_ERROR_FAILURE;
            }

            if (!nv_operation_p) {
                LOG("Error: nv_operation_p is NULL!\n");
                return NS_ERROR_FAILURE;
            }

            int ret = nv_operation_p(NV_OP_WRITE, item, value, &count);
            if (ret < 0) {
		LOG("NvOperation::WriteNv return error! --> ret:%d\n",ret);
                return NS_ERROR_FAILURE;
            }
#endif
	    LOG("NvOperation::WriteNv sucess\n");
            return NS_OK;
      }
    }// jrd
  }// dom
}// mozilla
