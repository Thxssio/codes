//MPU-6050 6 eixos DMP Demo v0.01
//por Noah Zerkin em 2011
//noazark AT gmail DOT com

// Adaptado ao IDE 1.0.X por Juan A. Villalpando - KIO4.COM

//Este esboço não se destina a ser uma biblioteca de uso geral para o MPU-6050. não é objeto
//orientado e não pode ser usado com vários sensores até que esteja adaptado para tal. É simplesmente
//um esboço de demonstração destinado a reproduzir o comportamento da demonstração do Invensense Embedded MotionApps
//para a placa Atmel AVR UC3-A3 Xplained para uso com a demonstração Invensense Teapot. Você pode baixar
//a solução TeaPot and Pointer Demo App Visual C++ do Invensense Developers Corner.

//Ah sim, isso só foi testado com silicone MPU-6050 Rev C. Se você estiver usando uma placa de avaliação Rev A,
//Não prometo que esta versão funcionará.

//Códigos mais úteis serão lançados em breve.

//E você pode precisar iniciar a demonstração do bule duas vezes para que ela seja executada. Ainda não tenho certeza do que há com isso.

//Não estou anexando uma licença formal a este código. Mas eu coloquei muitas horas de trabalho duro nisso,
//então se você fizer uso disso, um salve ou algum outro tipo de agradecimento seria ótimo.


#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#define MPU_ADDR 0x68
#define MEM_START_ADDR 0x6E
#define MEM_R_W 0x6F

long lastRead = 0;
byte processado_packet[8];
byte recebido_pacote[50];
byte temp = 0;
byte fifoCountL = 0;
byte fifoCountL2 = 0;
byte packetCount = 0x00;
boolean longPacket = false;
boolean firstPacket = true;
flutuar q[4];

//Este array 3D contém o binário do banco de memória DMP padrão que é carregado durante a inicialização.
//No firmware Invensense UC3-A3 isso é carregado em transmissões de 128 bytes, mas o Arduino Wire
//biblioteca suporta apenas transmissões de 32 bytes, incluindo o endereço de registro para o qual você está escrevendo,
// então eu dividi em cargas de transmissão de 16 bytes que são enviadas na função dmp_init() abaixo.
//
//Isso foi reconstruído a partir do tráfego I2C observado gerado pelo código de demonstração UC3-A3 e não extraído
//diretamente desse código. Isso é verdade para todas as transmissões neste esboço, e qualquer documentação
//foi adicionado após o fato referenciando o código Invensense.

