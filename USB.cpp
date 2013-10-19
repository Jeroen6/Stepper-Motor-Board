#include "mbed.h"
#include "USBHID.h"

// USB HID Objects
USBHID *hid;

HID_REPORT send_report;
HID_REPORT recv_report;
 
void USB_initialize(void){
	// HID mode
	// Create a USBHID device class. 
	static USBHID hid_object(8, 8);
	hid = &hid_object;
	
	send_report.length = 8;
}

void USB_routine(void){
	//try to read a msg
	if(hid->readNB(&recv_report)) {
		// Message read
		//Fill the report
		for (int i = 0; i < send_report.length; i++)
			send_report.data[i] = recv_report.data[i] * 2;	
		//Send the report
		hid->send(&send_report);
	}		
}
