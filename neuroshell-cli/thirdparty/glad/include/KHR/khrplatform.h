#ifndef __khrplatform_h_
#define __khrplatform_h_

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   if defined(_MSC_VER)
#       define KHRONOS_APIENTRY __stdcall
#   elif (defined(__GNUC__) && __GNUC__ >= 4) || defined(__SUNPRO_C)
#       define KHRONOS_APIENTRY __attribute__((stdcall))
#   else
#       define KHRONOS_APIENTRY
#   endif
#else
#   define KHRONOS_APIENTRY
#endif

typedef int32_t khronos_int32_t;
typedef uint32_t khronos_uint32_t;
typedef int64_t khronos_int64_t;
typedef uint64_t khronos_uint64_t;

#endif /* __khrplatform_h_ */
