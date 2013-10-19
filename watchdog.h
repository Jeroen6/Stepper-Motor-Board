/*
*           Using code Simon Ford written in his example
*           to implement WDT. 
*           It can run in seconds, milliseconds, and microseconds. 
*           All there is to do, is keep feeding the watchdog. 
*/

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "mbed.h"

/** Watchdog timer implementation (in seconds) */
class WatchDog {
public: 
    /** Creates Watchdog timer
    * 
    * @param Sets the WDT in seconds
    */
    WatchDog(float s);
    
    /** Keep feeding the watchdog in routine
    *
    * xxx.feed(); does the trick
    */
    void feed();
};
/** Watchdog timer implementation (in milliseconds) */
class WatchDog_ms {
public: 
    /** Creates Watchdog timer
    * 
    * @param Sets the WDT in milliseconds
    */
    WatchDog_ms(int ms);
    /** Keep feeding the watchdog in routine
    *
    * xxx.feed(); does the trick
    */
    void feed();
};
/** Watchdog timer implementation (in microseconds) */
class WatchDog_us {
public:
    /** Creates Watchdog timer
    * 
    * @param Sets the WDT in microseconds
    */
    WatchDog_us(int us);
    /** Keep feeding the watchdog in routine
    *
    * xxx.feed(); does the trick
    */    
    void feed();
};

#endif
