/*! @mainpage Guia 2 Ejercicio 1
 *
 * @section genDesc General Description
 *
 * Diseñar el firmware modelando con un diagrama de flujo de manera que cumpla con las siguientes funcionalidades:

 * Mostrar distancia medida utilizando los leds de la siguiente manera:
 * Si la distancia es menor a 10 cm, apagar todos los LEDs.
 * Si la distancia está entre 10 y 20 cm, encender el LED_1.
 * Si la distancia está entre 20 y 30 cm, encender el LED_2 y LED_1.
 * Si la distancia es mayor a 30 cm, encender el LED_3, LED_2 y LED_1.

 * Mostrar el valor de distancia en cm utilizando el display LCD.
 * Usar TEC1 para activar y detener la medición.
 * Usar TEC2 para mantener el resultado (“HOLD”).
 * Refresco de medición: 1 s
 * Se deberá conectar a la EDU-ESP un sensor de ultrasonido HC-SR04 y una pantalla LCD y utilizando los drivers provistos por la cátedra implementar la aplicación correspondiente. Se debe subir al repositorio el código. Se debe incluir en la documentación, realizada con doxygen, el diagrama de flujo. 
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
 * |  Display LCD   |   ESP32   	|
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
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 9 /05/2025 | Creacion del documento                         |
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
#include "gpio_mcu.h"
#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/

/** @def PERDIOD_MEDICION
 *  @brief Tiempo de espera (ms) para la lectura, escritura de las mediciones en LCD y actualizacion de los LEDS
 */
#define CONFIG_BLINK_PERIOD_MEDICION_uS 1000000

/*==================[internal data definition]===============================*/
/** @def hold
 * @brief Variable global de tipo booleana que indica si se debe mantener la ultima medicion en LCD */
bool hold = false;

/** @def medir
 * @brief Variable global de tipo booleana que indica si se realizaran las mediciones */
bool medir = false;

/** @def valor_medicion
 *  @brief Variable global de tipo entero sin signo para registrar los velores medidos  */
uint16_t valor_medicion = 0;


TaskHandle_t medir_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/** @fn FuncTimerA (void *parametro)
 * @brief timer que se encarga de controlar la tarea medicion */

void FuncTimerA(void* parametro){
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a medir */
}

/** @fn FuncTimerB (void *parametro)
 * @brief timer que se encarga de controlar la tarea mostrar en pantalla */

 void FuncTimerB(void* parametro){
    vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a mostar  */
}

/** @fn controlarLeds (void)
 * @brief funcion encargada de controlar los leds*/

void controlarLeds(void)
{
	if (valor_medicion < 10)
	{
		LedOff(LED_1);
		LedOff(LED_2);
		LedOff(LED_3);
	}
	else
	{
		if (valor_medicion > 10 && valor_medicion < 20)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		else
		{
			if (valor_medicion > 20 && valor_medicion < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else
			{
				if (valor_medicion > 30)
				{
					LedOn(LED_1);
					LedOn(LED_2);
					LedOn(LED_3);
				}
			}
		}
	}
}

/** @fn medicion (void *parametro)
 * @brief tarea encargada de medir la distancia a partir del sensor*/

static void medicion(void *parametro)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if (medir == true)
		{
			valor_medicion = HcSr04ReadDistanceInCentimeters(); // lector del sensor
		}

	
	}
}

/** @fn mostrar (void *parametro)
 * @brief tarea encargada de mostrar por pantalla la distancia medida por el sensor*/

static void mostrar(void *parametro)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  

		if (medir)
		{
			controlarLeds();
			if (!hold)
			{
				LcdItsE0803Write(valor_medicion);
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

/** @fn tecla_1 (void *parametro)
 * @brief funcion que se encarga de controlar la tecla 1 */
void tecla_1 ()
{
		medir = !medir;
}

/** @fn tecla_2 (void *parametro)
 * @brief funcion que se encarga de controlar la tecla 2 */
void tecla_2 ()
{
		hold = !hold;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
/* Inicialización de LCD, Leds, Sensor de Ultrasonido (configuracion de pines) y Switchs */
	LcdItsE0803Init();
	LedsInit();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();
	SwitchActivInt(SWITCH_1, tecla_1, NULL);
	SwitchActivInt(SWITCH_2, tecla_2, NULL);

	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_MEDICION_uS,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

    timer_config_t timer_mostrar = {
        .timer = TIMER_B,
        .period = CONFIG_BLINK_PERIOD_MEDICION_uS,
        .func_p = FuncTimerB,
        .param_p = NULL
    };
	TimerInit(&timer_mostrar);

	/* Creacion de las tareas teclas mostrar y medicion */
	xTaskCreate(&mostrar, "mostrar", 2048, NULL, 5, &mostrar_task_handle);
	xTaskCreate(&medicion, "medir", 2048, NULL, 5, &medir_task_handle);

	/* Inicio los timers */
	TimerStart(timer_medicion.timer);
    TimerStart(timer_mostrar.timer);
}
/*==================[end of file]============================================*/