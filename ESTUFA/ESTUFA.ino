#include <DHT.h>;

#define DHTPIN 7
#define DHTTYPE DHT22

int rele1 = 14;
int rele2 = 15;
int rele3 = 16;
int rele4 = 17;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(rele4, OUTPUT);



  dht.begin();
  delay(2000);
}
void loop() {
  Serial.print("Umidade: ");
  Serial.print(dht.readHumidity());
  Serial.print("%");
  Serial.print(" / Temperatura: ");
  Serial.print(dht.readTemperature(), 0);
  Serial.println("*C");
  //delay(2000);


  //digitalWrite(rele1, HIGH);
  //digitalWrite(rele2, HIGH);
  // digitalWrite(rele3, HIGH);
  //digitalWrite(rele4, !HIGH); //ventilador


  /*
     10-13 %
    O valor ideal da humidade em um filamento é entre 10-13 %.  */
  if (dht.readHumidity() < 10 or dht.readTemperature() > 75 ) 
      digitalWrite(rele4, HIGH)
    , digitalWrite(rele1, LOW)
    , digitalWrite(rele2, LOW)
    , digitalWrite(rele3, LOW);
  if (dht.readHumidity() > 10 or dht.readTemperature() < 60 ) 
      digitalWrite(rele4, LOW)
    , digitalWrite(rele1, HIGH)
    , digitalWrite(rele2, HIGH)
    , digitalWrite(rele3, HIGH);

}