unsigned const char dmpMem[8][16][16] PROGMEM = {
  {
    {0xFB, 0x00, 0x00, 0x3E, 0x00, 0x0B, 0x00, 0x36, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00},
    {0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0xFA, 0x80, 0x00, 0x0B, 0x12, 0x82, 0x00, 0x01},
    {0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x28, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0xFF, 0xFF, 0xFA, 0x72, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x03, 0xE8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x7F, 0xFF, 0xFF, 0xFE, 0x80, 0x01},
    {0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x3E, 0x03, 0x30, 0x40, 0x00, 0x00, 0x00, 0x02, 0xCA, 0xE3, 0x09, 0x3E, 0x80, 0x00, 0x00},
    {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00},
    {0x41, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x00, 0x16, 0x55, 0x00, 0x00, 0x21, 0x82},
    {0xFD, 0x87, 0x26, 0x50, 0xFD, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x05, 0x80, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00},
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x6F, 0x00, 0x02, 0x65, 0x32, 0x00, 0x00, 0x5E, 0xC0},
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xFB, 0x8C, 0x6F, 0x5D, 0xFD, 0x5D, 0x08, 0xD9, 0x00, 0x7C, 0x73, 0x3B, 0x00, 0x6C, 0x12, 0xCC},
    {0x32, 0x00, 0x13, 0x9D, 0x32, 0x00, 0xD0, 0xD6, 0x32, 0x00, 0x08, 0x00, 0x40, 0x00, 0x01, 0xF4},
    {0xFF, 0xE6, 0x80, 0x79, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0xD6, 0x00, 0x00, 0x27, 0x10}
  },
  {
    {0xFB, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xFA, 0x36, 0xFF, 0xBC, 0x30, 0x8E, 0x00, 0x05, 0xFB, 0xF0, 0xFF, 0xD9, 0x5B, 0xC8},
    {0xFF, 0xD0, 0x9A, 0xBE, 0x00, 0x00, 0x10, 0xA9, 0xFF, 0xF4, 0x1E, 0xB2, 0x00, 0xCE, 0xBB, 0xF7},
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x00, 0x0C},
    {0xFF, 0xC2, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0xCF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x14},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0x68, 0xB6, 0x79, 0x35, 0x28, 0xBC, 0xC6, 0x7E, 0xD1, 0x6C},
    {0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x6A, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x30},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x25, 0x4D, 0x00, 0x2F, 0x70, 0x6D, 0x00, 0x00, 0x05, 0xAE, 0x00, 0x0C, 0x02, 0xD0}
  },
  {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x01, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0xFF, 0xEF, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00},
    {0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
  },
  {
    {0xD8, 0xDC, 0xBA, 0xA2, 0xF1, 0xDE, 0xB2, 0xB8, 0xB4, 0xA8, 0x81, 0x91, 0xF7, 0x4A, 0x90, 0x7F},
    {0x91, 0x6A, 0xF3, 0xF9, 0xDB, 0xA8, 0xF9, 0xB0, 0xBA, 0xA0, 0x80, 0xF2, 0xCE, 0x81, 0xF3, 0xC2},
    {0xF1, 0xC1, 0xF2, 0xC3, 0xF3, 0xCC, 0xA2, 0xB2, 0x80, 0xF1, 0xC6, 0xD8, 0x80, 0xBA, 0xA7, 0xDF},
    {0xDF, 0xDF, 0xF2, 0xA7, 0xC3, 0xCB, 0xC5, 0xB6, 0xF0, 0x87, 0xA2, 0x94, 0x24, 0x48, 0x70, 0x3C},
    {0x95, 0x40, 0x68, 0x34, 0x58, 0x9B, 0x78, 0xA2, 0xF1, 0x83, 0x92, 0x2D, ​​0x55, 0x7D, 0xD8, 0xB1},
    {0xB4, 0xB8, 0xA1, 0xD0, 0x91, 0x80, 0xF2, 0x70, 0xF3, 0x70, 0xF2, 0x7C, 0x80, 0xA8, 0xF1, 0x01},
    {0xB0, 0x98, 0x87, 0xD9, 0x43, 0xD8, 0x86, 0xC9, 0x88, 0xBA, 0xA1, 0xF2, 0x0E, 0xB8, 0x97, 0x80},
    {0xF1, 0xA9, 0xDF, 0xDF, 0xDF, 0xAA, 0xDF, 0xDF, 0xDF, 0xF2, 0xAA, 0xC5, 0xCD, 0xC7, 0xA9, 0x0C},
    {0xC9, 0x2C, 0x97, 0x97, 0x97, 0x97, 0xF1, 0xA9, 0x89, 0x26, 0x46, 0x66, 0xB0, 0xB4, 0xBA, 0x80},
    {0xAC, 0xDE, 0xF2, 0xCA, 0xF1, 0xB2, 0x8C, 0x02, 0xA9, 0xB6, 0x98, 0x00, 0x89, 0x0E, 0x16, 0x1E},
    {0xB8, 0xA9, 0xB4, 0x99, 0x2C, 0x54, 0x7C, 0xB0, 0x8A, 0xA8, 0x96, 0x36, 0x56, 0x76, 0xF1, 0xB9},
    {0xAF, 0xB4, 0xB0, 0x83, 0xC0, 0xB8, 0xA8, 0x97, 0x11, 0xB1, 0x8F, 0x98, 0xB9, 0xAF, 0xF0, 0x24},
    {0x08, 0x44, 0x10, 0x64, 0x18, 0xF1, 0xA3, 0x29, 0x55, 0x7D, 0xAF, 0x83, 0xB5, 0x93, 0xAF, 0xF0},
    {0x00, 0x28, 0x50, 0xF1, 0xA3, 0x86, 0x9F, 0x61, 0xA6, 0xDA, 0xDE, 0xDF, 0xD9, 0xFA, 0xA3, 0x86},
    {0x96, 0xDB, 0x31, 0xA6, 0xD9, 0xF8, 0xDF, 0xBA, 0xA6, 0x8F, 0xC2, 0xC5, 0xC7, 0xB2, 0x8C, 0xC1},
    {0xB8, 0xA2, 0xDF, 0xDF, 0xDF, 0xA3, 0xDF, 0xDF, 0xDF, 0xD8, 0xD8, 0xF1, 0xB8, 0xA8, 0xB2, 0x86}
  },
  {
    {0xB4, 0x98, 0x0D, 0x35, 0x5D, 0xB8, 0xAA, 0x98, 0xB0, 0x87, 0x2D, ​​0x35, 0x3D, 0xB2, 0xB6, 0xBA},
    {0xAF, 0x8C, 0x96, 0x19, 0x8F, 0x9F, 0xA7, 0x0E, 0x16, 0x1E, 0xB4, 0x9A, 0xB8, 0xAA, 0x87, 0x2C},
    {0x54, 0x7C, 0xB9, 0xA3, 0xDE, 0xDF, 0xDF, 0xA3, 0xB1, 0x80, 0xF2, 0xC4, 0xCD, 0xC9, 0xF1, 0xB8},
    {0xA9, 0xB4, 0x99, 0x83, 0x0D, 0x35, 0x5D, 0x89, 0xB9, 0xA3, 0x2D, ​​0x55, 0x7D, 0xB5, 0x93, 0xA3},
    {0x0E, 0x16, 0x1E, 0xA9, 0x2C, 0x54, 0x7C, 0xB8, 0xB4, 0xB0, 0xF1, 0x97, 0x83, 0xA8, 0x11, 0x84},
    {0xA5, 0x09, 0x98, 0xA3, 0x83, 0xF0, 0xDA, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xD8, 0xF1, 0xA5},
    {0x29, 0x55, 0x7D, 0xA5, 0x85, 0x95, 0x02, 0x1A, 0x2E, 0x3A, 0x56, 0x5A, 0x40, 0x48, 0xF9, 0xF3},
    {0xA3, 0xD9, 0xF8, 0xF0, 0x98, 0x83, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0x97, 0x82, 0xA8, 0xF1},
    {0x11, 0xF0, 0x98, 0xA2, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xDA, 0xF3, 0xDE, 0xD8, 0x83, 0xA5},
    {0x94, 0x01, 0xD9, 0xA3, 0x02, 0xF1, 0xA2, 0xC3, 0xC5, 0xC7, 0xD8, 0xF1, 0x84, 0x92, 0xA2, 0x4D},
    {0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9},
    {0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0x93, 0xA3, 0x4D},
    {0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9},
    {0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0xA8, 0x8A, 0x9A},
    {0xF0, 0x28, 0x50, 0x78, 0x9E, 0xF3, 0x88, 0x18, 0xF1, 0x9F, 0x1D, 0x98, 0xA8, 0xD9, 0x08, 0xD8},
    {0xC8, 0x9F, 0x12, 0x9E, 0xF3, 0x15, 0xA8, 0xDA, 0x12, 0x10, 0xD8, 0xF1, 0xAF, 0xC8, 0x97, 0x87}
  },
  {
    {0x34, 0xB5, 0xB9, 0x94, 0xA4, 0x21, 0xF3, 0xD9, 0x22, 0xD8, 0xF2, 0x2D, ​​0xF3, 0xD9, 0x2A, 0xD8},
    {0xF2, 0x35, 0xF3, 0xD9, 0x32, 0xD8, 0x81, 0xA4, 0x60, 0x60, 0x61, 0xD9, 0x61, 0xD8, 0x6C, 0x68},
    {0x69, 0xD9, 0x69, 0xD8, 0x74, 0x70, 0x71, 0xD9, 0x71, 0xD8, 0xB1, 0xA3, 0x84, 0x19, 0x3D, 0x5D},
    {0xA3, 0x83, 0x1A, 0x3E, 0x5E, 0x93, 0x10, 0x30, 0x81, 0x10, 0x11, 0xB8, 0xB0, 0xAF, 0x8F, 0x94},
    {0xF2, 0xDA, 0x3E, 0xD8, 0xB4, 0x9A, 0xA8, 0x87, 0x29, 0xDA, 0xF8, 0xD8, 0x87, 0x9A, 0x35, 0xDA},
    {0xF8, 0xD8, 0x87, 0x9A, 0x3D, 0xDA, 0xF8, 0xD8, 0xB1, 0xB9, 0xA4, 0x98, 0x85, 0x02, 0x2E, 0x56},
    {0xA5, 0x81, 0x00, 0x0C, 0x14, 0xA3, 0x97, 0xB0, 0x8A, 0xF1, 0x2D, ​​0xD9, 0x28, 0xD8, 0x4D, 0xD9},
    {0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x84, 0x0D, 0xDA, 0x0E, 0xD8, 0xA3, 0x29, 0x83, 0xDA},
    {0x2C, 0x0E, 0xD8, 0xA3, 0x84, 0x49, 0x83, 0xDA, 0x2C, 0x4C, 0x0E, 0xD8, 0xB8, 0xB0, 0xA8, 0x8A},
    {0x9A, 0xF5, 0x20, 0xAA, 0xDA, 0xDF, 0xD8, 0xA8, 0x40, 0xAA, 0xD0, 0xDA, 0xDE, 0xD8, 0xA8, 0x60},
    {0xAA, 0xDA, 0xD0, 0xDF, 0xD8, 0xF1, 0x97, 0x86, 0xA8, 0x31, 0x9B, 0x06, 0x99, 0x07, 0xAB, 0x97},
    {0x28, 0x88, 0x9B, 0xF0, 0x0C, 0x20, 0x14, 0x40, 0xB8, 0xB0, 0xB4, 0xA8, 0x8C, 0x9C, 0xF0, 0x04},
    {0x28, 0x51, 0x79, 0x1D, 0x30, 0x14, 0x38, 0xB2, 0x82, 0xAB, 0xD0, 0x98, 0x2C, 0x50, 0x50, 0x78},
    {0x78, 0x9B, 0xF1, 0x1A, 0xB0, 0xF0, 0x8A, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x8B, 0x29, 0x51, 0x79},
    {0x8A, 0x24, 0x70, 0x59, 0x8B, 0x20, 0x58, 0x71, 0x8A, 0x44, 0x69, 0x38, 0x8B, 0x39, 0x40, 0x68},
    {0x8A, 0x64, 0x48, 0x31, 0x8B, 0x30, 0x49, 0x60, 0xA5, 0x88, 0x20, 0x09, 0x71, 0x58, 0x44, 0x68}
  },
  {
    {0x11, 0x39, 0x64, 0x49, 0x30, 0x19, 0xF1, 0xAC, 0x00, 0x2C, 0x54, 0x7C, 0xF0, 0x8C, 0xA8, 0x04},
    {0x28, 0x50, 0x78, 0xF1, 0x88, 0x97, 0x26, 0xA8, 0x59, 0x98, 0xAC, 0x8C, 0x02, 0x26, 0x46, 0x66},
    {0xF0, 0x89, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31},
    {0xA9, 0x88, 0x09, 0x20, 0x59, 0x70, 0xAB, 0x11, 0x38, 0x40, 0x69, 0xA8, 0x19, 0x31, 0x48, 0x60},
    {0x8C, 0xA8, 0x3C, 0x41, 0x5C, 0x20, 0x7C, 0x00, 0xF1, 0x87, 0x98, 0x19, 0x86, 0xA8, 0x6E, 0x76},
    {0x7E, 0xA9, 0x99, 0x88, 0x2D, ​​0x55, 0x7D, 0x9E, 0xB9, 0xA3, 0x8A, 0x22, 0x8A, 0x6E, 0x8A, 0x56},
    {0x8A, 0x5E, 0x9F, 0xB1, 0x83, 0x06, 0x26, 0x46, 0x66, 0x0E, 0x2E, 0x4E, 0x6E, 0x9D, 0xB8, 0xAD},
    {0x00, 0x2C, 0x54, 0x7C, 0xF2, 0xB1, 0x8C, 0xB4, 0x99, 0xB9, 0xA3, 0x2D, ​​0x55, 0x7D, 0x81, 0x91},
    {0xAC, 0x38, 0xAD, 0x3A, 0xB5, 0x83, 0x91, 0xAC, 0x2D, ​​0xD9, 0x28, 0xD8, 0x4D, 0xD9, 0x48, 0xD8},
    {0x6D, 0xD9, 0x68, 0xD8, 0x8C, 0x9D, 0xAE, 0x29, 0xD9, 0x04, 0xAE, 0xD8, 0x51, 0xD9, 0x04, 0xAE},
    {0xD8, 0x79, 0xD9, 0x04, 0xD8, 0x81, 0xF3, 0x9D, 0xAD, 0x00, 0x8D, 0xAE, 0x19, 0x81, 0xAD, 0xD9},
    {0x01, 0xD8, 0xF2, 0xAE, 0xDA, 0x26, 0xD8, 0x8E, 0x91, 0x29, 0x83, 0xA7, 0xD9, 0xAD, 0xAD, 0xAD},
    {0xAD, 0xF3, 0x2A, 0xD8, 0xD8, 0xF1, 0xB0, 0xAC, 0x89, 0x91, 0x3E, 0x5E, 0x76, 0xF3, 0xAC, 0x2E},
    {0x2E, 0xF1, 0xB1, 0x8C, 0x5A, 0x9C, 0xAC, 0x2C, 0x28, 0x28, 0x28, 0x9C, 0xAC, 0x30, 0x18, 0xA8},
    {0x98, 0x81, 0x28, 0x34, 0x3C, 0x97, 0x24, 0xA7, 0x28, 0x34, 0x3C, 0x9C, 0x24, 0xF2, 0xB0, 0x89},
    {0xAC, 0x91, 0x2C, 0x4C, 0x6C, 0x8A, 0x9B, 0x2D, ​​0xD9, 0xD8, 0xD8, 0x51, 0xD9, 0xD8, 0xD8, 0x79}
  },
  {
    {0xD9, 0xD8, 0xD8, 0xF1, 0x9E, 0x88, 0xA3, 0x31, 0xDA, 0xD8, 0xD8, 0x91, 0x2D, ​​0xD9, 0x28, 0xD8},
    {0x4D, 0xD9, 0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x83, 0x93, 0x35, 0x3D, 0x80, 0x25, 0xDA},
    {0xD8, 0xD8, 0x85, 0x69, 0xDA, 0xD8, 0xD8, 0xB4, 0x93, 0x81, 0xA3, 0x28, 0x34, 0x3C, 0xF3, 0xAB},
    {0x8B, 0xF8, 0xA3, 0x91, 0xB6, 0x09, 0xB4, 0xD9, 0xAB, 0xDE, 0xFA, 0xB0, 0x87, 0x9C, 0xB9, 0xA3},
    {0xDD, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x95, 0xF1, 0xA3, 0xA3, 0xA3, 0x9D, 0xF1, 0xA3, 0xA3, 0xA3},
    {0xA3, 0xF2, 0xA3, 0xB4, 0x90, 0x80, 0xF2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3},
    {0xA3, 0xB2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xB0, 0x87, 0xB5, 0x99, 0xF1, 0xA3, 0xA3, 0xA3},
    {0x98, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x97, 0xA3, 0xA3, 0xA3, 0xA3, 0xF3, 0x9B, 0xA3, 0xA3, 0xDC},
    {0xB9, 0xA7, 0xF1, 0x26, 0x26, 0x26, 0xD8, 0xD8, 0xFF}
  }
};

