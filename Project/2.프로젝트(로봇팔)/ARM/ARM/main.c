/*
 * ARM.c
 *
 * Created: 2021-11-23 오전 11:51:25
 * Author : user
 */ 

#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
char adcvalue_to_stfing[10]={0};

void serial_init(unsigned long baud){
	unsigned short ubrr;
	ubrr = (unsigned short) (F_CPU/(16*baud)-1);
	UBRR0H = (unsigned char)(ubrr >> 8);	//상위보레이트
	UBRR0L = (unsigned char)(ubrr & 0xff);	//하위보레이트
	UCSR0B = 0x18;// RX TX 허용
	UCSR0C = 0x06;// 비동기모드, 정지비트1bit, 문자크기8bit ,패리티x
}

void tx_data(unsigned char data)
{
	while((UCSR0A & 0X20) == 0X00); // 전송 준비가 될 때 까지 대기 데이터
	UDR0 = data;					// 시리얼 포트를 통해 데이터 전송
}
void tx_string( char *str)//문자열송신
{
	while(*str!='\0')    //문자열이 끝(NULL문자)날때까지 데이터를 보냄
	{
		tx_data(*str++);
	}
	
}
unsigned int get_ADC(unsigned char ch){
	unsigned int adc_value;
	ADCSRA=0x86;  //adc가능,adc정지,Free running모드off,변환전 플래그0,인터럽트 금지,분주비64
	ADMUX=ch|0x40; // adc ref를 내부 2.56v로 사용,오른쪽정렬,or 사용해 adc채널 ch번 사용
	ADCSRA|=1<<ADSC;// STAR CONVERSION
	
	while((ADCSRA&0x10)==0x00);//반환후 플래그 1 반환종료 기다림
	adc_value=ADC>>2;             //반환 종료후 ADC값을 adc_value 변수에 저장
	ADCSRA=ADCSRA|0x10;// 다음 conversion을위해 플래그 리셋(ADIF비트에 1을 쓴다)
	return(adc_value); //8bit로변환값을 반환함
}

int average_recursion(int x)
{
	static int average = 0,FILTERDATA=0;
	++FILTERDATA;
	average = (((FILTERDATA-1)*average)/FILTERDATA) + (x/FILTERDATA);
	return average;
}

int main(void)
{
	unsigned int adc_value=0;
	TCCR1A=0xaa;	//0b10101010  비교 일치 될때 OCnX 핀 0, TOP에서 1	출력
	TCCR1B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
	ICR1=18431;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용
	OCR1A=645;		//서보모터 초기값 0도
	OCR1B=645;		//서보모터 초기값 0도
	
	
	TCCR3A=0xaa;	//0b10101010  비교 일치 될때 OCnX 핀 0, TOP에서 1	출력
	TCCR3B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
	ICR3=18431;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용
	OCR3A=645;		//서보모터 초기값 0도
	OCR3B=645;		//서보모터 초기값 0도
	OCR3C=645;
	
	DDRB=0xff; 	//B포트 5 서보모터 출력
	DDRE=0xff;
	
	serial_init(9600);
	while (1)
	{
		
		
		adc_value=get_ADC(0x00); //채널 ch번의 ADC값을 adc_value에 저장
		OCR3A=600+5*adc_value;
		_delay_ms(4);
		
		adc_value=get_ADC(0x01); //채널 ch번의 ADC값을 adc_value에 저장
		OCR3B=500+6*adc_value;
		_delay_ms(4);
		
		adc_value=get_ADC(0x02); //채널 ch번의 ADC값을 adc_value에 저장
		OCR3C=500+4*adc_value;
		_delay_ms(4);
		
		adc_value=get_ADC(0x03); //채널 ch번의 ADC값을 adc_value에 저장
		OCR1A=500+5*adc_value;
		_delay_ms(4);
		
		adc_value=get_ADC(0x04); //채널 ch번의 ADC값을 adc_value에 저장
		OCR1B=500+6*adc_value;
		_delay_ms(4);
		
		
	}
}