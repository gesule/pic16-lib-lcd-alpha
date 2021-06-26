# pic16-lib-lcd-alpha
### Contolador para displays LCD alphanuméricos basados en el chip HITACHI 44780, para MCUs PIC16F1xxx

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

La ausencia de esta definición activará una sección de código que proporcionará una función de sustitución compatible.

## Ajustes de Integración y uso:

Esta no es una biblioteca orientada a su distribución precompilada. Puesto que su nicho objetivo es el de los MCU de escasos recursos se ha preferido plantear como repositorio de código para ser copiado en cada proyecto que lo requiera.

Por tanto no se rquiere de ninguna función para declarar el interfaz físico en tiempo de ejecución. Es decir, no existe algo como:

         lcd_init(D4, D3, D2, D1, RS, E);

En lugar de eso, se define el interfaz en tiempo de compilación, de ahí la necesidad de mantener el código a la vista. 
En cualquier caso esta configuración, se ha centralizado en este archivo *lcd.h*, y no es necesario tocar nada en el archivo *lcd.c*.

Este planteamiento permite algunos ahorros en memoria de programa y datos, además mejora el rendimiento en ejecución.
 
En esencia los ajustes consisten en:

Se define el tipo *lcd_bus_t* como un campo de bits que representa la conexión del LCD a los puertos de salida del MCU, por ejemlo:

    typedef struct {
      unsigned dbus   : 4;        // PORT[0:3] --> LCD_D[4:7]
      unsigned rs     : 1;        // PORT[4] --> LCD_RS
      unsigned en     : 1;        // PORT[5] --> LCD_E
    }lcd_bus_t;     

*Por supuesto, la definición debe encajar en un tipo char, no puede sumar mas de 8 bits.*

Se declaran sendas variables con direccionamiento absoluto para que hagan referencia a los correspondientes registros SFR. Por ejemplo en un PIC16F1713 podrían ser los siguientes:

    volatile lcd_bus_t  lcd             __at(0x0D);     // Referencia a PORTB
    volatile uint8_t    lcd_tris_port   __at(0x8D);     // Referencia a TRISB
    volatile uint8_t    lcd_ansel_port  __at(0x18D);    // Referencia a ANSELB
    
    
Observa que la referencia lcd, se declara de tipo lcd_bus_t esto permite acceder a las lineas del LCD mediante una simple asignación:

    lcd.rs = 0;
    ...
    lcd.en = 1;
    lcd.dbus = cmd >> 4;    
    ...
    lcd.en = 0;

En definitiva, estamos utilizando los registros del SFR bajo un alias significativo respecto ala aplicación evitando el abuso de macros. *¿No es precioso?*

