# HCI-PicoW-C
HCI PicoW layer in C

So far links in all of btstack just to get to the HCI layer of the CYW43 driver - work in progress there!!   

# Build problems

The driver files in pico-sdk/src/rp2_common/pico_cyw43_driver automatically pull in BTStack components, which causes link errors.
The missing functions are:
```
	btstack_cyw43_init		           src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background.c

	btstack_cyw43_deinit		         src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background.c

	cyw43_bluetooth_hci_process	     lib/cyw43-driver/src/cyw43_ctrl.c
```
```btstack_cyw43_init``` and  ```btstack_cyw43_deinit``` are only needed by BTStack, so can be omitted from the code (src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background).

```cyw43_bluetooth_hci_process``` is key to callbacks when data is received (and running without it seems to stop sleep_ms from working, so maybe messes up interrupts or the timer.   

# CMakeList.txt

```
cmake_minimum_required(VERSION 3.13)

include(./pico_sdk_import.cmake)

project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

pico_sdk_init()

add_executable(hci_pico hci_pico.c )
add_compile_definitions(CYW43_ENABLE_BLUETOOTH=1)

target_link_libraries(hci_pico pico_stdlib pico_cyw43_arch_none)
target_include_directories(hci_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR}) # bt stack config

pico_enable_stdio_usb(hci_pico 1)
pico_enable_stdio_uart(hci_pico 0)
pico_add_extra_outputs(hci_pico)
```

## Build

```
# Set SDK path to your path
export $PICO_SDK_PATH=~/pico_new/pico-sdk

cp $PICO_SDK_PATH/external/pico_sdk_import.cmake    .
mkdir build
cd build
cmake -DFAMILY=pico_w ..
make clean
make

```
