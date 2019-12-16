Getting Started with the Andes Corvette-F1 N25
==============================================

This tutorial provides instructions for getting started with the Andes
Corvette-F1 N25 Development Board. If you do not have the Andes Corvette-F1 N25
board, visit the AWS Partner Device Catalog to purchase one from our partner.

Before you begin, you must configure AWS IoT and your downloaded Amazon FreeRTOS
to connect your device to the AWS Cloud. See [First
Steps](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-prereqs.html)
for instructions. In this tutorial, the path to the downloaded Amazon FreeRTOS
package is referred to as <span style="color:red"> `<amazon-freertos>`</span>.

Overview
--------

This tutorial contains instructions for the following steps:

1.  Installing software on the host machine for developing and debugging
    embedded applications on your Andes Corvette-F1 N25 board.

2.  Cross compiling an Amazon FreeRTOS demo application to a binary image.

3.  Flashing the application binary image to your board, and then running the
    application.

4.  Monitoring and debugging the application that runs on your board thorugh a
    serial connection.

Set Up the Andes Corvette-F1 N25 Hardware
-----------------------------------------

Connect the CON2 (UART) port on the Corvette-F1 N25 Development Board to a USB
port on your computer using a micro USB cable.

## Set Up Your Development Environment

All tools necessary for developing Amazon FreeRTOS demo applications on the
Corvette-F1 N25 Development Board are available at the [GitHub
repository](https://github.com/andestech/aws_development_tools.git).


Download the repository and unzip it. The generated folder is refered to as <span style="color:red">`<aws-development-tools>`</span>. 

### Set Up Cygwin (for Windows only)

On Windows, you are required to use Cygwin to compile, burn, or execute Amazon
FreeRTOS applications on the Corvette-F1 N25 Development Board. Obtain the compressed Cygwin package (`Cygwin.tar.gz`) from <span style="color:red">`<aws-development-tools>`</span>`\windows\Cygwin` first and extract
it.

You will find the Cygwin batch file (`Cygwin.bat`) in the generated directory. Execute the batch file to launch the Cygwin terminal which allows you to perform any operation for developing Amazon FreeRTOS applications on Windows.

### Set Up the Toolchain
If you are using a 64-bit Linux, make sure you install additional necessary programs first by executing the shell
script `install_linux_package.sh` in the <span style="color:red">`<aws-development-tools>`</span>`/linux/toolchain/` directory. 

The Amazon FreeRTOS port for the Corvette-F1 N25 Development Board is configured
to use nds32le-elf-newlib-v5, a custom GNU toolchain for RISC-V, by default. The
toolchain is also in the <span style="color:red">`<aws-development-tools>`</span> repository you downloaded
earlier from GitHub. Find the compressed toolchain `nds32le-elf-newlib-v5.txz` from <span style="color:red">`<aws-development-tools>`</span>`/[linux|windows]/toolchain/` and unzip it. In the case of Windows environment, use command-line options to unzip the toolchain on Cygwin.

Include the path <span style="color:red">`<nds32le-elf-newlib-v5-toolchain-folder>`</span>`/bin/` to the nds32le-elf-newlib-v5 toolchain binary in your PATH variable. This is to ensure that the toolchain can be invoked successfully for building Amazon FreeRTOS applications later.

With Andes ICE management software “ICEman” that interfaces with the JTAG connector on the Corvette-F1 N25 board, the nds32le-elf-newlib-v5 toolchain can be used with the GNU Debugger (GDB) for debugging.

### Set Up ICEman

For debugging Amazon FreeRTOS applications or burning them to a flash, you will
need to execute the Andes ICE management software, ICEman, on a host computer
that's connected with the Corvette-F1 N25 Development Board.

The ICEman program can be executed directly in a Linux system. If you are using
the Windows operating system, make sure to install the ICEman driver `zadig_20171002_win7.exe` from <span style="color:red">`<aws-development-tools>`</span>`/windows/ICEman driver/zadig_20171002_win7/` first. 
    

For details about the installation procedure, refer to the Driver installation
guide `Driver_install_guide.pptx` in the directory  <span style="color:red">`<aws-development-tools>`</span>`/windows/ICEman driver/`.
    
To execute the ICEman program, change the current working directory to <span style="color:red">`<aws-development-tools>`</span>`/[linux|windows]/ICEman/` and issue the following commands:

    $ sudo ./ICEman -Z v5

You will see the following message when ICEman is launched:<br>
<img src="media\ICEman-output.png" width="500" height="150">

>**NOTE**<br>
>Be sure to keep ICEman running when you are debugging or burning a flash.


### Install CMake

The CMake build system is required to build the Amazon FreeRTOS demo and test
applications for the Corvette-F1 N25 Development Board. Amazon FreeRTOS supports
CMake versions 3.13 and later.

If you are using a Linux system, download the latest version of CMake, including
its source and binaries, from [CMake.org](https://cmake.org/).

If you are using the Windows operating system, CMake is ready-to-use in the
Cygwin you downloaded.

For more details about using CMake with Amazon FreeRTOS, see [Using CMake with
Amazon FreeRTOS](https://docs.aws.amazon.com/freertos/latest/userguide/getting-started-cmake.html). .


## Establish a Serial Connection ##

1.  Attach one end of a USB cable to your host computer, and the other end to
    your Corvette-F1 N25 board. Your host computer should be able to detect the
    board right away.

	On Linux, issue the `dmesg` command in a command line terminal and verify a successful connection from a message like below:<br>
	<img src="media\Linux-connection.png" width="535" height="135">
          
	>**NOTE** <br>
	>The Corvette-F1 N25 board exposes two USB interfaces to the host computer. One (ttyUSB0) is to the MCU’s JTAG functionality and the other (ttyUSB1) is to the MCU’s physical UARTx port.


	On Windows, open “Device Manager” and verify a successful connection from
	the listing of a new device designated with a COM port number under the node
	“Ports (COM and LPT)”. The figure below shows that a new entry “USB Serial
	Port (COM3)” is listed in the Device Manager after the USB cable is plugged,
	indicating a device has been attached to COM3 of the host computer.<br>
	<img src="media\Windows-connection.png" width="300" height="400">

1.  Use the following settings to start a serial connection to either USB interface (i.e., ttyUSBx or COMx for Windows): 

	+ BAUD rate: 115200
	+ Data: 8 bit 
	+ Parity: None
	+ Stop: 1 bit
	+ Flow control: None

	For example, proceed with the following steps if you are using PuTTY:

	1.  On Linux, issue the following command to launch the PuTTY program:

		`$ sudo ./putty ` 

		On Windows, double-click the desktop shortcut <img src="media\putty.png" width="25" height="25"> to start PuTTY.

	1.  Set the **Connection type** to “Serial”.

	2.  Set the **Serial line** to “/dev/ttyUSBx” for Linux or “COMx” for Windows.

	3.  Set the **Speed** to "115200".

	4.  Click on the **Open** button.

	For more information about installing a terminal emulator to set up a serial
	connection, refer to [Installing a Terminal Emulator](https://docs.aws.amazon.com/freertos/latest/userguide/uart-term.html) .

## Build, Flash, and Run a Amazon FreeRTOS Demo Project ##

### Set Wi-Fi and device information

Edit the file `aws_clientcredential.h` in the directory <span style="color:red">`<amazon-freertos>`</span>`/demos/include/` to specify your Wi-Fi SSID and password for your Amazon FreeRTOS demo project. This is to enable the project to connect to the internet and use MQTT to connect to the AWS cloud.

Next, edit the file `aws_clientcredential_keys.h` in the directory <span style="color:red">`<amazon-freertos>`</span>`/demos/include/` to provide your demo project with the device
information, which includes the certificate, MQTT endpoint and device name.

In the case of building the Amazon FreeRTOS test project, edit the two
files `aws_clientcredential.h` and `aws_clientcredential_keys.h` in the directory <span style="color:red">`<amazon-freertos>`</span>`/tests/include/`to configure the Wi-Fi settings and device information.

### Generate Build Files for Your Demo Project and Build the Demo

From the root directory of your downloaded Amazon FreeRTOS package, issue 
CMake commands as follows to generate build files for your demo project:

    $ cmake -DVENDOR=andes -DBOARD=corvette_f1_n25 -DAFR_IS_TESTING=0 -DCOMPILER=riscv32-elf -B build

The output message will be like the following:<br>
<img src="media\gen-build-files.png" width="600" height="350">

Issue the following commands to build the binary image of your demo application:

    $ cd build
    $ make
   

The binary `aws_demos.bin` will be generated in the directory <span style="color:red">`<amazon-freertos>`</span>`/vendors/andes/boards/corvette_f1_n25/aws_demos/`.

The output messages for the build will look like the following:
<img src="media\gen-demo-image.png" width="650" height="240">

To build the Amazon FreeRTOS test project, issue the following commands instead:

    $ cmake -DVENDOR=andes -DBOARD=corvette_f1_n25 -DAFR_IS_TESTING=1 -DCOMPILER=riscv32-elf -B build
    $ cd build
    $ make

The binary image of the test project `aws_tests.bin` will be generated in the directory <span style="color:red">`<amazon-freertos>`</span>`/vendors/andes/boards/corvette_f1_n25/aws_tests/`.


>**NOTE** <br>
>Be sure to use CMake commands to generate build files whenever you switch between the `aws_demos` project and the `aws_tests` project.

### Burn the Application to Flash and Run

To program an Amazon FreeRTOS demo application to the flash memory on the Corvette-F1 N25 board, find the script files (i.e., `target_burn_linux.sh` for Linux and `target_burn_windows.sh` for Windows) in the directory <span style="color:red">`<amazon-freertos>`</span>`/vendors/andes/tools`.


On Linux, issue the following command to perform the flash programming:
    
    bash target_burn_linux.sh <path-to-program-binary>

On Windows, issue the following command in the Cygwin terminal:

    bash target_burn_windows.sh <path-to-program-binary>


>**NOTE** <br>
> Make sure the ICEman program is launched before you proceed with flash programming.

You will see the following message in the serial terminal during the programming
process.<br>
<img src="media\while-programming.png" width="600" height="100">

The demo application will be executed automatically right after it is burned to
the board. Output messages like below show in your serial terminal:<br>
<img src="media\demo-runs.png" width="900" height="450">


### Debug the Application

1.  Execute the ICEman program.

2.  Open a console and issue the command `riscv32-elf-gdb`.

3.  In the GDB command line console, issue the following commands to load the program executable (`.elf` file).

    	file <amazon-freertos>/build/[aws_demos.elf|aws_tests.elf]


4.  Issue the command as follows to build the connection between your host computer and your board:

    	Target remote <IP_ADDRESS>:1111

5.  Use GDB commands to debug the FreeRTOS demo or test project.

### Monitoring MQTT Messages on the Cloud

You can use the MQTT client in the AWS IoT console to monitor the messages that
your device sends to the AWS Cloud.

**To subscribe to the MQTT topic with the AWS IoT MQTT client**

1.  Sign in to the **AWS IoT console**.

2.  In the navigation pane, choose **Test** to open the MQTT client.

3.  In **Subscription** topic, enter *iotdemo/topic/\#*, and then choose
    **Subscribe to topic**.

4.  Your **AWS IoT console** will display the topics that you subscribed, as shown below. 

	<img src="media\AWS-console.png" width="500" height="250">

## Troubleshooting ##

For troubleshooting information about Getting Started with Amazon FreeRTOS, see
[Troubleshooting Getting Started](https://docs.aws.amazon.com/freertos/latest/userguide/gsg-troubleshooting.html).
