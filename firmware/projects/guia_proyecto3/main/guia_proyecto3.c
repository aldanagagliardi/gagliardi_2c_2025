/*! @mainpage Proyecto 3
 *
 * @section genDesc General Description
 *
 *El proyecto consiste en adaptar un bastón para ciegos que permita, 
 *por medio de distintos tonos de sonidos avisar si hay algún obstáculo delante o a algún costado y,
 *además, avisar si hay alguna depresión brusca en el terreno, por ejemplo,
 *si la persona se esta acercando al fin de la vereda o si hay algún pozo. 
 *
 * 
 * @section hardConn Hardware Connection
 * 
 * |    HC-SR04     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_2		|
 * | 	TRIGGER	 	| 	GPIO_3		|
 * | 	+5V  	 	| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 *
 * * |    HC-SR04     |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	ECHO	 	| 	GPIO_20		|
 * | 	TRIGGER	 	| 	GPIO_19		|
 * | 	+5V  	 	| 	+5V	    	|
 * | 	GND 	 	| 	GND	    	|
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 06/06/2025 | Document creation		                         |
 *
 * @author Aldana gagliardi
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "ble_mcu.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "../../../../../../Users/Usuario/esp/v5.1.6/esp-idf/components/freertos/FreeRTOS-Kernel-SMP/include/freertos/task.h"
// #include <portmacro.h>


/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 500
#define LED_BT	LED_1
#define ECHO_1 GPIO_2
#define TRIGGER_1 GPIO_3
#define ECHO_2 GPIO_20
#define TRIGGER_2 GPIO_19
TaskHandle_t medir_task_handle = NULL;
TaskHandle_t medir2_task_handle = NULL;
/*==================[internal data definition]===============================*/
uint16_t valorParalelo = 0;
uint16_t valorProfundidad = 0;

/*==================[internal functions declaration]=========================*/

void read_data(uint8_t * data, uint8_t length){

}

static void medidaParalela(void *parametro)
{
	while (true)
	{
		HcSr04Init(ECHO_1, TRIGGER_1);
		valorParalelo = HcSr04ReadDistanceInCentimeters(); // lectura del sensor
        
        if(valorParalelo < 10){ //comparar si es menor a 10cm 
            BleSendString("*A"); //*A indica el sonido que se escuchara
        }else if(valorParalelo < 20){
                BleSendString("*B");
        }else if(valorParalelo < 30){
                BleSendString("*C");
        }else if(valorParalelo < 40){
                BleSendString("*D");  
        } else if(valorParalelo < 50 ){
                BleSendString("*E"); 
        } else if(valorParalelo < 60 ){
                BleSendString("*F"); 
        } else if(valorParalelo < 70 ){
                BleSendString("*G"); 
        } else if(valorParalelo < 80 ){
                BleSendString("*H"); 
        } else if(valorParalelo < 90 ){
                BleSendString("*I"); 
        } else if(valorParalelo < 100 ){
                BleSendString("*J"); 
        } 

        vTaskDelay(CONFIG_BLINK_PERIOD/ portTICK_PERIOD_MS);
	}
}

static void medidaProfundidad(void *parametro)
{
	while (true)
	{
		HcSr04Init(ECHO_2, TRIGGER_2);
		valorProfundidad = HcSr04ReadDistanceInCentimeters(); // lectura del sensor
    
        if(valorProfundidad>7){
            BleSendString("*K"); //*K indica el sonido que se escuchara
        } //comparo si es mayor a 7cm 
        vTaskDelay(CONFIG_BLINK_PERIOD/ portTICK_PERIOD_MS);
	}
}


/*==================[external functions definition]==========================*/
void app_main(void){
    // static neopixel_color_t color;
    
    ble_config_t ble_configuration = {
        "ESP_EDU_ALDANA",  //nombre para identificarlo con el bluetooth del celu
        read_data
    };

    LedsInit();
    BleInit(&ble_configuration);
  //  HcSr04Init(GPIO_3, GPIO_2);

    xTaskCreate(&medidaParalela, "medir", 2048, NULL, 5, &medir_task_handle);
    xTaskCreate(&medidaProfundidad, "medir2", 2048, NULL, 5, &medir2_task_handle);

    /* Se inicializa el LED RGB de la placa */
    while(1){ 
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleStatus()){
            case BLE_OFF:
                LedOff(LED_BT);
            break;
            case BLE_DISCONNECTED:  //parpadea el led si esta desconectado 
                LedToggle(LED_BT);
            break;
            case BLE_CONNECTED: //prende el led si esta conectado 
                LedOn(LED_BT);
            break;
        }
    }
    
   
}

/*==================[end of file]============================================*/
