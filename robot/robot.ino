
#include<Wire.h>
#include <Ultrasonic.h>
const int MPU = 0x68;
int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

#define pino_trigger 8
#define pino_echo 7

Ultrasonic ultrasonic(pino_trigger, pino_echo);

int E1 = 3;
int M1 = 12;
int E2 = 11;
int M2 = 13;

void setup()
{


  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);

  Wire.write(0);
  Wire.endTransmission(true);

}

void loop()
{
  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);
  //Exibe informacoes no serial monitor
  Serial.print("Distancia em cm: ");
  Serial.print(cmMsec);
  Serial.print(" - Distancia em polegadas: ");
  Serial.println(inMsec);


  if (cmMsec < 30) {
    digitalWrite(M1, LOW);
    digitalWrite(M2, LOW);
    analogWrite(E1, 120);   //PWM regulate speed
    analogWrite(E2, 120);   //PWM regulate speed
    delay(200);
    digitalWrite(M1, !LOW);
    digitalWrite(M2, LOW);
    analogWrite(E1, 120);   //PWM regulate speed
    analogWrite(E2, 120);   //PWM regulate speed
    delay(500);
  }
  else {
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 130);   // PWM regulate speed
    analogWrite(E2, 130);   // PWM regulate speed
  }

}
