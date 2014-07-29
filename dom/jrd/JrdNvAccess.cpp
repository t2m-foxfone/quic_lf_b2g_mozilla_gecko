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
#if defined(MOZ_WIDGET_GONK)
      int (*tracability_read_p)(tracability_item_type item, char* buf, uint16 size) = NULL;
      int (*tracability_write_p)(tracability_item_type item, char* buf, uint16 size) = NULL;
      tracability_item_struct_t *tracability_data_map_p = NULL;
#endif

      JrdNvAccess::JrdNvAccess()
      {
            LOG("constructor\n");
#if defined(MOZ_WIDGET_GONK)
            handle = dlopen("/system/lib/libtraceability.so", RTLD_LAZY);
            if (!handle) {
                LOG("maxh dlopen failure \n");
            } else {
                tracability_read_p = (int (*)(tracability_item_type, char*, uint16))dlsym(handle, "tracability_read");
                tracability_write_p = (int (*)(tracability_item_type, char*, uint16))dlsym(handle, "tracability_write");
                tracability_data_map_p = (tracability_item_struct_t *)dlsym(handle, "tracability_data_map");
            }
#endif
      }

      JrdNvAccess::~JrdNvAccess()
      {
            /* destructor code */
            LOG("destructor.\n");
#if defined(MOZ_WIDGET_GONK)
            if (!handle) {
                dlclose(handle);
                handle = NULL;
                tracability_read_p = NULL;
                tracability_write_p = NULL;
                tracability_data_map_p = NULL;
            }
#endif
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
    }// jrd
  }// dom
}// mozilla
