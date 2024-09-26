/*
 * timercounterreal.c
 *
 * Created: 2021-09-06 오전 10:12:13
 * Author : user
 */ 
#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>

char FND[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
volatile unsigned int adc_value=0,flag=0;

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
ISR(INT3_vect)
{
	flag=6;
}
ISR(ADC_vect){
	adc_value=(ADC>>2);// 인터럽트방식은 변환 후 자동리셋(bit4:1) ADCSRA|=1<<ADSC;필요없음
}

void serial_init(unsigned long baud){//USART0 RX:PE0   TX:PE1   SERIAL 초기화
	unsigned short ubrr;
	ubrr = (unsigned short) (F_CPU/(16*baud)-1);
	UBRR0H = (unsigned char)(ubrr >> 8);	//상위보레이트
	UBRR0L = (unsigned char)(ubrr & 0xff);	//하위보레이트
	UCSR0B = 0x18;// 수신완료송신완료데이터빔인터럽트금지(000),송신가능(1),수신가능(1),문자크기8bit(0),수신송신문자비트(0) 
	UCSR0C = 0x06;// 예약비트(0),비동기모드(0),패리티모드사용X(00),STOP비트1개(0),문자크기8bit(11),클럭극성사용X(0)
                  //USCR0A=0 수신완료플래그(0),송신완료플래그(0),데이터빔플래그(0),프레임에러(0),오버런플래그(0),패리티에러(0),비동기모드(0),다중프로세서(0)	
}
void tx_data(char data)//송신
{
	while((UCSR0A & 0X20) == 0X00); //데이터가 송신shift register로 넘어가 UDR0가 비었을때(1)데이터레지스터빔인터럽트발생가능
	UDR0 = data;		            //데이터가 송신준비가될때까지 대기
}                                   //컴퓨터  송수신shiftregister <<  UDR0   ATmega128
unsigned char rx_data(void)//수신
{
	while((UCSR0A&0X80)==0X00);//수신shift register에서 UDR0로 데이터가 들어올때(1)수신완료인터럽트 발생가능
	return UDR0;                //데이터가 쓰여질때까지 대기
}                                 //컴퓨터  송수신shiftregister >> UDR0   ATmega128

void tx_string(char *str)//문자열송신
{
	while(*str!='\0')    //문자열이 끝(NULL문자)날때까지 데이터를 보냄
	{
		tx_data(*str++);
	}
	
}
void rx_string(char*str) //문자열 수신     |n 다음줄로 이동     |r 첫 줄로 이동   |0 제어문자 널문자
{
	char dat; 
	while(1)
	{
		dat=rx_data(); //데이터를 수신
		if(dat=='\r') break;   //엔터를 누르면 데이터수신종료
		*str++=dat;   //포인터
	}
	*str++='\r';
	*str++='\n';
	*str='\0';      //문자열 끝에는 NULL문자 필요
}
  
void FND_SEGMENT(int fnd){
     unsigned char temp=0;
	for(int i=0;i<10;i++){
	for(int j=0;j<100;j++){
		
		PORTD=0X20;
		temp=(fnd%100)/10;
		PORTC=~FND[temp];
		_delay_us(500);
		PORTD=0x10;
		temp=(fnd%10)/1;
		PORTC=~FND[temp];
		_delay_us(500);
	}
	
	}
}
	
int main(void)
{
	DDRC=0xff;        // C 출력(FND)
	DDRD=0x30;		  // D포트 0,1,2,3 입력(스위치), 4,5 출력(FND 동적구동)
	DDRB=0x20; 	      //B포트 5 서보모터 출력
	
	PORTC=~0x00;	  // C포트 초기화
	
	EICRA=0xaa;       // 인터럽트 제어 레지스터, 하강 모서리
	EIMSK=0x0f;       // 외부 인터럽트 0,1,2,3 사용
	
	TCCR1A=0x82;	//0b10000010  비교 일치 될때 OCnX 핀 0, TOP에서 1 출력
	TCCR1B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
	ICR1=18431;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용
	OCR1A=645;		//서보모터 초기값 0도

	sei();            // SREG.I 세트 모든 인터럽트 허용
	serial_init(9600);
	
	char word[5]="",ADC_value[10]="";
	int angle=0;
	
	while (1)
	{
			switch(flag){
				case 1: 
				if(flag==1){
					rx_string(word);
					tx_string(word);
					if(word[1]=='\r'){
						angle=10*(word[0]-'0');
						OCR1A=645+angle;
						FND_SEGMENT(angle/10);
					}
				else if(word[2]=='\r'){
						angle=10*((word[0]-'0')*10+(word[1]-'0'));
						OCR1A=645+angle;
						FND_SEGMENT(angle/10);
					}
				}break;
				case 2: break;
				case 3: 
				rx_string(word);
				tx_string(word); 
				if(!strcmp(word,"CDS\r\n")) //문자CDS가 입력되었을 경우 free runnig 모드
				{
					ADMUX=0xc0; // adc ref를 내부 2.56v로 사용(11),오른쪽정렬(0),adc채널 0번 사용(00000 cds)
					ADCSRA=0xe6;  //adc가능(1),adc시작(1),Free running모드on(1),변환전 플래그0(0),인터럽트 금지(0),분주비64(110)
				}
					
					              
				else if(!strcmp(word,"R\r\n"))   //문자R이 입력되었을 경우  인터럽트방식
				{
					ADMUX=0xc1; // adc ref를 내부 2.56v로 사용(11),오른쪽정렬(0),adc채널 0번 사용(00000 cds)
					ADCSRA=0xee;  //adc가능(1),adc시작(1),Free running모드on(1),변환전 플래그0(0),인터럽트 금지(0),분주비64(110)
				}
				flag=4;
				break;
				case 4: 
				while((ADCSRA&0x10)==0x00);       //반환종료를 기다림
				adc_value=(ADC>>2);
				ADCSRA=ADCSRA|0x10;               //변환완료플래그 리셋
				sprintf(ADC_value,"%d",adc_value);
				tx_string(ADC_value);
				tx_string("\r\n");
				_delay_ms(1000);
				break;
				}
				case 5:
				break;
	}
}




