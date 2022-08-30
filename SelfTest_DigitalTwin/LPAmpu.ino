
//===================================================================================================================
//====== Conjunto de funções úteis para acessar dados de aceleração, giroscópio e temperatura
//===================================================================================================================

void getGres() {
  switch (Gscale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
    case GFS_250DPS:
      gRes = 250.0 / 32768.0;
      break;
    case GFS_500DPS:
      gRes = 500.0 / 32768.0;
      break;
    case GFS_1000DPS:
      gRes = 1000.0 / 32768.0;
      break;
    case GFS_2000DPS:
      gRes = 2000.0 / 32768.0;
      break;
  }
}

void getAres() {
  switch (Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    case AFS_2G:
      aRes = 2.0 / 32768.0;
      break;
    case AFS_4G:
      aRes = 4.0 / 32768.0;
      break;
    case AFS_8G:
      aRes = 8.0 / 32768.0;
      break;
    case AFS_16G:
      aRes = 16.0 / 32768.0;
      break;
  }
}


void readAccelData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z accel register data stored here
  readBytes(MPU6050_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
  destination[0] = (int16_t)((rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)((rawData[2] << 8) | rawData[3]) ;
  destination[2] = (int16_t)((rawData[4] << 8) | rawData[5]) ;
}

void readGyroData(int16_t * destination)
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes(MPU6050_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = (int16_t)((rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = (int16_t)((rawData[2] << 8) | rawData[3]) ;
  destination[2] = (int16_t)((rawData[4] << 8) | rawData[5]) ;
}

int16_t readTempData()
{
  uint8_t rawData[2];  // x/y/z gyro register data stored here
  readBytes(MPU6050_ADDRESS, TEMP_OUT_H, 2, &rawData[0]);  // Read the two raw data registers sequentially into data array
  return ((int16_t)rawData[0]) << 8 | rawData[1] ;  // Turn the MSB and LSB into a 16-bit value
}



// Configure the motion detection control for low power accelerometer mode
void LowPowerAccelOnlyMPU6050()
{

  // The sensor has a high-pass filter necessary to invoke to allow the sensor motion detection algorithms work properly
  // Motion detection occurs on free-fall (acceleration below a threshold for some time for all axes), motion (acceleration
  // above a threshold for some time on at least one axis), and zero-motion toggle (acceleration on each axis less than a
  // threshold for some time sets this flag, motion above the threshold turns it off). The high-pass filter takes gravity out
  // consideration for these threshold evaluations; otherwise, the flags would be set all the time!

  uint8_t c = readByte(MPU6050_ADDRESS, PWR_MGMT_1);
  writeByte(MPU6050_ADDRESS, PWR_MGMT_1, c & ~0x30); // Clear sleep and cycle bits [5:6]
  writeByte(MPU6050_ADDRESS, PWR_MGMT_1, c |  0x30); // Set sleep and cycle bits [5:6] to zero to make sure accelerometer is running

  c = readByte(MPU6050_ADDRESS, PWR_MGMT_2);
  writeByte(MPU6050_ADDRESS, PWR_MGMT_2, c & ~0x38); // Clear standby XA, YA, and ZA bits [3:5]
  writeByte(MPU6050_ADDRESS, PWR_MGMT_2, c |  0x00); // Set XA, YA, and ZA bits [3:5] to zero to make sure accelerometer is running

  c = readByte(MPU6050_ADDRESS, ACCEL_CONFIG);
  writeByte(MPU6050_ADDRESS, ACCEL_CONFIG, c & ~0x07); // Clear high-pass filter bits [2:0]
  // Set high-pass filter to 0) reset (disable), 1) 5 Hz, 2) 2.5 Hz, 3) 1.25 Hz, 4) 0.63 Hz, or 7) Hold
  writeByte(MPU6050_ADDRESS, ACCEL_CONFIG,  c | 0x00);  // Set ACCEL_HPF to 0; reset mode disbaling high-pass filter

  c = readByte(MPU6050_ADDRESS, CONFIG);
  writeByte(MPU6050_ADDRESS, CONFIG, c & ~0x07); // Clear low-pass filter bits [2:0]
  writeByte(MPU6050_ADDRESS, CONFIG, c |  0x00);  // Set DLPD_CFG to 0; 260 Hz bandwidth, 1 kHz rate

  c = readByte(MPU6050_ADDRESS, INT_ENABLE);
  writeByte(MPU6050_ADDRESS, INT_ENABLE, c & ~0xFF);  // Clear all interrupts
  writeByte(MPU6050_ADDRESS, INT_ENABLE, 0x40);  // Enable motion threshold (bits 5) interrupt only

  // Motion detection interrupt requires the absolute value of any axis to lie above the detection threshold
  // for at least the counter duration
  writeByte(MPU6050_ADDRESS, MOT_THR, 0x80); // Set motion detection to 0.256 g; LSB = 2 mg
  writeByte(MPU6050_ADDRESS, MOT_DUR, 0x01); // Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate

  delay (100);  // Add delay for accumulation of samples

  c = readByte(MPU6050_ADDRESS, ACCEL_CONFIG);
  writeByte(MPU6050_ADDRESS, ACCEL_CONFIG, c & ~0x07); // Clear high-pass filter bits [2:0]
  writeByte(MPU6050_ADDRESS, ACCEL_CONFIG, c |  0x07);  // Set ACCEL_HPF to 7; hold the initial accleration value as a referance

  c = readByte(MPU6050_ADDRESS, PWR_MGMT_2);
  writeByte(MPU6050_ADDRESS, PWR_MGMT_2, c & ~0xC7); // Clear standby XA, YA, and ZA bits [3:5] and LP_WAKE_CTRL bits [6:7]
  writeByte(MPU6050_ADDRESS, PWR_MGMT_2, c |  0x47); // Set wakeup frequency to 5 Hz, and disable XG, YG, and ZG gyros (bits [0:2])

  c = readByte(MPU6050_ADDRESS, PWR_MGMT_1);
  writeByte(MPU6050_ADDRESS, PWR_MGMT_1, c & ~0x20); // Clear sleep and cycle bit 5
  writeByte(MPU6050_ADDRESS, PWR_MGMT_1, c |  0x20); // Set cycle bit 5 to begin low power accelerometer motion interrupts

}
