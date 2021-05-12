#include "lcd.h"

static void lcd_data_alert(lcd_t * p_lcd);
static void lcd_add_ic2_byte_to_buffer(lcd_t * p_lcd, uint8_t byte, uint32_t delay);
static void lcd_pulse_enable(lcd_t * p_lcd, uint8_t data, uint32_t delay);
static void lcd_write_4_bits(lcd_t * p_lcd, uint8_t value, uint32_t delay);
static void lcd_expander_write(lcd_t * p_lcd, uint8_t data);
static void lcd_send(lcd_t * p_lcd, uint8_t value, uint8_t mode, uint32_t delay);
static void lcd_command(lcd_t * p_lcd, uint8_t value, uint32_t delay);

void lcd_init(lcd_t * p_lcd, circ_buff * p_circ_buff)
{
    p_lcd->wait_until = 0;
    p_lcd->p_instruction_buffer = p_circ_buff;
    p_lcd->backlight_val = 0x08;
    p_lcd->display_control = 0;
    p_lcd->display_function = 0;
    p_lcd->display_mode = 0;
    p_lcd->pos = 0;
    
    lcd_add_ic2_byte_to_buffer(p_lcd, p_lcd->backlight_val, 4500);
    lcd_write_4_bits(p_lcd, 0x03 << 4, 0);
    lcd_write_4_bits(p_lcd, 0x03 << 4, 0);
    lcd_write_4_bits(p_lcd, 0x03 << 4, 0);
    lcd_write_4_bits(p_lcd, 0x02 << 4, 0);
    p_lcd->display_function = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    lcd_command(p_lcd, (LCD_FUNCTIONSET | p_lcd->display_function), 0);
    p_lcd->display_control = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON;
    lcd_command(p_lcd, (LCD_DISPLAYCONTROL | p_lcd->display_control), 2000);
    lcd_command(p_lcd, LCD_CLEARDISPLAY, 2000);
    p_lcd->display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_command(p_lcd, (LCD_ENTRYMODESET | p_lcd->display_mode), 0);
    lcd_command(p_lcd, LCD_RETURNHOME, 2000);
    p_lcd->state = LCD_SENDING;
}

void lcd_process(lcd_t * p_lcd)
{
    switch (p_lcd->state)
    {
        case LCD_SENDING:
        {
            if (p_lcd->wait_until == 0)
            {
                lcd_instruction_t temp;
                circ_buff_read(p_lcd->p_instruction_buffer, 1, &temp, true);
                lcd_expander_write(p_lcd, temp.byte_to_send);
                p_lcd->wait_until = us_time + temp.micro_wait;
            }
            else 
            {
                if(us_time > p_lcd->wait_until)
                {
                    if(p_lcd->p_instruction_buffer->data_cur_length > 0)
                    {
                        lcd_instruction_t temp;
                        circ_buff_read(p_lcd->p_instruction_buffer, 1, &temp, true);
                        lcd_expander_write(p_lcd, temp.byte_to_send);
                        p_lcd->wait_until = us_time + temp.micro_wait;
                    }
                    else
                    {
                        p_lcd->wait_until = 0;
                        p_lcd->state = LCD_IDLE;
                    }
                }
            }
            break;
        }
        case LCD_IDLE:
        {
            break;
        }
    }
}

static void lcd_data_alert(lcd_t * p_lcd)
{
    if(p_lcd->state == LCD_IDLE)
    {
        p_lcd->state = LCD_SENDING;
    }
}

void lcd_send_char(lcd_t * p_lcd, char test)
{
    lcd_send(p_lcd, test, 1, 0);
    lcd_data_alert(p_lcd);
}


void lcd_send_chars(lcd_t * p_lcd, char * test)
{
    lcd_data_alert(p_lcd);
    uint8_t string_len = strlen(test);
    uint8_t iter = 0;
    for(; iter < string_len; iter++){
        lcd_send(p_lcd, *(test + iter), 1, 0);
    }
    lcd_data_alert(p_lcd);
}

