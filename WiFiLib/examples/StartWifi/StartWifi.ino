/*
 Name:		StartWifi.ino
 Created:	10/13/2017 8:26:55 AM
 Author:	paul
*/

// the setup function runs once when you press reset or power the board
#include <WiFiLib.h>
#include <WiFiMulti.h>
#include <WiFi.h>
#include <Preferences.h>

// this task waits for wifi connected bit to be set
void anotherTask(void *pvParams) {
	// block until we are notified of connection
	xEventGroupWaitBits(wifi_event_group, AP_STA_CONNECTED_BIT, false, true,
		portMAX_DELAY);
	Serial.println("starting second task");
	while (1) {
		vTaskDelay(portMAX_DELAY);
	}
}

void setup() {
	Serial.begin(115200);
	delay(5000);
	wifiTask.configStation("yourssid", "yourpass");
	wifiTask.addAP("ServoCtrl", "");
	wifiTask.serialFeedback(true);
	wifiTask.begin(100);
	xTaskCreate(WifiTaskRun, "wifiTask", 4000, NULL, 1, NULL);
	xTaskCreate(anotherTask, "another Task", 1000, NULL, 5, NULL);
}

// the loop function runs over and over again until power down or reset
void loop() {
	while (1) {
		vTaskDelay(portMAX_DELAY);
	}
}
