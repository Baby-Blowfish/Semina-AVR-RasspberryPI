#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>

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

unsigned int get_ADC(unsigned char ch){
	unsigned int adc_value;
	ADCSRA=0x86;  //adc가능,adc정지,Free running모드off,변환전 플래그0,인터럽트 금지,분주비64
	ADMUX=ch|0xc0; // adc ref를 내부 2.56v로 사용,오른쪽정렬,or 사용해 adc채널 ch번 사용
	ADCSRA|=1<<ADSC;// STAR CONVERSION
	
	while((ADCSRA&0x10)==0x00);//반환후 플래그 1 반환종료 기다림
	adc_value=ADC>>2;             //반환 종료후 ADC값을 adc_value 변수에 저장
	ADCSRA=ADCSRA|0x10;// 다음 conversion을위해 플래그 리셋(ADIF비트에 1을 쓴다)
	return(adc_value); //8bit로변환값을 반환함
}

void serial(unsigned int ADC0);

int main(void)
{
	unsigned int adc_value;
	
	
	serial_init(9600);
	while (1)
	{
		adc_value=get_ADC(0x00); //채널 ch번의 ADC값을 adc_value에 저장
		serial(adc_value);
	}
}

void serial(unsigned int ADC0){	
	int ADC_VALUE;
	ADC_VALUE=ADC0>>2;	                              // 송신하기 위해서 아스키코드로 문자화
	tx_data((ADC_VALUE%100)/10+'0');
	tx_data((ADC_VALUE%10)+'0');
	tx_data(0x0d);
	tx_data('\n');
	_delay_ms(500);
}