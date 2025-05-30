/*! @mainpage Guia 2 Ejercicio 1
 *
 * @section genDesc General Description
 *
 * Enunciado
 * Ejercicio 1 de la Guia Proyecto 2
 * Diseñar el firmware modelando con un diagrama de flujo de manera que cumpla 
 * con las siguientes funcionalidades:

 * Mostrar distancia medida utilizando los leds de la siguiente manera:
 * -Si la distancia es menor a 10 cm, apagar todos los LEDs.
 * -Si la distancia está entre 10 y 20 cm, encender el LED_1.
 * -Si la distancia está entre 20 y 30 cm, encender el LED_2 y LED_1.
 * -Si la distancia es mayor a 30 cm, encender el LED_3, LED_2 y LED_1.

 * Mostrar el valor de distancia en cm utilizando el display LCD.
 * -Usar TEC1 para activar y detener la medición.
 * -Usar TEC2 para mantener el resultado (“HOLD”).
 * -Refresco de medición: 1 s
 * Se deberá conectar a la EDU-ESP un sensor de ultrasonido HC-SR04 y una pantalla LCD 
 * y utilizando los drivers provistos por la cátedra implementar la aplicación correspondiente. 
 * Se debe subir al repositorio el código. Se debe incluir en la documentación, 
 * realizada con doxygen, el diagrama de flujo. 
 *
 * @section hardConn Hardware Connection
 *
 * |  Display LCD   |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	SEL_1	    | 	GPIO_19  	|
 * | 	SEL_2	    | 	GPIO_18  	|
 * | 	SEL_3     	|  	GPIO_9  	|
 * | 	D1	     	| 	GPIO_20		|
 * | 	D2	     	| 	GPIO_21  	|
 * | 	D3 	     	| 	GPIO_22  	|
 * | 	D4	     	| 	GPIO_23  	|
 * | 	+5V	     	| 	+5V    		|
 * | 	GND	     	| 	GND    		|
 * 
 * 
 * |    HC-SR04     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	GND 	 	| 	GND	    	|
 * | 	ECHO	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5V  	 	| 	+5V	    	|

 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                       |
 * |:----------:|:----------------------------------|
 * | 9 /05/2025 | Creacion del documento            |
 * | 23/05/2025 | Chequeo el funcionamiento 		|
 *
 * @author Aldana Gagliardi
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "led.h"
#include "switch.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/

/** @def CONFIG_BLINK_PERDIOD_MEDICION_uS
 *  @brief Tiempo de espera para la lectura, 
 * 			escritura de las mediciones en LCD y actualizacion de los LEDS, en ms
 */
#define CONFIG_BLINK_PERIOD_MEDICION_uS 1000000

/*==================[internal data definition]===============================*/
/** @def hold
 * @brief Variable para indicar si se debe mantener la ultima medicion en LCD 
 */
bool hold = false;

/** @def medir
 * @brief Variable para indicar si se realizaran las mediciones 
 */
bool medir = false;

/** @def valorMedido
 *  @brief Variable para registrar los valores medidos  
 */
uint16_t valorMedido= 0;

/*==================[internal functions declaration]=========================*/
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
		if (valorMedido > 10 && valorMedido < 20)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		else
		{
			if (valorMedido > 20 && valorMedido < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else
			{
				if (valorMedido > 30)
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
		if (medir == true)
		{
			valorMedido = HcSr04ReadDistanceInCentimeters(); // lector del sensor
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_MEDICION_uS / portTICK_PERIOD_MS);
	
	}
}

/** @fn mostrar 
 * @brief Muestra por pantalla la distancia que mide el sensor. Es una tarea
 */

static void mostrar(void *parametro)
{
	while (true)
	{
		if (medir)
		{
			controlarLEDS();
			if (!hold)
			{
				LcdItsE0803Write(valorMedido);
			}
		}
		else
		{
			LcdItsE0803Off();
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_MEDICION_uS / portTICK_PERIOD_MS);

	}
	
}

/** @fn teclas 
 * @brief Tarea-se encarga de controlar las acciones por tecla */

static void teclas(void *parametro)
{
	uint8_t tecla;
	while (true)
	{
		tecla = SwitchesRead();

		switch (tecla)
		{
		case SWITCH_1:
			medir = !medir;

			break;

		case SWITCH_2:
			hold = !hold;

			break;
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_MEDICION_uS / portTICK_PERIOD_MS);
	}

}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	/* Inicialización de el LCD, Leds, Sensor de Ultrasonido y Switch */
	LcdItsE0803Init();
	LedsInit();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();
	
	/* Se crean las tareas teclas, mostrar y medicion */
	xTaskCreate(&teclas, "teclas", 2048, NULL, 5, NULL);
	xTaskCreate(&mostrar, "mostrar", 2048, NULL, 5, NULL);
	xTaskCreate(&medicion, "medir", 2048, NULL, 5, NULL);
}
/*==================[end of file]============================================*/