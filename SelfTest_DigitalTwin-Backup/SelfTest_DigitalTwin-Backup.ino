

#include <Wire.h>

#define SELF_TEST_X      0x0D
#define SELF_TEST_Y      0x0E
#define SELF_TEST_Z      0x0F
#define SELF_TEST_A      0x10
#define ACCEL_CONFIG     0x1C
#define GYRO_CONFIG      0x1B
#define WHO_AM_I_MPU6050 0x75 // Deve retornar 0x68




// Usando a placa de breakout GY-521, defino ADO para 0 aterrando através de um resistor de 4k7
// O endereço do dispositivo de sete bits é 110100 para ADO = 0 e 110101 para ADO = 1
#define ADO 0
#if ADO
#define MPU6050_ADDRESS 0x69  // Endereço do dispositivo quando ADO = 1
#else
#define MPU6050_ADDRESS 0x68  // Endereço do dispositivo quando ADO = 0
#endif



int intPin = 12;  // Isso pode ser alterado, 2 e 3 são os pinos ext int do Arduino


float SelfTest[6];               // Saída do sensor de autoteste do giroscópio e do acelerômetro
uint32_t count = 0;

void setup()
{
  Wire.begin();
  Serial.begin(38400);

  // Configura o pino de interrupção, é definido como ativo alto, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);



}

void loop()
{
  // Leia o registro WHO_AM_I, este é um bom teste de comunicação
  uint8_t c = readByte(MPU6050_ADDRESS, WHO_AM_I_MPU6050); // Leia o registro WHO_AM_I para MPU-6050
  byte  error;
  int nDevices;

  nDevices = 0;
  if (c == 0x68) // WHO_AM_I deve ser sempre 0x68
  {
    Serial.println("MPU6050 está online...");
    nDevices++;

    MPU6050SelfTest(SelfTest); // Comece realizando autoteste e relatando valores
    Serial.print("Autoteste do eixo x: ajuste de aceleração dentro : "); Serial.print(SelfTest[0], 1); Serial.println("% de valor de fábrica");
    Serial.print("Autoteste do eixo y: ajuste de aceleração dentro de: "); Serial.print(SelfTest[1], 1); Serial.println("% de valor de fábrica");
    Serial.print("Autoteste do eixo z: ajuste de aceleração dentro de: "); Serial.print(SelfTest[2], 1); Serial.println("% de valor de fábrica");


    if (SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
      Serial.print("Passou no Autoteste!");
      Serial.println("");
      delay(1000);
    }
    else if (nDevices == 0)
    {
      Serial.print("Não foi possível conectar ao MPU6050: 0x");
      Serial.println(c, HEX);
      while (1) ;   // Loop para sempre se a comunicação não acontecer


    }

  }
}
