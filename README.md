# HCI-PicoW-C
HCI PicoW layer in C

So far links in all of btstack just to get to the HCI layer of the CYW43 driver - work in progress there!!   

## Build problems

The driver files in pico-sdk/src/rp2_common/pico_cyw43_driver automatically pull in BTStack components, which causes link errors.
The missing functions are:
```
	btstack_cyw43_init                   src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background.c

	btstack_cyw43_deinit                 src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background.c

	cyw43_bluetooth_hci_process          lib/cyw43-driver/src/cyw43_ctrl.c
```
```btstack_cyw43_init``` and  ```btstack_cyw43_deinit``` are only needed by BTStack, so can be omitted from the code (src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background).

So, edit pico-sdk/src/rp2_common/pico_cyw43_arch/cyw43_arch_threadsafe_background.c to add the #if as below on lines 49 and 62.   

```
#ifndef CYW43_DISABLE_BT_INIT
    ok &= btstack_cyw43_init(context);
#endif

#ifndef CYW43_DISABLE_BT_INIT
    btstack_cyw43_deinit(context);
#endif
```

```cyw43_bluetooth_hci_process``` is key to callbacks when data is received (and running without it seems to stop sleep_ms from working, so maybe messes up interrupts or the timer.   





## CMakeList.txt

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
add_compile_definitions(CYW43_DISABLE_BT_INIT=1)  # when set will disable btstack init
add_compile_definitions(CYW43_CUSTOM_HANDLER=1)   # when set will use my custom handler

target_link_libraries(hci_pico pico_stdlib
                      pico_cyw43_arch_none
                      #pico_btstack_ble
                      #pico_btstack_cyw43
                      )
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

## Tracing cyw43_bluetooth_hci_process

pico-sdk/src/rp2_common/pico_cyw43_driver/btstack_hci_transport_cyw43.c
```
// This is called from cyw43_poll_func.
void cyw43_bluetooth_hci_process(void) {
    if (hci_transport_ready) {
        btstack_run_loop_poll_data_sources_from_irq();
    }
}
```

lib/btstack/src/btstack_run_loop.c
```
void btstack_run_loop_poll_data_sources_from_irq(void){
    btstack_assert(the_run_loop != NULL);
    btstack_assert(the_run_loop->poll_data_sources_from_irq != NULL);
    the_run_loop->poll_data_sources_from_irq();
}
```

pico-sdk/src/rp2_common/pico_cyw43_driver/btstack_hci_transport_cyw43.c
```
static void hci_transport_data_source_process(btstack_data_source_t *ds, btstack_data_source_callback_type_t callback_type) {
    assert(callback_type == DATA_SOURCE_CALLBACK_POLL);
    assert(ds == &transport_data_source);
    (void)callback_type;
    (void)ds;
    hci_transport_cyw43_process();
}
```

```
static void hci_transport_cyw43_process(void) {
    CYW43_THREAD_LOCK_CHECK
    uint32_t len = 0;
    bool has_work;
    do {
        int err = cyw43_bluetooth_hci_read(hci_packet_with_pre_buffer, sizeof(hci_packet_with_pre_buffer), &len);
        BT_DEBUG("bt in len=%lu err=%d\n", len, err);
        if (err == 0 && len > 0) {
            hci_transport_cyw43_packet_handler(hci_packet_with_pre_buffer[3], hci_packet_with_pre_buffer + 4, len - 4);
            has_work = true;
        } else {
            has_work = false;
        }
    } while (has_work);
}
```

The trace is a little harder at this point because everything is directed via packet handlers in BTStack, but luckily we don't need to know what it does - at this point we have enough. We simply need the ```hci_transport_cyw43_process``` code in cyw43_ctrl.c   

Replace this at line 231:

```
    #if CYW43_ENABLE_BLUETOOTH
    if (self->bt_loaded && cyw43_ll_bt_has_work(&self->cyw43_ll)) {
        cyw43_bluetooth_hci_process();
    }
    #endif
```

With something like this (which really needs to do a callback into your program to deliver the HCI packet received).   

```
    #ifndef CYW43_ENABLE_BLUETOOTH_HANDLER
    if (self->bt_loaded && cyw43_ll_bt_has_work(&self->cyw43_ll)) {
        cyw43_bluetooth_hci_process();
    }
    #elif CYW43_ENABLE_BLUETOOTH_HANDLER
    if (self->bt_loaded && cyw43_ll_bt_has_work(&self->cyw43_ll)) {
        #define PBSIZE 500
        uint8_t hci_packet_with_pre_buffer[PBSIZE];
        CYW43_THREAD_LOCK_CHECK
        uint32_t len = 0;
        bool has_work;
        do {
            int err = cyw43_bluetooth_hci_read(hci_packet_with_pre_buffer, sizeof(hci_packet_with_pre_buffer), &len);
            if (err == 0 && len > 0) {
                printf("GOT A PACKET %lu\n", (unsigned long) len);
                for (int i = 0; i < len; i++) printf("%02x ", hci_packet_with_pre_buffer[i]);
                printf("\n");


                //hci_transport_cyw43_packet_handler(hci_packet_with_pre_buffer[3], hci_packet_with_pre_buffer + 4, len - 4);
                has_work = true;
            } else {
                has_work = false;
            }
        } while (has_work);
    }
    #endif
```