//Transmissões de atualização DMP (Banco, Endereço Inicial, Comprimento da Atualização, Atualizar Dados...)

byte estático dmp_updates[29][9] =
{
  {
    0x03, 0x7B, 0x03, 0x4C, 0xCD, 0x6C} //FCFG_1 inv_set_gyro_calibration
  ,
  {
    0x03, 0xAB, 0x03, 0x36, 0x56, 0x76 } //FCFG_3 inv_set_gyro_calibration
  ,
  {
    0x00, 0x68, 0x04, 0x02, 0xCB, 0x47, 0xA2 } //D_0_104 inv_set_gyro_calibration
  ,
  {
    0x02, 0x18, 0x04, 0x00, 0x05, 0x8B, 0xC1 } //D_0_24 inv_set_gyro_calibration
  ,
  {
    0x01, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00 } //D_1_152 inv_set_accel_calibration
  ,
  {
    0x03, 0x7F, 0x06, 0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97 } //FCFG_2 inv_set_accel_calibration
  ,
  {
    0x03, 0x89, 0x03, 0x26, 0x46, 0x66 } //FCFG_7 inv_set_accel_calibration
  ,
  {
    0x00, 0x6C, 0x02, 0x20, 0x00 } //D_0_108 inv_set_accel_calibration
  ,
  {
    0x02, 0x40, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_00 inv_set_compass_calibration
  ,
  {
    0x02, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_01
  ,
  {
    0x02, 0x48, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_02
  ,
  {
    0x02, 0x4C, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_10
  ,
  {
    0x02, 0x50, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_11
  ,
  {
    0x02, 0x54, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_12
  ,
  {
    0x02, 0x58, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_20
  ,
  {
    0x02, 0x5C, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_21
  ,
  {
    0x02, 0xBC, 0x04, 0x00, 0x00, 0x00, 0x00 } //CPASS_MTX_22
  ,
  {
    0x01, 0xEC, 0x04, 0x00, 0x00, 0x40, 0x00 } //D_1_236 inv_apply_endian_accel
  ,
  {
    0x03, 0x7F, 0x06, 0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97 } //FCFG_2 inv_set_mpu_sensors
  ,
  {
    0x04, 0x02, 0x03, 0x0D, 0x35, 0x5D } //CFG_MOTION_BIAS inv_turn_on_bias_from_no_motion
  ,
  {
    0x04, 0x09, 0x04, 0x87, 0x2D, ​​0x35, 0x3D } //FCFG_5 inv_set_bias_update
  ,
  {
    0x00, 0xA3, 0x01, 0x00 } //D_0_163 inv_set_dead_zone
  ,
  //SET INT_ENABLE em i=22
  {
    0x07, 0x86, 0x01, 0xFE } //CFG_6 inv_set_fifo_interupt
  ,
  {
    0x07, 0x41, 0x05, 0xF1, 0x20, 0x28, 0x30, 0x38 } //CFG_8 inv_send_quaternion
  ,
  {
    0x07, 0x7E, 0x01, 0x30 } //CFG_16 inv_set_footer
  ,
  {
    0x07, 0x46, 0x01, 0x9A } //CFG_GYRO_SOURCE inv_send_gyro
  ,
  {
    0x07, 0x47, 0x04, 0xF1, 0x28, 0x30, 0x38 } //CFG_9 inv_send_gyro -> inv_construct3_fifo
  ,
  {
    0x07, 0x6C, 0x04, 0xF1, 0x28, 0x30, 0x38 } //CFG_12 inv_send_accel -> inv_construct3_fifo
  ,
  {
    0x02, 0x16, 0x02, 0x00, 0x0A } //D_0_22 inv_set_fifo_rate
};
  
void configuração(){

  Serial.begin(115200);
  
  /*
  while(!Serial.available()){
    if(Serial.available()){
      byte aquiWeGo = Serial.read();
      if( hereWeGo == 't'){
        parar;
      }
    }
  }
  */
  
  Fio.begin();
  atraso(1);
  check_MPU();
  
  Serial.println("MPU-6050 6 eixos");

  regWrite(0x6B, 0xC0);
  regWrite(0x6C, 0x00);
  atraso(10);
  
// regWrite(0x6B, 0x70);
  regWrite(0x6B, 0x00);
  regWrite(0x6D, 0x70);
  regWrite(0x6E, 0x06);
  temp = regRead(0x6F);
  Serial.print("Banco 1, Reg 6 = ");
  Serial.println(temp, HEX);

// temp = regRead(0x6B);
// Serial.println(temp, HEX);
  
  regWrite(0x6D, 0x00);
  
  temp = regRead(0x00);
  Serial.println(temp, HEX);
  temp = regRead(0x01);
  Serial.println(temp, HEX);
  temp = regRead(0x02);
  Serial.println(temp, HEX);
  temp = regRead(0x6A);
  Serial.println(temp, HEX);
  
  regWrite(0x37, 0x32);
  
  temp = regRead(0x6B);
  Serial.println(temp, HEX);
  atraso(5);
// regWrite(0x25, 0x68); //Define Slave 0 para self
//
// regWrite(0x6A, 0x02);

  mem_init();
atraso(20);
}
  
void dmp_init(){
  
  for(int i = 0; i < 7; i++){
    banco_sel(i);
    for(byte j = 0; j < 16; j++){
      
      byte start_addy = j * 0x10;
      
      Wire.beginTransmission(MPU_ADDR);
      Wire.write(MEM_START_ADDR);
      Wire.write(start_addy);
      Wire.endTransmission();
  
      Wire.beginTransmission(MPU_ADDR);
      Wire.write(MEM_R_W);
      for(int k = 0; k < 16; k++){
        unsigned char byteToSend = pgm_read_byte(&(dmpMem[i][j][k]));
        Wire.write((byte) byteToSend);
      }
      Wire.endTransmission();
    }
    
// Wire.beginTransmission(MPU_ADDR);
//Wire.write(MEM_R_W);
// Fio.endTransmission();
// Wire.requestFrom(MPU_ADDR, 16);
// byte ecoback[16];
// for(int j = 0; j < 16; j++){
// echoback[j] = Wire.read();
// }
// for(int j = 0; j < 16; j++){
// Serial.print(echoback[j], HEX);
// }
    
  }
  
  banco_sel(7);

  for(byte j = 0; j < 8; j++){
    
    byte start_addy = j * 0x10;
    
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MEM_START_ADDR);
    Wire.write(start_addy);
    Wire.endTransmission();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MEM_R_W);
    for(int k = 0; k < 16; k++){
      unsigned char byteToSend = pgm_read_byte(&(dmpMem[7][j][k]));
      Wire.write((byte) byteToSend);
    }
    Wire.endTransmission();
  }
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MEM_START_ADDR);
  Wire.write(0x80);
  Wire.endTransmission();
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MEM_R_W);
  for(int k = 0; k < 9; k++){
      unsigned char byteToSend = pgm_read_byte(&(dmpMem[7][8][k]));
      Wire.write((byte) byteToSend);
  }
  Wire.endTransmission();
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MEM_R_W);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.requestFrom(MPU_ADDR,9);
// Fio.endTransmission();
  byte de entrada[9];
  for(int i = 0; i < 9; i++){
    entrada[i] = Wire.read();
  }
  
// bank_sel(3);
// Wire.beginTransmission(MPU_ADDR);
// Wire.write(MEM_START_ADDR);
//Wire.write(0x10);
// Fio.endTransmission();
// Wire.beginTransmission(MPU_ADDR);
//Wire.write(MEM_R_W);
// Fio.endTransmission();
// Wire.beginTransmission(MPU_ADDR);
// Wire.requestFrom(MPU_ADDR,16);
// Fio.endTransmission();
// byte de entrada[16];
// for(int i = 0; i < 16; i++){
// entrada[i] = Wire.read();
// }

}
  
void mem_init(){
  
  dmp_init();
  
  for(byte i = 0; i < 22; i++){
    bank_sel(dmp_updates[i][0]);
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MEM_START_ADDR);
    Wire.write(dmp_updates[i][1]);
    Wire.endTransmission();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MEM_R_W);
    for(byte j = 0; j < dmp_updates[i][2]; j++){
      Wire.write(dmp_updates[i][j+3]);
    }
    Wire.endTransmission();
  }

  regWrite(0x38, 0x32);

  for(byte i = 22; i < 29; i++){
    bank_sel(dmp_updates[i][0]);
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MEM_START_ADDR);
    Wire.write(dmp_updates[i][1]);
    Wire.endTransmission();

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(MEM_R_W);
    for(byte j = 0; j < dmp_updates[i][2]; j++){
      Wire.write(dmp_updates[i][j+3]);
    }
    Wire.endTransmission();
  }
  
  temp = regRead(0x6B);
  Serial.println(temp, HEX);
  temp = regRead(0x6C);
  Serial.println(temp, HEX);
  
  regWrite(0x38, 0x02);
  regWrite(0x6B, 0x03);
// regWrite(0x6B, 0x70);
// regWrite(0x38, 0x38);
// regWrite(0x6B, 0x73);
  regWrite(0x19, 0x04);
  regWrite(0x1B, 0x18);
  regWrite(0x1A, 0x0B);
  regWrite(0x70, 0x03);
  regWrite(0x71, 0x00);
  regWrite(0x00, 0x00);
  regWrite(0x01, 0x00);
  regWrite(0x02, 0x00);
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x13);
  for(byte i = 0; i < 6; i++){
    Wire.write(0x00);
  }
  Wire.endTransmission();
  
