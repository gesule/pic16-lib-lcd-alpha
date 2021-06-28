# pic16-lib-lcd-alpha
### Contolador de display LCD alfanumérico basado en el chip HITACHI 44780, para MCUs PIC16, PIC18...

## Especificaciones:
Solamente requiere de 6 puertos de salida conectados a las líneas:  D4, D5, D6, D7, RS, E. Los 6 puertos deben pertencer al mismo registro SFR.
La asignación exacta puede variar pero está sujeta a las siguientes restricciones: las lineas D4-D7 deben conectarse a 4 puertos consecutivos respetando el orden. 
- PORTX_i  ---- D4
- PORTX_i+1  -- D5
- PORTX_i+2  -- D6
- PORTX_i+3  -- D7
donde: 0 <= i <= 3

RS y E pueden conectarse libremente a cualquiera de los puertos restantes del mismo registro.
La línea R/W debe conectarse a Vss.

## Dependencias:

Esta biblioteca hace llamadas a una función de retardo con el siquiente prototipo:

    void wait_us(unsigned int us);

Se espera que esta función se haya definido en alguna parte del proyecto o se incluya con alguna biblioteca. Si no fuese así, bastará con comentar o eliminar la definición de la macro: 

    USE_EXTERN_FUNCTION_wait_us

La ausencia de esta definición, activará una sección de código que proporcionará una función de sustitución compatible.

## Ajustes de Integración y uso:

Para relacionar el código con la configuración de pines, edite la sección prevista a tal efecto en el archivo *lcd.h*.

    /*__ Ajustes de aplicación: __________________________________________________*/
    
    #define USE_EXTERN_FUNCTION_wait_us

    #ifndef USE_EXTERN_FUNCTION_wait_us
    #define _XTAL_FREQ 32000000
    #endif

    typedef struct {
        unsigned dbus   : 4;        // PORT[0:3] --> LCD_D[4:7]
        unsigned rs     : 1;        // PORT[4] --> LCD_RS
        unsigned en     : 1;        // PORT[5] --> LCD_E
                                // GND --> LCD_RW
    }lcd_bus_t;         

    volatile lcd_bus_t  lcd             __at(0x0D);     // PORTB
    volatile uint8_t    lcd_tris_port   __at(0x8D);     // TRISB
    volatile uint8_t    lcd_ansel_port  __at(0x18D);    // ANSELB

    /*_____________________________________________Fin de ajustes de aplicación __*/

    
He preferido este enfoque porque es económico, observa que no produce código, solo define un tipo de datos y crea tres referencias a posiciones del SFR.
Además simplifica el código posterior, permite manipular las líneas del LCD directamente, no requiere de la mediación de ninguna función.

    lcd.rs = 0;
    ...
    lcd.en = 1;
    lcd.dbus = cmd >> 4;    
    ...
    lcd.en = 0;
    
El coste de esta decisión es que, para integrar esta biblioteca en un proyecto debe copiar los archivos lcd.h y lcd.c y compilarlos junto al resto de su programa, no podrá almacenarlo ni distribuirlo en formato binario. Sin embargo este es un coste muy razonable puesto que el nicho objetivo de esta biblioteca es el de los MCUs de pocos recursos, donde cada byte cuenta.

