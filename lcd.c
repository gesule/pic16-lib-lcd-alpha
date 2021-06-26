/* 
 * File:    lcd.c
 * Author:  Jesús Fco. García
 * Comments:
 * Revision history: 
 * Copyright (c) 2021 Jesús Fco. García
 */

#include <stdbool.h>
#include <xc.h>
#include "lcd.h"



#ifdef USE_EXTERN_FUNCTION_wait_us
void wait_us(unsigned int us);

#else

void wait_us(unsigned int us)
{
    while(us--)
        __delay_us(1);
}
#endif


/**
 * Envía un comando al LCD en el modo bus de 8 bits, puesto que la conexión
 * física es de solo 4 bits, envía el nible superior y descarta el inferior.
 * Tras enviar el comando, demorará el retorno durante el tiempo especificado
 * en la llamada. Este retardo debe ser igual o mayor al máximo tiempo 
 * requerido por el LCD para procesar el comando enviado.
 * 
 * @param cmd   Comando a eviar
 * @param us    retardo de retorno en micro segundos.
 */
void lcd_command_mode8(uint8_t cmd, unsigned int us)
{
    lcd.rs = 0;             // address setup time min: 60ns
    asm("NOP");             // >= 125 ns @ Fosc: 32MHz
    asm("NOP");             // >= 250 ns @ Fosc: 32MHz
    lcd.en = 1;
    lcd.dbus = cmd >> 4;    // Data setup time min: 195ns
    asm("NOP");             // >= 125 ns @ Fosc: 32MHz
    asm("NOP");             // >= 250 ns @ Fosc: 32MHz
    asm("NOP");             // >= 375 ns @ Fosc: 32MHz
    asm("NOP");             // >= 500 ns @ Fosc: 32MHz

    lcd.en = 0;             // E pulse width > 500ns
    wait_us(us);
}

/**
 * Configura el puerto al que está conectado el LCD y ejecuta un secuencia
 * de reset.
 */
void lcd_init() {
// Definición de pares: Comando, us

    
    union {
        uint8_t     all;
        lcd_bus_t   lcd;
    } mask;
    
    mask.all = 0xFF;
    mask.lcd.dbus = 0;
    mask.lcd.en = 0;
    mask.lcd.rs = 0;
    
    lcd_tris_port &= mask.all;
    lcd_ansel_port &= mask.all;
    wait_us(16000);         
    lcd_command_mode8(FUNCTION_SET(1, 1, 0)+4100);
    lcd_command_mode8(FUNCTION_SET(1, 1, 0)+100);
    lcd_command_mode8(FUNCTION_SET(1, 1, 0));  
    lcd_command_mode8(FUNCTION_SET(0, 1, 0));
    
    lcd_command(FUNCTION_SET(0, 1, 0)); 
    lcd_command(DISPLAY_ON_OFF(0,0,0));
    lcd_command(CLEAR_DISPLAY);
    lcd_command(ENTRY_MODE_SET(1,0));
    
    lcd_command(DISPLAY_ON_OFF(1, 0, 0));
        
}



/**
 * Envía un byte al LCD a través de un bus de 4 bits.
 * @param byte
 */
void lcd_put_byte(uint8_t byte)
{
    lcd.dbus = byte >> 4;
    asm("NOP");             // >= 125 ns @ Fosc: 32MHz
    asm("NOP");             // >= 250 ns @ Fosc: 32MHz
    lcd.en = 0;
    asm("NOP");             // >= 125 ns @ Fosc: 32MHz
    asm("NOP");             // >= 250 ns @ Fosc: 32MHz
    asm("NOP");             // >= 375 ns @ Fosc: 32MHz
    asm("NOP");             // >= 500 ns @ Fosc: 32MHz
    lcd.en = 1;
    lcd.dbus = byte;
    asm("NOP");             // >= 125 ns @ Fosc: 32MHz
    asm("NOP");             // >= 250 ns @ Fosc: 32MHz
}

void lcd_command(uint8_t cmd, unsigned int us)
{
    lcd.rs = 0;             // RS setup time > 60ns:
    asm("NOP");             // >= 125 ns @ 32MHz >= Fosc
    asm("NOP");             // >= 250 ns @ 32MHz >= Fosc
    lcd.en = 1;
    lcd_put_byte(cmd);
    lcd.en = 0;    
    wait_us(us);
}


void lcd_putc(char chr)
{
    lcd.rs = 1;             // RS stup time > 60ns:
    asm("NOP");             // >= 125 ns @ 32MHz >= Fosc
    asm("NOP");             // >= 250 ns @ 32MHz >= Fosc
    lcd.en = 1;
    lcd_put_byte(chr);
    lcd.en = 0;
    wait_us(60);
}

void lcd_puts(const char* str)
{    
    if(str != 0){
        while(*str != '\0'){
            lcd_putc(*str);
            ++str;
        }
    }
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t cmd = 0x80 + col;
    switch(row)
    {
        //case 0:
        //    offset += 0;
        //    break;
        case 1:
            cmd += 0x40;
            break;
        case 2:
            cmd += 0x14;
            break;
        case 3:
            cmd += 0x54;
            break;
    }
    lcd_command(cmd, 50);
}
