#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>



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
char rx_data(void)//수신
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
void rx_string(char *str) //문자열 수신     |n 다음줄로 이동     |r 첫 줄로 이동   |0 제어문자 널문자
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
  
	
int main(void)
{
	/*DDRC=0xff;        // C 출력(FND)
	DDRD=0x30;		  // D포트 0,1,2,3 입력(스위치), 4,5 출력(FND 동적구동)
	DDRB=0x20; 	//B포트 5 서보모터 출력
	PORTC=~0x00;	  // C포트 초기화
	
	EICRA=0xaa;       // 인터럽트 제어 레지스터, 하강 모서리
	EIMSK=0x0f;       // 외부 인터럽트 0,1,2,3 사용
	
	TCCR1A=0x82;	//0b10000010  비교 일치 될때 OCnX 핀 0, TOP에서 1 출력
	TCCR1B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
	ICR1=18431;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용
	OCR1A=645;		//서보모터 초기값 0도

	//sei();            // SREG.I 세트 모든 인터럽트 허용*/
	serial_init(9600); 
	
	char dat[10]="";
    
	DDRA=0XFF;
	while (1)
	{
			rx_string(dat);
			tx_string(dat);
			//dat=rx_data();
			//tx_data(dat);
			//PORTA=0XFF;
			//_delay_ms(500);
		//	PORTA=0X00;
			//_delay_ms(500);
			
	}
}