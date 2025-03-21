/*! @mainpage Template
 *
 * @section Ejercicio 3- Guia 1
 *
 * Realice un función que reciba un puntero a una estructura LED como la que se muestra a continuación: 
 *	struct leds
 *	{
 *  	uint8_t mode;       ON, OFF, TOGGLE
 * 		uint8_t n_led;        indica el número de led a controlar
 *		uint8_t n_ciclos;   indica la cantidad de ciclos de ncendido/apagado
 *		uint16_t periodo;    indica el tiempo de cada ciclo
 *	} my_leds; 

 * Usando como guía para la implementación el diagrama de flujo presente en la guia 
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
 * | 21/03/2025 | Document creation		                         |
 *
 * @author Aldana Gagliardi
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 100
#define ON 0
#define OFF 1
#define TOGGLE 2

/*==================[internal data definition]===============================*/
struct leds
{
    uint8_t mode;     //  ON, OFF, TOGGLE
	uint8_t n_led;      // indica el número de led a controlar
	uint8_t n_ciclos;   // indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;   // indica el tiempo de cada ciclo
} my_leds; 

/*==================[internal functions declaration]=========================*/
void controlLeds ( struct leds* myleds){
	if(myleds->mode == ON){
	
		switch(myleds->n_led){
			case 1:
				LedOn(LED_1);
			break;
			case 2:
				LedOn(LED_2);
			break;
			case 3:
				LedOn(LED_3);
			break;
		}
	
		}else if (myleds->mode == OFF)
	
			{
				switch(myleds->n_led){
				case 1:
					LedOff(LED_1);
				break;
				case 2:
					LedOff(LED_2);
				break;
				case 3:
					LedOff(LED_3);
				break;
				}
			}
			else if (myleds->mode== TOGGLE){
			
			
				for (int i=0; i< myleds->n_ciclos;i++){
					switch(myleds->n_led){
						case 1:
							LedToggle(LED_1);
						break;
						case 2:
							LedToggle(LED_2);
						break;
						case 3:
							LedToggle (LED_3);
						break;
						}
			
						vTaskDelay(myleds->periodo / portTICK_PERIOD_MS); 
					
				}
	
			}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	struct leds MisLeds;
	MisLeds.mode= TOGGLE;
	MisLeds.n_led = 3;
	MisLeds.n_ciclos = 10;
	MisLeds.periodo =500;
	controlLeds(&MisLeds);
}

/*==================[end of file]============================================*/