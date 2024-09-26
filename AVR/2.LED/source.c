/*
 * LED.c
 *
 * Created: 2021-07-06 오후 10:56:45
 * Author : user
 */ 

#include <avr/io.h>                         //i/o 레지스터를 사용하기위해 WinAVR의 헤더파일
#include <util/delay.h>                     //_delay_ms()함수 사용하기 위한 헤더파일
#include <stdlib.h>                         //random()함수 사용하기 위한 헤더파일
#define DEBOUNCING_DELAY 10

unsigned char delay_value=1;

void delay( unsigned char n);
void SW_0(void);
void SW_1(void);
void SW_2(unsigned char num);
void SW_3(void);

int main(void)
{
	unsigned char seed=0, led=0x01, i=0, random_num=0;
	
	DDRA=0xff;
	DDRC=0x00;
	
	while(1)
	{
		seed++;
		srand(seed);
		random_num=rand()%8; //램덤함수 seed값 변경
		
		for(i=0;i<8;i++) // 평상시 2Hz led 1개 shift
		{
			PORTA=~led; // 출력A 풀업저항 달려있는 led
			led=(led<<1);
			if(led==0x00) led=0x01;
			
			SW_0();     // delay 느리게
			SW_1();     // delay 빠르게
			
			delay(delay_value); //delay 함수
			
			if((~PINC&0x0f)==0x04){ //led random shift
				_delay_ms(DEBOUNCING_DELAY);
				
				while((~PINC&0xf)==0x04);
				_delay_ms(DEBOUNCING_DELAY);
				
				SW_2(random_num);
			}
			
			else if((~PINC&0x0f)==0x08) {//accumulate led
				_delay_ms(DEBOUNCING_DELAY);
				SW_3();
			}
			
		}
	}
	
}

void delay( unsigned char n) {           //delay 함수
	for(;n>0;n--){
	_delay_ms(250);}
}

void SW_0(void){                         //shift 시간 250ms씩 증가 풀업방식 입력  nonblocking방식
	if((~PINC&0x0f)==0x01){
		_delay_ms(DEBOUNCING_DELAY);

		while((~PINC&0x0f)==0x01);       //스위치를 뗄떼까지 blocking함
		_delay_ms(DEBOUNCING_DELAY);
		delay_value++;
	}
}

void SW_1(void){                        //shift 시간 250ms씩 감소
	if((~PINC&0x0f)==0x02){
		_delay_ms(DEBOUNCING_DELAY);

		while((~PINC&0x0f)==0x02);
		_delay_ms(DEBOUNCING_DELAY);
		delay_value--;
	}
}

void SW_2(unsigned char num){            //led random shift
	unsigned char pattern[8]={ 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
	unsigned char i;
	delay_value=4;
	while(1)
	{
		for(i=0;i<8;i++)
		{
			PORTA=~pattern[num];
			pattern[num]=pattern[num]<<1;
			if(7<=num+i)
			pattern[num]=pattern[num]|0x01;
			
			SW_0();
			SW_1();
			delay(delay_value);
			
		}
		if((~PINC&0x0f)==0x08)
		{
			_delay_ms(DEBOUNCING_DELAY);
			break;
		}
	}
}

void SW_3(void){
	
	unsigned char led=0x00,i=0;
	
	while((~PINC&0x0f)==0x08)
	{
		delay(1);
		PORTA=~led;
		led=led<<1;
		led=led|0x01;
		if(i%9==0) led=0x00;
		i++;
	}
	
	delay(2);
	
	if(i%2==0)
	for(;i-2>0;i--)
	{
		delay(2);
		PORTA=0x55;
		delay(2);
		PORTA=0xff;
	}
	else
	for(;i-2>0;i--)
	{
		delay(2);
		PORTA=0xaa;
		delay(2);
		PORTA=0xff;
	}
}



	
	
		 
		 
	
			
			
			
	
	
