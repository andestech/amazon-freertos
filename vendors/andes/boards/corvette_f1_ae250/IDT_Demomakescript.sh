cmake -DVENDOR=andes -DBOARD=corvette_f1_ae250 -DCOMPILER=riscv32-elf -S /home/kevin/Amazon_FreeRTOS/20190606_AWSFreeRTOS/amazon-freertos/  -B /home/kevin/Amazon_FreeRTOS/20190606_AWSFreeRTOS/amazon-freertos/build/
echo Cmake Done
cd /home/kevin/Amazon_FreeRTOS/20190606_AWSFreeRTOS/amazon-freertos/build
make
