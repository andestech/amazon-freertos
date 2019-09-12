set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
set(bsp_dir "${CMAKE_CURRENT_LIST_DIR}/../../V5/bsp")
set(v5_source "${CMAKE_CURRENT_LIST_DIR}/../../V5")
set(amsi_dir "${CMAKE_CURRENT_LIST_DIR}/../../V5/AMSI")
set(wifi_dir "${CMAKE_CURRENT_LIST_DIR}/../../V5/wifi")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "Andes_CF1")
afr_set_board_metadata(DISPLAY_NAME "CF1")
afr_set_board_metadata(DESCRIPTION "Development kit for corvette_f1_ae250")
afr_set_board_metadata(VENDOR_NAME "Andes")
afr_set_board_metadata(FAMILY_NAME "AE250")
afr_set_board_metadata(DATA_RAM_MEMORY "200KB")
afr_set_board_metadata(PROGRAM_MEMORY "1MB")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(IS_ACTIVE "TRUE")

afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)
if(AFR_IS_TESTING)
    target_compile_definitions(
        AFR::compiler::mcu_port
        INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
        -DportasmHANDLE_INTERRUPT=FreeRTOS_IRQ_handler
        -D__freertos_irq_stack_top=_stack
        -DUNITY_INCLUDE_CONFIG_H
        -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS
        -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"
        -DCFG_MAKEFILE
        -DCFG_XIP
        -DCFG_CACHE_ENABLE
        -DCFG_FLASHEXEC
    )
else()
    target_compile_definitions(
        AFR::compiler::mcu_port
        INTERFACE $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
        -DportasmHANDLE_INTERRUPT=FreeRTOS_IRQ_handler
        -D__freertos_irq_stack_top=_stack
        -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"
        -DCFG_MAKEFILE
        -DCFG_XIP
        -DCFG_CACHE_ENABLE
        -DCFG_FLASHEXEC
    )
endif()

target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE $<$<COMPILE_LANGUAGE:ASM>:${assembler_defined_symbols}>
    -D__ASSEMBLY__
)

set (compiler_flags -O3 -g3 -mcmodel=large -fomit-frame-pointer -fno-strict-aliasing -fno-builtin -funroll-loops -ffunction-sections -fdata-sections -std=gnu99)
set (assembler_flags -O3 -g3 -mcmodel=large -fomit-frame-pointer -fno-strict-aliasing -fno-builtin -funroll-loops -ffunction-sections -fdata-sections)

target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)


# Linker flags
set(
   linker_flags -T${board_dir}/application_code/nds_code/ae250.ld -O3 -g3 -static -nostartfiles -Wl,-error-limit=0,--gc-sections,--defsym=__rtos_signature_freertos_v10_1_1=0x0 -mcmodel=large
)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE 
        $<$<COMPILE_LANGUAGE:C>:${linker_flags}>
)

target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE ${link_dependent_libs}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Normally the portable layer for kernel should be vendor's driver code.
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE

        "${AFR_KERNEL_DIR}/portable/GCC/RISC-V/port.c"
	"${AFR_KERNEL_DIR}/portable/GCC/RISC-V/portASM.S"
	"${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"

	"${bsp_dir}/FreeRTOS_asm_interrupt.S"
	"${bsp_dir}/FreeRTOS_tick_config.c"
        "${bsp_dir}/reset.c"
        "${bsp_dir}/initfini.c"
        "${bsp_dir}/trap.c"
        "${bsp_dir}/cache.c"
	"${bsp_dir}/start.S"
        "${bsp_dir}/ae250/ae250.c"
        "${bsp_dir}/ae250/interrupt.c"
        "${bsp_dir}/ae250/iochar.c"
        "${bsp_dir}/libc/stdio/do_printf.c"
        "${bsp_dir}/libc/stdio/printf.c"
        "${bsp_dir}/driver/uart.c"
        "${afr_ports_dir}/I2C_EEPROM/src/i2c_eeprom.c"
        "${amsi_dir}/src/ae250/dma_ae250.c"
        "${amsi_dir}/src/ae250/gpio_ae250.c"
        "${amsi_dir}/src/ae250/i2c_ae250.c"
        "${amsi_dir}/src/ae250/spi_ae250.c"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${bsp_dir}"
        "${bsp_dir}/ae250"
        "${AFR_KERNEL_DIR}/portable/GCC/RISC-V"
        "${AFR_KERNEL_DIR}/portable/GCC/RISC-V/chip_specific_extensions/Andes_V5_RV32RV64"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/test"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/tls/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/utils/include"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/aws/ota/test"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/wifi/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/platform/freertos/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/platform/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/src"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/mbedtls"
        "${bsp_dir}/driver/include"
        "${afr_ports_dir}/I2C_EEPROM/inc"
        "${amsi_dir}/src/ae250"
        "${amsi_dir}/src/ae250/config"
        "${amsi_dir}/include"

        "${board_dir}/config_files"
        "${board_dir}/application_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"

        "${AFR_3RDPARTY_DIR}/pkcs11"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/utils/include"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
        "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/Include"
)


# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE
        "${afr_ports_dir}/wifi/aws_wifi.c"
        "${afr_ports_dir}/wifi/aws_wifi_assert.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/wdrvext_wilc1000.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/wilc1000_task.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/common/source/nm_common.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/spi_flash/source/spi_flash.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmdrv.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmbus.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/m2m_periph.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/m2m_wifi.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/m2m_hif.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmasic.c"
        "${wifi_dir}/wilc1000/dev/spi/wdrv_wilc1000_spi.c"
        "${wifi_dir}/wilc1000/dev/gpio/wdrv_wilc1000_gpio.c"
        "${wifi_dir}/wilc1000/dev/gpio/wdrv_wilc1000_eint.c"
        "${wifi_dir}/wilc1000/osal/wdrv_wilc1000_osal.c"
        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_main.c"
        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_connmgr.c"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source/nmspi.c"
        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_scan_helper.c"
        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_events.c"
        "${wifi_dir}/wilc1000/wireless_driver/wdrv_wilc1000_config.c"
        "${wifi_dir}/wilc1000/include/tmr/sys_tmr.c"
        "${wifi_dir}/framework/osal/src/osal_freertos.c"
        "${wifi_dir}/framework/tcpip/src/tcpip_helpers.c"
)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/source"
	"${wifi_dir}/wilc1000/wireless_driver_extension"
        "${wifi_dir}/wilc1000/include"
        "${wifi_dir}/wilc1000/include/tmr"
        "${wifi_dir}/wilc1000/wireless_driver/include"
        "${wifi_dir}/framework"
	"${v5_source}"
        "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/include"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"

)

# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/andes/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/andes/NetworkInterface.c"
)

target_include_directories(                                                                                                                       
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${wifi_dir}/wilc1000/wireless_driver_extension"
        "${wifi_dir}/wilc1000/wireless_driver_extension/driver/include"
	"${wifi_dir}/framework"
	"${wifi_dir}/wilc1000/include"
	"${wifi_dir}/wilc1000/include/tmr"
	"${v5_source}"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE
        "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
)
# Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE AFR::pkcs11_mbedtls
)

# Secure sockets
afr_mcu_port(secure_sockets)
# Link to AFR::secure_sockets_freertos_tcp if you want use default implementation based on
# FreeRTOS-Plus-TCP.
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)
# Or provide your own implementation.
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")
add_executable( 
    ${exe_target}
    ${config_files}
    "${board_dir}/application_code/nds_code/ae250.ld"
    "${board_dir}/application_code/main.c"
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
	${link_extra_flags}
)

if(AFR_METADATA_MODE)
    return()
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".elf")


set_source_files_properties(${AFR_MODULES_C_SDK_DIR}/common/logging/aws_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_pal.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign -Wno-sizeof-pointer-memaccess"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_agent.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign"
)

set_source_files_properties(${AFR_DEMOS_DIR}/posix/aws_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")


find_program(riscv32-elf-objdump riscv32-elf-objdump)
find_program(riscv32-elf-objcopy riscv32-elf-objcopy)
find_program(riscv32-elf-gcc riscv32-elf-gcc)

set(output_bin_file "${board_dir}/${exe_target}.bin")
set(output_elf_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.elf")

add_custom_command(
    TARGET ${exe_target} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}" 
    COMMAND "echo" "Creating .bin file"
    COMMAND "${riscv32-elf-objcopy}" -O binary -S ${output_elf_file} ${output_bin_file}
)
