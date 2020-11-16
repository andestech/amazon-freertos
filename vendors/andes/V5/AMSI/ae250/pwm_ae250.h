/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __PWM_AE250_H
#define __PWM_AE250_H

#include "ae250.h"
#include "Driver_PWM.h"
#include "platform.h"

// Number of PWM channels
#define PWM_NUMBER_OF_CHANNELS           (4)

#define PWM_EXTCLK_FREQ                  (32768)
#define PWM_APBCLK_FREQ                  PCLKFREQ

// PIT Channel Control register
#define PIT_CH_CTRL_PWMPARK_LOW          (0U << 4)
#define PIT_CH_CTRL_PWMPARK_HIGH         (1U << 4)
#define PIT_CH_CTRL_EXTCLK               (0U << 3)
#define PIT_CH_CTRL_APBCLK               (1U << 3)
#define PIT_CH_CTRL_MODE_PWM             (4U << 0)

// PWM MAX duty length
#define PWM_DUTY_MAX                     255

// PWM flags
#define PWM_FLAG_INITIALIZED             (1U << 0)
#define PWM_FLAG_POWERED                 (1U << 1)
#define PWM_FLAG_CHANNEL_ACTIVED(CHN)    (1U << (8 + CHN))

// PWM Information (Run-Time)
typedef struct _PWM_INFO {
	NDS_PWM_SignalEvent_t   cb_event;      // Event callback
	uint32_t                flags;         // PWM driver flags
	uint32_t                clk[PWM_NUMBER_OF_CHANNELS];
	uint32_t                freq[PWM_NUMBER_OF_CHANNELS];       // PWM channel frequency
	uint32_t                duty[PWM_NUMBER_OF_CHANNELS];       // PWM channel duty cycle (0 ~ 255)
} PWM_INFO;

// PWM Resources definitions
typedef struct {
	NDS_PWM_CAPABILITIES    capabilities;  // Capabilities
	PIT_RegDef		*reg;          // Pointer to PIT peripheral
	PWM_INFO                *info;         // Run-Time Information
} const PWM_RESOURCES;

#endif /* __PWM_AE250_H */
