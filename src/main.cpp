#include <Arduino.h>
#include <SPI.h>
#include <stdio.h>
#include <stdarg.h>

/*

주의 Protoceltral 보드의 실크중 MISO, MOSI가 바뀌어 있다.

*/

#define WREG					0x40
#define RREG					0x20
#define ADS1292_ID				0x00
#define ADS1292_CONFIG1			0x01
#define ADS1292_CONFIG2			0x02
#define ADS1292_LOFF			0x03
#define ADS1292_CH1SET			0x04
#define ADS1292_CH2SET			0x05
#define ADS1292_RLDSENS			0x06
#define ADS1292_LOFFSENS		0x07
#define ADS1292_LOFFSTAT		0x08
#define ADS1292_RESP1			0x09
#define ADS1292_RESP2			0x0A
#define ADS1292_GPIO			0x0B

// System command 
#define WAKEUP      0x02    // wake up from standby mode
#define STANDBY     0x04
#define RESET       0x06
#define START       0x08
#define STOP        0x0A
#define OFFSETCAL   0x1A

// Data read command 
#define RDATAC      0x10    // Enable read data continous mode
#define SDATAC      0x11    // Stop read data continous mode
#define RDATA       0x12    // Read data

const int ADS1292_DRDY_PIN = 6;
const int ADS1292_CS_PIN = 10;
const int ADS1292_START_PIN = 5;
const int ADS1292_PWDN_PIN = 4;

uint8_t id;

void ads1292Reset(const int pwdnPin)
{
  digitalWrite(pwdnPin, HIGH);
  delay(100);					// Wait 100 mSec
  digitalWrite(pwdnPin, LOW);
  delay(100);
  digitalWrite(pwdnPin, HIGH);
  delay(100);
}

void ads_cs(uint8_t level) 
{
	if (level == LOW) {
		digitalWrite(ADS1292_CS_PIN, LOW);
	}else {
		digitalWrite(ADS1292_CS_PIN, HIGH);
	}
}

int ads1292RegWrite(uint8_t add, uint8_t value)
{
	uint8_t sendBuf[3] = {uint8_t(add | WREG), 0, value};
	uint8_t pTxTemp;

	ads_cs(LOW);

	pTxTemp = SDATAC;
	SPI.transfer(pTxTemp);
	SPI.transfer(sendBuf[0]);
	SPI.transfer(sendBuf[1]);
	SPI.transfer(sendBuf[2]);

	ads_cs(HIGH);

	return 0;
}

uint8_t ads1292RegRead(uint8_t add)
{
	uint8_t sendBuf[2] = {uint8_t(add | RREG), 0};
	uint8_t returnValue = 0;
	uint8_t pTxTemp;

	ads_cs(LOW);
	
	pTxTemp = SDATAC;
	SPI.transfer(pTxTemp);
	SPI.transfer(sendBuf[0]);
	SPI.transfer(sendBuf[1]);
	returnValue = SPI.transfer(0x00);

	ads_cs(HIGH);

	return returnValue;
}

int ads1292PrintRegister(void)
{
	uint8_t pRxTemp;
#if 0  
	char *regName[] = {"ID-------", "CONFIG1--", "CONFIG2--", "LOFF-----", "CH1SET---", \
				"CH2SET---", "RLD_SENS-", "LOFF_SENS", "LOFF_STAT", "RESP1----", "RESP2----", \
				"GPIO-----" };
#endif

	Serial.print("**** Register Value ****\r\n");
	for(uint8_t i=0; i<12; i++)
	{
		pRxTemp = ads1292RegRead(i);
		// dbg("%s", regName[i]);
		// dbg("[%02Xh]: 0x%02X\r\n", i, pRxTemp);
    Serial.println(pRxTemp);
	}

	return 0;
}


void setup() {

    delay(1000);

    Serial.begin(115200);

    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1);
    SPI.setClockDivider(SPI_CLOCK_DIV16);

    pinMode(ADS1292_DRDY_PIN, INPUT);
    pinMode(ADS1292_CS_PIN, OUTPUT);
    pinMode(ADS1292_START_PIN, OUTPUT);
    pinMode(ADS1292_PWDN_PIN, OUTPUT);

    digitalWrite(ADS1292_START_PIN, LOW);
    digitalWrite(ADS1292_CS_PIN, HIGH);

    ads1292Reset(ADS1292_PWDN_PIN);

    Serial.println("\r\nInitiliziation is done");
    id = ads1292RegRead(ADS1292_ID);
    Serial.print("ADS1292 ID = ");
    Serial.println(id, HEX);

    // ads1292PrintRegister();

}

void loop() {
  // put your main code here, to run repeatedly:

    id = ads1292RegRead(ADS1292_GPIO);
    Serial.print("VAL = ");
    Serial.println(id, HEX);

}