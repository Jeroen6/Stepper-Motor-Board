#
#Simple example on how to send and receive data to the Mbed over USB (on windows) using pywinusb
#
import pywinusb.hid as hid
from time import sleep
import random
from array import array
import struct

gotdata = False

# handler called when a report is received
def rx_handler(data):
    print 'recv: ', data
    gotdata = True

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

                        # set ledblink
                        x = [0x12, 0,0,0,0,0,0,0,0,0]
                        for i in range(1):   
                            report[mbed_usage][i] = x[i]
                        print 'sent: ',  x
                        report.send()
                        sleep(0.250)

                        # set pwm
                        duty        = 50
                        period      = 100
                        prescaler   = 48
                        
                        x = [0x32,
                             int(duty & 0xff),
                             int((duty >> 8) & 0xff),
                             int(period & 0xff),
                             int((period >> 8) & 0xff),
                             int(prescaler & 0xff),
                             int((prescaler >> 8) & 0xff)]
                        #x = [0x22, 0,0,0,0,0,0,0,0,0]
                        for i in range(10):   
                            report[mbed_usage][i] = x[i]
                        print 'sent: ',  x
                        report.send()
                        sleep(0.250)
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
