/*
 * Project:      Common Driver definitions
 */

#ifndef __DRIVER_COMMON_H
#define __DRIVER_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define NDS_DRIVER_VERSION_MAJOR_MINOR(major,minor) (((major) << 8) | (minor))

/**
\brief Driver Version
*/
typedef struct _NDS_DRIVER_VERSION {
  uint16_t api;                         ///< API version
  uint16_t drv;                         ///< Driver version
} NDS_DRIVER_VERSION;

/* General return codes */
#define NDS_DRIVER_OK                 0 ///< Operation succeeded 
#define NDS_DRIVER_ERROR             -1 ///< Unspecified error
#define NDS_DRIVER_ERROR_BUSY        -2 ///< Driver is busy
#define NDS_DRIVER_ERROR_TIMEOUT     -3 ///< Timeout occurred
#define NDS_DRIVER_ERROR_UNSUPPORTED -4 ///< Operation not supported
#define NDS_DRIVER_ERROR_PARAMETER   -5 ///< Parameter error
#define NDS_DRIVER_ERROR_SPECIFIC    -6 ///< Start of driver specific errors 

/**
\brief General power states
*/ 
typedef enum _NDS_POWER_STATE {
  NDS_POWER_OFF,                        ///< Power off: no operation possible
  NDS_POWER_LOW,                        ///< Low Power mode: retain state, detect and signal wake-up events
  NDS_POWER_FULL                        ///< Power on: full operation at maximum performance
} NDS_POWER_STATE;

#endif /* __DRIVER_COMMON_H */
