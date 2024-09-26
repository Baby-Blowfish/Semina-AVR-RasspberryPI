#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define  ReadyToSend 0xff

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

unsigned char rx_data(void)
{
	while((UCSR0A & 0X80) == 0X00); // 데이터가 다 들어 올떄 까지 대기 데이터
	return UDR0;				// 시리얼 포트를 통해 데이터 받음
}

static unsigned char FND[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
volatile unsigned int adc_value=0,temp=0,flag=0;
volatile unsigned char dat=0,led_input=0;

void FND_SEGMENT(void); 
void LED(void);
void serial(void);

ISR(INT0_vect)
{
	flag=1;
}
ISR(INT1_vect)
{
	flag=2;	
}
ISR(INT2_vect)
{
	flag=3;
}

int main(void)
{
	
	
	DDRA=0xff;        // A 출력(LED)
	DDRC=0xff;        // C 출력(FND)
	DDRD=0x30;		  // D포트 0,1,2,3 입력(스위치), 4,5 출력(FND 동적구동)
	EICRA=0xaa;       // 인터럽트 제어 레지스터, 하강 모서리
	EIMSK=0x0f;       // 외부 인터럽트 0,1,2,3 사용
	
	ADCSRA=0xe7; //adc 가능, adc변환시작,free running mode,인터럽트금지,변환속도1/128
	ADMUX=0xc1; //adc ref 내부 2.56v로 사용,오른쪽정렬(0), adc채널 1번사용 싱글엔드
	
	sei();            // SREG.I 세트 모든 인터럽트 허용
	
	PORTC=~0x00;	  // C포트 초기화
	PORTA=~0x00;	  // A포트 초기화
	serial_init(9600);
	while (1)
	{
		PORTA=0xff;
		if(flag==1)LED();
		else if(flag==2)FND_SEGMENT();
		else if(flag==3)serial();
		else continue;	
			
		
	}
}

void LED(void){
	
	if((led_input-'0')==8)
	PORTA=~((led_input-'0')<<1);
	else if((led_input-'0')==9)
	PORTA=~((led_input-'0')>>1);
	else {
		led_input=rx_data();
		PORTA=~(led_input-'0');
	}
	
}
void FND_SEGMENT(void){
	
	while((ADCSRA&0x01)==0x00); //반환종료를 기다림
	adc_value=(ADC>>2);
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

void serial(void){	
	int ADC_VALUE;
	ADC_VALUE=ADC>>2;	                              // 송신하기 위해서 아스키코드로 문자화
	tx_data((ADC_VALUE%100)/10+'0');
	tx_data((ADC_VALUE%10)+'0');
	tx_data(0x0d);
	tx_data('\n');
	_delay_ms(500);
}

