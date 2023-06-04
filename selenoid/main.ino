#include <Ticker.h> 
Ticker ticker1; 
Ticker ticker2; 
Ticker ticker3; 

const int sensorPin = 15; // Analog pin connected to the voltage divider output
const int relay_battery = 2; 
const int relay_selenoid = 4; 
const float voltageRef = 3.3; // Reference voltage (in volts)
const float voltageDividerRatio = 4.0; // Voltage divider ratio (R2 / (R1 + R2))
float read_volt;
const int delay_pc_mati = 10 * 1000; //1 = menit

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(relay_battery, OUTPUT);
  pinMode(relay_selenoid, OUTPUT);




  digitalWrite(relay_selenoid, HIGH);
  // digitalWrite(relay_battery, LOW);
  delay(1000);
  digitalWrite(relay_selenoid, LOW);
  digitalWrite(relay_battery, HIGH);
  ticker1.attach(1, utama);
  

}

void loop() {
  

}


void utama(){
  read_volt = volt_sensor();
  Serial.print("Voltage: ");
  Serial.print(read_volt);
  Serial.println(" V");

  if(read_volt <= 9.00){
  Serial.println("Daya Kurang ");
  // relay_control(relay_selenoid,HIGH);
  // delay(500);
  // relay_control(relay_selenoid,LOW);
  ticker3.attach(1,click_relay,relay_selenoid);
  // performTask();
  ticker2.attach(1, proses_mati_pc);
  ticker1.detach();




  }
}

float volt_sensor(){
  int sensorValue = analogRead(sensorPin); // Read analog value from voltage divider
  float voltage = (sensorValue * voltageRef) / 4095.0; // Calculate voltage
  
  // Scale the voltage to the actual input voltage based on the voltage divider ratio
  float actualVoltage = voltage * voltageDividerRatio;
  
  return actualVoltage;
}


void relay_control(const int pin,int value){
  digitalWrite(pin,value);
}


void proses_mati_pc() {
  // Get the current time
  unsigned long currentMillis = millis();
    Serial.println("proccess pc mati");

  
  // Do something based on the current time
  if (currentMillis > delay_pc_mati) {
    // Do something for the first 5 seconds
    relay_control(relay_battery,LOW);
    Serial.println("pc mati");
    ticker2.detach();


  }else{
    Serial.println(currentMillis);

  }
}


void click_relay(const int relay_pin){
  unsigned long currentMillis2 = millis();
    Serial.println("proccess click relay");
    relay_control(relay_pin,HIGH);


  
  // Do something based on the current time
  if (currentMillis2 > 6000) {
    // Do something for the first 5 seconds
    relay_control(relay_pin,LOW);
    Serial.println("relay telah diclick");
    ticker3.detach();


  }else{
    Serial.print("milis 2 : ");
    Serial.println(currentMillis2);

  }
}