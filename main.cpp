#include "mbed.h"
#include "USBHID.h"
 
//We declare a USBHID device. By default input and output reports are 64 bytes long.
//USBHID hid(8, 8);
//USBHID (uint8_t output_report_length=64, uint8_t input_report_length=64, uint16_t vendor_id=0x1234, uint16_t product_id=0x0006, uint16_t product_release=0x0001, bool connect=true)
USBHID hid(8, 8, 0x1234, 0x0006, 0xFFFF, true);
 
Serial pc(USBTX, USBRX);
 
//This report will contain data to be sent
HID_REPORT send_report;
HID_REPORT recv_report;
 
DigitalOut l1(P0_21);
 
int main(void) {
    send_report.length = 8;
 
    while (1) {
        
        //Fill the report
        for (int i = 0; i < send_report.length; i++)
            send_report.data[i] = rand() & 0xff;
 
        //Send the report
        hid.send(&send_report);
 
        //try to read a msg
        if(hid.read(&recv_report)) {
            l1 = !l1;
        }
    }
}
