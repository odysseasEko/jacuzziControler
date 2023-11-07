#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// WiFi settings
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Pin for the DS18B20 sensor
const int oneWireBus = 2; // Define the pin where the DS18B20 is connected

// Relay control pin
const int heaterRelayPin0 = 5; // Define the pin where the heater relay is connected
const int heaterRelayPin1 = 6;

const int bublerRelaypin0 = 7; 

// Web server on port 80
ESP8266WebServer server(80);

// Create a OneWire object
OneWire oneWire(oneWireBus);

// Create a DallasTemperature object to interface with the DS18B20
DallasTemperature sensors(&oneWire);

// Variables to store temperature values
float currentTemperature = 0.0;
float setTemperature = 37.0; // Change this to your desired set temperature

void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize DS18B20 sensor
  sensors.begin();

  // Initialize relay pin as an OUTPUT
  pinMode(heaterRelayPin0, OUTPUT);
  pinMode(heaterRelayPin1, OUTPUT);

  pinMode(bublerRelaypin0, OUTPUT);

  // Handle HTTP requests
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_GET, handleSetTemperature);

  server.begin();

  digitalWrite(heaterRelayPin0, LOW);
  digitalWrite(heaterRelayPin1, LOW);

}

void loop() {
  server.handleClient();
  
  // Read temperature from DS18B20 sensor
  sensors.requestTemperatures(); 
  currentTemperature = sensors.getTempCByIndex(0);

  // Check if the current temperature is below the set temperature
  if (currentTemperature < setTemperature) {
    digitalWrite(heaterRelayPin0, HIGH); // Turn on the relay
    digitalWrite(heaterRelayPin1, HIGH);
  } else {
    digitalWrite(heaterRelayPin0, LOW); // Turn off the relay
    digitalWrite(heaterRelayPin0, LOW);
  }

  // Print temperature readings for debugging
  Serial.print("Current Temperature: ");
  Serial.print(currentTemperature);
  Serial.print(" °C, Set Temperature: ");
  Serial.print(setTemperature);
  Serial.println(" °C");
}

void handleRoot() {
  String page = "<html><body>";
  page += "<h1>Jacuzzi Controller</h1>";
  page += "<p>Current Temperature: " + String(currentTemperature) + " °C</p>";
  page += "<p>Set Temperature: " + String(setTemperature) + " °C</p>";
  page += "<p><a href='/set?temp=38'>Set Temperature to 38°C</a></p>";
  page += "<p><a href='/set?temp=35'>Set Temperature to 35°C</a></p>";
  page += "<h1>ESP8266 Relay Control</h1>";
  page += "<p>Relay is ";
  page += (digitalRead(bublerRelaypin0) == HIGH) ? "ON" : "OFF";
  page += "<br><a href='/relay'>Toggle Relay</a></p>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleSetTemperature() {
  if (server.args() > 0) {
    setTemperature = server.arg("temp").toFloat();
    server.send(200, "text/plain", "Temperature set to " + String(setTemperature) + " °C");
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handlebublerControl() {
  if (digitalRead(bublerRelaypin0) == LOW) {
    digitalWrite(bublerRelaypin0, HIGH); // Turn the relay ON
  } else {
    digitalWrite(bublerRelaypin0, LOW); // Turn the relay OFF
  }
}