/*! @mainpage Examen 9-06-2025
 *
 * @section genDesc Descripcion General del enunciado 
 * 
 * Consigna de Aplicación: estación de monitoreo
 * Se debe sensar la temperatura y humedad y compararlo con valores umbrales de radiacion para saber si hay 
 * riesgo de nevada.
 * A su vez tambien, se deben comparar valores de radiacion para saber si hay riesgo de radiacion elevada.
 * Si no hay riesgo de nevada ni de radiacion, se debe encender el LED VERDE.
 * Si hay riesgo de radiacion elevada, se debe encender el LED AMARILLO.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral      |   ESP32   	|
 * |:------------------:|:--------------|
 * |Entrada analogica	| 	CH1  		|
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
#include <stdbool.h>
#include "gpio_mcu.h"
#include "uart_mcu.h" 
#include "dht11.h"
#include "led.h"
#include "switch.h"
#include "analog_io_mcu.h"
#include "../../../../../../Users/Usuario/esp/v5.1.6/esp-idf/components/freertos/FreeRTOS-Kernel-SMP/portable/riscv/include/freertos/portmacro.h"
#include <freertos/portmacro.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000  //delay mediciones de temperatura y humedad
#define CONFIG_BLINK_PERIOD_2 5000  //delay mediciones de radiacion

uint16_t rad;
uint8_t teclas;
/*==================[internal data definition]===============================*/
float valorTemperatura =0;
float valorHumedad=0;

TaskHandle_t medir_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

/** @fn mediciones
 * @brief Sensa la temperatura y humedad y devuelve un leyenda por UART dependiendo si se dan o no
 * 		las condiciones para una nevada. 
 */
static void mediciones(void *parametro)
{
	LedOff(LED_2);
	LedOff(LED_3);

    dht11Read(&valorTemperatura, &valorHumedad);
		
	if(valorHumedad<85){  //si es menor al 85% de humedad no hay riesgo de nevada
				UartSendString(UART_PC, " Temperatura: ");
				UartSendString(UART_PC, (char *)UartItoa(valorTemperatura, 10));
				UartSendString(UART_PC, " °C, Humedad: ");
				UartSendString(UART_PC, (char *)UartItoa(valorHumedad,10));
				UartSendString(UART_PC, " % ");
		}
	else { if(valorTemperatura > 0){ //si la humedad es mayor a 85% y las temperaturas estan entre 0 y 2°C hay riesgo de nevada
			if (valorTemperatura < 3){
        			UartSendString(UART_PC, " Temperatura: ");
					UartSendString(UART_PC, (char *)UartItoa(valorTemperatura, 10));
					UartSendString(UART_PC, " °C, Humedad: ");
					UartSendString(UART_PC, (char *)UartItoa(valorHumedad,10));
					UartSendString(UART_PC, " % - RIESGO DE NEVADA ");
			}
	}
	}
	//delay para 1 segundos 
	vTaskDelay(CONFIG_BLINK_PERIOD/ portTICK_PERIOD_MS);
}


/** @fn radiaciones
* @brief compara la radiacion para dar aviso si hay riesgo de radiacion elevada. 
*/
static void radiaciones(void *parametro)
{
	// apago los leds para que no haya error de que alguno quede prendido por error 
	LedOff(LED_2);
	LedOff(LED_3);
	AnalogInputReadSingle(CH1, &rad); // rad valor leido

	if (rad > 0){
		if(rad < 40){
			if(valorHumedad > 85 && valorTemperatura => 0 && valorTemperatura <3 ){
				LedOn(LED_3);
			}
		}
	}else if(rad > 40){
		UartSendString(UART_PC, "Radiacion: "); //muestro la leyenda pedida "RADIACION XVALOR mR/h-Radiacion elevada"
		UartSendString(UART_PC, (char *)UartItoa(rad, 10));
		UartSendString(UART_PC, "mR/h-RADIACION ELEVADA");
		LedOn(LED_2); //enciendo led amarillo 
			
	}
	//delay para los 5segundos 
	vTaskDelay(CONFIG_BLINK_PERIOD_2/ portTICK_PERIOD_MS); 
}
/*==================[external functions definition]==========================*/
void app_main(void){
	/* Inicialización de los LEDS, el sensor, y los botones  */
	LedsInit();
	dht11Init(GPIO_3);
	SwitchesInit();
	AnalogOutputInit();

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = NULL,
		.param_p = NULL};
	UartInit(&my_uart);
	
	analog_input_config_t miestructura = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0};

	AnalogInputInit(&miestructura);
	
	while(1) {
	teclas  = SwitchesRead();
	switch(teclas){
    		case SWITCH_1:
				xTaskCreate(&mediciones, "medir", 2048, NULL, 5, &medir_task_handle);
				xTaskCreate(&radiaciones, "medir", 2048, NULL, 5, &medir_task_handle);
			break;
    		case SWITCH_2:
				 // se apaga el dispositivo 
    		break;
    	}
	}
}


/*==================[end of file]============================================*/