/*! @mainpage Template
 *
 * @section genDesc Guia 1 Ejercicio 2 
 *
 * Modifique la aplicación 1_blinking_switch de manera de hacer titilar los leds 1 y 2 
 * al mantener presionada las teclas 1 y 2 correspondientemente.
 * También se debe poder hacer titilar el led 3 al presionar 
 * simultáneamente las teclas 1 y 2.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Aldana Gagliardi (aldanagagliardi1997@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	
	uint8_t teclas;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    			LedToggle(LED_1);
    		break;
    		case SWITCH_2:
    			LedToggle(LED_2);
    		break;
			case SWITCH_1 | SWITCH_2:
				LedToggle ( LED_3);
			break;

    	}
	  
	}
}
/*==================[end of file]============================================*/