// regWrite(0x24, 0x00);

  banco_sel(0x01);
  regWrite(0x6E, 0xB2);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6F);
  Wire.write(0xFF); Wire.write(0xFF);
  Wire.endTransmission();

  banco_sel(0x01);
  regWrite(0x6E, 0x90);
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6F);
  Wire.write(0x09); Wire.write(0x23); Wire.write(0xA1); Wire.write(0x35);
  Wire.endTransmission();
  
  temp = regRead(0x6A);
  
  regWrite(0x6A, 0x04);
  
  //Inserir contagem FIFO lida?
  fifoPronto();
  
  regWrite(0x6A, 0x00);
  regWrite(0x6B, 0x03);
  
  atraso(2);
  
  temp = regRead(0x6C);
// Serial.println(temp, HEX);
  regWrite(0x6C, 0x00);
  temp = regRead(0x1C);
// Serial.println(temp, HEX);
  regWrite(0x1C, 0x00);
  atraso(2);
  temp = regRead(0x6B);
// Serial.println(temp, HEX);
  regWrite(0x1F, 0x02);
  regWrite(0x21, 0x9C);
  regWrite(0x20, 0x50);
  regWrite(0x22, 0x00);
  regWrite(0x6A, 0x04);
  regWrite(0x6A, 0x00);
  regWrite(0x6A, 0xC8);
  
  banco_sel(0x01);
  regWrite(0x6E, 0x6A);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6F);
  Wire.write(0x06); Wire.write(0x00);
  Wire.endTransmission();
  
  banco_sel(0x01);
  regWrite(0x6E, 0x60);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6F);
  for(byte i = 0; i < 8; i++){
    Wire.write(0x00);
  }
  Wire.endTransmission();
  
