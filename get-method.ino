#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "nama_jaringan_wifi";
const char* password = "kata_sandi_wifi";
bool wifiConnected = false;
const int voltagePin = 32; // GPIO pin sensor tegangan

void setup() {
  Serial.begin(115200);
  pinMode(voltagePin, INPUT);
  connectToWiFi();
}

void loop() {
  if (wifiConnected) {
    // Membaca nilai tegangan dari sensor
    float voltage = readVoltage();

    if (voltage <= 0.1) { //0.1 adalah nilai analog menjadi tegangan (GPIO ESP32 menggunakan tegangan referensi 3.3V)
      // Melakukan permintaan GET ke server hanya jika tidak ada daya masuk
      performGetRequest();
      // delay(1000); // Tunggu 5 detik sebelum mengirim permintaan GET selanjutnya
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
  float voltage = sensorValue * (3.3 / 4095.0); // Mengubah nilai analog menjadi tegangan (GPIO ESP32 menggunakan tegangan referensi 3.3V)

  Serial.print("Voltage: ");
  Serial.println(voltage);

  return voltage;
}

void performGetRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Data yang akan dikirim sebagai parameter dalam permintaan GET
    //String getData = "key1=value1&key2=value2"; // Sesuaikan dengan data yang ingin Anda kirim

    String url = "http://alamat_server.com/shutdown.php"; // + getData; // Ganti dengan URL server, end point, dan parameter yang sesuai

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
