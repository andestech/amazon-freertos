#
# Copyright (c) 2012-2017 Andes Technology Corporation
# All rights reserved.
#

PROG	?= Demo

# For Amazon FreeRTOS,default has two APP, TEST and DEMO
APP	?= TEST

# Amazon FreeRTOS only support XIP mode
MODE	= XIP

# Platform: only use corvette_f1_ae250 in Amazon FreeRTOS
PLAT    = corvette_f1_ae250
MCU     = AE250
MCUNAME = $(shell echo $(MCU) | tr A-Z a-z)

ifndef CROSS_COMPILE
	ifeq (riscv32,$(COMPILER))
		CROSS_COMPILE = riscv32-elf-
	endif
endif
ifndef CROSS_COMPILE
	ifeq (riscv64,$(COMPILER))
		CROSS_COMPILE = riscv64-elf-
	endif
endif
ifndef CROSS_COMPILE
$(error You have to specify 'COMPILER' to build ! Ex: make COMPILER=[riscv32|riscv64])
endif

CC	:= $(CROSS_COMPILE)gcc
C++     := $(CROSS_COMPILE)g++
OBJDUMP	:= $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
AR	:= $(CROSS_COMPILE)ar
AS	:= $(CROSS_COMPILE)as

ifeq ($(DEBUG),1)
	OPTIM   := -O0 -g3
else
	OPTIM   := -O3 -g3
endif


# Define the directories
ROOT			= ../../../../../amazon-freertos
NDS_SOURCE_DIR		= $(ROOT)/vendors/andes/V5
BSP_SOURCE_DIR          = $(ROOT)/vendors/andes/V5/bsp
DEVICE_DRI_DIR		= $(NDS_SOURCE_DIR)/bsp/driver
AMSI_DRI_DIR		= $(NDS_SOURCE_DIR)/AMSI/src
WIFI_DRI_DIR		= $(NDS_SOURCE_DIR)/wifi
FREERTOS_SOURCE_DIR	= $(ROOT)/freertos_kernel
TEST_APP_SOURCE_DIR	= $(ROOT)/tests/common
DEMO_APP_SOURCE_DIR	= $(ROOT)/demos/common
AWS_LIBPORT_DIR		= $(ROOT)/vendors/andes/boards/corvette_f1_ae250/ports

AWS_TEST_DIR		= $(ROOT)/vendors/andes/boards/corvette_f1_ae250/aws_tests
AWS_DEMO_DIR	        = $(ROOT)/vendors/andes/boards/corvette_f1_ae250/aws_demos