// bank_sel(0x01);
// regWrite(0x6E, 0x60);
// Wire.beginTransmission(MPU_ADDR);
//Wire.write(0x6F);
//Wire.write(0x04); Wire.write(0x00); Wire.write(0x00); Wire.write(0x00);
// Fio.endTransmission();
  
  banco_sel(0x00);
  regWrite(0x6E, 0x60);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6F);
  Wire.write(0x40); Wire.write(0x00); Wire.write(0x00); Wire.write(0x00);
  Wire.endTransmission();
  
  //resetFifo();
  
}

void regWrite(byte addy, byte regUpdate){
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(add);
  Wire.write(regUpdate);
  Wire.endTransmission();
}

byte regRead(byte addy){
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(add);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.requestFrom(MPU_ADDR,1);
// Fio.endTransmission();
  while(!Wire.available()){
  }
  byte de entrada = Wire.read();
  retorno recebido;
}

void getPacket(){
  if(fifoCountL > 32){
    fifoCountL2 = fifoCountL - 32;
    longPacket = true;
  }
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x74);
  Wire.endTransmission();
// Wire.requestFrom(MPU_ADDR, 42);
// for(byte i = 0; i < fifoCountL; i++){
  if(longPacket){
    Wire.beginTransmission(MPU_ADDR);
    Wire.requestFrom(MPU_ADDR, 32);
    for(byte i = 0; i < 32; i++){
      pacote_recebido[i] = Wire.read();
    }
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x74);
    Wire.endTransmission();
    Wire.beginTransmission(MPU_ADDR);
    Wire.requestFrom(MPU_ADDR, (unsigned int)fifoCountL2);
    for(byte i = 32; i < fifoCountL; i++){
      pacote_recebido[i] = Wire.read();
    }
    longPacket = false;
  }
  senão{
    Wire.beginTransmission(MPU_ADDR);
    Wire.requestFrom(MPU_ADDR, (unsigned int)fifoCountL);
    for(byte i = 0; i < fifoCountL; i++){
      pacote_recebido[i] = Wire.read();
    }
  }
}

