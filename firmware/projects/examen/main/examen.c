/*! @mainpage Examen 9-06-2025
 *
 * @section genDesc General Description
 *
 * Consigna de Aplicación: estación de monitoreo
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
 * | 09-06-2025 | Creacion del documento                         |
 *
 * @author Aldana Gagliardi aldanagagliardi1997@gmail.com
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "uart_mcu.h" 
#include "dht11.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000  //delay mediciones de temperatura y humedad
#define CONFIG_BLINK_PERIOD_2 5000  //delay mediciones de radiacion
/*==================[internal data definition]===============================*/
float valorTemperatura =0;
float valorHumedad=0;

TaskHandle_t medir_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

/** @fn mediciones
 * @brief Sensa la temperatura y humedad y devuelve un leyenda por UART dependiendo si se dan o no
 * 		las condiciones para una nevada
 */
static void mediciones(void *parametro)
{
    dht11Read(valorTemperatura, valorHumedad);
		
	if(valorHumedad<85){  //si es menor al 85% de humedad no hay riesgo de nevada
				UartSendString(UART_PC, " Temperatura: ");
				UartSendString(UART_PC, (char *)UartItoa(valorTemperatura, 10));
				UartSendString(UART_PC, " °C, Humedad: ");
				UartSendString(UART_PC, (char *)UartItoa(valorHumedad,10));
				UartSendString(UART_PC, " % ");
		}
	} else if(valorTemperatura>0){
			if (valorTemperatura<2){
        			UartSendString(UART_PC, " Temperatura: 10ºC - Húmedad: 70% ");
					UartSendString(UART_PC, (char *)UartItoa(valorTemperatura, 10));
					UartSendString(UART_PC, " °C, Humedad: ");
					UartSendString(UART_PC, (char *)UartItoa(valorHumedad,10));
					UartSendString(UART_PC, " - RIESGO DE NEVADA ");
			}
	}
	vTaskDelay(CONFIG_BLINK_PERIOD/ portTICK_PERIOD_MS);

	
}

/*==================[external functions definition]==========================*/
void app_main(void){
	/* Inicialización de los LEDS, el sensor, y los botones  */
	LedsInit();
	dht11Init(GPIO_3);
	SwitchesInit();

	SwitchActivInt(SWITCH_1, tecla1, NULL);

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = NULL,
		.param_p = NULL};
	UartInit(&my_uart);
	
	xTaskCreate(&mediciones, "medir", 2048, NULL, 5, &medir_task_handle);
}
/*==================[end of file]============================================*/