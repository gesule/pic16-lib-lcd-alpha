/* 
 * File:   main.c
 * Author: jesus
 * Comments: Programa de test para controlador de lcd alfanumerico.
 * MCU: PIC16F1713   @    Fosc: 32MHz
 * Compilador: XC8
 * 
 * Conexión física: 
 *  MCU         LCD
 * --------------------
 * PORTB0 ----- D4
 * PORTB1 ----- D5
 * PORTB2 ----- D6
 * PORTB3 ----- D7
 * PORTB4 ----- RS
 * PORTB5 ----- E                
 *              RW ---| 4K7 |---> GND
 * 
 * Created on 25 de junio de 2021, 9:27
 */
#include <xc.h>

#include "lcd.h"

#pragma config FOSC = HS        // Crystal oscillator   ***
#pragma config WDTE = OFF       // Watchdog Disabled 
#pragma config PWRTE = OFF      // Power-up Timer Disabled 
#pragma config MCLRE = ON       // MCLR/VPP pin function is MCLR
#pragma config CP = OFF         // Program memory code protection is disabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config CLKOUTEN = OFF   // Clock Out function is disabled.
#pragma config IESO = ON        // Switchover Mode is enabled
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor is enabled

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection off
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control
#pragma config ZCDDIS = ON      // Zero-cross detect disable
#pragma config PLLEN = ON       // Phase Lock Loop enable->4x PLL enable ***
#pragma config STVREN = ON      // Stack Overflow/Underflow will cause a Reset
#pragma config BORV = LO        // Brown-out Reset: low trip point selected.
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset is disabled
#pragma config LVP = ON         // Low-Voltage Programming Enable enabled


#define _XTAL_FREQ 32000000

#ifdef USE_EXTERN_FUNCTION_wait_us

/**
 * Detiene el programa durante el número especificado de micro segundos, 
 * utilizando el Timer 1 para medir el tiempo.
 * El TMR1, debe haber sido configurado previamente, una llaamda a la función
 * TMR1_setup() al inicio del programa será suficiente.
 * 
 * @param us
 */
void wait_us(unsigned int us) 
{
    if(!us) return;
    
    TMR1 = -us;
    PIR1bits.TMR1IF = 0;
    
    T1CONbits.TMR1ON = 1;    
    while(!PIR1bits.TMR1IF);
    T1CONbits.TMR1ON = 0;
}

/**
 * Establece los ajustes de TMR1, para que 1 tic == 1 us.
 */
void TMR1_setup()
{
    T1GCON = 0;                 
    T1CON = 0;
    
#ifdef  _XTAL_FREQ
#if _XTAL_FREQ == 32000000    
    T1CONbits.TMR1CS = 0;       // TMR1_ osc = Fosc/4
    T1CONbits.T1CKPS = 3;       // 1:8 --> 1MHz --> T = 1us
#else
#error "NO se ha configurado TMR1 para la frecuencia declarada en _XTAL_FREQ"
#endif
#else
#error "No ha definido la macro _XTAL_FREQ declarando la frecuencia de reloj "    
#endif    
    
}
#endif  /* USE_EXTERN_FUNCTION_wait_us */


/**
 * Detiene el programa durante el número especificado de mili segundos, 
 * utilizando el Timer 2 para medir el tiempo.
 * El TMR2, debe haber sido configurado previamente, una llaamda a la función
 * TMR2_setup() al inicio del programa será suficiente.
 * 
 * @param us
 */
void delay(unsigned int ms) 
{
    TMR2IF = 0;
    TMR2 = 0;
    T2CONbits.TMR2ON = 1;
    while(ms) {        
        
        if(TMR2IF) {
            TMR2IF = 0;
            --ms;
        }
    }
    T2CONbits.TMR2ON = 0;
}

/**
 * Establece los ajustes de TMR2, para que 1 tic == 1 ms.
 */
void TMR2_setup()
{
    T2CON = 0;
    T2CONbits.T2OUTPS = 0;
#ifdef  _XTAL_FREQ
#if _XTAL_FREQ == 32000000        
    T2CONbits.T2CKPS = 3;       // Prescaler 1:64
    PR2 = 124;
#else
#error "NO se ha configurado TMR1 para la frecuencia declarada en _XTAL_FREQ"
#endif
#else
#error "No ha definido la macro _XTAL_FREQ declarando la frecuencia de reloj "    
#endif        
}

#define LCD_COLS 16
#define LINE_COLS 40

void main()
{
#ifdef USE_EXTERN_FUNCTION_wait_us
    TMR1_setup();
#endif
    
    TMR2_setup();
    delay(100);
    
    lcd_init();
    
    for ( ;; ) {
        lcd_clear();
        lcd_command(ENTRY_MODE_SET(1,0));
        delay(500);
        
        lcd_set_cursor(0, 3);
        lcd_puts("Hola Mundo");           
        delay(1000);
        lcd_set_cursor(1,3);
        lcd_puts("1234567890");
        delay(1000);
        
        // Se situa el cursor justo despues del último caracter visible.
        lcd_set_cursor(0, LCD_COLS);        
        
        // Escribe avanzando hacia la derecha mediante scroll hasta alcanzar
        // la última columna de la línea.
        lcd_command(ENTRY_MODE_SET(1,1));        
        for(char i = 0; i < (LINE_COLS-LCD_COLS); i++){
            lcd_putc('a'+i);
            delay(250);
        }
        
        // Escribe avanzando hacia la izquierda, sin scroll hasta alcanzar
        // el margen izquierdo.
        lcd_command(ENTRY_MODE_SET(0,0));
        lcd_set_cursor(1, LINE_COLS-1);        
        for(char i = 1; i <= 16; i++){
            lcd_putc('A'+(LINE_COLS-LCD_COLS)-i);
            delay(250);
        }
        
        // Activa el scroll hacia la izquierda y continúa retrocediendo
        // un poco mas.
        lcd_command(ENTRY_MODE_SET(0,1));
        for(char i = 1; i <= (LINE_COLS-2*LCD_COLS); i++){
            lcd_putc('A'+(LINE_COLS-2*LCD_COLS)-i);
            delay(250);
        }
        
        // Un guiño
        delay(1000);
        lcd_command(DISPLAY_ON_OFF(0, 0, 0));
        delay(500);
        lcd_command(DISPLAY_ON_OFF(1, 0, 0));
        delay(500);
        
        // Sobrescribimos la cadena de dígitos mientras está oculta
        // por el scroll
        lcd_set_cursor(1,3);
        lcd_command(ENTRY_MODE_SET(1,0));
        lcd_puts("Sorpresa! ");
        
        // Se descubre la leyenda inicial, y sorpresa, se ha sobrescrito
        // lac adena de digitos.
        for(char i = 0; i < LCD_COLS; i++)
        {
            lcd_command(CURSOR_DISPLAY_SHIFT(1, 1));
            delay(200);
        }
        delay(1000);

	}
}



