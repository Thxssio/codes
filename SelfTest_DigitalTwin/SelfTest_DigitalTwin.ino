

#include <Wire.h>



#define XGOFFS_TC        0x00 // Bit 7 PWR_MODE, bits 6:1 XG_OFFS_TC, bit 0 OTP_BNK_VLD             
#define YGOFFS_TC        0x01
#define ZGOFFS_TC        0x02
#define X_FINE_GAIN      0x03 // [7:0] Ganho fino
#define Y_FINE_GAIN      0x04
#define Z_FINE_GAIN      0x05
#define XA_OFFSET_H      0x06 // Valores de ajuste definidos pelo usuário para acelerômetro
#define XA_OFFSET_L_TC   0x07
#define YA_OFFSET_H      0x08
#define YA_OFFSET_L_TC   0x09
#define ZA_OFFSET_H      0x0A
#define ZA_OFFSET_L_TC   0x0B
#define SELF_TEST_X      0x0D
#define SELF_TEST_Y      0x0E
#define SELF_TEST_Z      0x0F
#define SELF_TEST_A      0x10
#define XG_OFFS_USRH     0x13  // Valores de ajuste definidos pelo usuário para giroscópio; suportado em MPU-6050
#define XG_OFFS_USRL     0x14
#define YG_OFFS_USRH     0x15
#define YG_OFFS_USRL     0x16
#define ZG_OFFS_USRH     0x17
#define ZG_OFFS_USRL     0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define FF_THR           0x1D  // Free-fall
#define FF_DUR           0x1E  // Free-fall
#define MOT_THR          0x1F  // Bits de limite de detecção de movimento [7:0]
#define MOT_DUR          0x20  // Limite do contador de duração para geração de interrupção de movimento, taxa de 1 kHz, LSB = 1 ms
#define ZMOT_THR         0x21  // Bits de limite de detecção de movimento zero [7:0]
#define ZRMOT_DUR        0x22  // Limite do contador de duração para geração de interrupção de movimento zero, taxa de 16 Hz, LSB = 64 ms
#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Verifique a interrupção do DMP
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL   0x69
#define USER_CTRL        0x6A  // Bit 7 habilita DMP, bit 3 reinicia DMP
#define PWR_MGMT_1       0x6B // O dispositivo padroniza o modo SLEEP
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Ativa um banco específico no DMP
#define DMP_RW_PNT       0x6E  // Definir ponteiro de leitura/gravação para um endereço inicial específico no banco DMP especificado
#define DMP_REG          0x6F  // Registre-se no DMP do qual ler ou no qual escrever
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU6050 0x75 // Deve retornar 0x68


// Usando a placa de breakout GY-521, defino ADO para 0 aterrando através de um resistor de 4k7
// O endereço do dispositivo de sete bits é 110100 para ADO = 0 e 110101 para ADO = 1
#define ADO 0
#if ADO
#define MPU6050_ADDRESS 0x69  // Endereço do dispositivo quando ADO = 1
#else
#define MPU6050_ADDRESS 0x68  // Endereço do dispositivo quando ADO = 0
#endif

// Define os parâmetros de entrada iniciais
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

// Especificar a escala total do sensor
int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
float aRes, gRes; // dimensiona resoluções por LSB para os sensores

// Pin definitions
int intPin = 12;  // Isso pode ser alterado, 2 e 3 são os pinos ext int do Arduino

int16_t accelCount[3];           // Armazena a saída do sensor do acelerômetro com sinal de 16 bits
float ax, ay, az;                // Armazena o valor real de aceleração em g's
int16_t gyroCount[3];            // Armazena a saída do sensor de giroscópio assinado de 16 bits
float gyrox, gyroy, gyroz;       // Armazena o valor real do giroscópio em graus por segundo
float gyroBias[3], accelBias[3]; // Correções de viés para giroscópio e acelerômetro
int16_t tempCount;               // Armazena a saída do sensor de temperatura do chip interno
float temperature;               // Temperatura escalada em graus Celsius
float SelfTest[6];               // Saída do sensor de autoteste do giroscópio e do acelerômetro
uint32_t count = 0;

