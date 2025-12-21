#include <cstdint>
#include <cstring>
#include <cstdio>

/*
 * STUB: USB CDC Middleware
 *
 * In a real project, this file would be replaced by the output of STM32CubeMX
 * (usbd_cdc_if.c, usbd_conf.c, usb_device.c) and the STM32 USB Device Library.
 *
 * This stub allows the project to link and provides the hook 'USB_CDC_RxHandler'
 * that we defined in main.cpp, so the logic is testable.
 */

// Function signature from main.cpp
extern "C" void USB_CDC_RxHandler(uint8_t* Buf, uint32_t Len);

// Mock receive function to simulate incoming data (e.g., from a test harness or debugger)
void Mock_USB_Receive(const char* cmd) {
    size_t len = strlen(cmd);
    USB_CDC_RxHandler((uint8_t*)cmd, len);
}

// Stub for CDC Transmit (Output to Host)
extern "C" uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len) {
    // In real hardware, this sends data to the PC.
    // Here, we could printf it for debugging.
    fwrite(Buf, 1, Len, stdout);
    return 0; // USBD_OK
}

/*
 * NOTE TO USER:
 * To enable real USB functionality:
 * 1. Generate USB Device (CDC) code using STM32CubeMX for STM32H743VIT6.
 * 2. Copy the 'Middlewares/ST/STM32_USB_Device_Library' folder to 'lib/'.
 * 3. Copy 'USB_DEVICE' folder (App/Target) to 'src/'.
 * 4. In 'usbd_cdc_if.c', inside 'CDC_Receive_FS', call 'USB_CDC_RxHandler(Buf, *Len);'.
 */
