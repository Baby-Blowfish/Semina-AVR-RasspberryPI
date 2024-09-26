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

char FND[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}, word[10]="";
volatile unsigned int adc_value=0,flag=0,count=0,sw_1_angle=0;

void serial_init(unsigned long baud)
{//USART0 RX:PE0   TX:PE1   SERIAL 초기화
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
		*str++=dat;   
	}
	*str++='\r';
	*str++='\n';
	*str='\0';      //문자열 끝에는 NULL문자 필요
}

void FND_SEGMENT(int fnd);  //fnd 동적구동

void SERIAL_OUTPUT(char serial[],int value);

void SERIALNUM_FND_ANGLE(void);   //SW0 시리얼창0~99입력  0~180서보모터,각도FND출력

void SERVOMOTOR_THREEMODE(void);  //SW1 모드1 서보모터 증가   모드2 서보모터 정지  모드3 각도0 시리얼중지

void ADCMODE_CDS_R(void);         //SW2 CDS는 FREERUNNIGMODE작동 OR R은 인터럽트방식작동

void CHANGE_ADC_TO_SERVOMOTER(void);;

ISR(INT0_vect)             //SW0
{
	_delay_ms(100); //디바운싱
	while(~PIND&0x01);
	_delay_ms(100); 
	EIFR|=0x01;     // 외부인터럽트0에대한 강제로 클리어 다시인터럽트를 받아들일수있는상태로
	rx_string(word);
	tx_string(word);
	flag=1;
}
ISR(INT1_vect)             //SW1
{
	_delay_ms(100); //디바운싱
	while(~PIND&0x02);
	_delay_ms(100); //디바운싱
	EIFR|=0x02;     // 외부인터럽트1에대한 강제로 클리어 다시인터럽트를 받아들일수있는상태로
	flag=2;
	count=count+1;
	if(count>3) count=1;
	
}
ISR(INT2_vect)             //SW2
{
	_delay_ms(100); //디바운싱
	while(~PIND&0x04);
	_delay_ms(100); //디바운싱
	EIFR|=0x04;     // 외부인터럽트2에대한 강제로 클리어 다시인터럽트를 받아들일수있는상태로
	rx_string(word);
	tx_string(word);
	flag=3;
	
}
ISR(INT3_vect)             //SW3
{
	flag=4;
}
ISR(ADC_vect)             //ADC인터럽트사용
{
	adc_value=(ADC>>2);
}


int main(void)
{
	DDRC=0xff;        // C 출력(FND)
	DDRD=0x30;		  // D포트 0,1,2,3 입력(스위치), 4,5 출력(FND 동적구동)
	DDRB=0x20; 	      //OC1A=PB5로 PWM신호 출력 서보모터가 받음
	
	PORTC=~0x00;	  // C포트 초기화
	
	EICRA=0xaa;       // PD(0,1,2,3) 하강 모서리에서 트리거
	EIMSK=0x0f;       // 외부 인터럽트 0,1,2,3 사용
	
	TCCR1A=0x82;	//0b10000010  비교 일치 될때 OCnX 핀 0, TOP에서 1 출력
	TCCR1B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
	ICR1=18431;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용
	OCR1A=645;		//서보모터 초기값 0도

	sei();            // SREG.I 세트 모든 인터럽트 허용
	serial_init(2400);    
	
	
	while (1)
	{
		
		switch(flag)
		{
			case 1: SERIALNUM_FND_ANGLE();      break;
			case 2: SERVOMOTOR_THREEMODE();     break;
			case 3: ADCMODE_CDS_R();            break;
			case 4: CHANGE_ADC_TO_SERVOMOTER(); break;
			default: flag=0;
		}
	}
}
void FND_SEGMENT(int fnd)  //fnd 동적구동
{
	unsigned char temp=0;
	for(int j=0;j<100;j++)
	{
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
void SERIAL_OUTPUT(char serial[],int value)   //정수 문자열로 변환 후 시리얼 출력
{
	sprintf(serial,"%d",value);
	tx_string(serial);
	tx_string("\r\n");
}
void SERIALNUM_FND_ANGLE(void)   //SW0 시리얼창0~99입력  0~180서보모터,각도FND출력
{
	int sw_0_angle=0;
	if(word[1]=='\r')
	{	sw_0_angle=16*(word[0]-'0');
		OCR1A=645+sw_0_angle;
	}
	else if(word[2]=='\r')
	{
		sw_0_angle=16*((word[0]-'0')*10+(word[1]-'0'));
		OCR1A=645+sw_0_angle;
	}
	FND_SEGMENT(sw_0_angle/16);
}
void SERVOMOTOR_THREEMODE(void)  //SW1 모드1 서보모터 증가   모드2 서보모터 정지  모드3 각도0 시리얼중지
{
	char serial_sw1_angle[10]="";
	
	switch(count){
		case 1:
		OCR1A=sw_1_angle+654;
		sw_1_angle=sw_1_angle+8;
		if(sw_1_angle>1440) sw_1_angle=0;
		SERIAL_OUTPUT(serial_sw1_angle,sw_1_angle/8);
		break;
		
		case 2:SERIAL_OUTPUT(serial_sw1_angle,sw_1_angle/8);  break;
		
		default:
		OCR1A=645;
		break;
	}
	
	
}
void ADCMODE_CDS_R(void)           //SW2 CDS는 FREERUNNIGMODE작동 OR R은 인터럽트방식작동
{
	char serial_sw2_adc_value[10]="";
	
	if(!strcmp(word,"CDS\r\n"))     //문자CDS가 입력되었을 경우 free runnig 모드
	{
		ADMUX=0xc0; // adc ref를 내부 2.56v로 사용(11),오른쪽정렬(0),adc채널 0번 사용(00000 cds)
		ADCSRA=0xe6;  //adc가능(1),adc시작(1),Free running모드on(1),변환전 플래그0(0),인터럽트 금지(0),분주비64(110)
		adc_value=(ADC>>2);
		ADCSRA=ADCSRA|0x10;               //변환완료플래그 리셋
	}
	else if(!strcmp(word,"R\r\n"))    //문자R이 입력되었을 경우  인터럽트방식
	{
		ADMUX=0xc1; // adc ref를 내부 2.56v로 사용(11),오른쪽정렬(0),adc채널 1번 사용(00001 R)
		ADCSRA=0xee;  //adc가능(1),adc시작(1),Free running모드on(1),변환전 플래그0(0),인터럽트 허용(1),분주비64(110)
	}
	SERIAL_OUTPUT(serial_sw2_adc_value,adc_value);
}
void CHANGE_ADC_TO_SERVOMOTER(void)         //SW3 ADC값을 PWM신호로 변환하여 서보모터 구동
{
	char serial_sw3_adc_value[10]="";
	
	OCR1A=ADC*1.62+645;                    //0~1024를 645~2300으로 범위 변환
	
	SERIAL_OUTPUT(serial_sw3_adc_value,ADC);
}






