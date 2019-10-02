cmake -DVENDOR=andes -DBOARD=corvette_f1_n25 -DAFR_IS_TESTING=1 -DCOMPILER=riscv32-elf -S /home/kevin/amazon-freertos/  -B /home/kevin/amazon-freertos/build/
cd /home/kevin/amazon-freertos/build
make
