/*
 * Project:      GPIO Driver definitions
 */

#ifndef __DRIVER_GPIO_H
#define __DRIVER_GPIO_H

#include "Driver_Common.h"

// API version
#define NDS_GPIO_API_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2, 02)

// GPIO direction
#define NDS_GPIO_DIR_INPUT             0x0
#define NDS_GPIO_DIR_OUTPUT            0x1

// GPIO control mode
#define NDS_GPIO_INTR_ENABLE            (1UL << 0)
#define NDS_GPIO_INTR_DISABLE           (1UL << 1)
#define NDS_GPIO_SET_INTR_LOW_LEVEL     (1UL << 2)
#define	NDS_GPIO_SET_INTR_HIGH_LEVEL    (1UL << 3)
#define	NDS_GPIO_SET_INTR_NEGATIVE_EDGE (1UL << 4)
#define	NDS_GPIO_SET_INTR_POSITIVE_EDGE (1UL << 5)
#define	NDS_GPIO_SET_INTR_DUAL_EDGE     (1UL << 6)

// GPIO signal event ID
#define	NDS_GPIO_EVENT_PIN0             (1UL << 0)
#define	NDS_GPIO_EVENT_PIN1             (1UL << 1)
#define	NDS_GPIO_EVENT_PIN2             (1UL << 2)
#define	NDS_GPIO_EVENT_PIN3             (1UL << 3)
#define	NDS_GPIO_EVENT_PIN4             (1UL << 4)
#define NDS_GPIO_EVENT_PIN5             (1UL << 5)
#define	NDS_GPIO_EVENT_PIN6             (1UL << 6)
#define	NDS_GPIO_EVENT_PIN7             (1UL << 7)
#define	NDS_GPIO_EVENT_PIN8             (1UL << 8)
#define	NDS_GPIO_EVENT_PIN9             (1UL << 9)
#define	NDS_GPIO_EVENT_PIN10            (1UL << 10)
#define	NDS_GPIO_EVENT_PIN11            (1UL << 11)
#define	NDS_GPIO_EVENT_PIN12            (1UL << 12)
#define	NDS_GPIO_EVENT_PIN13            (1UL << 13)
#define	NDS_GPIO_EVENT_PIN14            (1UL << 14)
#define	NDS_GPIO_EVENT_PIN15            (1UL << 15)
#define	NDS_GPIO_EVENT_PIN16            (1UL << 16)
#define	NDS_GPIO_EVENT_PIN17            (1UL << 17)
#define	NDS_GPIO_EVENT_PIN18            (1UL << 18)
#define	NDS_GPIO_EVENT_PIN19            (1UL << 19)
#define	NDS_GPIO_EVENT_PIN20            (1UL << 20)
#define	NDS_GPIO_EVENT_PIN21            (1UL << 21)
#define	NDS_GPIO_EVENT_PIN22            (1UL << 22)
#define	NDS_GPIO_EVENT_PIN23            (1UL << 23)
#define	NDS_GPIO_EVENT_PIN24            (1UL << 24)
#define	NDS_GPIO_EVENT_PIN25            (1UL << 25)
#define	NDS_GPIO_EVENT_PIN26            (1UL << 26)
#define	NDS_GPIO_EVENT_PIN27            (1UL << 27)
#define	NDS_GPIO_EVENT_PIN28            (1UL << 28)
#define	NDS_GPIO_EVENT_PIN29            (1UL << 29)
#define	NDS_GPIO_EVENT_PIN30            (1UL << 30)
#define	NDS_GPIO_EVENT_PIN31            (1UL << 31)

// callback function
typedef void (*NDS_GPIO_SignalEvent_t) (uint32_t event);

// access structure of the GPIO driver
typedef struct _NDS_DRIVER_GPIO {
	NDS_DRIVER_VERSION     (*GetVersion)     (void);
	int32_t                (*Initialize)     (NDS_GPIO_SignalEvent_t cb_event);
	int32_t                (*Uninitialize)   (void);
        void                   (*PinWrite)       (uint32_t pin_num, int32_t val);
	uint8_t                (*PinRead)        (uint32_t pin_num);
	void                   (*Write)          (uint32_t mask, uint32_t val);
	uint32_t               (*Read)           (void);
	void                   (*SetDir)         (uint32_t mask, int32_t dir);
	int32_t                (*Control)        (uint32_t mode, uint32_t val);
} const NDS_DRIVER_GPIO;

#endif /* __DRIVER_GPIO_H */
