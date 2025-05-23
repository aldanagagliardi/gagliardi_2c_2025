/*! @mainpage Guia 2 Ejercicio 4
 * @section genDesc General Description
 * Ejercicio 4 de la Guia Proyecto 2


 * @section hardConn Hardware Connection

 * |    HC-SR04     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5V  	 	| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 *
 *
 * |    Display LCD |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	D1	     	| 	GPIO_20		|
 * | 	D2	     	| 	GPIO_21  	|
 * | 	D3 	     	| 	GPIO_22  	|
 * | 	D4	     	| 	GPIO_23  	|
 * | 	SEL_1	    | 	GPIO_19  	|
 * | 	SEL_2	    | 	GPIO_18  	|
 * | 	SEL_3     	|  	GPIO_9  	|
 * | 	+5V	     	| 	+5V    		|
 * | 	GND	     	| 	GND    		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                            |
 * |:----------:|:---------------------------------------|
 * | 16/05/2025 | Creacion del documento                 |
 * 
 *
 * @author Aldana Gagliardi
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"
#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "stdint.h"
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/

/** @def CONFIG_BLINK_PERIOD_MEDICION_uS
 *  @brief Tiempo de espera en ms para la lectura, escritura de las mediciones en el LCD
 *         y actualizacion de los LEDS
 */
#define CONFIG_BLINK_PERIOD_MEDICION_uS 1000000

uint16_t ValorLectura;
TaskHandle_t medir_task_handle = NULL;
unsigned int indice=0;
unsigned char ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17
} ;

/*==================[internal data definition]===============================*/

/** @fn FuncTimerA 
 * @brief Timer que controla la Tarea- Medicion
 */

void FuncTimerA(void *parametro)
{
	vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE); 
	/* Envía una notificación a la tarea asociada a medir */
}

/*==================[internal functions declaration]=========================*/
/** @fn medicion 
 * @brief Tarea- encargada de leer el dato desde el micro 
*/

static void medicion(void *parametro)
{
	while (true)
	{

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1, &ValorLectura);
		
		UartSendString(UART_PC, ">tension:");
		UartSendString(UART_PC, (char *)UartItoa(ValorLectura, 10));
		UartSendString(UART_PC, "\r\n");
		AnalogOutputWrite (ECG[indice]);
		indice++;

		if (indice==256){
			indice=0;
		}
	}
}
/*==================[external functions definition]==========================*/
void app_main(void)
{
	analog_input_config_t miStruct = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0};

	AnalogInputInit(&miStruct);
	AnalogOutputInit();

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL

	};
	UartInit(&my_uart);

	timer_config_t timer_medicion = {
		.timer = TIMER_A,
		.period = CONFIG_BLINK_PERIOD_MEDICION_uS,
		.func_p = FuncTimerA,
		.param_p = NULL};
	TimerInit(&timer_medicion);

	xTaskCreate(&medicion, "medir", 4096, NULL, 5, &medir_task_handle);

	/* Inicio los timers */
	TimerStart(timer_medicion.timer);
}
/*==================[end of file]============================================*/