void lcd_write_line(lcd_t * p_lcd, char * test)
{
    lcd_data_alert(p_lcd);
    uint8_t string_len = strlen(test);
    if(string_len > 20)
    {
        string_len = 20;
    }
    uint8_t iter = 0;
    for(; iter < string_len; iter++){
        lcd_send(p_lcd, *(test + iter), 1, 0);
    }
    p_lcd->pos += 1;
    if(p_lcd->pos == 4)
    {
        p_lcd->pos = 0;
    }
    if(p_lcd->pos == 1 || p_lcd->pos == 3)
    {
        if(string_len % LCD_MAX_COL != 0)
        {
            lcd_shift(p_lcd, LCD_SHIFT_RIGHT, LCD_MAX_COL * 2 - string_len % LCD_MAX_COL);
        }
        else
        {
            lcd_shift(p_lcd, LCD_SHIFT_RIGHT, LCD_MAX_COL);
        }
    }
    else if(p_lcd->pos == 2)
    {
        if(string_len % LCD_MAX_COL != 0)
        {
            lcd_shift(p_lcd, LCD_SHIFT_LEFT, LCD_MAX_COL + string_len % LCD_MAX_COL);
        }
        else
        {
           lcd_shift(p_lcd, LCD_SHIFT_LEFT, LCD_MAX_COL * 2);
        }
    }
    else if(p_lcd->pos == 0)
    {
        lcd_home(p_lcd);
    }
    lcd_data_alert(p_lcd);
}

void lcd_clear(lcd_t * p_lcd)
{
    lcd_command(p_lcd, LCD_CLEARDISPLAY, 2000);
    lcd_data_alert(p_lcd);
}

void lcd_home(lcd_t * p_lcd)
{
    p_lcd->pos = 0;
    lcd_command(p_lcd, LCD_RETURNHOME, 2000);
    lcd_data_alert(p_lcd);
}

void lcd_shift(lcd_t * p_lcd, lcd_shift_dir dir, uint8_t len)
{
    uint8_t command = 0b00010000;
    if(dir == LCD_SHIFT_RIGHT)
    {
        command |= LCD_SHIFTRIGHTMASK;
    }
    int iter = 0;
    for(; iter < len; iter++)
    {
        lcd_command(p_lcd, command, 100);
    }
    lcd_data_alert(p_lcd);
}

static void lcd_add_ic2_byte_to_buffer(lcd_t * p_lcd, uint8_t byte, uint32_t delay)
{
    lcd_instruction_t temp;
    temp.byte_to_send = byte;
    temp.micro_wait = delay;
    circ_buff_write(p_lcd->p_instruction_buffer, 1, &temp);
}

static void lcd_pulse_enable(lcd_t * p_lcd, uint8_t data, uint32_t delay){
    lcd_add_ic2_byte_to_buffer(p_lcd, (data | En), 50);
    lcd_add_ic2_byte_to_buffer(p_lcd, (data | ~En), 50 + delay);
}

static void lcd_write_4_bits(lcd_t * p_lcd, uint8_t value, uint32_t delay){
    lcd_add_ic2_byte_to_buffer(p_lcd, value, 50);
    lcd_pulse_enable(p_lcd, value, delay);
}

static void lcd_expander_write(lcd_t * p_lcd, uint8_t data){
    start_i2c();
    idle_i2c()
    send_i2c(0x27 << 1);
    idle_i2c();
    send_i2c(data | p_lcd->backlight_val);
    idle_i2c();
    stop_i2c();
}

static void lcd_send(lcd_t * p_lcd, uint8_t value, uint8_t mode, uint32_t delay) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    lcd_write_4_bits(p_lcd, ((highnib)|mode), delay);
	lcd_write_4_bits(p_lcd, ((lownib)|mode), delay); 
}

static void lcd_command(lcd_t * p_lcd, uint8_t value, uint32_t delay) {
	lcd_send(p_lcd, value, 0, delay);
}

void lcd_debug_instruction_buffer(lcd_t * p_lcd, char * debug_msg, uint32_t len)
{
    lcd_instruction_t temp[256];
    uint32_t length_of_instruction_buffer = p_lcd->p_instruction_buffer->data_cur_length;
    snprintf(debug_msg, len, "%slen=%u\r\n", debug_msg, length_of_instruction_buffer);
    circ_buff_read(p_lcd->p_instruction_buffer, 256, &temp, false);
    int iter = 0;
    for(; iter < length_of_instruction_buffer; iter++)
    {
        snprintf(debug_msg, len, "%sbyte=%02x,wait=%u\r\n", debug_msg, temp[iter].byte_to_send, temp[iter].micro_wait);
    }
}