FREERTOS_PLUS_DIR	= $(ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp
MBEDTLS_DIR		= $(ROOT)/libraries/3rdparty/mbedtls

# Define the source files we have

# BSP source files
STARTUP_SRCS = $(BSP_SOURCE_DIR)/start.S

BSP_SRCS = \
	$(BSP_SOURCE_DIR)/reset.c \
	$(BSP_SOURCE_DIR)/initfini.c \
	$(BSP_SOURCE_DIR)/trap.c \
	$(BSP_SOURCE_DIR)/cache.c \
	$(BSP_SOURCE_DIR)/$(MCUNAME)/$(MCUNAME).c \
	$(BSP_SOURCE_DIR)/$(MCUNAME)/interrupt.c \
	$(BSP_SOURCE_DIR)/$(MCUNAME)/iochar.c \
	$(BSP_SOURCE_DIR)/libc/stdio/do_printf.c \
	$(BSP_SOURCE_DIR)/libc/stdio/printf.c

DRI_SRCS = \
	$(DEVICE_DRI_DIR)/uart.c \
	$(AWS_LIBPORT_DIR)/I2C_EEPROM/src/i2c_eeprom.c \
	$(AMSI_DRI_DIR)/ae250/dma_ae250.c \
	$(AMSI_DRI_DIR)/ae250/gpio_ae250.c \
	$(AMSI_DRI_DIR)/ae250/i2c_ae250.c \
	$(AMSI_DRI_DIR)/ae250/spi_ae250.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/wdrvext_wilc1000.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/wilc1000_task.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/common/source/nm_common.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/spi_flash/source/spi_flash.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/nmdrv.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/nmbus.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/m2m_periph.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/m2m_wifi.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/m2m_hif.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/nmasic.c \
	$(WIFI_DRI_DIR)/wilc1000/dev/spi/wdrv_wilc1000_spi.c \
	$(WIFI_DRI_DIR)/wilc1000/dev/gpio/wdrv_wilc1000_gpio.c \
	$(WIFI_DRI_DIR)/wilc1000/dev/gpio/wdrv_wilc1000_eint.c \
	$(WIFI_DRI_DIR)/wilc1000/osal/wdrv_wilc1000_osal.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver/wdrv_wilc1000_main.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver/wdrv_wilc1000_connmgr.c\
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source/nmspi.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver/wdrv_wilc1000_scan_helper.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver/wdrv_wilc1000_events.c \
	$(WIFI_DRI_DIR)/wilc1000/wireless_driver/wdrv_wilc1000_config.c \
	$(WIFI_DRI_DIR)/wilc1000/include/tmr/sys_tmr.c \
	$(WIFI_DRI_DIR)/framework/osal/src/osal_freertos.c \
	$(WIFI_DRI_DIR)/framework/tcpip/src/tcpip_helpers.c \

TCP_SRCS = \
	$(FREERTOS_PLUS_DIR)/source/portable/NetworkInterface/andes/NetworkInterface.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_Sockets.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_IP.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_DNS.c \
	$(FREERTOS_PLUS_DIR)/source/portable/NetworkInterface/andes/BufferAllocation_2.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_UDP_IP.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_ARP.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_DHCP.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_TCP_WIN.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_Stream_Buffer.c \
	$(FREERTOS_PLUS_DIR)/source/FreeRTOS_TCP_IP.c \
	$(ROOT)/libraries/abstractions/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c


TLS_SRCS = \
	$(MBEDTLS_DIR)/library/pk.c \
	$(MBEDTLS_DIR)/library/pkparse.c \
	$(MBEDTLS_DIR)/library/pk_wrap.c \
	$(MBEDTLS_DIR)/library/pkwrite.c \
	$(MBEDTLS_DIR)/library/platform.c \
	$(MBEDTLS_DIR)/library/platform_util.c \
	$(MBEDTLS_DIR)/library/md.c \
	$(MBEDTLS_DIR)/library/md_wrap.c \
	$(MBEDTLS_DIR)/library/sha1.c \
	$(MBEDTLS_DIR)/library/sha256.c \
	$(MBEDTLS_DIR)/library/ctr_drbg.c \
	$(MBEDTLS_DIR)/library/x509.c \
	$(MBEDTLS_DIR)/library/x509_crt.c \
	$(MBEDTLS_DIR)/library/ecp.c \
	$(MBEDTLS_DIR)/library/ecp_curves.c \
	$(MBEDTLS_DIR)/library/threading.c \
	$(MBEDTLS_DIR)/library/entropy.c \
	$(MBEDTLS_DIR)/library/bignum.c \
	$(MBEDTLS_DIR)/library/pem.c \
	$(MBEDTLS_DIR)/library/asn1parse.c \
	$(MBEDTLS_DIR)/library/asn1write.c \
	$(MBEDTLS_DIR)/library/rsa.c \
	$(MBEDTLS_DIR)/library/rsa_internal.c \
	$(MBEDTLS_DIR)/library/oid.c \
	$(MBEDTLS_DIR)/library/aes.c \
	$(MBEDTLS_DIR)/library/ecdsa.c \
	$(MBEDTLS_DIR)/library/base64.c \
	$(MBEDTLS_DIR)/library/ssl_tls.c \
	$(MBEDTLS_DIR)/library/ssl_cli.c \
	$(MBEDTLS_DIR)/library/ssl_ciphersuites.c \
	$(MBEDTLS_DIR)/library/cipher.c \
	$(MBEDTLS_DIR)/library/cipher_wrap.c \
	$(MBEDTLS_DIR)/library/ecdh.c \
	$(MBEDTLS_DIR)/library/gcm.c \
	$(MBEDTLS_DIR)/library/entropy_poll.c \
	$(ROOT)/libraries/freertos_plus/standard/tls/src/aws_tls.c \

PKCS_SRCS = \
	$(ROOT)/libraries/abstractions/pkcs11/mbedtls/aws_pkcs11_mbedtls.c \
	$(ROOT)/demos/dev_mode_key_provisioning/src/aws_dev_mode_key_provisioning.c \
	$(ROOT)/libraries/freertos_plus/standard/crypto/src/aws_crypto.c \
	$(AWS_LIBPORT_DIR)/pkcs11/aws_pkcs11_pal.c \
	$(ROOT)/demos/dev_mode_key_provisioning/src/pem2der.c

MQTT_SRCS = \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/aws_mqtt_agent.c \

LIBC_SRCS = \
	$(AWS_LIBPORT_DIR)/wifi/aws_wifi.c \
	$(AWS_LIBPORT_DIR)/wifi/aws_wifi_assert.c

# Kernel source files
FREERTOS_SRCS = \
	$(FREERTOS_SOURCE_DIR)/list.c \
	$(FREERTOS_SOURCE_DIR)/queue.c \
	$(FREERTOS_SOURCE_DIR)/tasks.c \
	$(FREERTOS_SOURCE_DIR)/timers.c \
	$(FREERTOS_SOURCE_DIR)/event_groups.c \
	$(FREERTOS_SOURCE_DIR)/stream_buffer.c \
	$(FREERTOS_SOURCE_DIR)/portable/MemMang/heap_4.c

PORT_SRCS = \
	$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/port.c \
	$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/portASM.S \
	$(NDS_SOURCE_DIR)/FreeRTOS_asm_interrupt.S \
	$(NDS_SOURCE_DIR)/FreeRTOS_tick_config.c

RTOS_SRCS = $(FREERTOS_SRCS) $(PORT_SRCS)

AWS_SRCS = \
	$(ROOT)/libraries/c_sdk/standard/common/logging/aws_logging_task_dynamic_buffers.c \
	$(ROOT)/libraries/freertos_plus/standard/utils/src/aws_system_init.c

ifeq ($(APP),TEST)
AWS_SRCS += \
	$(ROOT)/libraries/3rdparty/unity/src/unity.c \
	$(ROOT)/libraries/3rdparty/unity/extras/fixture/src/unity_fixture.c
endif

# App source files
ifeq ($(APP),TEST)
APP_SRCS = \
	$(TEST_APP_SOURCE_DIR)/aws_test_runner.c \
	$(TEST_APP_SOURCE_DIR)/aws_test_framework.c \
	$(TEST_APP_SOURCE_DIR)/aws_test.c \
	$(TEST_APP_SOURCE_DIR)/iot_test_afr.c \
	$(ROOT)/libraries/abstractions/wifi/test/aws_test_wifi.c \
	$(ROOT)/libraries/abstractions/secure_sockets/test/aws_test_tcp.c \
	$(ROOT)/libraries/abstractions/pkcs11/test/aws_test_pkcs11.c \
	$(ROOT)/libraries/freertos_plus/standard/tls/test/aws_test_tls.c \

DEMO_SRCS = \
	$(AWS_TEST_DIR)/application_code/main.c
else
APP_SRCS = \
	$(ROOT)/demos/demo_runner/aws_demo.c \
	$(ROOT)/demos/demo_runner/iot_demo_afr.c \
	$(ROOT)/demos/demo_runner/iot_demo_runner.c \
	$(ROOT)/demos/demo_runner/aws_demo_network_addr.c \
	$(ROOT)/demos/mqtt/iot_demo_mqtt.c \
	$(ROOT)/demos/network_manager/aws_iot_network_manager.c \
	$(ROOT)/libraries/abstractions/platform/freertos/iot_threads_afr.c \
	$(ROOT)/libraries/c_sdk/standard/common/iot_init.c \
	$(ROOT)/libraries/c_sdk/standard/common/taskpool/iot_taskpool.c \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_api.c \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_operation.c \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_serialize.c \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_subscription.c \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_validate.c \
	$(ROOT)/libraries/c_sdk/standard/mqtt/src/iot_mqtt_network.c \
	$(ROOT)/libraries/c_sdk/standard/common/logging/iot_logging.c \
	$(ROOT)/libraries/abstractions/platform/freertos/iot_network_afr.c \
	$(ROOT)/libraries/abstractions/platform/freertos/iot_clock_afr.c

DEMO_SRCS = \
	$(AWS_DEMO_DIR)/application_code/main.c
endif

# Define all object files.
SRCS = $(STARTUP_SRCS) $(BSP_SRCS) $(LIBC_SRCS) $(DRI_SRCS) $(TCP_SRCS) $(TLS_SRCS) $(AWS_SRCS) $(PKCS_SRCS) $(RTOS_SRCS) $(APP_SRCS) $(DEMO_SRCS) $(MQTT_SRCS)
OBJS = $(patsubst %.S,%.o,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,${SRCS})))

