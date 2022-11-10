#include <SoftwareSerial.h>
SoftwareSerial dataSerial(2, 3);
#include <DS3231.h>

// Sensor DS18b20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
float suhuDS18B20;

// sensor ultrasonik jarak pakan
#define trigPin 5
#define echoPin 6
long distance, duration;

// sensor ultrasonik ketinggian air
#define trigPin2 7
#define echoPin2 8
long duration_2, distance_2;

// sensor turbidity
int pinTurb = A1;
float V, kekeruhan, Vrata2, Vhasil;

// rtc
DS3231  rtc(SDA, SCL);
Time t;
String jam, menit, waktu;

// relay
const int relay1 = 9;
const int relay2 = 10;
const int relay3 = 11;
const int relay4 = 12;

int relayON = LOW; //relay nyala
int relayOFF = HIGH; //relay mati

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dataSerial.begin(9600);
  sensor.begin();      //Menginisialisasikan sensor One-Wire DS18B20
  rtc.begin();
   //   rtc.setDate(21, 7, 2022);   //mensetting tanggal 29 maret 2020
   //   rtc.setDOW(5);              //menset hari "Minggu"
      //rtc.setTime(19, 33, 50);    //menset jam 13:02:40

  pinMode(trigPin, OUTPUT); // ultrasonik pakan
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT); // ultrasonik ketinggian air
  pinMode(echoPin2, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay1, relayOFF);
  digitalWrite(relay2, relayOFF);
  digitalWrite(relay3, relayOFF);
  digitalWrite(relay4, relayOFF);
}

void loop() {
  // put your main code here, to run repeatedly:
  String minta = "";
  while (dataSerial.available() > 0) {
    char c = dataSerial.read();
    minta += c;
  }

  minta.trim();

  if (minta == "Ya") {
    kirim_data();
  }
  minta = "";
  delay(1000);
}

void kirim_data() {
  // KENDALI SUHU AIR AKUARIUM
  // sensor ds18b20
  sensor.setResolution(9);  // Sebelum melakukan pengukuran, atur resolusinya
  sensor.requestTemperatures();  // Perintah konversi suhu
  suhuDS18B20 = sensor.getTempCByIndex(0);  //Membaca data suhu dari sensor dan mengkonversikannya ke nilai Celsius

  // KENDALI SUHU AIR
  if (suhuDS18B20 < 24) {
    digitalWrite(relay3, relayON);
    digitalWrite(relay4, relayOFF);
  }
  else if (suhuDS18B20 > 28) {
    digitalWrite(relay4, relayON);
    digitalWrite(relay3, relayOFF);
  }
  else {
    digitalWrite(relay3, relayOFF);
    digitalWrite(relay4, relayOFF);
  }

  // sensor ultrasonik pakan
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Waktu RTC
  t = rtc.getTime();
  jam = t.hour;
  menit = t.min;
  waktu = jam + ":" + menit;

  // sensor ultrasonik ketinggian air
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration_2 = pulseIn(echoPin2, HIGH);
  distance_2 = duration_2 * 0.034 / 2;

  // KENDALI KETinggian AIR
  if (distance_2 <= 7) {
    digitalWrite(relay2, relayOFF);
  }
  else if (distance_2 >= 20) {
    digitalWrite(relay2, relayON);
    digitalWrite(relay1, relayOFF);
  } 

  V = 0;
  for(int i=0; i<800; i++){
    V += ((float)analogRead(pinTurb)/1023)*5;
  }
  
  Vrata2 = V/800;
  Vhasil = roundf(Vrata2*10.0f)/10.0f;

  if(Vhasil < 2.5)
  {
    kekeruhan = 30;
  }
  else
  {
    kekeruhan = (-1120.4*square(Vhasil)+5742.3*Vhasil-4353.8)/100;
  }
  
  if (kekeruhan > 25 && distance_2 < 7) {
    digitalWrite(relay1, relayON);
  } 
  else if (kekeruhan < 25) {
    digitalWrite(relay1, relayOFF);
  }

  // sediakan variabel penampung data yg akan dikirim
  String datakirim = String (suhuDS18B20) + "#" + String (distance) + "#" + String (waktu) + "#" + String (distance_2) + "#" + String (Vhasil) + "#" + String (kekeruhan);
  dataSerial.println(datakirim);
}