void setup()
{
  Wire.begin();
  Serial.begin(38400);

  // Configura o pino de interrupção, é definido como ativo alto, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);


  // Leia o registro WHO_AM_I, este é um bom teste de comunicação
  uint8_t c = readByte(MPU6050_ADDRESS, WHO_AM_I_MPU6050); // Leia o registro WHO_AM_I para MPU-6050

  if (c == 0x68) // WHO_AM_I deve ser sempre 0x68
  {
    Serial.println("MPU6050 está online...");

    MPU6050SelfTest(SelfTest); // Comece realizando autoteste e relatando valores
    Serial.print("autoteste do eixo x: ajuste de aceleração dentro : "); Serial.print(SelfTest[0], 1); Serial.println("% de valor de fábrica");
    Serial.print("autoteste do eixo y: ajuste de aceleração dentro de: "); Serial.print(SelfTest[1], 1); Serial.println("% de valor de fábrica");
    Serial.print("Autoteste do eixo z: ajuste de aceleração dentro de: "); Serial.print(SelfTest[2], 1); Serial.println("% de valor de fábrica");
    //  Serial.print("x-axis self test: gyration trim within : "); Serial.print(SelfTest[3], 1); Serial.println("% of factory value");
    //  Serial.print("y-axis self test: gyration trim within : "); Serial.print(SelfTest[4], 1); Serial.println("% of factory value");
    //  Serial.print("z-axis self test: gyration trim within : "); Serial.print(SelfTest[5], 1); Serial.println("% of factory value");

    if (SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
      Serial.print("Passou no Autoteste!");
      Serial.println("");
      delay(1000);

      //calibrateMPU6050(gyroBias, accelBias); // Calibrar giroscópios e acelerômetros, carregar polarizações nos registros de polarização
      //initMPU6050(); Serial.println("MPU6050 inicializado para modo de dados ativo...."); // Inicializa o dispositivo para leitura de modo ativo de acelerômetro, giroscópio e temperatura
    }
    else
    {
      Serial.print("Não foi possível conectar ao MPU6050: 0x");
      Serial.println(c, HEX);
      while (1) ;   // Loop para sempre se a comunicação não acontecer
    }

  }
}

void loop()
{

 /* 
  // Se o bit de dados pronto estiver definido, todos os registradores de dados terão novos dados
  if (readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) { // verifica se os dados estão prontos para interromper

    readAccelData(accelCount); // Lê os valores x/y/z adc
    getAres();

    // Agora vamos calcular o valor da aceleração em g's reais
    ax = (float)accelCount[0] * aRes - accelBias[0]; // obtém o valor real de g, isso depende da escala que está sendo definida
    ay = (float)accelCount[1] * aRes - accelBias[1];
    az = (float)accelCount[2] * aRes - accelBias[2];

    readGyroData(gyroCount);  // Lê os valores x/y/z adc
    getGres();

   // Calcula o valor do giroscópio em graus reais por segundo
    gyrox = (float)gyroCount[0] * gRes - gyroBias[0]; // obtém o valor real do giroscópio, isso depende da escala que está sendo definida
    gyroy = (float)gyroCount[1] * gRes - gyroBias[1];
    gyroz = (float)gyroCount[2] * gRes - gyroBias[2];

    tempCount = readTempData();  // Lê os valores x/y/z adc
    temperature = ((float) tempCount) / 340. + 36.53 ; // Temperatura em graus Centígrados
  }

  uint32_t deltat = millis() - count;
  if (deltat > 500) {

 // Imprime valores de aceleração em miligramas!
    Serial.print("X-acceleration: "); Serial.print(1000 * ax); Serial.print(" mg ");
    Serial.print("Y-acceleration: "); Serial.print(1000 * ay); Serial.print(" mg ");
    Serial.print("Z-acceleration: "); Serial.print(1000 * az); Serial.println(" mg");

    // Imprime os valores do giroscópio em graus/s
    //Serial.print("X-gyro rate: "); Serial.print(gyrox, 1); Serial.print(" degrees/sec ");
    //Serial.print("Y-gyro rate: "); Serial.print(gyroy, 1); Serial.print(" degrees/sec ");
    //Serial.print("Z-gyro rate: "); Serial.print(gyroz, 1); Serial.println(" degrees/sec");

   // Imprime a temperatura em graus Centígrados
    Serial.print("Temperature is ");  Serial.print(temperature, 2);  Serial.println(" degrees C"); // Print T values to tenths of s degree C
    Serial.println("");
    count = millis();
  }



*/


//Escreva Aqui
}
