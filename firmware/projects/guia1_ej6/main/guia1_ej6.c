/*! @mainpage Ejercicio 6 Guia 1
 *
 * @section Ejercicio 6
 * Escriba una función que reciba un dato de 32 bits,
 * la cantidad de dígitos de salida y dos vectores de estructuras del tipo  gpioConf_t.
 * Uno  de estos vectores es igual al definido en el punto anterior
 * y el otro vector mapea los puertos con el dígito del LCD a donde mostrar un dato
 * Dígito 1 -> GPIO_19
 * Dígito 2 -> GPIO_18
 * Dígito 3 -> GPIO_9
 * La función deberá mostrar por display el valor que recibe.
 * Reutilizar las funciones creadas en los ejercicios anteriores
 *
 * @section hardConn Hardware Connection
 *
 * |      LCD       |   ESP32   |
 * |:--------------:|:----------|
 * | 	 D1	 	    |  GPIO_20  |
 * |     D2         |  GPIO_21  |
 * |     D3         |  GPIO_22  |
 * |     D4         |  GPIO_23  |
 * |     SEL_1      |  GPIO_19  |
 * |     SEL_2      |  GPIO_18  |
 * |     SEL_3      |  GPIO_9   |
 * |     +5V        |   +5V     |
 * |     GND        |   GND     |

 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 5/04/2025  | Creacion del documento	                     |
 * | 11/04/2025 | Complete la documentacion	                     |
 * | 11/04/2025 | Correccion de errores                          |
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

/* Definición de los pines GPIO */
gpioConf_t gpio_bcd[4] = {
	{GPIO_20, GPIO_OUTPUT}, // b0 -> GPIO_20, salida
	{GPIO_21, GPIO_OUTPUT}, // b1 -> GPIO_21, salida
	{GPIO_22, GPIO_OUTPUT}, // b2 -> GPIO_22, salida
	{GPIO_23, GPIO_OUTPUT}	// b3 -> GPIO_23, salida
};

gpioConf_t gpio_select[3] = {
	{GPIO_19, GPIO_OUTPUT}, // Dígito 1 -> GPIO_19 (SEL_1)
	{GPIO_18, GPIO_OUTPUT}, // Dígito 2 -> GPIO_18 (SEL_2)
	{GPIO_9, GPIO_OUTPUT}	// Dígito 3 -> GPIO_9  (SEL_3)
};
/*==================[internal functions declaration]=========================*/
/**
 * @brief Esta función convierte un número entero a un arreglo de dígitos BCD.
 *
 * @param data Es el número entero a convertir, sin signo de 32 bits.
 * @param digits Es la cantidad de dígitos que tiene el número,sin signo de 8 bits.
 * @param bcd_number Es el arreglo donde se almacenará la representación BCD, de 8 bit.
 * @return 0 si la conversión fue exitosa.
 */

int8_t convToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for (int i = digits - 1; i >= 0; i--)
	{
		bcd_number[i] = data % 10;
		data /= 10;
	}
	return 0;
}

/**
 * @brief Funcion que permite configurar los pines GPIO según un valor BCD
 * @param digito Digito de interes
 * @param gpio_map Configuración de los pines GPIO
 */
void gpioState(uint8_t digito, gpioConf_t *gpio_map)
{
	for (int i = 0; i < 4; i++)
	{
		if ((digito >> i) & 1)
		{
			GPIOOn(gpio_map[i].pin); // Activa el pin si el bit correspondiente es 1
		}
		else
		{
			GPIOOff(gpio_map[i].pin); // Apaga el pin si el bit es 0
		}
	}
}


/**
 * @brief Esta función permite mostrar el numero en el display
 * @param data Numero que se mostrara
 * @param digits Es la cantidad de dígitos que tiene el número
 * @param gpio_bcd Configuracion de los pines
 * @param gpio_select  Configuracion de los pines
 * @param bcd_number Es el arreglo donde se almacenará el numero en BCD
 */
void showDisplay(uint32_t data, uint8_t digits, gpioConf_t *gpio_bcd, gpioConf_t *gpio_select, uint8_t *bcd_number)
{
	convToBcdArray(data, digits, bcd_number);

	for (int i = 0; i < digits; i++)
	{
		gpioState(bcd_number[i], gpio_bcd); // Configura segmentos
		GPIOOn(gpio_select[i].pin);			 // Activa el dígito actual
		GPIOOff(gpio_select[i].pin);		 // Desactiva el dígito
	}
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	// Número de prueba
	uint32_t number = 123;
	uint8_t digits = 3;
	uint8_t bcd_number[digits];

	// Inicializar los GPIOs
	for (uint8_t i = 0; i < 4; i++)
	{
		GPIOInit(gpio_bcd[i].pin, gpio_bcd[i].dir);
	}
	for (uint8_t i = 0; i < 3; i++)
	{
		GPIOInit(gpio_select[i].pin, gpio_select[i].dir);
	}

	showDisplay(number, digits, gpio_bcd, &gpio_select, bcd_number);
}
/*==================[end of file]============================================*/