### Verbosity control. Use 'make V=1' to get verbose builds.

ifeq ($(V),1)
TRACE_CC  =
TRACE_C++ =
TRACE_LD  =
TRACE_AR  =
TRACE_AS  =
Q=
else
TRACE_CC  = @echo "  CC       " $<
TRACE_C++ = @echo "  C++      " $<
TRACE_LD  = @echo "  LD       " $@
TRACE_AR  = @echo "  AR       " $@
TRACE_AS  = @echo "  AS       " $<
Q=@
endif

# Compilation options

LDSCRIPT = $(AWS_TEST_DIR)/application_code/nds_code/$(MCUNAME).ld

CMODEL = -mcmodel=large

#
# FreeRTOS needs to define an "portasmHANDLE_INTERRUPT" assembler macro to tell
# the FreeRTOS kernel which external interrupt handler to call.
# Besides, it also needs to specify the linker variable called "__freertos_irq_stack_top"
# that holds the highest address of the interrupt stack. We set __freertos_irq_stack_top
# to equal the value of linker variable "_stack".
#
DEFINES = -DportasmHANDLE_INTERRUPT=FreeRTOS_IRQ_handler -D__freertos_irq_stack_top=_stack -DUNITY_INCLUDE_CONFIG_H -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"

ifeq ($(USE_FLASHEXEC), 1)
DEFINES += -DCFG_FLASHEXEC
LDFLAGS += -Wl,--section-start=.init=0x80000000
endif

