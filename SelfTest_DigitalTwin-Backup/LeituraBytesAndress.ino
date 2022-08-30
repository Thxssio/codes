//===================================================================================================================
//====== Conjunto de funções úteis para acessar dados de aceleração, giroscópio e temperatura
//===================================================================================================================


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
