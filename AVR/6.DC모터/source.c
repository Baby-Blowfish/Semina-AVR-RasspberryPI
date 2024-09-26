#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

volatile unsigned int flag=0,Flag=2, cm=0;

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
void tx_data( char data)//송신
{
	while((UCSR0A & 0X20) == 0X00); //데이터가 송신shift register로 넘어가 UDR0가 비었을때(1)데이터레지스터빔인터럽트발생가능
	UDR0 = data;		            //데이터가 송신준비가될때까지 대기
}                                   //컴퓨터  송수신shiftregister <<  UDR0   ATmega128

void tx_string( char *str)//문자열송신
{
	while(*str!='\0')    //문자열이 끝(NULL문자)날때까지 데이터를 보냄
	{
		tx_data(*str++);
	}
	
}
void speedchanges_according_distance(void)
{
	char distance[10]={0};   //거리변수 
	PORTE = 0x10;     //10us동안 trig 신호 보냄
	_delay_us(10);
	PORTE = 0x00;
	
	sprintf(distance,"%d\r\n",cm); //정수 cm를 문자열로 변환
	tx_string(distance);           // 문자열 cm를 출력
	
	
	
	if (cm < 11)                      //0>>10갈수록 모터 속도 증가  11,12모터 정지 13>>30갈수록 모터 속도 증가
	{
		OCR1B = cm*3000;
	}
	else if ((cm==11)||(cm == 12)) OCR1B=0;
	else if ((12 < cm)&&(cm < 30))
	{
		OCR1B = (cm-12)*3000;
	}
}


ISR(INT5_vect)		//에코 인터럽트 PE5   (INT5)
{
	if(flag==0)         //처음 8개의 초음파 버스트 보내고 echo신호가 상승엣지가 될때
	{
		TCNT3=0; 		//TCNT값을 0부터 타이머시작 tcnt3값 하나당 1us씩 걸림
		EICRB=0x08;     //PE5번 INT 5 하강엣지 인터럽트 발생으로 바꿈
		flag=1;
	}
	else if(flag==1)
	{	                         //1cm=0.034cm/usX에코펄스시간/2 :(에코펄스 시간= 58.8us)(1cm를 움직이는데 tcnt3은 58까지 세야함) 
		cm = TCNT3/58; 			//tcnt3을 1cm(tcnt3=58)단위로 나누어 cm로 변환
		EICRB=0x0C; 			//PE5번 INT 5 상승엣지 인터럽트 발생으로 바꿈
		flag=0;
	}

}


int main(void)
{char distance[10]={0};
	unsigned int  direction=0;
	
	DDRE = 0x10;			//pE4 Trlg , PE5 (echo) PE0 RX   PE1  TX
	DDRD =0xC0;             //PD7,PD6 모터 방향 제어 신호 출력,PD1,2,3 : 스위치 입력 
	DDRB=0xff;              // 모터PWM신호 출력
	
	TCCR3B=0x02;			//(TIMER3=초음파센서의 ECHO시간세기) 일반모드  프리스케일 8 ,주기60ms,분주비8, tcnt3=0~55295,60ms/55296=1us, tcnt3값 하나당 1us씩 걸림
	
	TCCR1A=0x22;            //(TIMER1=DC모터의 회전수 조절) 0b00100010  TCNT1과 OCR1B 비교 일치 될때 OC1B 핀 0, TOP에서 1 출력
	TCCR1B=0x1A;            // 0b00011010  TOP값을 ICR1으로 하는 FAST PWM모드 설정, 분주비 8
	ICR1=40000;             // 주기 43msec 로 하는 카운터값 40000 TOP값으로 사용
	
	
	EICRB=0x0C;				//PE5번 INT 5 상승엣지 인터럽트 발생
	EIMSK=0x20;  			// INT5 개별인터럽트 허용
	sei();					//전역 인터럽트 허용
	
	serial_init(9600);
	
	PORTD=0x80;            //모터 정방향
    OCR1B=0;                // DC모터 정지(듀티비 0)
	
	while(1)
	{
		if((~PIND&0x01)==0x01)               // SW0 누를때 모터 회전
		{
			_delay_ms(20);                   // 디바운싱
			OCR1B=30000;                 // 모터 회전
			while((~PIND&0x01)==0x01);          // 스위치 뗄때까지 기다림
			_delay_ms(20); 
			OCR1B=0;                      // 모터 정지
			Flag=2;
		}
		else if((~PIND&0x02)==0x02)	         // SW1 회전 방향 결정
		{
			_delay_ms(20);   
			if(direction) PORTD=0x80;
			else PORTD=0x40;                
			direction=~direction;            // moter_direction[0]:순방향    moter_direction[1]:역방향
			while((~PIND&0x02)==0x02);          
			_delay_ms(20);
			Flag=2;
		}
		else if((~PIND&0x04)==0x04)	Flag=1;  // SW2 SERIAL 초음파 센서값(CM)출력, CM에 따라 모터 회전 속도 변화
		if(Flag==1)
		{
			_delay_ms(20);
			PORTE = 0x10;     //10us동안 trig 신호 보냄
			_delay_us(10);
			PORTE = 0x00;
			
			sprintf(distance,"%d\r\n",cm); //정수 cm를 문자열로 변환
			tx_string(distance);           // 문자열 cm를 출력
			
			
			
			if (cm < 11)                      //0>>10갈수록 모터 속도 증가  11,12모터 정지 13>>30갈수록 모터 속도 증가
			{
				OCR1B = cm*3000;
			}
			else if ((cm==11)||(cm == 12)) OCR1B=0;
			else if ((12 < cm)&&(cm < 30))
			{
				OCR1B = (cm-12)*3000;
			}
			while((~PIND&0x02)==0x02);
			_delay_ms(20);
			
		}
		
	}
}

