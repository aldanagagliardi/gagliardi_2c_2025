/*! @mainpage Guia 1 Ejercicio 4
 * 
 * @section genDesc General Description
 *
 *  Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de salida 
 *  y un puntero a un arreglo donde se almacene los n dígitos.
 *  La función deberá convertir el dato recibido a BCD, 
 *  guardando cada uno de los dígitos de salida en el arreglo pasado como puntero.
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
 * | 5/04/2025  | Creacion del documento                         |
 *
 * @author Aldana Gagliardi
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
/**
 * @brief Esta función convierte un número entero a un arreglo de dígitos BCD.
 *
 * @param data Es el número entero a convertir
 * @param digits Es la cantidad de dígitos que tiene el número
 * @param bcd_number Es el arreglo donde se almacenará la representación BCD
 * @return 0 si la conversión fue exitosa.
 */

int8_t  convToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for (int i = digits - 1; i >= 0; i--)
	{
		bcd_number[i] = data % 10;
		data /= 10;
	}
	return 0;
}
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	printf("Hello world!\n");
}
/*==================[end of file]============================================*/