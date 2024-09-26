#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>  
#include <softPwm.h>

#define SERVO 5

unsigned int fd;


void I2C_Seting(unsigned char Channel)
{
	//wiringPiI2CWriteReg8(fd, device 주소, bit data)
	wiringPiI2CWriteReg8 (fd, 0x00, 0x01);	// RR(1)모든 레지스터 초기화
	wiringPiI2CWriteReg8 (fd, 0x00, 0x96);	
	// RR(0) 표준동작, PUD(1) 디지털회로 전원 ON ,PUA(1) 아날로그회로 전원 ON,CS(1) ADC 변환시작, AVDDS(1) 내부AVDD사용
	wiringPiI2CWriteReg8 (fd, 0x01, 0x11);	//VLDO 011(3.6V), 실제로 측정되는 값이 3.3V임
	wiringPiI2CWriteReg8(fd, 0x02, (Channel << 7));//CHS(0)(default)>> CH1,CHS(1)>>CH2
	wiringPiI2CWriteReg8 (fd, 0x15, 0x30);	//ADC 레지스터 부분, 클럭을 꺼짐('1')상태 설정
	
	//wiringPiI2CWriteReg8 (fd, 0x11, 0x30); 
	// 내부풀업설정 SPE(1),WPD(1)>> 1.6K옴    SPE(0),WPD(0) >> 50k옴(default)
	//wiringPiI2CWriteReg8 (fd, 0x1c, 0x80);	
	//단일채널사용시 높은 PGA게인 설정에서 향상된 ENOB를 위해 Cfilter 추가 가능 이떄 3.3V(330pF),4.5V(680pF)
	                                           
}

int ADC()
{
	int adc_data=0;
	unsigned int data_H=0, data_M=0, data_L=0;
	int st = 0;
	
	do
	{
		st = wiringPiI2CReadReg8(fd,0x00); 
	}
	while( st & 0x28 == 0x00 );  // CR(1) ADC DATA ready, PUR(1) Power Up ready
	data_H = wiringPiI2CReadReg8(fd, 0x12);     //23:16 상위 비트  읽는 순서 중요!
	data_M = wiringPiI2CReadReg8(fd, 0x13);     //15:8 중간 비트
	data_L = wiringPiI2CReadReg8(fd, 0x14);     //7:0 하위 비트

	adc_data = (data_H<<16)|(data_M<<8)|(data_L);  // 24bit의 길이로 data를 합친다.
	return adc_data;    // 0~8388608(23bit) 싱글입력 부호화 시 양의 범위만 사용하므로 전체범위의 절반만 사용하게 된다.
}                   
int main()
{
	unsigned int adcvalue=0,adc_cal=0;
	int adc_results[10] = {0};
	char index = 0;
	
	
	if(wiringPiSetup() < 0)
	{
		fprintf (stdout, "Unable to start wiringPi : %s\n",strerror(errno));
		return 1;
	}
	
	if ((fd = wiringPiI2CSetup(0x2a))<0) //NAU7802KG I2C 확인 (반환값 : 표준리눅스 파일, -1이면 오류)
	{
		fprintf (stdout, "Unable to initialise I2C : %s\n",strerror(errno));
		return 1;
	}
	
	I2C_Seting(0); 
	
	pinMode(SERVO,PWM_OUTPUT);
	
	softPwmCreate(SERVO,0,200);
	
	
	while(1)
	{
		adcvalue = ADC();
		
		adc_results[index] = adcvalue;
		index++;
		if(index == 10) index = 0;
		
		adc_cal = 0;
		for(char count = 0; count < 10; count++) adc_cal += adc_results[count];
		adc_cal /= 10;
		printf("Variable resistance ADC = %d \nangle=%d\n" ,adc_cal,10*(adc_cal/466006));
	
		softPwmWrite(SERVO,6+adc_cal/466006); 
		delay(100);
	}
}