byte read_interrupt(){
  byte int_status = regRead(0x3A);
  return int_status;
}

boolean fifoReady(){
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x72);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.requestFrom(MPU_ADDR,2);
// Fio.endTransmission();
  byte fifoCountH = Wire.read();
  fifoCountL = Wire.read();
// Serial.println(fifoCountL, DEC);
  if(fifoCountL == 42 || fifoCountL == 44){
    retorno 1;
  }
// senão if(fifoCountL != 0){
// resetFifo();
// }
  
  
// senão if(fifoCountL == 42){
// getPacket();
// bank_sel(0);
// regWrite(0x6E, 0x60);
// Wire.beginTransmission(MPU_ADDR);
//Wire.write(0x40); Wire.write(0x00); Wire.write (0x00); Wire.write(0x00);
// Fio.endTransmission();
// resetFifo();
// }
  
// if(Wire.read() == 0x2C){
// retorna 1;
// }
  senão retorna 0;
}

void resetFifo(){
  byte ctrl = regRead(0x6A);
  ctrl |= 0b00000100;
  regWrite(0x6A, ctrl);
}

void loop(){

  if(millis() >= lastRead + 10){
    lastRead = millis();
// byte int_status = read_interrupt();
// if(int_status & 0b00010000 != 0){
// resetFifo();
// Serial.println("FIFO Overflow");
// }
// if(int_status & 0b00000011 != 0){
// getPacket();
// enviarPacote();
// }
    if(fifoReady()){
      getPacote();
      temp = regRead(0x3A);
      if(primeiroPacote){
        atraso(1);
        banco_sel(0x00);
        regWrite(0x6E, 0x60);
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x6F);
        Wire.write(0x04); Wire.write(0x00); Wire.write(0x00); Wire.write(0x00);
//Wire.write(0x00); Wire.write(0x80); Wire.write(0x00); Wire.write(0x00);
        Wire.endTransmission();
        banco_sel(1);
        regWrite(0x6E, 0x62);
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x6F);
        Wire.endTransmission();
        Wire.beginTransmission(MPU_ADDR);
        Wire.requestFrom(MPU_ADDR,2);
        temp = Wire.read();
        temp = Wire.read();
        primeiroPacote = false;
        
        fifoPronto();
      }

 // resetFifo();
 
      if(fifoCountL == 42){
        processQuat();
        sendQuat();
      }
// int recebidosInts[10] = {(((int)received_packet[0] << 8) | receive_packet[1]),
// (((int)received_packet[4] << 8) | receive_packet[5]),
// (((int)received_packet[8] << 8) | receive_packet[9]),
// (((int)received_packet[12] << 8) | receive_packet[13]),
// (((int)received_packet[16] << 8) | receive_packet[17]),
// (((int)received_packet[20] << 8) | receive_packet[21]),
// (((int)received_packet[24] << 8) | receive_packet[25]),
// (((int)received_packet[28] << 8) | receive_packet[29]),
// (((int)recebido_pacote[32] << 8) | recebido_pacote[33]),
// (((int) pacote_recebido[36] << 8) | pacote_recebido[37])
// };
//
// for(int i = 0; i<10; i++){
// Serial.print(receivedInts[i], DEC); Serial.print(" ");
// }
// Serial.println();
  
  // enviarPacote();
    }
  }

}

