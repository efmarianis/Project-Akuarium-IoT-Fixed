#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//#define BLYNK_AUTH_TOKEN "-EwkjYaZzBuXlm3BHrGd215TQux9OGLJ"
#define BLYNK_AUTH_TOKEN "Y2aKl5agDy-3yj3uJwT8KCh-M19Qnau_"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h>

BlynkTimer timer;
SoftwareSerial dataSerial(D8, D7);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "right";//Enter your WIFI name
char pass[] = "thatsright50";//Enter your WIFI password

// Motor Servo
Servo servo;

//unsigned long previousMillis = 0; // millis permintaan data ke arduino uno
//const long interval = 1000;
String arrData[7];

// millis untuk pemberian pakan ikan
unsigned long previousMillisPakan = 0;
const long intervalPakan = 59000; // 59 detik

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dataSerial.begin(9600);
  
  servo.write(0);
  timer.setInterval (1000L, send_sensor); // time interval pemanggilan waktu pakan
  Blynk.begin(auth, ssid, pass, "prakitblog.com", 8181);
}


void send_sensor() {

  String data = "";
  while (dataSerial.available() > 0) {
    char d = dataSerial.read();
    data += d;
  }

  data.trim();

  if (data != "") {
    int index = 0;
    for (int i = 0; i <= data.length(); i++) {
      char delimeter = '#';
      if (data[i] != delimeter)
        arrData[index] += data[i];
      else
        index++;
    }

    if (index == 5) {
      Serial.println("Suhu : " + arrData[0] + "C");
      Serial.println("Jarak Pakan : " + arrData[1] + "cm");
      Serial.println("Waktu Pakan : " + arrData[2]);
      Serial.println("Ketinggian Air : " + arrData[3] + "cm");
      Serial.println("Tegangan Turbidity : " + arrData[4] + "Volt");
      Serial.println("Kekeruhan Air : " + arrData[5] + "NTU");
      Serial.println("--------------------------------------");
      Blynk.virtualWrite(V0, arrData[0]); //menampilkan suhu pada blynk
      Blynk.virtualWrite(V10, arrData[0]); //menampilkan suhu air pada blynk chart
      Blynk.virtualWrite(V2, arrData[1]); //menampilkan jarak pakan pada blynk
      Blynk.virtualWrite(V8, arrData[1]); //menampilkan jarak pakan pada blynk chart
      Blynk.virtualWrite(V3, arrData[2]); // menampilkan waktu
      Blynk.virtualWrite(V1, arrData[3]); //menampilkan ketinggian air pada blynk
      Blynk.virtualWrite(V7, arrData[3]); //menampilkan ketinggian air pada blynk chart
      Blynk.virtualWrite(V5, arrData[5]); // menampilkan tegangan turbidity
      Blynk.virtualWrite(V9, arrData[5]); //menampilkan kekeruhan air pada blynk chart

      //Pemberian Pakan dan notifikasi ketersedian pakan
      int jarakPakan = arrData[1].toInt();
      String waktuPakan = arrData[2];
      if (jarakPakan > 7) {
        Blynk.notify("Pakan Habis!!!");
      }
      if ((waktuPakan == "2:10" && jarakPakan <= 7) || (waktuPakan == "2:12" && jarakPakan <= 7)) {
        pakan();
        Blynk.notify("Pakan telah diberikan!!!");
      }

    }

    arrData[0] = "";
    arrData[1] = "";
    arrData[2] = "";
    arrData[3] = "";
    arrData[4] = "";
    arrData[5] = "";
  }
  dataSerial.println("Ya");

}

BLYNK_WRITE (V4) { 
  servo.attach(D3);
  servo.write(param.asInt());
}

void pakan() {
  unsigned long currentMillisPakan = millis();
  if (currentMillisPakan - previousMillisPakan >= intervalPakan) {
    previousMillisPakan = currentMillisPakan;
    servo.attach(D3);
    servo.write(0);
    delay(25);
    servo.write(45);
    delay(700);
    servo.write(0);
    delay(1000);
    servo.detach();
    delay(1000);
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
