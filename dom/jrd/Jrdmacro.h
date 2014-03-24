#if !defined(__JRDMACRO_H__)
#define __JRDMACRO_H__

#undef LOG
#if TARGET_BUILD_VARIANT == eng // eng

#if defined(MOZ_WIDGET_GONK) // android

#include <android/log.h>
#define LOG(...)  __android_log_print(ANDROID_LOG_INFO, "__FUNCTION__", "%s:%d %s. ", __FILE__, __LINE__, __PRETTY_FUNCTION__ ) ;  __android_log_print(ANDROID_LOG_INFO, "JrdNvAccess", __VA_ARGS__);

#else // desktop

#define LOG(...) printf("%s:%d %s. ", __FILE__, __LINE__, __PRETTY_FUNCTION__ );printf(__VA_ARGS__);

#endif

#else // not eng

#define LOG(...)

#endif

#undef NULLPOINTER
#define NULLPOINTER(P) if (NULL == P){ \
    LOG("null pointer %x \n", P);	\
	  return NS_ERROR_NULL_POINTER; \
	}

#endif // __JRDMACRO_H__
