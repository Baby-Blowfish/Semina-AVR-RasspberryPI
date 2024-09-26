#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>

volatile char gpio_led[8]={13,12,3,2,0,7,9,8};
volatile char gpio_sw[4]={14,30,21,22};
volatile int shift=0,delay_count=0;

void reset_led(void)
{
	for(int led=0; led<8; led++)           //풀업 방식 HIGH(OFF)로 초기화
				digitalWrite(gpio_led[led],HIGH);
}
void time_plus(int t)
{
	for(int count=0; count<t;count++)
		delay(100);
}

void sw0(void)
{
	digitalWrite(gpio_led[shift],0);
	delay(200);
	time_plus(delay_count);
	shift++;
	if(shift==8)
	{
		reset_led();
		delay(200);
		shift=0;
	}
	if(digitalRead(gpio_sw[0])==1)
	{ reset_led(); shift=0;}
}

void sw1(void)
{
	for(int led=0; led<8; led+=2)           
				digitalWrite(gpio_led[led],LOW);
	delay(200);
	time_plus(delay_count);
	reset_led();
	
	for(int led=1; led<8; led+=2)           
				digitalWrite(gpio_led[led],LOW);
	delay(200);
	time_plus(delay_count);
	reset_led();	
}
void sw2(void)
{
	delay(100);
	delay_count++;
	if(delay_count==4)
		delay_count=0;
	while(digitalRead(gpio_sw[2])==0);
	delay(100);
		
}
void sw3(void)
{
	delay(100);
	int random=0;
	random=rand()%8;
	for(int count=0; count<random;count++)
		digitalWrite(gpio_led[count],0);
	while(digitalRead(gpio_sw[3])==0);
	delay(100);
	reset_led();
}

int main(void)
{
	
	if(wiringPiSetup()==-1)
		return -1;
	
	srand(time(NULL));
	
	for(int count=0; count<8; count++)
		pinMode(gpio_led[count],OUTPUT);
	for(int count=0; count<4; count++)
		pinMode(gpio_sw[count],	INPUT);
	
	reset_led();
	
	for(;;)
	{
		
		if(digitalRead(gpio_sw[0])==0)
			sw0();
		else if(digitalRead(gpio_sw[1])==0)	
			sw1();
		else if(digitalRead(gpio_sw[2])==0)
			sw2();
		else if(digitalRead(gpio_sw[3])==0)
			sw3();
		else reset_led();
	}
	return 0;
}