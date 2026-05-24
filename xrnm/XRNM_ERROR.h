#pragma once

#define XRNM_ERROR_OK           ((int32_t)0x00000000) // Success.
#define XRNM_ERROR_ENOMEM       ((int32_t)0x8007000E) // Failed to allocate internal buffer.
#define XRNM_ERROR_ERANGE       ((int32_t)0x807A1001) // Output buffer is too small, if provided, the size parameter will contain the size required for a successful call.
#define XRNM_ERROR_ENOTEADY     ((int32_t)0x807A1002) // 
#define XRNM_ERROR_ENETDOWN     ((int32_t)0x807A1003) // 
#define XRNM_ERROR_EADDRINUSE   ((int32_t)0x807A1004) // 
#define XRNM_ERROR_ENOBUFS      ((int32_t)0x807A1005) // Buffers limit exceeded
#define XRNM_ERROR_1006         ((int32_t)0x807A1006) // 
#define XRNM_ERROR_EINVAL       ((int32_t)0x807A1009) // Invalid parameter value (sent NULL to a required pointer for example).
#define XRNM_ERROR_EUNEXPECTED  ((int32_t)0x807A100A) // Unexpected error
#define XRNM_ERROR_EHOSTUNREACH ((int32_t)0x807A1014) //
#define XRNM_ERROR_ESENDTIMEOUT ((int32_t)0x807A1016) //
#define XRNM_ERROR_101E         ((int32_t)0x807A101E) // 
#define XRNM_ERROR_1020         ((int32_t)0x807A1020) // 
#define XRNM_ERROR_ENOTCONN     ((int32_t)0x807A1039) // 
#define XRNM_ERROR_ENOTSOCK     ((int32_t)0x807A103B) // 
#define XRNM_ERROR_EINVAL_WSA   ((int32_t)0x807A103C) // 
#define XRNM_ERROR_EACCESS      ((int32_t)0x807A103D) // 
#define XRNM_ERROR_EFAULT       ((int32_t)0x807A103E) // 
#define XRNM_ERROR_ENOSUPPORT   ((int32_t)0x807A103F) // 

