
#include"main.h"
#include"mcal_i2c.h"
#include "esp_log.h"
#include "mcal_uart.h"
#include<stdio.h>
#include<string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "L1_bc660.h"
#include "L1_stc3117.h"

#define BC660

#define ERROR   0
#define SUCCESS 1
#define stc3117

int size=0;
uint8_t selection[100]={0};
static const char *TAG = "STC3117_EVENT";
float battery_voltage= 0;
uint16_t SOC = 0;
uint8_t socdest[3]={0};
char *soc_data, *temp_data, *humid_data;
uint32_t soc_length,temp_length, humid_length;

uint8_t  host_name[] = "\"aht1gz8r8x4vg-ats.iot.us-east-1.amazonaws.com\"";

void app_main() 
{
product sense2_0;
uint8_t status;


mcal_uartinit();
ESP_LOGI(TAG,"working");
i2c_master_init();

HTS221_init();

set_HTS221_power_mode(HTS221_POWER_MODE_ACTIVE);

#ifdef BC660
L1_bc660_configure_sleep_mode(DISABLE_SLEEP_MODE);
L1_bc660_ssl_configuration(0,0,2);
L1_bc660_MQTT_configuration(3,1,0,0,1);
L1_bc660_MQTT_clienconnection(3,host_name,8883);
#endif


#ifdef stc3117
L1_voltage_GasGuage_intialization();
#endif

while(1)
{
    #ifdef stc3117
    SOC= L1_read_soc();
    soc_length = snprintf( NULL, 0, "%d", SOC );
    soc_data = malloc(soc_length+1);
    snprintf( soc_data, soc_length + 1, "%d", SOC );
    L1_read_voltage(&battery_voltage);
    ESP_LOGI(TAG,"battery volatage=%.2fV", battery_voltage);
    ESP_LOGI(TAG,"battery percentage=%d %%", SOC);
    #endif

    read_HTS221_temperature(&sense2_0.sensor_data.temperature_data);
    read_HTS221_humidity(&sense2_0.sensor_data.humidity_data);
    temp_length = snprintf( NULL, 0, "%d", sense2_0.sensor_data.temperature_data );
    humid_length = snprintf( NULL, 0, "%d", sense2_0.sensor_data.humidity_data );
    temp_data = malloc(temp_length+1);
    humid_data = malloc(humid_length+1);
    snprintf( temp_data, temp_length + 1, "%d", sense2_0.sensor_data.temperature_data);
    snprintf( humid_data, humid_length + 1, "%d", sense2_0.sensor_data.humidity_data );
   
    ESP_LOGI(TAG,"temperature=%d", sense2_0.sensor_data.temperature_data);
    ESP_LOGI(TAG,"humidity=%d%%", sense2_0.sensor_data.humidity_data);

   
    #ifdef BC660
    L1_bc660_configure_sleep_mode(DISABLE_SLEEP_MODE);
   
    L1_bc660_MQTT_Publish(3,0,0,0,"\"soc\"",soc_length,soc_data);
    L1_bc660_MQTT_Publish(3,0,0,0,"\"temp\"",temp_length,temp_data);
    L1_bc660_MQTT_Publish(3,0,0,0,"\"humid\"",humid_length,humid_data);
    
    L1_bc660_configure_sleep_mode(ENABLE_LIGHT_SLEEP_DEEP_SLEEP);
    vTaskDelay(100);

    free(soc_data);
    free(temp_data);
    free(humid_data);
    #endif

  


}

}