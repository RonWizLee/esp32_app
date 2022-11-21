#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>

//BLE server
#define bleDeviceName "ESP32"
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

// Alturas de tanque
#define ALT_MAX_DEPOSIT 5
#define ALT_MIN_DEPOSIT 25

#define MOTOR 26

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;  // 1 seg

bool deviceConnected = false;

static char distance1Char[5];
static char distance2Char[5];
static char phChar[5];
static char turbChar[5];

// Variables de los sensores
int distDeposit = 0;
int pinEco1 = 12;
int pinGatillo1 = 13;
int distCisterna = 0;
int pinEco2 = 27;
int pinGatillo2 = 14;

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH);
}

// Distance 1 Characteristic
BLECharacteristic sr04Distance1Characteristics("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);

// Distance 2 Characteristic
BLECharacteristic sr04Distance2Characteristics("3c49eb0c-abca-40b5-8ebe-368bd46a7e5e", BLECharacteristic::PROPERTY_NOTIFY);

// pH Characteristic
BLECharacteristic sensorPHCharacteristics("96f89428-696a-11ed-a1eb-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);

// pH Characteristic
BLECharacteristic sensorTurbCharacteristics("0730ee7e-69e9-11ed-a1eb-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  // Salidad del Motor
  pinMode(MOTOR, OUTPUT);
  digitalWrite(MOTOR, LOW);

  // Create the BLE Device
  BLEDevice::init(bleDeviceName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *sensorService = pServer->createService(SERVICE_UUID);

  // distDeposit Deposito
  sensorService->addCharacteristic(&sr04Distance1Characteristics);
  sr04Distance1Characteristics.addDescriptor(new BLE2902());

  // distDeposit Cisterna
  sensorService->addCharacteristic(&sr04Distance2Characteristics);
  sr04Distance2Characteristics.addDescriptor(new BLE2902());

  // pH
  sensorService->addCharacteristic(&sensorPHCharacteristics);
  sensorPHCharacteristics.addDescriptor(new BLE2902());

  // Turbidez
  sensorService->addCharacteristic(&sensorTurbCharacteristics);
  sensorTurbCharacteristics.addDescriptor(new BLE2902());

  // Start the service
  sensorService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
}

void loop() {
  double turb_Value = Serial2.parseFloat();
  double ph_Value = Serial2.parseFloat();
  // Serial.println(turb_Value);
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      // Leer distancia del deposito
      distDeposit = 0.01723 * readUltrasonicDistance(pinGatillo1, pinEco1);
      Serial.println(distDeposit);
      if (distDeposit < 500) {
        dtostrf(distDeposit, 6, 2, distance1Char);
        dtostrf(turb_Value, 3, 2, turbChar);
        dtostrf(ph_Value, 3, 2, phChar);
        // Notify distance 1 reading from HC-SR04
        sr04Distance1Characteristics.setValue(distance1Char);
        sr04Distance1Characteristics.notify();
        // Set turbidez Characteristic value and notify
        sensorTurbCharacteristics.setValue(turbChar);
        sensorTurbCharacteristics.notify();
        // Set ph Characteristic value and notify
        sensorPHCharacteristics.setValue(phChar);
        sensorPHCharacteristics.notify();
        // Set distance 2 Characteristic value and notify
        distCisterna = 0.01723 * readUltrasonicDistance(pinGatillo2, pinEco2);
        dtostrf(distCisterna, 6, 2, distance2Char);
        sr04Distance2Characteristics.setValue(distance2Char);
        sr04Distance2Characteristics.notify();
        Serial.println(distCisterna);
        if (distCisterna < ALT_MIN_DEPOSIT) {   // Si la cisterna tiene agua...
          if (distDeposit < ALT_MAX_DEPOSIT) {  // Si el agua está en el limite superior...
            digitalWrite(MOTOR, LOW);           // Apagar motor
          }
          if (distDeposit > ALT_MIN_DEPOSIT) {  // Si el agua está en el limite inferior...
            digitalWrite(MOTOR, HIGH);          // Enceder motor
          }
        } else {
          digitalWrite(MOTOR, LOW);  // Apagar motor+
        }
      }
      lastTime = millis();
    }
  }
}