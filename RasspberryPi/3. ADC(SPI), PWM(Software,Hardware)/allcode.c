#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>

#define SERVO 1
#define CS_MCP3208 8
#define SPI_CHANNEL 0
#define SPI_SPEED 1000000

int adcValue = 0;
int read_mcp3208_adc(unsigned char adcChannel)
{
      unsigned char buff[3];

      buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
      buff[1] = ((adcChannel & 0x07) << 6);
      buff[2] = 0x00;

     digitalWrite(CS_MCP3208, 0);            // Low : CS Active

     wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);

     buff[1] = 0x0f & buff[1];               //12비트 중 상위 4비트
     adcValue = ( buff[1] << 8) | buff[2];   //12비트 중 상위 4비트 + 12비트 중 하위 8비트

     digitalWrite(CS_MCP3208, 1);            // High : CS Inactive

     return adcValue;
}

int main(void)
{
	int adc_results[10] = {0};
	char index = 0;
	int adc_cal = 0;
	
	if(wiringPiSetup()< 0)															     //wiringPi 준비 완료 확인
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));//라즈벨파이에서 표준출력(모니터)
		return 1;
	}
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) return 1;
	
	pinMode(CS_MCP3208, OUTPUT);
	pinMode(SERVO,PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(384);
	pwmSetRange(1000);
	
	while(1)
	{
		adcValue = read_mcp3208_adc(0);
		
		adc_results[index] = adcValue;
		index++;
		if(index == 10) index = 0;
		
		adc_cal = 0;
		for(char count = 0; count < 10; count++) adc_cal += adc_results[count];
		adc_cal /= 10;
		
		printf("adcValue = %d\n", adc_cal/22);
		pwmWrite(SERVO,30+adc_cal/45);
		delay(200);
		
	}
	return 0;
}