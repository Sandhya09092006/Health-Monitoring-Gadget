#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <UniversalTelegramBot.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <math.h>

//================== WiFi ==================//

const char* WIFI_NAME = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

//================ ThingSpeak ==============//

unsigned long myChannelNumber = 3441562;
const char *myWriteAPIKey = "YOUR_THINGSPEAK_WRITE_API_KEY";

//================ Telegram ===============//

#define BOT_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

WiFiClientSecure securedClient;
UniversalTelegramBot bot(BOT_TOKEN, securedClient);

//================ Pins ===================//

#define ONE_WIRE_BUS 4

#define HEART_BUTTON_PIN 13

#define SOS_BUTTON_PIN 14

#define BATTERY_PIN 34

#define BUZZER_PIN 26

//================ Objects ================//

Adafruit_MPU6050 mpu;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

WiFiClient client;

//================ Variables ==============//

float bodyTemperature = 0.0;

int heartRate = 75;

int fallDetected = 0;

int healthRisk = 0;

int batteryPercentage = 100;

float totalAccel = 0;

//============= Alert Flags ===============//

bool healthAlertSent = false;

bool batteryAlertSent = false;

bool sosAlertSent = false;

//=========================================
// Telegram Function
//=========================================

void sendTelegram(String msg)
{
  bot.sendMessage(CHAT_ID, msg, "");
}
//=========================================
// SETUP
//=========================================

void setup()
{
  Serial.begin(115200);

  pinMode(HEART_BUTTON_PIN, INPUT_PULLUP);

  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, HIGH);

  // Temperature Sensor
  sensors.begin();

  // MPU6050
  if (!mpu.begin())
  {
    Serial.println("MPU6050 NOT FOUND!");
    while (1);
  }

  Serial.println("MPU6050 Ready");

  // WiFi
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  // Telegram SSL
  securedClient.setInsecure();

  // ThingSpeak
  ThingSpeak.begin(client);

  // Startup Message
  sendTelegram(
    "✅ Health Monitoring Gadget Started Successfully."
  );

  Serial.println("System Ready");
}
//=========================================
// LOOP
//=========================================

void loop()
{
  // -------- Temperature --------
  sensors.requestTemperatures();
  bodyTemperature = sensors.getTempCByIndex(0);

  // -------- Heart Rate Simulation --------
  if (digitalRead(HEART_BUTTON_PIN) == LOW)
  {
    heartRate = 120;
  }
  else
  {
    heartRate = 75;
  }

  // -------- MPU6050 --------
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  totalAccel =
      sqrt(
        a.acceleration.x * a.acceleration.x +
        a.acceleration.y * a.acceleration.y +
        a.acceleration.z * a.acceleration.z);

  fallDetected = 0;

  if (totalAccel > 25.0)
  {
    fallDetected = 1;
    Serial.println("⚠ FALL DETECTED");
  }

  // -------- Battery Percentage --------
  int batteryValue = analogRead(BATTERY_PIN);

  batteryPercentage = map(
      batteryValue,
      0,
      4095,
      0,
      100);

  batteryPercentage =
      constrain(
          batteryPercentage,
          0,
          100);

  // -------- Health Risk --------
  healthRisk = 0;

  if (heartRate > 100 ||
      bodyTemperature > 38.0 ||
      fallDetected == 1)
  {
    healthRisk = 1;
  }
    //==============================
  // Battery Low BUZZER
  //==============================


if (batteryPercentage <= 20)
{
  tone(BUZZER_PIN,1000);

  if (!batteryAlertSent)
  {
    String msg =
    "🔋 LOW BATTERY ALERT!\n"
    "Battery Level : " + String(batteryPercentage) + "%";

    sendTelegram(msg);

    batteryAlertSent = true;
  }
}
else
{
  noTone(BUZZER_PIN);
  batteryAlertSent = false;
}

  //==============================
  // SOS Button
  //==============================

  if (digitalRead(SOS_BUTTON_PIN) == LOW)
  {
    if (!sosAlertSent)
    {
      String msg =
      "🆘 SOS EMERGENCY!\n"
      "Patient pressed the SOS button.\n"
      "Immediate assistance required.";

      sendTelegram(msg);

      sosAlertSent = true;
    }
  }
  else
  {
    sosAlertSent = false;
  }

  //==============================
  // Health Emergency Alert
  //==============================

  if (healthRisk == 1)
  {
    if (!healthAlertSent)
    {
      String msg =
      "🚨 HEALTH ALERT!\n"
      "Heart Rate : " + String(heartRate) + " BPM\n" +
      "Temperature : " + String(bodyTemperature) + " °C\n" +
      "Fall : " + String(fallDetected);

      sendTelegram(msg);

      healthAlertSent = true;
    }
  }
  else
  {
    healthAlertSent = false;
  }

  //==============================
  // Serial Monitor
  //==============================

  Serial.println("--------------------------");
  Serial.print("Heart Rate : ");
  Serial.println(heartRate);

  Serial.print("Temperature : ");
  Serial.println(bodyTemperature);

  Serial.print("Fall : ");
  Serial.println(fallDetected);

  Serial.print("Health Risk : ");
  Serial.println(healthRisk);

  Serial.print("Battery : ");
  Serial.print(batteryPercentage);
  Serial.println("%");
    //==============================
  // ThingSpeak Upload
  //==============================

  // Correct Field Mapping
  // Field 1 -> Heart Rate
  // Field 2 -> Health Risk
  // Field 3 -> Fall Detection
  // Field 4 -> Body Temperature

  ThingSpeak.setField(1, heartRate);
  ThingSpeak.setField(2, healthRisk);
  ThingSpeak.setField(3, fallDetected);
  ThingSpeak.setField(4, bodyTemperature);

  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (httpCode == 200)
  {
    Serial.println("ThingSpeak Updated Successfully.");
  }
  else
  {
    Serial.print("ThingSpeak Error : ");
    Serial.println(httpCode);
  }

  Serial.println("--------------------------");

  delay(15000);

}
