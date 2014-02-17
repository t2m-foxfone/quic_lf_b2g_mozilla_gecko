/*
  File Name: JrdNvAccess.h
  Author   : Xu Zhe <zhe.xu@tcl.com>
*/
#ifndef __JRDNVACCESS__
#define __JRDNVACCESS__

#include "JrdINvAccess.h"

#define JRDNVACCESS_CID { 0x2dafea4c, 0x7163, 0x4b70, { 0x95, 0x4e, 0x5a, 0xd4, 0x09, 0x94, 0x83, 0xd8 } }
#define JRDNVACCESS_CONTRACTID "@jrdcom.com/JrdNvAccess;1"

namespace mozilla {
  namespace dom {
    namespace jrd {

      class JrdNvAccess : public nsIJrdNvAccess
      {
      public:
	NS_DECL_ISUPPORTS
	  NS_DECL_NSIJRDNVACCESS

	  JrdNvAccess();

      private:
	~JrdNvAccess();

      protected:
	/* additional members */
      };
    } // jrd
  } // dom
} // mozilla

#endif
