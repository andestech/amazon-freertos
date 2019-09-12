target_burn_frontend --erase-all --verify -P 4444 --unlock --image=/home/kevin/Amazon_FreeRTOS/20190606_AWSFreeRTOS/tmp/amazon-freertos/vendors/andes/boards/corvette_f1_ae250/aws_demos/aws_demos.bin --algorithm-bin=/home/kevin/build-flash/linux/target_bin/target_SPI_v5_32.bin
riscv32-elf-gdb -x /home/kevin/Amazon_FreeRTOS/amazon-freertos/burnscript
echo FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF

