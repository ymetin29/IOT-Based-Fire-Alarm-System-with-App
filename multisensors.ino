
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <DHT.h>
// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "*****"
#define WIFI_PASSWORD "******"
// Insert Firebase project API Key
#define API_KEY "*******"
// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "*******"
#define USER_PASSWORD "********"
// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "iotsensordb-9b204-default-rtdb.firebaseio.com"
#define DHTPIN D4
#define DHTTYPE DHT11
#define Buzzer 9
#define MQ2 A0
#define flame D0

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;
String tempPath;
String humPath;
String hicPath;
String flmPath;
String gasPath;

float h ;
float t ;
float hic;
int gval;
bool fval;

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 5000;  //Data Sending Delay

DHT dht(DHTPIN, DHTTYPE);

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}
void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}
void sendInt(String path, int value){
  if (Firebase.RTDB.setInt(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}
void sendBool(String path, bool value){
  if (Firebase.RTDB.setBool(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);
  pinMode(flame, INPUT);
  initWiFi();
  dht.begin(); 

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid;

  // Update database path for sensor readings
  tempPath = databasePath + "/temperature"; // --> UsersData/<user_uid>/temperature
  humPath = databasePath + "/humidity"; // --> UsersData/<user_uid>/humidity
  hicPath = databasePath + "/heatindex"; // --> UsersData/<user_uid>/heatindex
  flmPath = databasePath + "/flamevalue"; // --> UsersData/<user_uid>/flamevalue
  gasPath = databasePath + "/gasvalue"; // --> UsersData/<user_uid>/gasvalue
}

void gassensor() {
  gval = analogRead(MQ2);  
  gval = map(gval, 0, 1024, 0, 100);
  if (gval <= 45) {
    Serial.print("Gas Level:");
    Serial.print(gval);
    Serial.println(" No gas detected");  // Gas Leakage Notification NOT DETECTED
    digitalWrite(Buzzer, LOW);
  } else if (gval > 45) {
    Serial.print("Gas Level:");
    Serial.print(gval);
    Serial.println(" Warning! Gas leak detected");  // Gas Leakage Notification DETECTED
    digitalWrite(Buzzer, HIGH);
  }
  delay(1000);
}
void DHT11sensor() { 
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);   
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(t);  // Temperature
  Serial.print("*C");
  Serial.print(" Humidity: %");
  Serial.print(h);  // Humidity
  Serial.print(" Heat index: ");  
  Serial.print(hic); // Hueat Index
  delay(1000);
}

void flamesensor() {
  fval = digitalRead(flame); 
  if (fval == 1) {
    Serial.print('\n'); 
    Serial.println("No Fire detected");  // Fire Source Notification NOT DETECTED
    digitalWrite(Buzzer, LOW);
  } else if (fval == 0) {
    Serial.print('\n'); 
    Serial.println("Warning! Fire was detected");  // Fire Source Notification DETECTED
    digitalWrite(Buzzer, HIGH);
  }
  delay(1000);
}

void loop() {
  digitalWrite(Buzzer,LOW);
  DHT11sensor(); 
  gassensor();
  flamesensor();
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  sendFloat(tempPath, t);
  sendFloat(humPath, h);
  sendFloat(hicPath, hic);
  sendInt(gasPath, gval);
  sendBool(flmPath, fval); 
  }
  delay(1000);
}
