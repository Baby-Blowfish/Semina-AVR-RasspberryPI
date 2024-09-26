/*
 * ex1.c
 *
 * Created: 2021-09-09 오후 9:07:03
 * Author : user
 */ 

#include <avr/io.h>
#define F_CPU 7372800UL
#include <util/delay.h>


int main(void)
{
	DDRB=0x20; 	      //B포트 5 서보모터 출력
    TCCR1A=0x82;	//0b10000010  비교 일치 될때 OCnX 핀 0, TOP에서 1 출력
    TCCR1B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
    ICR1=18431;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용
    OCR1A=645;		//서보모터 초기값 0도
	
	
    while (1) 
    {
		_delay_ms(1000);
		OCR1A=2400;
		_delay_ms(1000);
		OCR1A=645;
    }
}

