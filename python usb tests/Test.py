#
#Simple example on how to send and receive data to the Mbed over USB (on windows) using pywinusb
#
import pywinusb.hid as hid
from time import sleep
import random
	
# handler called when a report is received
def rx_handler(data):
    print 'recv: ', data

def findHIDDevice(mbed_usage, mbed_vendor_id):
    # Find all devices connected
    all_devices = hid.HidDeviceFilter(vendor_id = mbed_vendor_id).get_devices()
    
    if not all_devices:
        print "No device connected"
    else:
        # search for the Mbed
        for HIDdevice in all_devices:
            try:
                HIDdevice.open()
                # browse output reports
                for report in HIDdevice.find_output_reports():
                    if mbed_usage in report:

                        #MBED found
                        print 'Mbed detected'

                        #Attach a custom handler when a data is received
                        HIDdevice.set_raw_data_handler(rx_handler)
                        
                        #send a report each 0.2 second. The report is a random array of 8 bytes
                        while True:
                            x = [0,0,0,0,0,0,0,0,0]
                            for i in range(8):
                                x[i+1] = random.randint(0, 126)
                                report[mbed_usage][i] = x[i+1]
                            print 'sent: ',  x
                            report.send()
                            sleep(1)
            except:
                print 'close'
                HIDdevice.close()
		

if __name__ == '__main__':
    # The vendor ID used in the Mbed program
    mbed_vendor_id = 0x1234 

    # Vendor page and usage_id = 2
    mbed_usage = hid.get_full_usage_id(0xffab, 0x02)

    # Search the Mbed, attach rx handler and send data
    findHIDDevice(mbed_usage, mbed_vendor_id)
