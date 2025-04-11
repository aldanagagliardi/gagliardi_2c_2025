/*! @mainpage Guia 1 - Ejercicio 5
 *
 * @section genDesc General Description
 *          Escribir una función que reciba como parámetro un dígito BCD
 *          y un vector de estructuras del tipo gpioConf_t. Incluya el archivo de cabecera gpio_mcu.h
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	B0	     	| 	GPIO_20     |
 * |    B1          |   GPIO_21     |
 * | 	B2  	 	| 	GPIO_22     |
 * | 	B3  	 	| 	GPIO_23     |
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 4/04/2025  | Creacion del documento                         |
 * | 11/04/2025 | Documentacion y corregi unos errores           |
 * 
 *
 * @author Aldana Gagliardi
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>
#include <stdbool.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	/*!< Número del pin GPIO */
	gpio_t pin;
	/*!< Dirección: '0' entrada, '1' salida */
	io_t dir;
} gpioConf_t;

gpioConf_t gpio_map[4] = {
	{GPIO_20, GPIO_OUTPUT}, // b0 -> GPIO_20, salida
	{GPIO_21, GPIO_OUTPUT}, // b1 -> GPIO_21, salida
	{GPIO_22, GPIO_OUTPUT}, // b2 -> GPIO_22, salida
	{GPIO_23, GPIO_OUTPUT}	// b3 -> GPIO_23, salida
};

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void)
{
	// Inicializo los GPIOs

	for (uint8_t i = 0; i < 4; i++)
	{
		GPIOInit(gpio_map[i].pin, gpio_map[i].dir);

		if ((5 >> i) & 1)
		{
			GPIOOn(gpio_map[i].pin); // Activa el pin si el bit correspondiente es 1
		}
		else
		{
			GPIOOff(gpio_map[i].pin); // Apaga el pin si el bit es 0
		}
	}
}
/*==================[end of file]============================================*/