cmake_minimum_required(VERSION 3.13)

include(./pico_sdk_import.cmake)

project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

pico_sdk_init()
add_executable(hci_pico hci_pico.c )

target_link_libraries(hci_pico pico_stdlib pico_cyw43_arch_none pico_btstack_ble pico_btstack_cyw43)
target_include_directories(hci_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR}) # bt stack config
pico_enable_stdio_usb(hci_pico 1)
pico_enable_stdio_uart(hci_pico 0)

pico_add_extra_outputs(hci_pico)
