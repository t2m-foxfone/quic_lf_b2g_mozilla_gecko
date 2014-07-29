#ifndef _UNUSED_H_
#define _UNUSED_H_

/*
   This macro allows us to compile with full warnings sensitivity
   and correct eventual warning about unused parameters
*/

#ifdef UNUSED
#elif defined(__GNUC__)
#if defined(linux)

# if (__GNUC__>3)
#  define UNUSED(x) x __attribute__ ((unused))
# else
#  if (__GNUC__==3)
#   if (__GNUC_MINOR__>2)
#    define UNUSED(x) x __attribute__ ((unused))
#   else
     /* GCC_Version <= 3.2.X: The __attribute__ directive is not supported. Then
    we won't correct the warning!
      */
#    define UNUSED(x) x
#   endif
#  else
    /*  __GNUC__ < 3  */
#   define UNUSED(x) x
#  endif
# endif

#elif defined(__APPLE__)
# define UNUSED(x) x
#else
//# define UNUSED(x) UNUSED_ ## x __attribute__ ((unused))
# define UNUSED(x)  x __attribute__ ((unused))
#endif
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif


#endif /* _UNUSED_H_ */

