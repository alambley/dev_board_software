#ifndef _DEV_BOARD_00_00_04_PLIB    /* Guard against multiple inclusion */
#define _DEV_BOARD_00_00_04_PLIB

#include <plib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#define LED1Port    IOPORT_B
#define LED1Pin     BIT_15
    
#define LED2Port    IOPORT_B
#define LED2Pin     BIT_14

#define LED3Port    IOPORT_B
#define LED3Pin     BIT_13

#define LED4Port    IOPORT_B
#define LED4Pin     BIT_12

#define TestPort    IOPORT_D
#define TestPin     BIT_10

#define LED1Init()      PORTSetPinsDigitalOut(LED1Port, LED1Pin); \
                            PORTClearBits(LED1Port, LED1Pin);
#define LED1On()        PORTSetBits(LED1Port, LED1Pin);
#define LED1Off()       PORTClearBits(LED1Port, LED1Pin);
#define LED1Toggle()    PORTToggleBits(LED1Port, LED1Pin);
    
#define LED2Init()      PORTSetPinsDigitalOut(LED2Port, LED2Pin); \
                            PORTClearBits(LED2Port, LED2Pin);
#define LED2On()        PORTSetBits(LED2Port, LED2Pin);
#define LED2Off()       PORTClearBits(LED2Port, LED2Pin);
#define LED2Toggle()    PORTToggleBits(LED2Port, LED2Pin);

#define LED3Init()      PORTSetPinsDigitalOut(LED3Port, LED3Pin); \
                            PORTClearBits(LED3Port, LED3Pin);
#define LED3On()        PORTSetBits(LED3Port, LED3Pin);
#define LED3Off()       PORTClearBits(LED3Port, LED3Pin);
#define LED3Toggle()    PORTToggleBits(LED3Port, LED3Pin);

#define LED4Init()      PORTSetPinsDigitalOut(LED4Port, LED4Pin); \
                            PORTClearBits(LED4Port, LED4Pin);
#define LED4On()        PORTSetBits(LED4Port, LED4Pin);
#define LED4Off()       PORTClearBits(LED4Port, LED4Pin);
#define LED4Toggle()    PORTToggleBits(LED4Port, LED4Pin);

#define TestInit()      PORTSetPinsDigitalOut(TestPort, TestPin); \
                            PORTClearBits(TestPort, TestPin);
#define TestOn()        PORTSetBits(TestPort, TestPin);
#define TestOff()       PORTClearBits(TestPort, TestPin);
#define TestToggle()    PORTToggleBits(TestPort, TestPin);

  
#ifdef __cplusplus
}
#endif

#endif 