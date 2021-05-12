#ifndef LCD_H   
#define LCD_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
#include <plib.h>
    
#include "circ_buff.h"
#include "dev_board_00_00_04_plib.h"

// These should be set to their equivalent I2C commands in your environment    
    
#define start_i2c()     StartI2C1();
#define idle_i2c()      IdleI2C1();
#define send_i2c(X)     MasterWriteI2C1(X);
#define stop_i2c()      StopI2C1();

// This should be a uint32_t global microsecond timer
extern volatile uint64_t us_time;
    
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001  // Register select bit

#define LCD_SHIFTLEFTMASK 0x00
#define LCD_SHIFTRIGHTMASK 0x04
#define LCD_MAX_ROW 4
#define LCD_MAX_COL 20
    
typedef struct __attribute__((__packed__))
{
    uint8_t     byte_to_send;
    uint32_t    micro_wait;
}lcd_instruction_t;

typedef enum
{
    LCD_SENDING,   
    LCD_IDLE,
}lcd_state_t;

typedef enum
{
    LCD_SHIFT_LEFT,
    LCD_SHIFT_RIGHT,
}lcd_shift_dir;

typedef struct
{
    lcd_state_t state;
    circ_buff * p_instruction_buffer;
    uint32_t wait_until;
    uint8_t backlight_val;
    uint8_t display_control;
    uint8_t display_function;
    uint8_t display_mode;
    uint8_t pos;
}lcd_t;

void lcd_init(lcd_t *, circ_buff *);
void lcd_process(lcd_t *);
void lcd_send_char(lcd_t *, char);
void lcd_send_chars(lcd_t *, char *);
void lcd_write_line(lcd_t *, char *);
void lcd_clear(lcd_t *);
void lcd_home(lcd_t *);
void lcd_shift(lcd_t *, lcd_shift_dir, uint8_t);

void lcd_debug_instruction_buffer(lcd_t *, char * , uint32_t);
    
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif LCD_H