INCLUDES = \
	-I. \
	-I$(BSP_SOURCE_DIR) \
	-I$(BSP_SOURCE_DIR)/$(MCUNAME) \
	-I$(FREERTOS_SOURCE_DIR)/include \
	-I$(ROOT)/libraries/3rdparty/unity/src \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/Andes_V5_RV32RV64 \
	-I$(ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/include \
	-I$(ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/GCC \
	-I$(ROOT)/libraries/freertos_plus/standard/freertos_plus_tcp/test \
	-I$(ROOT)/libraries/freertos_plus/standard/tls/include \
	-I$(ROOT)/libraries/freertos_plus/standard/pkcs11/include \
	-I$(ROOT)/libraries/freertos_plus/standard/crypto/include \
	-I$(ROOT)/libraries/freertos_plus/standard/utils/include \
	-I$(ROOT)/libraries/freertos_plus/aws/ota/test \
	-I$(ROOT)/libraries/3rdparty/mbedtls/include \
	-I$(ROOT)/libraries/3rdparty/unity/extras/fixture/src \
	-I$(ROOT)/libraries/abstractions/wifi/include \
	-I$(ROOT)/libraries/abstractions/pkcs11/include \
	-I$(ROOT)/libraries/abstractions/platform/freertos/include \
	-I$(ROOT)/libraries/abstractions/platform/include \
	-I$(ROOT)/libraries/abstractions/secure_sockets/include \
	-I$(ROOT)/libraries/abstractions/pkcs11/mbedtls \
	-I$(ROOT)/libraries/c_sdk/standard/common/include \
	-I$(ROOT)/libraries/c_sdk/standard/mqtt/src \
	-I$(ROOT)/libraries/c_sdk/standard/mqtt/include \
	-I$(ROOT)/libraries/c_sdk/standard/common/include/private \
	-I$(ROOT)/libraries/c_sdk/standard/mqtt/src/private \
	-I$(ROOT)/libraries/3rdparty/pkcs11 \
	-I$(DEVICE_DRI_DIR)/include \
	-I$(NDS_SOURCE_DIR)/AMSI/include \
	-I$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension \
	-I$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/source \
	-I$(WIFI_DRI_DIR)/wilc1000/wireless_driver_extension/driver/include \
	-I$(WIFI_DRI_DIR)/wilc1000/include \
	-I$(WIFI_DRI_DIR)/wilc1000/include/tmr \
	-I$(WIFI_DRI_DIR)/wilc1000/wireless_driver/include \
	-I$(NDS_SOURCE_DIR) \
	-I$(ROOT)/demos/dev_mode_key_provisioning/include \
	-I$(WIFI_DRI_DIR)/framework \
	-I$(AWS_LIBPORT_DIR)/I2C_EEPROM/inc \
	-I$(AMSI_DRI_DIR)/$(MCUNAME) \
	-I$(AMSI_DRI_DIR)/$(MCUNAME)/config

ifeq ($(APP), TEST)
INCLUDES += \
	-I$(ROOT)/tests/include \
	-I$(AWS_TEST_DIR)/config_files
else
INCLUDES += \
	-I$(ROOT)/demos/include \
	-I$(ROOT)/demos/network_manager \
	-I$(AWS_DEMO_DIR)/config_files
endif

CFLAGS = \
	-Wall $(OPTIM) $(INCLUDES) $(DEFINES) \
	-fomit-frame-pointer -fno-strict-aliasing -fno-builtin -funroll-loops \
	-ffunction-sections -fdata-sections \
	-DCFG_MAKEFILE -DCFG_$(MODE) \
	$(CMODEL)

CXXFLAGS = $(CFLAGS)

ASFLAGS = -D__ASSEMBLY__ $(CFLAGS) -c

LDFLAGS = -T$(LDSCRIPT) $(OPTIM) -static -nostartfiles -Wl,-error-limit=0,--gc-sections,--defsym=__rtos_signature_freertos_v10_1_1=0x0 \
	$(CMODEL)

### Make variables

ifeq ($(USE_CACHE), 1)
	CFLAGS += -DCFG_CACHE_ENABLE
endif

# Compilation rules
.SUFFIXES : %.o %.c %.cpp %.S

%.o: %.c
	$(TRACE_CC)
	$(Q)$(CC) -c -MMD $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(TRACE_C++)
	$(Q)$(C++) -c -MMD $(CXXFLAGS) -o $@ $<

%.o: %.S
	$(TRACE_CC)
	$(Q)$(CC) -c -MMD $(ASFLAGS) -o $@ $<

all: $(PROG).elf

$(PROG).elf  : $(OBJS) $(ROOT)/vendors/andes/boards/corvette_f1_ae250/Makefile
	$(TRACE_LD)
	$(Q)$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	$(OBJCOPY) -O binary -S $(PROG).elf $(PROG).bin
	$(OBJDUMP) -d $(PROG).elf > $(PROG).asm
	@echo Completed

clean :
	@rm -f $(OBJS) $(OBJS:.o=.d)
	@rm -f $(PROG).elf
	@rm -f $(PROG).bin
	@rm -f $(PROG).asm

# Automatic dependency generation
ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif
