

#include <Wire.h>
const int MPU6050_ADDRESS = 0x68;

#define SELF_TEST_X      0x0D
#define SELF_TEST_Y      0x0E
#define SELF_TEST_Z      0x0F
#define SELF_TEST_A      0x10
#define ACCEL_CONFIG     0x1C
#define GYRO_CONFIG      0x1B
#define WHO_AM_I_MPU6050 0x75 // Deve retornar 0x68



int intPin = 12;  // Isso pode ser alterado, 2 e 3 são os pinos ext int do Arduino


float SelfTest[6];               // Saída do sensor de autoteste do giroscópio e do acelerômetro
uint32_t count = 0;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);

  Serial.begin(38400);
  while (!Serial);
  Serial.println("\nI2C Scanner");
}

void loop() {
  int nDevices = 0;
  int nDevicesSoma = 3;
  Serial.println("Procurando...");


  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo I2C encontrado 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");

      ++nDevices;
    } else if (error == 4) {
      Serial.print("Erro desconhecido no endereço 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("Nenhum dispositivo I2C\n");
    delay(1000);
  //  asm volatile ("jmp 0"); // Função de Reset do Arduino
    
  } else {
    Serial.println("Feito!!\n");
    //Serial.println(nDevices);
    //Serial.print(anddress);

    nDevices = nDevices + nDevicesSoma;
    //Serial.println(nDevices);
  }
  delay(1000);
  do {
    if (nDevices == 4) {
      Wire.beginTransmission(MPU6050_ADDRESS);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU6050_ADDRESS, 14, true);
      AcX = Wire.read() << 8 | Wire.read();
      AcY = Wire.read() << 8 | Wire.read();
      AcZ = Wire.read() << 8 | Wire.read();
      Tmp = Wire.read() << 8 | Wire.read();


      uint8_t c = readByte(MPU6050_ADDRESS, WHO_AM_I_MPU6050); // Leia o registro WHO_AM_I para MPU-6050
      byte  error;
      int nDevices;

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
          //while(1);
        }
  /*      else if (nDevices == 0)
        {
          Serial.print("Não foi possível conectar ao MPU6050: 0x");
          Serial.println(c, HEX);
          while (1) ;   // Loop para sempre se a comunicação não acontecer


        }
*/
      }
    }
  }
  while (AcX != -1 && AcY != -1 && AcZ != -1);
}




void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);  // Initialize the Tx buffer
  Wire.write(subAddress);           // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
}



uint8_t readByte(uint8_t address, uint8_t subAddress)
{
  uint8_t data;// `data` irá armazenar os dados do registrador
  Wire.beginTransmission(address);         // Inicializa o buffer Tx
  Wire.write(subAddress);                  // Coloca o endereço do registrador escravo no buffer Tx
  Wire.endTransmission(false);             // Envia o buffer Tx, mas envia uma reinicialização para manter a conexão ativa
  Wire.requestFrom(address, (uint8_t) 1);  // Lê um byte do endereço do registrador escravo
  data = Wire.read();                      // Preencha o buffer Rx com o resultado
  return data;                             // Retorna os dados lidos do registrador
}


void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{
  Wire.beginTransmission(address);   // Inicializa o buffer Tx
  Wire.write(subAddress);            // Coloca o endereço do subregistrador no buffer Tx
  Wire.endTransmission(false);       // Envia o buffer Tx, mas envia uma reinicialização para manter a conexão ativa
  uint8_t i = 0;
  Wire.requestFrom(address, count);  // Lê os bytes do endereço do registrador contador
  while (Wire.available()) {
    dest[i++] = Wire.read();
  }         // Coloca os resultados da leitura no buffer Rx
}



void MPU6050SelfTest(float * destination) // Deve retornar o desvio percentual dos valores de ajuste de fábrica, +/- 14 ou menos desvio é uma aprovação
{
  uint8_t rawData[4];
  uint8_t selfTest[6];
  float factoryTrim[6];

  // Configura o acelerômetro para autoteste
  writeByte(MPU6050_ADDRESS, ACCEL_CONFIG, 0xF0); // Ativa o autoteste em todos os três eixos e define a faixa do acelerômetro para +/- 8 g
  writeByte(MPU6050_ADDRESS, GYRO_CONFIG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
  delay(250);  // Atrasa um pouco para deixar o dispositivo executar o autoteste
  rawData[0] = readByte(MPU6050_ADDRESS, SELF_TEST_X); // resultados do autoteste do eixo X
  rawData[1] = readByte(MPU6050_ADDRESS, SELF_TEST_Y); // Resultados do autoteste do eixo Y
  rawData[2] = readByte(MPU6050_ADDRESS, SELF_TEST_Z); // Resultados do autoteste do eixo Z
  rawData[3] = readByte(MPU6050_ADDRESS, SELF_TEST_A); // Resultados de autoteste de eixo misto



  // Extraia os resultados do teste de aceleração primeiro
  selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // O resultado XA_TEST é um inteiro sem sinal de cinco bits
  selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 2 ; // O resultado YA_TEST é um inteiro sem sinal de cinco bits
  selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 0 ; // resultado ZA_TEST é um inteiro sem sinal de cinco bits



  // Extraia os resultados do teste de giro primeiro
  selfTest[3] = rawData[0]  & 0x1F ; // o resultado XG_TEST é um inteiro sem sinal de cinco bits
  selfTest[4] = rawData[1]  & 0x1F ; // O resultado YG_TEST é um inteiro sem sinal de cinco bits
  selfTest[5] = rawData[2]  & 0x1F ; // O resultado ZG_TEST é um inteiro sem sinal de cinco bits



  // Resultados do processo para permitir a comparação final com os valores definidos de fábrica
  factoryTrim[0] = (4096.0 * 0.34) * (pow( (0.92 / 0.34) , (((float)selfTest[0] - 1.0) / 30.0))); // cálculo de ajuste de fábrica FT[Xa]
  factoryTrim[1] = (4096.0 * 0.34) * (pow( (0.92 / 0.34) , (((float)selfTest[1] - 1.0) / 30.0))); // Cálculo de ajuste de fábrica FT[Ya]
  factoryTrim[2] = (4096.0 * 0.34) * (pow( (0.92 / 0.34) , (((float)selfTest[2] - 1.0) / 30.0))); // cálculo de ajuste de fábrica FT[Za]
  factoryTrim[3] =  ( 25.0 * 131.0) * (pow( 1.046 , ((float)selfTest[3] - 1.0) ));         //  cálculo de ajuste de fábrica FT[Xg]
  factoryTrim[4] =  (-25.0 * 131.0) * (pow( 1.046 , ((float)selfTest[4] - 1.0) ));         // cálculo de ajuste de fábrica FT[Yg]
  factoryTrim[5] =  ( 25.0 * 131.0) * (pow( 1.046 , ((float)selfTest[5] - 1.0) ));         // cálculo de ajuste de fábrica FT[Zg]




  // Saída de resultados de autoteste e cálculo de ajuste de fábrica, se desejado



  //Serial.println(selfTest[0]); Serial.println(selfTest[1]); Serial.println(selfTest[2]);
  //Serial.println(selfTest[3]); Serial.println(selfTest[4]); Serial.println(selfTest[5]);
  //Serial.println(factoryTrim[0]); Serial.println(factoryTrim[1]); Serial.println(factoryTrim[2]);
  //Serial.println(factoryTrim[3]); Serial.println(factoryTrim[4]); Serial.println(factoryTrim[5]);




  // Relatório de resultados como uma razão de (STR - FT)/FT; a mudança do Ajuste de Fábrica da Resposta de Autoteste
  // Para chegar ao percentual, deve-se multiplicar por 100 e subtrair o resultado de 100
  for (int i = 0; i < 6; i++) {
    destination[i] = 100.0 + 100.0 * ((float)selfTest[i] - factoryTrim[i]) / factoryTrim[i]; // Reportar diferenças percentuais
  }

}
