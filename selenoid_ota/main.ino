#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

unsigned long ms_from_start =0;
unsigned long ms_from_read =0;
unsigned long ms_from_next = 5000;
// unsigned long ms_delay_pc_boot_up = 900 * 1000;
// unsigned long ms_from_next = 5000;


// unsigned long ms_booting_up_start =0;
// unsigned long ms_booting_up_read =0;

#define voltage_pin 32    // Voltage Sensor
#define relay_pin_sl 16   // ESP32 pin GIOP16 connected to the IN pin of Relay Selenoid
#define relay_pin_ups 17  // ESP32 pin GIOP17 connected to the IN pin of Relay UPS
// unsigned long ms_from_start =0;

float adc_voltage = 0.0;      // adc voltage
float in_voltage = 0.0;       // input voltage
int adc_value = 0;            //adc_value
int thresholdVolt = 8;        // batas tegangan
const char* mode = "PC_OFF";  // status mini pc

const char* host = "esp32";
const char* ssid = "Ihub61";
const char* password = "Ihub2019";

const IPAddress staticIP(192, 168, 1, 121);  // Set your desired static IP address
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);


WebServer server(80);

/*
 * Login page
 */

const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='antasari61')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * Server Index Page
 */

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

/*
 * setup function
 */
void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
    pinMode(relay_pin_sl, OUTPUT);
    pinMode(relay_pin_ups, OUTPUT);
    digitalWrite(relay_pin_sl, HIGH); 
    digitalWrite( relay_pin_ups, HIGH);

}

void loop() {
  // delay(1);                   // wait for a second
  ms_from_start = millis();
    digitalWrite(2, HIGH);

  // Serial.println(ms_from_start);
  if(ms_from_start-ms_from_read > ms_from_next){
      digitalWrite(2, LOW);

      ms_from_read = ms_from_start;
      adc_value = analogRead(voltage_pin);
      adc_voltage = adc_value * (12 / 2900.0);

      // Tampilkan Voltase Analog
      // Serial.print("Input Voltage Analog = ");
      // Serial.println(adc_value);

      // Tampilkan Voltase
      Serial.print("Input Voltage = ");
      Serial.println(adc_voltage, 2);

      // Tampilkan Mode
      Serial.print("Mode = ");
      Serial.println(mode);

      if (mode == "PC_OFF") 
      {
        if (adc_voltage > thresholdVolt) 
        {
          
          power_up_pc();
          
        }
      }

      // if the analog value is high enough, turn on the LED:
      if (mode == "PC_ON") 
      {
          

          if(WiFi.status() != WL_CONNECTED)
          {
            OTA();
          }
            
            server.handleClient();
        if (adc_voltage < thresholdVolt) 
        {
          // short Delay (2s)
          power_down_pc();
        }
      }

  }
}



void power_up_pc(){
          // Wait For PC and UPS to Power Up Properly
          delay(3000);
          // Menyalakan UPS
          Serial.println("Menyalakan UPS");
          digitalWrite(relay_pin_ups, LOW);
          Serial.println("Power Up");
          Serial.println("press");
          digitalWrite(relay_pin_sl, LOW);
          delay(500);
          Serial.println("release");
          digitalWrite(relay_pin_sl, HIGH);
          Serial.println("Mini PC Booting Up");
          // Delay Booting Up Mini PC, Sebelum Membaca sensor Tegangan kembali (1.5 Min)
          mode = "PC_ON";
          delay(90000UL);

}

void power_down_pc(){
    delay(2000);
    Serial.println("shutdown");
    Serial.println("press");
    digitalWrite(relay_pin_sl, LOW);
    delay(500);
    Serial.println("release");
    digitalWrite(relay_pin_sl, HIGH);
    Serial.println("Mini PC Shutdown");
    // Delay Shutdown Mini PC, Sebelum Membaca sensor Tegangan kembali (30 sec)
    mode = "PC_OFF";
    delay(30000UL);

    // Mematikan UPS
    Serial.println("Mematikan UPS");
    digitalWrite(relay_pin_ups, HIGH);
}

void OTA(){
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("Failed to configure static IP");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}
