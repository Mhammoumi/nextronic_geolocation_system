#define TINY_GSM_MODEM_SIM900
#include <TinyGsmClient.h>




//#include <SoftwareSerial.h>
#include <AltSoftSerial.h>



#define Reset 9
int i = 0 ;
#include <ThingsBoard.h>

// Your GPRS credentials
const char apn[]  = "internet1.orange.ma";
const char user[] = "";
const char pass[] = "";

  unsigned long lastSend;

// to understand how to obtain an access token
#define TOKEN               "UJvRYEGp7buo3HZifREE"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

// Serial port for GSM shield
AltSoftSerial serialGsm(7, 8); // RX, TX pins for communicating with modem

// Initialize GSM modem
TinyGsm modem(serialGsm);

// Initialize GSM client
TinyGsmClient client(modem);

// Initialize ThingsBoard instance
ThingsBoard tb(client);

// Set to true, if modem is connected
bool modemConnected = false;

float lattitude, longitude;

void setup() {

pinMode(Reset , OUTPUT);
digitalWrite(Reset,HIGH);
  Serial.begin(9600);
 


  // Start configuration of shield SIM900 with baudrate and  connect to internet.
  serialGsm.begin(115200);
  serialGsm.write("AT+IPR=9600\r\n");
  serialGsm.end();
  serialGsm.begin(9600);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem: "));
  Serial.println(modemInfo);


digitalWrite(Reset,LOW);
delay(100);
digitalWrite(Reset,HIGH);


}



void getAndSendTemperatureAndHumidityData() {
  

  tb.sendTelemetryFloat("latitude",lattitude);
  tb.sendTelemetryFloat("longtitude",longitude);
  
}

void loop()
{
 

//
  if (!modemConnected) {
    Serial.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
        Serial.println(" fail");
        //delay(10000);
        delay(100);
        return;
    }
    Serial.println(" OK");

    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, user, pass)) {
        Serial.println(" fail");
        //delay(10000);
        delay(100);
        return;
    }

    modemConnected = true;
    Serial.println(" OK");
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
// Update and send only after 1 seconds
  if (millis() - lastSend > 1000){
    Serial.println("Sending data...");
    tb.sendTelemetryFloat("latitude",lattitude);
    tb.sendTelemetryFloat("longtitude",longitude);
    lastSend = millis();
 }

  tb.loop();
}
