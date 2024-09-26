/*
 * LED.c
 *
 * Created: 2021-07-27 오후 5:13:13
 * Author : user
 */ 
#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>  
#include <time.h>

static int count=0,flag=4,num=0;
static unsigned char FND[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
static unsigned char LED[8]={0xfe,0x0fc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};

void increase(void);
void decrease(void);
void FND_SEGMENT(void);
void LED_SHIFT(void);
void UP_DOWN_RANDOM(void);
void stop(void);

ISR(INT0_vect){             
	
	flag=0;                  //숫자증가
	    
}
ISR(INT1_vect)
{
	flag=1;  	             //숫자감소
}
ISR(INT2_vect)
{   
	flag=2;                  //정지 led 깜박이기;
	
}
ISR(INT3_vect)
{	
	_delay_ms(20);
	while(~PIND&0x08);
	_delay_ms(20);
	EIFR=0x08;                         
   flag=3;
}
int main(void)
{
    DDRA=0xff;        // A 출력포트 사용
	DDRC=0xff;        // C 출력포트 사용
	DDRD=0x30;		  // D포트 0,1,2,3 입력, 4,5 출력포트 사용
	
	EICRA=0xaa;       // 인터럽트 제어 레지스터, 하강 모서리
	EIMSK=0x0f;       // 외부 인터럽트 0,1,2,3 사용
	sei();            // SREG.I 세트 모든 인터럽트 허용
	
	PORTC=~0x00;	  // C포트 초기화
	PORTA=~0x00;	  // A포트 초기화
    while (1) 
    {
		PORTA=0xff;
		
		PORTD=~0x20;
		PORTC=~FND[0];
		_delay_us(10);
		PORTD=~0x10;
		PORTC=~FND[0];
		_delay_us(10);
				
		if(flag==0)	increase();
		else if(flag==1) decrease();
		else if(flag==2) stop();
		else if(flag==3) UP_DOWN_RANDOM();
		else continue;
	}
}

void increase(void)                 //0~99~0 반복증가
{
	
	if(count>99) count=0;
	else count++;
	FND_SEGMENT();
	
}

void decrease(void)                 // 99~0~99 반복감소
{
	count--;
	if(count<0)	count=99;
	FND_SEGMENT();
	
}

void FND_SEGMENT(void)				//FND를 동적으로 표시하는 함수
{
	LED_SHIFT();
	for(int j=0;j<100;j++){
		
	PORTD=0X20;
	PORTC=~FND[(count%100)/10];
	_delay_us(500);
	PORTD=0x10;
	PORTC=~FND[(count%10)/1];
	_delay_us(500);
	}
	
}
void LED_SHIFT(void){
	if((count>=2)&&(count<=96)) PORTA=LED[((count-1)/12)];
	else PORTA=LED[0];
}
void UP_DOWN_RANDOM(void){
	num=!num;
	srand(time(NULL));
	if(num)count=count+(rand()%100);
	else count=count-(rand()%100);
	while(~PIND&0x04){			
		FND_SEGMENT();		
	}
}
void stop(void){
	while(~PIND&0x04)
	{
		FND_SEGMENT();
		_delay_ms(30);
		PORTA=0xff;
		_delay_ms(30);
	}
}