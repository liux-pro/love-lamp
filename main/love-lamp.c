/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <cJSON.h>
#include <nvs.h>
#include <esp_timer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "touch_element/touch_slider.h"
#include "esp_log.h"
#include "wifi_utils.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "communication.h"
#include "led.h"
#include "utils.h"


static const char *TAG = "Touch Slider Example";
static char *TOPIC_CONTROL = "device/love-lamp/control";
#define TOUCH_SLIDER_CHANNEL_NUM     5

static touch_slider_handle_t slider_handle; //Touch slider handle

static const touch_pad_t channel_array[TOUCH_SLIDER_CHANNEL_NUM] = { //Touch slider channels array
        TOUCH_PAD_NUM12,
        TOUCH_PAD_NUM13,
        TOUCH_PAD_NUM14,
        TOUCH_PAD_NUM2,
        TOUCH_PAD_NUM1
};

//单位是微秒
#define LONG_PRESS_THRESHOLD (1000*1000)

/**
 * Using finger slide from slider's beginning to the ending, and output the RAW channel signal, then calculate all the
 * channels sensitivity of the slider, and you can decrease or increase the detection sensitivity by adjusting the threshold divider
 * which locates in touch_slider_global_config_t. Please keep in mind that the real sensitivity totally depends on the
 * physical characteristics, if you want to decrease or increase the detection sensitivity, keep the ratio of those channels the same.
 */
static const float channel_sens_array[TOUCH_SLIDER_CHANNEL_NUM] = {
        0.472 / 3,
        0.424 / 3,
        0.400 / 3,
        0.379 / 3,
        0.507 / 3};


/* Slider event handler task */
static void slider_handler_task(void *arg) {
    static uint32_t  pressStartTime = 0;


    (void) arg; //Unused
    touch_elem_message_t element_message;
    while (1) {
        /* Waiting for touch element messages */
        if (touch_element_message_receive(&element_message, portMAX_DELAY) == ESP_OK) {
            if (element_message.element_type != TOUCH_ELEM_TYPE_SLIDER) {
                continue;
            }
            /* Decode message */
            const touch_slider_message_t *slider_message = touch_slider_get_message(&element_message);
            if (slider_message->event == TOUCH_SLIDER_EVT_ON_PRESS) {
                ESP_LOGI(TAG, "Slider Press, position: %"PRIu32, slider_message->position);
                pressStartTime =  esp_timer_get_time();
            } else if (slider_message->event == TOUCH_SLIDER_EVT_ON_RELEASE) {
                ESP_LOGI(TAG, "Slider Release, position: %"PRIu32, slider_message->position);
                if (esp_timer_get_time() - pressStartTime <= LONG_PRESS_THRESHOLD) {
                    printf("短按\n");
                    if (baseControl.state == LED_STATUS_OFF){
                        led_set_all(baseControl.r, baseControl.g, baseControl.b);
                        baseControl.state = LED_STATUS_ON;
                    } else{
                        led_set_all(0, 0, 0);
                        baseControl.state = LED_STATUS_OFF;
                    }

                }
            } else if (slider_message->event == TOUCH_SLIDER_EVT_ON_CALCULATION) {
                ESP_LOGI(TAG, "Slider Calculate, position: %"PRIu32, slider_message->position);
                  //进入长按状态
                if (esp_timer_get_time() - pressStartTime > LONG_PRESS_THRESHOLD) {
//                    led_set_all_hsv(slider_message->position, 255, 255);
                    led_set_all_hsv(slider_message->position, 255, 255);
                }
            }
        }
    }
}


/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, TOPIC_CONTROL, 0);
            ESP_LOGI(TAG, "sent subscribe successful,device/love-lamp/control, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            if (event->data_len > 0) {
                printf("DATA=%.*s\r\n", event->data_len, event->data);
            } else {
                printf("DATA=NULL\r\n");
                break;
            }

            if (stringCompare(event->topic, event->topic_len, TOPIC_CONTROL, strlen(TOPIC_CONTROL))) {
                if (parseBaseControl(event->data, event->data_len, &baseControl)) {
                    if (baseControl.state == LED_STATUS_ON) {
                        led_set_all(baseControl.r, baseControl.g, baseControl.b);
                    } else {
                        led_set_all(0, 0, 0);
                    }
                }

            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
                ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
                ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                         strerror(event->error_handle->esp_transport_sock_errno));
            } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
                ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
            } else {
                ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}



static void mqtt_app_start(void) {
    extern const uint8_t server_root_cert_pem_start[] asm("_binary_emqxsl_ca_crt_start");

    const esp_mqtt_client_config_t mqtt_cfg = {
            .broker = {
                    .address.uri = "mqtts://b5091a39.ala.cn-hangzhou.emqxsl.cn:8883",
                    .verification.certificate = (const char *) server_root_cert_pem_start
            },
            .credentials ={
                    .client_id = "love-lamp",
                    .username = "love-lamp",
                    .authentication = {
                            .password = "?????",
                    }
            }
    };

    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}


void touch_init() {
    /* Initialize Touch Element library */
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_element_install(&global_config));
    ESP_LOGI(TAG, "Touch element library installed");

    touch_slider_global_config_t slider_global_config = TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(touch_slider_install(&slider_global_config));
    ESP_LOGI(TAG, "Touch slider installed");
    /* Create Touch slider */
    touch_slider_config_t slider_config = {
            .channel_array = channel_array,
            .sensitivity_array = channel_sens_array,
            .channel_num = (sizeof(channel_array) / sizeof(channel_array[0])),
            .position_range = 255
    };
    ESP_ERROR_CHECK(touch_slider_create(&slider_config, &slider_handle));
    /* Subscribe touch slider events (On Press, On Release, On Calculation) */
    ESP_ERROR_CHECK(touch_slider_subscribe_event(slider_handle,
                                                 TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE |
                                                 TOUCH_ELEM_EVENT_ON_CALCULATION, NULL));

    /* Set EVENT as the dispatch method */
    ESP_ERROR_CHECK(touch_slider_set_dispatch_method(slider_handle, TOUCH_ELEM_DISP_EVENT));
    /* Create a handler task to handle event messages */
    xTaskCreate(&slider_handler_task, "slider_handler_task", 4 * 1024, NULL, 5, NULL);

    ESP_LOGI(TAG, "Touch slider created");
    touch_element_start();
    ESP_LOGI(TAG, "Touch element library start");
}


//void test_read_nvs() {
//    nvs_handle_t my_handle;
//    esp_err_t err;
//    nvs_open("storage", NVS_READONLY, &my_handle);
//    size_t required_size;
//    nvs_get_str(my_handle, "key1", NULL, &required_size);
//    char* server_name = malloc(required_size);
//    nvs_get_str(my_handle, "key1", server_name, &required_size);
//    while (1){
//        ESP_LOGI(TAG, "server_name: %s", server_name);
//        vTaskDelay(2000 / portTICK_PERIOD_MS);
//    }
//
//}
void app_main(void) {
    baseControl.r=50;
    baseControl.g=50;
    baseControl.b=50;
    wifi_connect();
    led_init();
    touch_init();
    mqtt_app_start();
//    xTaskCreate(&test_read_nvs, "test_read_nvs", 4 * 1024, NULL, 5, NULL);

}
