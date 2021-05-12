#include <plib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "lcd.h"
#include "dev_board_00_00_04_plib.h"

volatile uint64_t us_time = 0;
uint8_t backlightval = 0x08;
uint8_t displaycontrol = 0;
uint8_t displayfunction = 0;
uint8_t displaymode = 0;

uint8_t circ_buffer_data[5120];  // 1024 * 5

lcd_t lcd_driver_t;
circ_buff lcd_circ_buffer;

void WriteString(const char *string)
{
  while (*string != '\0')
    {
      while (!UARTTransmitterIsReady(UART1))
        ;

      UARTSendDataByte(UART1, *string);
      string++;

      while (!UARTTransmissionHasCompleted(UART1))
        ;
    }
}

#pragma config FNOSC =  FRCPLL      // Internal Oscillator with PLL, SYS_CLOCK = 8 Mhz
#pragma config FPLLIDIV = DIV_2     // SYS_CLOCK = 4 Mhz
#pragma config FPLLMUL =  MUL_20    // SYS_CLOCK = 80 Mhz 
#pragma config FPLLODIV = DIV_1     // SYS_CLOCK = 80 Mhz 

#pragma config FPBDIV = DIV_1       // PER_CLOCK = SYS_CLOCK

#pragma config ICESEL = ICS_PGx1

#define	SYS_CLOCK()                 (80E6)
#define PER_CLOCK()                 (SYS_CLOCK()/(1 << OSCCONbits.PBDIV))

#define Fsck                           375000
#define BRG_VAL (PER_CLOCK()/2/Fsck)

bool WithinRange(uint32_t observed, uint32_t expected, uint32_t range)
{
    return (expected - range <= observed) && (observed <= expected + range);
}
    
int main(void)
{
    SYSTEMConfigPerformance(SYS_CLOCK());
    mJTAGPortEnable(DEBUG_JTAGPORT_OFF);
    INTEnableSystemMultiVectoredInt();
   
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_1, 80);
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_1);
    
    UARTConfigure(UART1, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART1, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART1, PER_CLOCK(), 9600);
    UARTEnable(UART1, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    INTEnable(INT_SOURCE_UART_RX(UART1), INT_ENABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART1), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART1), INT_SUB_PRIORITY_LEVEL_0);
   
    LED1Init();
    LED2Init();
    LED3Init();
    LED4Init();
    TestInit();
    
    LED1On();
    LED2On();
    LED3On();
    LED4On();
        
    bool led_once = true;
    
    circ_buff_init(&lcd_circ_buffer, 5, 1024, &circ_buffer_data);
    lcd_init(&lcd_driver_t, &lcd_circ_buffer);
    
    
    
    WriteString("\033[2J");
    WriteString("\033[H");
    WriteString("Dev Board 00.00.05\r\n");
    lcd_write_line(&lcd_driver_t, "Dev Board 00.00.05");
    
    OpenI2C1(I2C_EN, BRG_VAL);
    while(1)
    {
        lcd_process(&lcd_driver_t);
        if(WithinRange(us_time % (uint32_t)1E6, 25, 25) && us_time != 0 && led_once)
        {
//            LED1Toggle();
            led_once = false;
        }
        else if(!WithinRange(us_time % (uint32_t)1E6, 25, 25))
        {
            led_once = true;
        }
    }
}

void __ISR(_TIMER_1_VECTOR, ipl2) _Timer1Handler(void)
{
    us_time++;
    mT1ClearIntFlag();
}

void __ISR(_UART1_VECTOR, ipl2) IntUart1Handler(void)
{
    //RX Interrupt
    if(INTGetFlag(INT_SOURCE_UART_RX(UART1)))
    {
        char byte_received = UARTGetDataByte(UART1);
        if (byte_received == 127)
        {
            lcd_clear(&lcd_driver_t);
            lcd_home(&lcd_driver_t);
        }
        else if (byte_received == 't')
        {
            char buffer[20];
            snprintf(buffer, 20, "t=%" PRIu64, us_time);
            WriteString(buffer);
            WriteString("\r\n");
            lcd_write_line(&lcd_driver_t, buffer);
        }
        else
        {
            lcd_send_char(&lcd_driver_t, byte_received);
        }
        INTClearFlag(INT_SOURCE_UART_RX(UART1));
    }
    
    //TX Interrupt
    if( INTGetFlag(INT_SOURCE_UART_TX(UART1)) )
    {
        INTClearFlag(INT_SOURCE_UART_TX(UART1));
    }
}