cmake_minimum_required(VERSION 3.13)

include(./pico_sdk_import.cmake)

project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)
set(FAMILY pico_w)

pico_sdk_init()

add_executable(hci_pico hci_pico.c )

add_compile_definitions(CYW43_ENABLE_BLUETOOTH=1)
add_compile_definitions(CYW43_DISABLE_BT_INIT=1)   # when set will disable btstack init
#add_compile_definitions(CYW43_CUSTOM_HANDLER=1)    # when set will use my custom handler

#target_link_libraries(hci_pico pico_stdlib pico_cyw43_arch_none pico_btstack_ble pico_btstack_cyw43)

target_link_libraries(hci_pico pico_stdlib pico_cyw43_arch_none
                      #pico_btstack_hci_transport_cyw43
                      #pico_btstack_ble
                      #pico_btstack_cyw43
                      #pico_cyw43_driver
                      #cyw43_driver_picow
                      #pico_cyw43_arch
                      #cyw43_driver
                      )

target_include_directories(hci_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR}) # bt stack config
pico_enable_stdio_usb(hci_pico 1)
pico_enable_stdio_uart(hci_pico 0)

pico_add_extra_outputs(hci_pico)
