#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Pustakadigital.id";
const char* password = "12345678";
bool wifiConnected = false;
const int voltagePin = 32; // GPIO pin sensor tegangan
const float voltageDividerRatio = 4.0; // Rasio pembagi tegangan


void setup() {
  Serial.begin(115200);
  pinMode(voltagePin, INPUT);
  connectToWiFi();
}

void loop() {
  if (wifiConnected) {
    // Membaca nilai tegangan dari sensor
    float voltage = readVoltage();
      delay(1000); // Tunggu 5 detik sebelum mengirim permintaan GET selanjutnya

    Serial.println("up normal");


    if (voltage <= 9.0) { // Treshold tegangan yang disesuaikan dengan pembagi tegangan
      
      performGetRequest(); // Melakukan permintaan GET ke server hanya jika tidak ada daya masuk
      Serial.println("shutdown");

      delay(1000); // Tunggu 5 detik sebelum mengirim permintaan GET selanjutnya
    }
  } else {
    connectToWiFi();
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  wifiConnected = true;
  Serial.println("Connected to WiFi");
}

float readVoltage() {
  int sensorValue = analogRead(voltagePin);
  int remap = map(sensorValue,0,4095,4095,0);
  float voltage = (remap * (3.3 / 4095.0)) * voltageDividerRatio; // Mengubah nilai analog menjadi tegangan dan memperhitungkan pembagi tegangan

  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("sensorValue: ");
  Serial.println(sensorValue);

  return voltage;
}

void performGetRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Data yang akan dikirim sebagai parameter dalam permintaan GET
    String getData = "pwd=123&cmd=shutdown%2Fs"; // Sesuaikan dengan data yang ingin Anda kirim

    String url = "http://localhost/exec.php?" + getData; // Ganti dengan URL server, end point, dan parameter yang sesuai

    http.begin(url);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      wifiConnected = false; // Tandai bahwa koneksi terputus
    }

    http.end();
  } else {
    wifiConnected = false; // Tandai bahwa koneksi terputus
  }
}
