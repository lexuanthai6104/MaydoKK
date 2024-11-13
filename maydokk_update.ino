//doneeee

#define BLYNK_TEMPLATE_ID "TMPL6t8KRNLZc"
#define BLYNK_TEMPLATE_NAME "maydokk"
#define BLYNK_AUTH_TOKEN "JR-Wm73cSozv6VU4gPrjNnO6ELKYP07y"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFiManager.h>  // Thêm WiFiManager
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <MQ135.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <AHT20.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
AHT20 aht20;

//----------------------------------------
#define PIN_MQ135 39
#define RainPin 32
#define WindPin 27
// char ssid[100];
// char pass[100];

#define LATITUDE "21.0093357"    // Ex: vĩ độ
#define LONGITUDE "105.7923755"  // Ex: kinh độ
#define API_KEY "9781d079b7e7cb9cc381c1c153487d69"
unsigned long lastTime = 0;
#define NAME_URL "http://api.openweathermap.org/data/2.5/weather"
#define PARAM_LAT "?lat="  //vĩ độ
#define PARAM_LON "&lon="  //kinh độ
#define PARAM_API "&appid="
#define PARAM_EXCLUDE "&exclude=hourly,daily"
#define PARAM_UNIT "&units=metric"
String URL = String(NAME_URL) + String(PARAM_LAT) + String(LATITUDE) + String(PARAM_LON) + String(LONGITUDE) + String(PARAM_UNIT) + String(PARAM_EXCLUDE) + String(PARAM_API) + String(API_KEY);
MQ135 mq135_sensor(PIN_MQ135);

String Feellike, Hummain, Tempmain, Weatherdes, Weathermain;
float temperature, humidity, AQI, Gio_val;
float Windspeed, Cloud, WindDir;
volatile int pulseCount = 0;
unsigned long lastCalWindSpeed = 0;
const float bladeCircumference = 2 * 3.14 * 0.09;
bool wasRainy = false;  // Biến theo dõi trạng thái thời tiết trước đó
bool isRain;

// JSON
String jsonBuffer;
JSONVar myObject;

WiFiManager wm;  // Tạo đối tượng WiFiManager

void setup() {
  Serial.begin(115200);

  pinMode(RainPin, INPUT);

  pinMode(WindPin, INPUT_PULLUP);
  attachInterrupt(WindPin, countPulse, RISING);

  Wire.begin();
  aht20.begin();
  bool res = wm.autoConnect("AutoConnectAP", "12345678");

  // Sử dụng WiFiManager để kết nối với Wi-Fi
  if (!res) {
    Serial.println("Failed to connect and hit timeout");
    ESP.restart();  // Khởi động lại nếu không kết nối thành công
  } else {
    Serial.println("Connected to Wi-Fi successfully");
  }
  String ssid = WiFi.SSID();  // Lấy SSID của mạng Wi-Fi
  String pass = WiFi.psk();

  // Kết nối Blynk sau khi Wi-Fi được kết nối thành công
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid.c_str(), pass.c_str());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    readSensorData();  // Đọc dữ liệu từ các cảm biến và gửi lên Blynk
  }
  delay(1000);

  Blynk.run();
  delay(1000);  // Chạy Blynk để xử lý các yêu cầu từ máy chủ
}

void readSensorData() {
  unsigned long currentTime = millis();
  if (currentTime - lastCalWindSpeed > 1000) {
    float rpm = (pulseCount / 20.0) * 60.0;
    Windspeed = (rpm * bladeCircumference) / 60.0;
    pulseCount = 0;
    lastCalWindSpeed = currentTime;
  }

  if (aht20.available() == true) {  // Đọc nhiệt độ và độ ẩm
    temperature = aht20.getTemperature();
    humidity = aht20.getHumidity();
  }
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from AHT20 sensor!"));
    return;
  }

  // Đọc dữ liệu từ cảm biến MQ135
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
  float a = 1.2, b = 0.1, c = 0.05, d = 25;
  AQI = a * correctedPPM + b * temperature + c * humidity + d;

  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("ppm\t Corrected PPM: ");
  Serial.println(correctedPPM);

  bool isRainSensor = digitalRead(RainPin);  // Giả định HIGH = không có mưa, LOW = có mưa


  if (WiFi.status() == WL_CONNECTED) {
    jsonBuffer = httpGETRequest(URL.c_str());
    myObject = JSON.parse(jsonBuffer);

    if (JSON.typeof(myObject) == "undefined") {
      Serial.println(F("Parsing input failed!"));
      return;
    }

    Weathermain = String(myObject["weather"][0]["main"]);
    Weatherdes = String(myObject["weather"][0]["description"]);
    // Tempmain = double(myObject["main"]["temp"]);
    // Hummain = double(myObject["main"]["humidity"]);
    // Feellike = double(myObject["main"]["feels_like"]);
    WindDir = double(myObject["wind"]["deg"]);
    Cloud = double(myObject["clouds"]["all"]);

    if (Weathermain == "Rain") {
      isRain = true;
    } else {
      isRain = false;
    }
    checkWeatherChange();
    // Gửi dữ liệu lên Blynk
    Blynk.virtualWrite(V0, Windspeed);
    Blynk.virtualWrite(V1, AQI);
    Blynk.virtualWrite(V2, temperature);
    Blynk.virtualWrite(V3, humidity);
    Blynk.virtualWrite(V4, "->Weather main: " + Weathermain + "\n->Description: " + Weatherdes + "\n \n");
    Blynk.virtualWrite(V5, isRainSensor);
    Blynk.virtualWrite(V6, WindDir);
    Blynk.virtualWrite(V7, Cloud);
    Blynk.virtualWrite(V8, isRain);
  } else {
    Serial.println(F("WiFi Disconnected"));
  }
}

void checkWeatherChange() {
  isRain = (Weathermain == "Rain");

  if (!wasRainy && isRain) {
    // Chuyển từ nắng sang mưa
    Blynk.logEvent("alert", "Sunny to Rain, be careful!!");
    // Thực hiện hành động khi chuyển từ nắng sang mưa (ví dụ: gửi thông báo)
  } else if (wasRainy && !isRain) {
    // Chuyển từ mưa sang nắng
    Blynk.logEvent("alert", "Rain to Sunny, be funn!!");
    // Thực hiện hành động khi chuyển từ mưa sang nắng
  }

  // Cập nhật trạng thái thời tiết trước đó
  wasRainy = isRain;
}

String httpGETRequest(const char *serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  int httpResponseCode = http.GET();
  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print(F("HTTP Response code: "));
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print(F("Error code: "));
    Serial.println(httpResponseCode);
  }

  http.end();
  return payload;
}

void countPulse() {
  pulseCount++;
}
