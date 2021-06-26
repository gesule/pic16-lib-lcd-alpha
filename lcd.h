/* 
 * File:        lcd.h
 * Author:      Jesús Fco. García
 * Comments:    Contolador para displays LCD alphanuméricos basados en el
 *              chip HITACHI 44780, para MCUs PIC16F1xxx
 * 
 * Revision history: 
 * Copyright (c) 2021 Jesús Fco. García
 *
 * Especificaciones:
 * ------------------
 * Solamente requiere de 6 puertos de salida conectados a las líneas: 
 * D4, D5, D6, D7, RS, E.
 * La asignación exacta puede variar pero está sujeta a las siguientes 
 * restricciones.
 * Las lineas D4-D7 deben conectarse a 4 puertos consecutivos y respetando
 * el mismo orden: 
 * PORTX_i  ---- D4
 * PORTX_i+1  -- D5
 * PORTX_i+2  -- D6
 * PORTX_i+3  -- D7
 * donde: i >= 0 && 4 > i:
 * RS y E pueden conectarse libremente a cualquiera de los puertos restantes
 * asociados al registro.
 * 
 * DEPENDENCIAS:
 * =============
 * Esta biblioteca hace llamadas a una función de retardo con el siquiente 
 * prototipo:
 *               void wait_us(unsigned int us);
 * 
 * Se espera que esta función se haya definido en alguna parte del proyecto
 * o se incluya con alguna biblioteca. Pero si no fuese así, bastará con 
 * comentar o eliminar la definición de la macro: 
 * 
 *              USE_EXTERN_FUNCTION_wait_us
 * 
 * para auto generar una función compatible de sustitución.
 * 
 * AJUSTES de INTEGRACION y USO:
 * =============================
 * Esta no es una biblioteca orientada a su distribución precompilada. Puesto
 * que su nicho objetivo es el de los MCU de escasos recursos se ha preferido
 * plantear como repositorio de código para ser copiado en cada proyecto que
 * lo requiera.
 * No existe una función para declarar el interfaz físico en tiempo de 
 * ejecución, es decir, no existe algo como:
 * 
 *          lcd_init(D4, D3, D2, D1, RS, E);
 * 
 * En lugar de eso, es preciso definir el interfaz en tiempo de compilación, de
 * ahí la necesidad de mantener el código a la vista. 
 * En cualquier caso esta configuración, se ha centralizado en este archivo:
 * "lcd.h", y no requiere abrir para nada el archivo "lcd.c".
 * 
 * Este planteamiento permite algunos ahorros en memoria de datos y programa,
 * además mejora el rendimiento en ejecución.
 * 
 * En esencia los ajustes consisten en:
 * 
 * 1.- Se define un campo de bits que representa la conexión del LCD a los
 *     puertos desalida del MCU.
 * 2.- Se declaran sendas variables con direccionamiento absoluto para que 
 *     hagan referencia a los siguientes registros SFR
 *     a) El registro PORTx que controla los puertos de interes, además
 *        esta variable se declara con el tipo del campo de bits anterior.
 *     b) El registro TRISx 
 *     c) El registro ASELx
 *     
 * Es decir, nada de punteros ni variables nuevas, utilizamos la memoria que ya
 * está reservada por hardware. ¿No es precioso?
 * 
 */

#ifndef JFGB_LIB_LCD_H
#define	JFGB_LIB_LCD_H

#include <xc.h> 
#include <stdint.h>

/*__ Ajustes de aplicación: __________________________________________________*/

/* Al definir la siguiente macro, se está declarando que en el proyecto existe
 * una función de retardo con el siguiente prototipo:
 * 
 *                  void wait_us(unsigned int us);
 * 
 * Si este no fuese el caso, coomente esta la definición para habilitar la 
 * compilación de una función compatible definida en el archivo lcd.c
 */
#define USE_EXTERN_FUNCTION_wait_us


/* Si comenta la macro anterior es preciso declarar aquí la frecuencia del 
 * oscilador principal del MCU para producir la función de sustitucion 
 * adecuada.
 */
#ifndef USE_EXTERN_FUNCTION_wait_us

#define _XTAL_FREQ 32000000

#endif

/**
 * Reordene los campos de la siguiente estructura para que reflejen la conexión
 * física entre el MCU y el LCD.
 * Puede insertar campos anónimos para dejar bits libres entre campos
 * consecutivos. 
 * El total no debe pasar de 8 bits, esta estructura debe encajar en
 * un byte.
 */
typedef struct {
    unsigned dbus   : 4;        // PORT[0:3] --> LCD_D[4:7]
    unsigned rs     : 1;        // PORT[4] --> LCD_RS
    unsigned en     : 1;        // PORT[5] --> LCD_E
                                // GND --> LCD_RW
}lcd_bus_t;         

/**
 *  Rellene las clausulas __at(FSR_ADDRESS) con la dirección correspondiente
 * al registro SFR de interes en cada caso:
 * lcd: Puerto al que se conecta el LCD
 * lcd_tris_port: Registro de control de dirección de E/S de anterior
 * lcd_ansel_port: Registro de selección de entrada analógica del puerto. Si
 * no exitiese, asignelé la misma dirección que a lcd_tris_port.
 */
volatile lcd_bus_t  lcd             __at(0x0D);     // PORTB
volatile uint8_t    lcd_tris_port   __at(0x8D);     // TRISB
volatile uint8_t    lcd_ansel_port  __at(0x18D);    // ANSELB

/*_____________________________________________Fin de ajustes de aplicación __*/



#define CLEAR_DISPLAY                   1, 1600
#define RETURN_HOME                     2, 1600
#define ENTRY_MODE_SET(ID, SH)          (0x04 | (ID<<1) | SH), 50
#define DISPLAY_ON_OFF(D, C, B)         (0x08 | (D << 2) | (C<<1) | B), 50
#define CURSOR_DISPLAY_SHIFT(SC, RL)    (0x10 | (SC<<3) | (RL<<2)), 50
#define FUNCTION_SET(DL, N, F)          (0x20 | (DL<<4) | (N<<3) | (F<<2)), 50
#define SET_CGRAM_ADDRESS(A)            (0x40 + A), 50
#define SET_DDRAM_ADDRESS(A)            (0x40 + A), 50

#define lcd_clear() lcd_command(1, 1600)
#define lcd_return_home() lcd_command(2, 1600)


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    
void lcd_init(void);
void lcd_command(uint8_t cmd, unsigned int us);
void lcd_putc(char chr);
void lcd_puts(const char* str);
void lcd_set_cursor(uint8_t row, uint8_t col);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

