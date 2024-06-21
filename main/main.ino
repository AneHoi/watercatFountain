
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <iostream>
#include <string>  // for string and to_string()
#include <LiquidCrystal_I2C.h>
#include <iostream>
#include <ctime>

#include "network_lib.h"
#include "motor.h"
#include "distancesensor.h"
#include "responsedto.h"
#include "errormessage_dto.h"
#include "timemanager.h"
#define SOUND_SPEED 0.034

const int deviceId = 1;

const int switchpin = 4;
const int waterpin = 36;

int switchstate = 0;
int stateChanged = 0;

//Temperature sensor
float tempC;  // temperature in Celsius
const int tempsensor = 13;
OneWire oneWire(tempsensor);
DallasTemperature DS18B20(&oneWire);
float maxRuntemp = 30.0;


//Wifi and broker connection
const char* ssid = "Ane";
const char* password = "qwertyuiop";
const char* mqttServer = "mqtt.flespi.io";
const int mqttPort = 1883;
const char* mqttUser = "FlespiToken R7ioy0LLhLzMw0pAUsadQ5tH67LS44a4ne21Uc5g3x80x44t7WIyab0GQ9XkFuFP";
const char* mqttPassword = "";

//Make the subscribe toppic
const std::string subscribeingtopic = "catfountain/clientrequests/" + std::to_string(deviceId);
const char* subTopic = subscribeingtopic.c_str();

//Make the publish topic
const char* pubTopic = "catfountain/waterstate";
const char* pubErrorTopic = "catfountain/error";

DeviceReadingsDto readings;
TimeManager timeManager(ssid, password);


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long startMillis;
unsigned long currentMillis;

//The motor instance
long onTimeForMotor = 10000;  //In milliseconds
bool isMotorOnNow = false;
const int motorpin = 12;
Motor motor(motorpin);

//The distance sensor
const int distSensorEcho = 14;
const int distSensorTrig = 15;
DistanceSensor distSensor(distSensorEcho, distSensorTrig);

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

bool conditionsAreMet = true;

void setup() {
  Serial.begin(115200);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  printToLCD("Device on", 0);
  printToLCD("Searching Wifi", 1);

  timeManager.begin();
  pinMode(switchpin, INPUT_PULLUP);
  pinMode(waterpin, INPUT_PULLUP);
  DS18B20.begin();         // initialize the DS18B20 sensor
  startMillis = millis();  //initial start time
}


void loop() {
  //Keeps connection open, for incomming brokermessages
  if (!client.connected()) {
    connectToBroker();
  }
  client.loop();

  //Make sure, there is water
  if (digitalRead(waterpin) == HIGH) {

    ErrorReadingsDto errorReading;
    errorReading.unacceptableWaterheight = true;
    handleError(errorReading, "Not enough water");
  }
  //Make sure the water temp is not too high
  else if (getTempperatur() > maxRuntemp) {

    ErrorReadingsDto errorReading;
    errorReading.waterTooHot = true;
    handleError(errorReading, "Water is too hot");

  }
  //Check for temerature sensor faliure
  else if (getTempperatur() < -30.0) {
    ErrorReadingsDto errorReading;
    errorReading.temperatureSensorError = true;
    handleError(errorReading, "Temp sensor fail");
    
  }

  //ALL the conditions have been met
  else {
    if (!conditionsAreMet) {
      lcd.clear();
      conditionsAreMet = true;
    }

    switchstate = digitalRead(switchpin);

    //Monitor the cats distance to the device and listen for buttonpres
    if (distSensor.measureDistanceInCM() < 5 || switchstate == LOW) {
      turnOnMotor();
    }
  }
  currentMillis = millis();  //get the current "time" (The number of milliseconds since the program started)

  //If motor is turned on and, it has been for the value of onTimeForMotor, then turn off
  if (currentMillis - startMillis >= onTimeForMotor && motor.ison()) {
    turnOffMotor();
  }
}

void turnOffMotor() {
  motor.off();
  statechanged(pubTopic, motor.ison());
  startMillis = currentMillis;
}
void turnOnMotor() {
  motor.on();
  startMillis = currentMillis;
  if (motor.ison() != isMotorOnNow) {
    statechanged(pubTopic, motor.ison());
  }
}

void printToLCD(String message, int displaycolum) {
  lcd.setCursor(0, displaycolum);
  // print message to LCD
  lcd.print(message);
}


void statechanged(const char* topic, bool isOn) {
  lcd.clear();
  if (isOn) {
    printToLCD("Waterpump: on", 0);
  } else {
    printToLCD("Waterpump: off", 0);
  }
  printToLCD("Temp: " + String(getTempperatur()) + " C", 1);


  isMotorOnNow = isOn;
  std::string currentTime = timeManager.getCurrentTime();  //gets current real time

  SensorDto reading;
  reading.MotorValue = isOn;
  reading.TemperatureValue = tempC;
  reading.TimeStamp = currentTime;
  //Add the reading
  readings.DeviceData.push_back(reading);

  //Topic is ready, here we create the payload to send the object
  DeviceData deviceData(deviceId, readings);
  std::string jsonString = deviceData.toJsonString();

  sendDataToBroker(topic, jsonString.c_str());
  //clear all readings
  readings.DeviceData.clear();
}

float getTempperatur() {
  DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  return tempC;
}

void publishError(ErrorReadingsDto errorReading) {
  std::string currentTime = timeManager.getCurrentTime();  //gets current real time
  errorReading.TimeStamp = currentTime;
  ErrorData errorData(deviceId, errorReading);
  std::string jsonString = errorData.toJsonString();
  sendDataToBroker(pubErrorTopic, jsonString.c_str());
}

//For publishing
void sendDataToBroker(const char* topic, const char* payload) {
  client.publish(topic, payload);
}

void connectToBroker() {
  // Ensure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    printToLCD("Connect to WiFi", 0);
    connectWifi(ssid, password);
    lcd.clear();
  }

  // Connect to the MQTT broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("connected");

      //Setting the subscribe topic
      client.subscribe(subTopic);

    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// For subscribing
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert byte* to char*
  char* str = reinterpret_cast<char*>(payload);

  // Convert the string to an integer using std::atoi
  int seconds = std::atoi(str);
  Serial.print(seconds);
  onTimeForMotor = seconds * 1000;
  turnOnMotor();
}

void handleError(ErrorReadingsDto errorReading, char* message) {
  if (motor.ison()) {
    turnOffMotor();
  }

  if (conditionsAreMet) {
    lcd.clear();
    conditionsAreMet = false;
    publishError(errorReading);

    printToLCD(message, 0);
  }
  delay(500);
  printToLCD(String(getTempperatur()), 1);
}