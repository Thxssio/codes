//===================================================================================================================
//====== Conjunto de funções úteis para acessar dados de aceleração, giroscópio e temperatura
//===================================================================================================================



// Autoteste do acelerômetro e do giroscópio; verifique as configurações de fábrica do wrt de calibração

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
  factoryTrim[3] =  ( 25.0 * 131.0) * (pow( 1.046 , ((float)selfTest[3] - 1.0) ));         // FT[Xg] cálculo de ajuste de fábrica
  factoryTrim[4] =  (-25.0 * 131.0) * (pow( 1.046 , ((float)selfTest[4] - 1.0) ));         // cálculo de ajuste de fábrica FT[Yg]
  factoryTrim[5] =  ( 25.0 * 131.0) * (pow( 1.046 , ((float)selfTest[5] - 1.0) ));         // cálculo de ajuste de fábrica FT[Zg]

 
  
  
  // Saída de resultados de autoteste e cálculo de ajuste de fábrica, se desejado


  
  //  Serial.println(selfTest[0]); Serial.println(selfTest[1]); Serial.println(selfTest[2]);
  //  Serial.println(selfTest[3]); Serial.println(selfTest[4]); Serial.println(selfTest[5]);
  //  Serial.println(factoryTrim[0]); Serial.println(factoryTrim[1]); Serial.println(factoryTrim[2]);
  //  Serial.println(factoryTrim[3]); Serial.println(factoryTrim[4]); Serial.println(factoryTrim[5]);




  // Relatório de resultados como uma razão de (STR - FT)/FT; a mudança do Ajuste de Fábrica da Resposta de Autoteste
  // Para chegar ao percentual, deve-se multiplicar por 100 e subtrair o resultado de 100
  for (int i = 0; i < 6; i++) {
    destination[i] = 100.0 + 100.0 * ((float)selfTest[i] - factoryTrim[i]) / factoryTrim[i]; // Reportar diferenças percentuais
  }

}
