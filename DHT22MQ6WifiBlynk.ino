#include <Wire.h>
#include <WiFi.h>

#define DHT22_PIN 14
#define BLYNK_TEMPLATE_ID "TMPL6EDI2j2A4"
#define BLYNK_TEMPLATE_NAME "co2"
#define BLYNK_AUTH_TOKEN "RjqsfquZCIia8I99LZq4kNELIYa3KigH"

#include <BlynkSimpleEsp32.h>
#include <DHT.h>

int LED = 32;
int MQSensor_input = 35;
DHT dht22(DHT22_PIN, DHT22);

char ssid[] = "B10-510 4G";
char pass[] = "20210531";

void setup()
{
  Serial.begin(9600);
  delay(10);

  // Kết nối Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  pinMode(LED, OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht22.begin();
}

void dht() {
  // Đọc giá trị từ cảm biến DHT22
  float humi = dht22.readHumidity();
  float tempC = dht22.readTemperature();
  float tempF = dht22.readTemperature(true);

  // Kiểm tra lỗi đọc từ cảm biến DHT22
  if (isnan(tempC) || isnan(tempF) || isnan(humi)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }

  // Gửi giá trị nhiệt độ và độ ẩm lên Blynk
  Blynk.virtualWrite(V3, humi);
  Blynk.virtualWrite(V2, tempC);

  // In thông tin nhiệt độ và độ ẩm lên Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humi);
  Serial.print("%, Temperature: ");
  Serial.print(tempC);
  Serial.print("°C ~ ");
  Serial.print(tempF);
  Serial.print("°F | ");
}

void loop()
{
  // Đọc giá trị từ cảm biến MQ2
  int MQsensor_Aout = analogRead(MQSensor_input);
  
  // Gửi giá trị MQ2 lên Blynk
  Blynk.virtualWrite(V0, MQsensor_Aout);

  // Kiểm tra nồng độ khí gas
  if (MQsensor_Aout > 1800) {
    Serial.print("Gas Detected! | ");
    digitalWrite(LED, HIGH);  // Bật LED khi có gas
    Blynk.virtualWrite(V1, 1);  // Thông báo có gas trên Blynk
  } else {
    Serial.print("No Gas | ");
    digitalWrite(LED, LOW);  // Tắt LED khi không có gas
    Blynk.virtualWrite(V1, 0);  // Thông báo không có gas trên Blynk
  }

  // In ra các giá trị và trạng thái lên Serial Monitor
  dht();
  Serial.print("Gas Sensor: ");
  Serial.println(MQsensor_Aout);

  delay(5000);  // Đợi 5 giây trước khi đọc lại
  Blynk.run();  // Chạy Blynk để giữ kết nối
}
