/*
 Name:		WiFiLib.h
 Created:	10/13/2017 8:14:09 AM
 Author:	paul
 Editor:	http://www.visualmicro.com
*/

#ifndef _WiFiLib_h
#define _WiFiLib_h

#include <WiFi.h>
#include <WiFiMulti.h>
#include <Preferences.h>
#include <freertos\event_groups.h>


void WifiTaskRun(void *pvParameters);

extern EventGroupHandle_t wifi_event_group;




/* The event group allows multiple bits for each event,
but we use two to say if we are connected to AP as station AP_STA_CONNECTED_BIT
or a sta has connected to us STA_AP_CONNECTED_BIT so application can check status*/
const int STA_AP_CONNECTED_BIT = 0x000000001;
const int AP_STA_CONNECTED_BIT = 0x000000010;

class WIFITASK {
public:
	WIFITASK();
	void serialFeedback(bool feedbackOn);
	void closeApOnStationConnect(bool closeAP);
	void clearStoredSettings();
	void addAP(String ssid, String pass);
	void configStation(String ssid, String pass);
	void begin(uint16_t taskDelay = 100);
	void run(void *pvParams);
	void doEvent(WiFiEvent_t event);
	bool isConnected();
	~WIFITASK();
private:
	void WiFiEvent(WiFiEvent_t event);
	void onConnect();
	void OnDisconnect();
	bool wifiConnected;
	bool wantSerialInfo = true;
	bool closeApOnStaConnect = false;
	bool storeSettings;
	bool startAccessPoint = false;
	
	uint16_t taskDelay = 100;
	std::vector<WifiAPlist_t> APlist;
	WiFiMulti wifiApConnection;
	Preferences preferences;
	String ssid;
	String passWord;
	String apSSID;
	String apPassWord;
	IPAddress localIp;
	IPAddress gatewayIp;
	IPAddress dnsServerIp;
	IPv6Address localIpV6;
	IPv6Address remoteIpV6;
	IPAddress remoteIp;
};

extern WIFITASK wifiTask;


#endif

