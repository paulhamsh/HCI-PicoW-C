#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bsp/board.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"


//#include "/root/pico_base/pico-sdk/lib/cyw43-driver/src/cyw43.h"


#define BUF_MAX 500
uint8_t buf[BUF_MAX];
uint32_t buf_len;


#define PRE_BUFFER_LEN 3

uint32_t hci_receive_raw(uint8_t *data, uint32_t max_len) {
    int ret;
    uint32_t len;

    ret = cyw43_bluetooth_hci_read(buf, max_len, &len);
    if (ret) len = 0;

    // Remove the padding
    if (len > PRE_BUFFER_LEN) len -= PRE_BUFFER_LEN;
    memcpy(data, &buf[PRE_BUFFER_LEN], len);
    return len;
}



void hci_send_raw(uint8_t *data, uint32_t data_len) {
    memcpy(&buf[PRE_BUFFER_LEN], data, data_len);
    cyw43_bluetooth_hci_write(buf, data_len + PRE_BUFFER_LEN);
    printf("Sent %lu: ", (unsigned long) data_len);
    for (int i = 0; i < data_len; i++) printf("%02x ", data[i]);
    printf("\n");
}


uint8_t dat1[] = {0x01, 0x03, 0x0C, 0x00};
uint8_t dat2[] = {0x01, 0x01, 0x10, 0x00};

uint8_t indat[BUF_MAX];

int main() {
    uint32_t len;

    stdio_init_all();
    if (cyw43_arch_init()) return -1;
    sleep_ms(5000);
    printf("Started\n");


    cyw43_init(&cyw43_state);
    buf_len = 0;

    sleep_ms(1000);
    hci_send_raw(dat1, 4);
    sleep_ms(1000);
    len = hci_receive_raw(indat, BUF_MAX);
    printf("Received %lu: ", (unsigned long) len);
    for (int i = 0; i < len; i++) printf("%02x ", indat[i]);
    printf("\n");

    sleep_ms(1000);
    hci_send_raw(dat2, 4);
    sleep_ms(1000);
    len = hci_receive_raw(indat, BUF_MAX);
    printf("Received %lu: ", (unsigned long) len);
    for (int i = 0; i < len; i++) printf("%02x ", indat[i]);
    printf("\n");

    while (1) {
        sleep_ms(2000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(2000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

        len = hci_receive_raw(indat, BUF_MAX);
        printf("Received %lu: ", (unsigned long) len);
        for (int i = 0; i < len; i++) printf("%2x ", indat[i]);
        printf("\n");

    };

    printf("Finished\n");
    return 0;
}
