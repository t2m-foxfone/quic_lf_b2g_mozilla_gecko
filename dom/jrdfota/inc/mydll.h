#ifndef _MYDLL_H_
#define _MYDLL_H_

#if defined(WIN32)

#ifdef BUILD_DLL
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
/* #define EXPORT __declspec(dllimport) */
#define EXPORT

#endif

#else

#define EXPORT

#endif


#endif /* _MYDLL_H_ */
