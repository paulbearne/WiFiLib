/*
 Name:		WiFiLib.cpp
 Created:	10/13/2017 8:14:09 AM
 Author:	paul
 Editor:	http://www.visualmicro.com
*/

#include "WiFiLib.h"


EventGroupHandle_t wifi_event_group;
WIFITASK wifiTask;


static void wifiTaskEvent(WiFiEvent_t event) {
	wifiTask.doEvent(event);
}

void WifiTaskRun(void *pvParameters) {
	wifiTask.run(pvParameters);
}



// initialize the variable so that everything empty we'll check for this later
WIFITASK::WIFITASK()
{
	wifiConnected = false;
	ssid = "";
	passWord = "";
	apPassWord = "";
	apSSID = "";
}



void WIFITASK::serialFeedback(bool feedbackon)
{
	wantSerialInfo = feedbackon;
}

void WIFITASK::closeApOnStationConnect(bool closeAP)
{
	closeApOnStaConnect = closeAP;
}

void WIFITASK::addAP(String ssid = "ESP32AP", String pass = "")
{
	startAccessPoint = true;

	if (storeSettings) {
		preferences.begin("wifi", false);
		preferences.putString("apssid", ssid);
		preferences.putString("appassword", pass);
		preferences.end();
	}
	apSSID = ssid;
	apPassWord = pass;
	//wifiApConnection.addAP(ssid.c_str() , pass.c_str());
}

void WIFITASK::clearStoredSettings() {
	preferences.remove("ssid");
	preferences.remove("password");
	preferences.remove("wifi");

}

void WIFITASK::configStation(String ssid, String pass)
{
	this->ssid = ssid;
	this->passWord = pass;
	if (storeSettings) {

		preferences.begin("wifi", false);
		preferences.putString("ssid", ssid);
		preferences.putString("password", passWord);
		preferences.end();
	}
}

void WIFITASK::begin( uint16_t taskDelay)
{
	String tempSsid;
	String tempPassWord;
	String tempApSsid;
	String tempApPassWord;
	wifi_event_group = xEventGroupCreate();
	
	this->taskDelay = taskDelay;
	// see if we have stored preferances
	if (storeSettings) {
		preferences.begin("wifi", false);
		tempSsid = preferences.getString("ssid", "none");           //NVS key ssid
		tempPassWord = preferences.getString("password", "none");   //NVS key password
		tempApSsid = preferences.getString("apssid", "none");           //NVS key apssid
		tempApPassWord = preferences.getString("appassword", "none");   //NVS key appassword
		preferences.end();
		if (tempSsid != "none") {
			ssid = tempSsid;
		}
		if (tempPassWord != "none") {
			passWord = tempPassWord;
		}
		if (tempApSsid != "none") {
			apSSID = tempApSsid;
		}
		if (tempApPassWord != "none") {
			apPassWord = tempApPassWord;
		}
	}
	WiFi.onEvent(wifiTaskEvent);
	WiFi.mode(WIFI_MODE_APSTA);
	WiFi.softAP(apSSID.c_str());


}

void WIFITASK::doEvent(WiFiEvent_t event)
{
	WiFiEvent(event);
}

bool WIFITASK::isConnected()
{
	return wifiConnected;
}




// set of callback functions that need to be declared out side of our task
void WIFITASK::WiFiEvent(WiFiEvent_t event)
{
	switch (event) {

	case SYSTEM_EVENT_AP_START:
		//can set ap hostname here
		WiFi.softAPsetHostname(apSSID.c_str());
		//enable ap ipv6 here

		WiFi.softAPenableIpV6();
		Serial.print("Starting Access Point :");
		Serial.println(apSSID.c_str());
		break;

	case SYSTEM_EVENT_STA_START:
		//set sta hostname here
		WiFi.setHostname(ssid.c_str());
		Serial.print("Station Connecting to : ");
		Serial.println(ssid.c_str());
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		//enable sta ipv6 here
		WiFi.enableIpV6();
		break;
	case SYSTEM_EVENT_AP_STA_GOT_IP6:
		//both interfaces get the same event
		if (wantSerialInfo) {
			Serial.print("STA IPv6: ");
			Serial.println(WiFi.localIPv6());
			Serial.print("STA IPv4: ");
			Serial.println(WiFi.localIP());
			Serial.print("AP IPv6: ");
			Serial.println(WiFi.softAPIPv6());
			Serial.print("AP IPv4: ");
			Serial.println(WiFi.softAPIP());

		}
		remoteIpV6 = WiFi.softAPIPv6();
		remoteIp = WiFi.softAPIP();
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:         /**< a station connected to ESP32 soft-AP */
		xEventGroupSetBits(wifi_event_group, STA_AP_CONNECTED_BIT);

		

		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		xEventGroupClearBits(wifi_event_group, STA_AP_CONNECTED_BIT);


		break;
	case SYSTEM_EVENT_AP_PROBEREQRECVED:        /**< Receive probe request packet in soft-AP interface */
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		onConnect();
		wifiConnected = true;

		xEventGroupSetBits(wifi_event_group, AP_STA_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		wifiConnected = false;

		xEventGroupClearBits(wifi_event_group, AP_STA_CONNECTED_BIT);
		OnDisconnect();
		break;
	default:
		break;
	}
}

void WIFITASK::onConnect()
{
	localIpV6 = WiFi.localIPv6();
	localIp = WiFi.localIP();
	gatewayIp = WiFi.gatewayIP();
	dnsServerIp = WiFi.dnsIP();
	if (wantSerialInfo) {
		Serial.println("STA Connected");
		Serial.print("STA SSID: ");
		Serial.println(WiFi.SSID());
		Serial.print("STA IPv4: ");
		Serial.println(WiFi.localIP());
		Serial.print("STA IPv6: ");
		Serial.println(WiFi.localIPv6());
		Serial.print("Gateway: ");
		Serial.println(WiFi.gatewayIP());
		Serial.print("Dns Server: ");
		Serial.println(WiFi.dnsIP());
	}
	if (closeApOnStaConnect) {
		WiFi.mode(WIFI_MODE_STA);     //close AP network
	}

	

}

void WIFITASK::OnDisconnect()
{
	if (wantSerialInfo) {
		Serial.println("STA Disconnected");
	}
	delay(1000);
	WiFi.begin(ssid.c_str(), passWord.c_str());

	

}


void WIFITASK::run(void *pvParams)
{

	WiFi.begin(ssid.c_str(), passWord.c_str());
	while (1) {
		vTaskDelay(portTICK_PERIOD_MS*taskDelay);
	}
}



WIFITASK::~WIFITASK()
{
}