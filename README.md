# HCI-PicoW-C
HCI PicoW layer in C

So far links in all of btstack just to get to the HCI layer of the CYW43 driver - work in progress there!!   

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