void check_MPU(){
    
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);
  Wire.endTransmission();
  Wire.beginTransmission(MPU_ADDR);
  Wire.requestFrom(MPU_ADDR,1);
  byte aByte = Wire.read();
 // Fio.endTransmission();
 // while(Wire.available() == 0){
    
 // }
 // byte temp = Wire.read();
// Serial.println(temp);

  if(aByte == 0x68){
  Serial.println("Encontrado MPU6050");
  }
  senão{
   Serial.println("Não foi encontrado o MPU6050");
  }
}

void processQuat(){
    pacote_processado[0] = pacote_recebido[0];
    pacote_processado[1] = pacote_recebido[1];
    pacote_processado[2] = pacote_recebido[4];
    pacote_processado[3] = pacote_recebido[5];
    pacote_processado[4] = pacote_recebido[8];
    pacote_processado[5] = pacote_recebido[9];
    pacote_processado[6] = pacote_recebido[12];
    pacote_processado[7] = pacote_recebido[13];
    
}
  
void sendQuat(){
  /*
  byte tipo de pacote = 0x02;
  botão byte = 0x00;
  Serial.print("$"); Serial.print(tipo de pacote);
  for(byte i = 0; i < 8; i++){
    Serial.print(processed_packet[i]);
  }
  Serial.print(botão); Serial.print(packetCount);
  Serial.print("\r\n");
  if(packetCount < 0xFF){packetCount++;}
  else{packetCount = 0x00;}
  */
  
  // após a conversão adaptada do exemplo TeaPot do Invensense
  q[0] = (longo) ((((longo sem sinal) pacote_processado[0]) << 8) + ((longo sem sinal) pacote_processado[1]));
  q[1] = (longo) ((((longo sem sinal) pacote_processado[2]) << 8) + ((longo sem sinal) pacote_processado[3]));
  q[2] = (longo) ((((longo sem sinal) pacote_processado[4]) << 8) + ((longo sem sinal) pacote_processado[5]));
  q[3] = (longo) ((((longo sem sinal) pacote_processado[6]) << 8) + ((longo sem sinal) pacote_processado[7]));
  for(int i = 0; i < 4; i++ ) {
    if(q[i] > 32767) {
      q[i] -= 65536;
    }
    q[i] = ((flutuante) q[i]) / 16384,0f;
  }
  
  serialPrintFloatArr(q, 4);
  Serial.print("\n");
}

void sendPacket(){
  for(byte i = 0; i < fifoCountL-1; i++){
    Serial.print(received_packet[i], HEX); Serial.print(" ");
  }
  Serial.println(received_packet[fifoCountL-1], HEX); Serial.println();
}

void sendHeader(){
  for(byte i = 0; i < 2; i++){
    Serial.print(received_packet[i], HEX); Serial.print(" ");
  }
  Serial.println();
}

void bank_sel(banco de bytes){
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6D);
  Wire.write(banco);
  Wire.endTransmission();
}

void serialPrintFloatArr(float * arr, comprimento int) {
  for(int i=0; i<comprimento; i++){
    serialFloatPrint(arr[i]);
    Serial.print(",");
  }
}


void serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  for(int i=0; i<4; i++) {
    
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    
    char c1 = (b1 < 10) ? ('0' + b1): 'A' + b1 - 10;
    caractere c2 = (b2 < 10) ? ('0' + b2): 'A' + b2 - 10;
    
    Serial.print(c1);
    Serial.print(c2);
  }
}
