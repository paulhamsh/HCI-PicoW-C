# HCI-PicoW-C
HCI PicoW layer in C

So far links in all of btstack just to get to the HCI layer of the CYW43 driver - work in progress there!!   

# Build problems

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
#add_compile_definitions(CYW43_ENABLE_BLUETOOTH_BT_INIT=1)  # because I changed some code in a few places
add_compile_definitions(CYW43_ENABLE_BLUETOOTH_HANDLER=1)  # because I changed some code in a few places

#target_link_libraries(hci_pico pico_stdlib pico_cyw43_arch_none pico_btstack_ble pico_btstack_cyw43)

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
