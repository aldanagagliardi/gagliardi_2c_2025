/*! @mainpage Guia 2 Ejercicio 2
 * @section genDesc General Description
 * Enunciado 
 * Ejercicio 2 de la Guia Proyecto 2
 * Cree un nuevo proyecto en el que modifique la actividad del punto 1 de manera de utilizar
 * interrupciones para el control de las teclas y el control de tiempos (Timers). 
 *
 * @section hardConn Hardware Connection
 *
 * |    HC-SR04     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5V  	 	| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 *
 * |    display LCD |   ESP32   	|
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
 * @section changelog Changelog
 *
 * |   Date	    | Description                    |
 * |:----------:|:-------------------------------|
 * | 25/04/2025 | Creacion del documento         |
 * | 9/05/2025	| Resolucion de errores          |
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

/*==================[macros and definitions]=================================*/

/** @def CONFIG_BLINK_PERIOD_MEDICION_uS
 *  @brief Tiempo de espera para la lectura, escritura de las mediciones en LCD 
 *         y actualizacion de los LEDS, en ms
 */
#define CONFIG_BLINK_PERIOD_MEDICION_uS 1000000

/*==================[internal data definition]===============================*/

/** @def hold
 * @brief Variable para indicar si se debe mantener la ultima medicion en LCD 
*/
bool hold = false;

/** @def medir
 * @brief Variable que indica si se realizaran las mediciones 
*/
bool medir = false;

/** @def valorMedido
 *  @brief Variable para registrar los velores medidos  
*/
uint16_t valorMedido = 0;

TaskHandle_t medir_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

/** @fn TimerA
 * @brief Timer que controla la Tarea- Medicion */
void TimerA(void* parametro){
    /* Envía una notificación a la tarea asociada a medir */
	vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);    
}

/** @fn TimerB
 * @brief Timer que controla la Tarea- Mostrar */
 void TimerB(void* parametro){
    vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);   
	/* Envía una notificación a la tarea asociada a mostar  */
}

/** @fn controlarLEDS
 * @brief Funcion que controla los LEDS 1,2 y 3
*/

void controlarLEDS(void)
{
	if (valorMedido < 10)
	{
		LedOff(LED_1);
		LedOff(LED_2);
		LedOff(LED_3);
	}
	else
	{
		if (valorMedido  > 10 && valorMedido  < 20)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		else
		{
			if (valorMedido  > 20 && valorMedido  < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else
			{
				if (valorMedido  > 30)
				{
					LedOn(LED_1);
					LedOn(LED_2);
					LedOn(LED_3);
				}
			}
		}
	}
}

/** @fn medicion 
 * @brief Mide la distancia a partir del sensor. Es una tarea
*/

static void medicion(void *parametro)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (medir == true)
		{
			valorMedido = HcSr04ReadDistanceInCentimeters(); // lector del sensor
		}

	
	}
}

/** @fn mostrar
 * @brief Muestra por pantalla la distancia que mide el sensor. Es una tarea
 */
static void mostrar(void *parametro)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  

		if (medir)
		{
			controlarLEDS();
			if (!hold)
			{
				LcdItsE0803Write(valorMedido );
			}
		}
		else
		{
			LcdItsE0803Off();
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		
	}
	
}
/** @fn tecla1 
 * @brief Controla la tecla 1 
 */
void tecla1 ()
{
		medir = !medir;
}

/** @fn tecla2
 * @brief Controla la tecla 2 
 */
void tecla2 ()
{
		hold = !hold;
}


/*==================[external functions definition]==========================*/
void app_main(void){
	/* Inicialización de LCD, Leds, Sensor de Ultrasonido y Switchs */
	LcdItsE0803Init();
	LedsInit();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();
	SwitchActivInt(SWITCH_1, tecla1, NULL);
	SwitchActivInt(SWITCH_2, tecla2, NULL);

	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_MEDICION_uS,
        .func_p = TimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

    timer_config_t timer_mostrar = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_MEDICION_uS,
        .func_p = TimerB,
        .param_p = NULL
    };
	TimerInit(&timer_mostrar);

	/* Creacion de las tareas mostrar y medicion */
	xTaskCreate(&mostrar, "mostrar", 2048, NULL, 5, &mostrar_task_handle);
	xTaskCreate(&medicion, "medir", 2048, NULL, 5, &medir_task_handle);

	/* Inicializo los timers */
	TimerStart(timer_medicion.timer);
    TimerStart(timer_mostrar.timer);
}
/*==================[end of file]============================================*/