#include <Arduino.h>
#include "../include/checkState.h"
#include "../include/logdata.h"
#include "../include/readsensors.h"
#include "../include/defs.h"
#include "../include/kalmanfilter.h"
#include "../include/transmitwifi.h"

TimerHandle_t ejectionTimerHandle = NULL;

TaskHandle_t WiFiTelemetryTaskHandle = NULL;
TaskHandle_t GetDataTaskHandle = NULL;
TaskHandle_t GPSTaskHandle = NULL;

float BASE_ALTITUDE = 0;

float previousAltitude = 0.0;

int state = 0;

static uint16_t wifi_queue_length = 100;
static uint16_t sd_queue_length = 500;
static uint16_t gps_queue_length = 100;

static QueueHandle_t wifi_telemetry_queue;
static QueueHandle_t sdwrite_queue;
static QueueHandle_t gps_queue;

//  switch off drogue ejection
void DrogueEjectionTimerCallback(TimerHandle_t ejectionTimerHandle)
{
    digitalWrite(DROGUE_EJECTION_PIN, LOW);
}

// switch off main ejection
void MainEjectionTimerCallback(TimerHandle_t ejectionTimerHandle)
{
    digitalWrite(MAIN_EJECTION_PIN, LOW);
}

// Ejection fires the explosive charge using a mosfet
void ejection(uint8_t ejectionPin)
{
    if (ejectionPin == MAIN_EJECTION_PIN)
    {
        digitalWrite(MAIN_EJECTION_PIN, HIGH);
        ejectionTimerHandle = xTimerCreate("MainEjectionTimer", EJECTION_PIN_ON_TIME_MS / portTICK_PERIOD_MS, pdFALSE, (void *)0, MainEjectionTimerCallback);
        xTimerStart(ejectionTimerHandle, portMAX_DELAY);
    }
    else if (ejectionPin == DROGUE_EJECTION_PIN)
    {
        digitalWrite(DROGUE_EJECTION_PIN, HIGH);
        ejectionTimerHandle = xTimerCreate("DrogueEjectionTimer", EJECTION_PIN_ON_TIME_MS / portTICK_PERIOD_MS, pdFALSE, (void *)0, DrogueEjectionTimerCallback);
        xTimerStart(ejectionTimerHandle, portMAX_DELAY);
    }
}

struct LogData readData()
{
    struct LogData ld = {0};
    struct SensorReadings readings = {0};
    struct FilteredValues filtered_values = {0};

    readings = get_readings();

    // TODO: very important to know the orientation of the altimeter
    filtered_values = kalmanUpdate(readings.altitude, readings.ay);
    float currentAltitude = filtered_values.displacement;
    debugln(currentAltitude);

    // Update state
    state = checkState(currentAltitude, previousAltitude, filtered_values.velocity, filtered_values.acceleration, state);

    previousAltitude = currentAltitude;

    ld = formart_SD_data(readings, filtered_values);
    ld.state = state;

    ld.timeStamp = millis();

    return ld;
}

/*
**********Time Taken for each Task******************
        Get Data Task  - 36ms
        WiFiTelemetryTask -74ms
        GPS Task - 1000ms
        SD Write Task - 60ms
*/

// get dt tsk

void GetDataTask(void *parameter)
{

    struct LogData ld = {0};
    struct SendValues sv = {0};

    static int droppedWiFiPackets = 0;
    static int droppedSDPackets = 0;

    for (;;)
    {

        ld = readData();
        sv = formart_send_data(ld);

        if (xQueueSend(wifi_telemetry_queue, (void *)&sv, 0) != pdTRUE)
        {
            droppedWiFiPackets++;
        }
        if (xQueueSend(sdwrite_queue, (void *)&ld, 0) != pdTRUE)
        {
            droppedSDPackets++;
        }

        debugf("Dropped WiFi Packets : %d\n", droppedWiFiPackets);
        debugf("Dropped SD Packets : %d\n", droppedSDPackets);

        // yield to WiFi Telemetry task
        vTaskDelay(74 / portTICK_PERIOD_MS);
    }
}
// void GetDataTask(void *parameter)
// {

//     struct LogData ld = {0};

//     for (;;)
//     {

//         ld = readData();

//         // yield to idle task on core 0
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
// }

// gps tsk

void readGPSTask(void *parameter)
{

    struct GPSReadings gpsReadings = {0};

    static int droppedGPSPackets = 0;

    for (;;)
    {
        gpsReadings = get_gps_readings();

        if (xQueueSend(gps_queue, (void *)&gpsReadings, 0) != pdTRUE)
        {
            droppedGPSPackets++;
        }

        debugf("Dropped GPS Packets : %d\n", droppedGPSPackets);

        // TODO: increase this up from 60 to 1000 in steps of 60 to improve queue performance at the expense of GPS
        // GPS will send 1 reading in 2s when set to 1000
        vTaskDelay(960 / portTICK_PERIOD_MS);
    }
}
// void readGPSTask(void *parameter)
// {

//     struct GPSReadings gpsReadings = {0};

//     for (;;)
//     {
//         gpsReadings = get_gps_readings();
//         //  yield to idle task on core 1
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
// }

void WiFiTelemetryTask(void *parameter)
{
    struct SendValues sv = {0};
    struct SendValues svRecords[5];
    struct GPSReadings gpsReadings = {0};
    float latitude = 0;
    float longitude = 0;

    for (;;)
    {

        for (int i = 0; i < 5; i++)
        {
            xQueueReceive(wifi_telemetry_queue, (void *)&sv, 10);
            svRecords[i] = sv;
            svRecords[i].latitude = latitude;
            svRecords[i].longitude = longitude;

            if (xQueueReceive(gps_queue, (void *)&gpsReadings, 10) == pdTRUE)
            {
                latitude = gpsReadings.latitude;
                longitude = gpsReadings.longitude;
            }
        }

        // handleWiFi(svRecords);

        // yield to Get Data task
        vTaskDelay(36 / portTICK_PERIOD_MS);
    }
}
void setup()
{

    Serial.begin(BAUD_RATE);

    // set up ejection pins
    pinMode(DROGUE_EJECTION_PIN, OUTPUT);
    // main used for single ejection test
    pinMode(MAIN_EJECTION_PIN, OUTPUT);
    ejection(DROGUE_EJECTION_PIN);
    init_sensors();
    void initFlashMemory();

    // get the base_altitude
    BASE_ALTITUDE = get_base_altitude();

    wifi_telemetry_queue = xQueueCreate(wifi_queue_length, sizeof(SendValues));
    sdwrite_queue = xQueueCreate(sd_queue_length, sizeof(LogData));
    gps_queue = xQueueCreate(gps_queue_length, sizeof(GPSReadings));

    // initialize core tasks 1.data 2.gps
    xTaskCreatePinnedToCore(GetDataTask, "GetDataTask", 3000, NULL, 1, &GetDataTaskHandle, pro_cpu);
    xTaskCreatePinnedToCore(WiFiTelemetryTask, "WiFiTelemetryTask", 4000, NULL, 1, &WiFiTelemetryTaskHandle, 0);
    xTaskCreatePinnedToCore(readGPSTask, "ReadGPSTask", 3000, NULL, 1, &GPSTaskHandle, app_cpu);

    vTaskDelete(NULL);
}
void loop()
{
    // vTaskDelete(NULL);
}