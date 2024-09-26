#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

volatile unsigned int flag=0,Flag=2, cm=0;

void serial_init(unsigned long baud)
{//USART0 RX:PE0   TX:PE1   SERIAL �ʱ�ȭ
	unsigned short ubrr;
	ubrr = (unsigned short) (F_CPU/(16*baud)-1);
	UBRR0H = (unsigned char)(ubrr >> 8);	//����������Ʈ
	UBRR0L = (unsigned char)(ubrr & 0xff);	//����������Ʈ
	UCSR0B = 0x18;// ���ſϷ�۽ſϷᵥ���ͺ����ͷ�Ʈ����(000),�۽Ű���(1),���Ű���(1),����ũ��8bit(0),���ż۽Ź��ں�Ʈ(0)
	UCSR0C = 0x06;// �����Ʈ(0),�񵿱���(0),�и�Ƽ�����X(00),STOP��Ʈ1��(0),����ũ��8bit(11),Ŭ���ؼ����X(0)
	//USCR0A=0 ���ſϷ��÷���(0),�۽ſϷ��÷���(0),�����ͺ��÷���(0),�����ӿ���(0),�������÷���(0),�и�Ƽ����(0),�񵿱���(0),�������μ���(0)
}
void tx_data( char data)//�۽�
{
	while((UCSR0A & 0X20) == 0X00); //�����Ͱ� �۽�shift register�� �Ѿ UDR0�� �������(1)�����ͷ������ͺ����ͷ�Ʈ�߻�����
	UDR0 = data;		            //�����Ͱ� �۽��غ񰡵ɶ����� ���
}                                   //��ǻ��  �ۼ���shiftregister <<  UDR0   ATmega128

void tx_string( char *str)//���ڿ��۽�
{
	while(*str!='\0')    //���ڿ��� ��(NULL����)�������� �����͸� ����
	{
		tx_data(*str++);
	}
	
}
void speedchanges_according_distance(void)
{
	char distance[10]={0};   //�Ÿ����� 
	PORTE = 0x10;     //10us���� trig ��ȣ ����
	_delay_us(10);
	PORTE = 0x00;
	
	sprintf(distance,"%d\r\n",cm); //���� cm�� ���ڿ��� ��ȯ
	tx_string(distance);           // ���ڿ� cm�� ���
	
	
	
	if (cm < 11)                      //0>>10������ ���� �ӵ� ����  11,12���� ���� 13>>30������ ���� �ӵ� ����
	{
		OCR1B = cm*3000;
	}
	else if ((cm==11)||(cm == 12)) OCR1B=0;
	else if ((12 < cm)&&(cm < 30))
	{
		OCR1B = (cm-12)*3000;
	}
}


ISR(INT5_vect)		//���� ���ͷ�Ʈ PE5   (INT5)
{
	if(flag==0)         //ó�� 8���� ������ ����Ʈ ������ echo��ȣ�� ��¿����� �ɶ�
	{
		TCNT3=0; 		//TCNT���� 0���� Ÿ�̸ӽ��� tcnt3�� �ϳ��� 1us�� �ɸ�
		EICRB=0x08;     //PE5�� INT 5 �ϰ����� ���ͷ�Ʈ �߻����� �ٲ�
		flag=1;
	}
	else if(flag==1)
	{	                         //1cm=0.034cm/usX�����޽��ð�/2 :(�����޽� �ð�= 58.8us)(1cm�� �����̴µ� tcnt3�� 58���� ������) 
		cm = TCNT3/58; 			//tcnt3�� 1cm(tcnt3=58)������ ������ cm�� ��ȯ
		EICRB=0x0C; 			//PE5�� INT 5 ��¿��� ���ͷ�Ʈ �߻����� �ٲ�
		flag=0;
	}

}


int main(void)
{char distance[10]={0};
	unsigned int  direction=0;
	
	DDRE = 0x10;			//pE4 Trlg , PE5 (echo) PE0 RX   PE1  TX
	DDRD =0xC0;             //PD7,PD6 ���� ���� ���� ��ȣ ���,PD1,2,3 : ����ġ �Է� 
	DDRB=0xff;              // ����PWM��ȣ ���
	
	TCCR3B=0x02;			//(TIMER3=�����ļ����� ECHO�ð�����) �Ϲݸ��  ���������� 8 ,�ֱ�60ms,���ֺ�8, tcnt3=0~55295,60ms/55296=1us, tcnt3�� �ϳ��� 1us�� �ɸ�
	
	TCCR1A=0x22;            //(TIMER1=DC������ ȸ���� ����) 0b00100010  TCNT1�� OCR1B �� ��ġ �ɶ� OC1B �� 0, TOP���� 1 ���
	TCCR1B=0x1A;            // 0b00011010  TOP���� ICR1���� �ϴ� FAST PWM��� ����, ���ֺ� 8
	ICR1=40000;             // �ֱ� 43msec �� �ϴ� ī���Ͱ� 40000 TOP������ ���
	
	
	EICRB=0x0C;				//PE5�� INT 5 ��¿��� ���ͷ�Ʈ �߻�
	EIMSK=0x20;  			// INT5 �������ͷ�Ʈ ���
	sei();					//���� ���ͷ�Ʈ ���
	
	serial_init(9600);
	
	PORTD=0x80;            //���� ������
    OCR1B=0;                // DC���� ����(��Ƽ�� 0)
	
	while(1)
	{
		if((~PIND&0x01)==0x01)               // SW0 ������ ���� ȸ��
		{
			_delay_ms(20);                   // ��ٿ��
			OCR1B=30000;                 // ���� ȸ��
			while((~PIND&0x01)==0x01);          // ����ġ �������� ��ٸ�
			_delay_ms(20); 
			OCR1B=0;                      // ���� ����
			Flag=2;
		}
		else if((~PIND&0x02)==0x02)	         // SW1 ȸ�� ���� ����
		{
			_delay_ms(20);   
			if(direction) PORTD=0x80;
			else PORTD=0x40;                
			direction=~direction;            // moter_direction[0]:������    moter_direction[1]:������
			while((~PIND&0x02)==0x02);          
			_delay_ms(20);
			Flag=2;
		}
		else if((~PIND&0x04)==0x04)	Flag=1;  // SW2 SERIAL ������ ������(CM)���, CM�� ���� ���� ȸ�� �ӵ� ��ȭ
		if(Flag==1)
		{
			_delay_ms(20);
			PORTE = 0x10;     //10us���� trig ��ȣ ����
			_delay_us(10);
			PORTE = 0x00;
			
			sprintf(distance,"%d\r\n",cm); //���� cm�� ���ڿ��� ��ȯ
			tx_string(distance);           // ���ڿ� cm�� ���
			
			
			
			if (cm < 11)                      //0>>10������ ���� �ӵ� ����  11,12���� ���� 13>>30������ ���� �ӵ� ����
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

