#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


static unsigned char FND[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
//static unsigned char LED[8]={0xfe,0x0fc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};
volatile unsigned int adc_value=0;

void ADC_init(char ch){
	ADCSRA=0xef; //adc 가능, adc변환시작,free running mode,,인터럽트허용,분주비128
	ADMUX=ch|0xc0; //adc ref 내부 2.56v로 사용,오른쪽정렬(0), adc채널 0번사용
}

ISR(ADC_vect){
	adc_value=(ADC>>2);// 인터럽트방식은 변환 후 자동리셋(bit4:1) ADCSRA|=1<<ADSC;필요없음
}                      // 10_BIT의 분해능을 온도센서 8-BIT로 해야 온도가 정상적으로 나옴

int main(void)
{
	unsigned int temp=0;
	
	DDRA=0xff;        // A 출력(LED)
	DDRC=0xff;        // C 출력(FND)
	DDRD=0x30;		  // D포트 0,1,2,3 입력(스위치), 4,5 출력(FND 동적구동)
	EICRA=0xaa;       // 인터럽트 제어 레지스터, 하강 모서리
	EIMSK=0x0f;       // 외부 인터럽트 0,1,2,3 사용
	
	ADC_init(0);
	sei();            // SREG.I 세트 모든 인터럽트 허용
	
	PORTC=~0x00;	  // C포트 초기화
	PORTA=~0x00;	  // A포트 초기화
	while (1)
	{
		PORTA=0xff;
		
		
		for(int j=0;j<100;j++){
			
			PORTD=0X20;
			temp=(adc_value%100)/10;
			PORTC=~FND[temp];
			_delay_us(500);
			PORTD=0x10;
			temp=(adc_value%10)/1;
			PORTC=~FND[temp];
			_delay_us(500);
		}
		
	}
}