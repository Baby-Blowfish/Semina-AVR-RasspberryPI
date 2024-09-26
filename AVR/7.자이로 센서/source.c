/*
 * mpu6050.c
 *
 * Created: 2021-10-07 오후 5:04:18
 * Author : user
 */ 


#define F_CPU 7372800UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>
#include <string.h>

volatile double accel_x=0,accel_y=0,accel_z=0,gyro_x,gyro_y,gyro_z,angle_x=0,angle_y=0,angle_z=0;
volatile double angle_x_gyro=0,angle_y_gyro=0,angle_z_gyro=0,angle_x_accel=0,angle_y_accel=0,angle_z_accel=0;
volatile double gx=0,gy=0,gz=0,ax=0,ay=0,az=0,Roll=0,Pitch=0,Yaw=0;
volatile char mpu6050[12]={0};
volatile char Roll_Pitch_value[20]={0};
	
volatile float roll_target_angle=0.0;
volatile float roll_input_angle=0.0;
volatile float roll_prev_error=0.0;
volatile float roll_kp=2;        // 비례 매개변수
volatile float roll_ki=0.6;         // 적분 매개변수
volatile float roll_kd=0.7;        // 미분 매개변수
volatile float roll_iterm=0.0,roll_pterm=0.0,roll_dterm=0.0;
volatile float roll_output_angle=0.0;
volatile float roll_dt=0.02;
volatile float roll_error_p=0.0;
volatile float roll_error_d=0.0;
void stdPID_roll(float roll_input_angle)
{
	roll_pterm=0;
	roll_iterm=0;
	roll_dterm=0;
	roll_error_p=roll_input_angle-roll_target_angle;      // 오차 = 설정값 - 현재 입력값
	//roll_error_d = roll_error_p - roll_prev_error;   // 현재 입력값에서 이전 입력값의 차이
	
	//PID 제어
	roll_pterm=roll_kp*roll_error_p;             // 비례항(비례 매개변수*오차)
	roll_iterm+=roll_ki*(roll_error_p+roll_prev_error)*roll_dt;         // 적분항(시간에 걸친 오차의 합=이전 적분항+현재오차값*적분 매개변수*센서 입력 주기(dt)값)
	roll_dterm=roll_kd*roll_error_d/roll_dt;      // 미분항(미분 비례변수*오차의 변화율/센서입력주기)
	// 미분항은 갑작스러 외력의 변화에 저항= setpoint에 의한 내부적인 요소를 제외하기위해 부호음수
	roll_prev_error = roll_error_p;                 // 다음 주기에 사용하기 위해서 현재 입력값을 저장(피드백 구조)

	roll_output_angle = roll_pterm + roll_iterm + roll_dterm;
}
void mpu6050_init() // mpu6050 초기 설정
{
	//I2C통신 설정
	TWBR = 10;			//204.8khz    SCL주파수 = F_CPU/16+2*TWBR*4^TWPS		
	TWSR = 0x00;		//TWPS0=0,TWPS1=0,분주비 1
	
	//mpu설정 
	MPU6050_write(0x68,0x1a,0x01);		//CONFIG레지스터(ox1a)=0x01 : DLPF 활성화 시(sample rate(1khz)=gyro output rate(1khz)/(1+smplrt_div(0))
	MPU6050_write(0x68,0x19,0x00);		//smplrt_div(0x19)=0x00
	MPU6050_write(0x68,0x6b,0x00);		//PWR_MGMT_1(0x6b)=0x00 : sleep 모드 해제. 내부클럭8mhz사용
	
}
void MPU6050_write(unsigned char add,unsigned char reg_addr,unsigned char data)//(MPU주소, 레지스터 주소, 비트설정)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// START조건 전송(TWCR=0xa4)
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x08);	// Slave 수신 대기 : TWINT=1인지(수신되면 TWINT=1이됨), TWSR=0x08(START 상태)인지
	
	TWDR=(add<<1);	                                        // 마스터 송신 모드 : TWDR=address(7)+Write(0), MPU주소0b11010000(AD0=0) 전송
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// 송신완료 후 TWCR=1 됨,데이터 전송하려면 TWDR=1써서 클리어해줘야함(TWCR=0x84)
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x18);	// Slave 수신 대기 : TWINT=1인지, TWSR=0x18(SLA_W전송완료,ack 수신완료)인지
	
	TWDR= reg_addr;	                                        // 접근할 레지스터 주소
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);      // 레지스터 접근   
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x28);	// Slave 수신 대기 : TWINT=1인지, TWSR=0x28(레지스터 주소 전송완료,ack 수신완료)인지
	
	TWDR=data;	                                            // 레지스터에 쓸 데이터 
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// 레지스터에 데이터 전송
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x28);	// Slave 수신 대기 : TWINT=1인지, TWSR=0x28(data 전송완료,ack 수신완료)인지
	
	TWCR = (1<<TWINT)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN);		// STOP조건 전송(TWCR=0x94)
}
char MPU6050_read(char add, char reg_addr)                  //(MPU주소, 레지스터 주소)
{
	//_delay_ms(2);                                           // data sheet DLPF할때 2ms 필요
	TWCR = (1<<TWINT)|(1<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// data sheet에 따라 mpu에 데이터 받기 전  mpu에 송신 접근 필요
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x08);	
	
	TWDR=(add<<1);	
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x18);	
	
	TWDR=reg_addr;	
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x28);	
	
	                                                        
	TWCR = (1<<TWINT)|(1<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// REPEAT START조건 전송(TWCR=0xa4)
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x10);	// Slave 수신 대기 : TWINT=1인지(수신되면 TWINT=1이됨), TWSR=0x10(R.s 상태)인지
	
	TWDR=(add<<1)|0x01;	                                    // 마스터 수신 모드 : TWDR=address(7)+Read(1), MPU주소0b11010000(AD0=0) 전송
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// 레지스터 접근
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x40);	// Slave 수신 대기 : TWINT=1인지, TWSR=0x40(data 전송완료,ack 수신완료)인지
	
	TWCR = (1<<TWINT)|(0<<TWSTA)|(0<<TWSTO)|(1<<TWEN);		// 수신 받기위해 TWINT=1써서 초기화
	while(((TWCR&(1<<TWINT))==0x00)||(TWSR&0xf8)!=0x58);	// 데이터 수신  : TWINT=1인지, TWSR=0x58(Data수신상태,ack 수신 안함)인지
	
	TWCR = (1<<TWINT)|(0<<TWSTA)|(1<<TWSTO)|(1<<TWEN);		// STOP조건 전송(TWCR=0x94)
	
	return TWDR;	//TWDR값 반환
}
void mpu6050_getData(double *ax,double *ay,double *az,double *gx,double *gy,double *gz)//가속도,자이로 센서값을 받아오기
{                    //참조에 의한 호출
	//가속도 (MPU주소, 레지스터주소),가장 최근의 측정값 Sample rate(1KHZ)주기로 저장됨
	mpu6050[0] = MPU6050_read(0x68,0x3b); // ACCEL_XOUT_H 16_bit ADC
	mpu6050[1] = MPU6050_read(0x68,0x3c); // ACCEL_XOUT_L
	mpu6050[2] = MPU6050_read(0x68,0x3d); // ACCEL_YOUT_H
	mpu6050[3] = MPU6050_read(0x68,0x3e); // ACCEL_YOUT_L
	mpu6050[4] = MPU6050_read(0x68,0x3f); // ACCEL_ZOUT_H
	mpu6050[5] = MPU6050_read(0x68,0x40); // ACCEL_ZOUT_L
	//자이로 (MPU주소, 레지스터주소),가장 최근의 측정값 Sample rate(1KHZ)주기로 저장됨
	mpu6050[6] = MPU6050_read(0x68,0x43); // GYRO_XOUT_H
	mpu6050[7] = MPU6050_read(0x68,0x44); // GYRO_XOUT_L
	mpu6050[8] = MPU6050_read(0x68,0x45); // GYRO_YOUT_H
	mpu6050[9] = MPU6050_read(0x68,0x46); // GYRO_YOUT_L
	mpu6050[10] = MPU6050_read(0x68,0x47);// GYRO_ZOUT_H
	mpu6050[11] = MPU6050_read(0x68,0x48);// GYRO_ZOUT_L
	//가속도
	*ax = mpu6050[0] << 8 | mpu6050[1];   // 16-bit	adc값 변수 ax에 저장
	*ay = mpu6050[2] << 8 | mpu6050[3];
	*az = mpu6050[4] << 8 | mpu6050[5];
	//자이로
	*gx = mpu6050[6] << 8 | mpu6050[7];
	*gy = mpu6050[8] << 8 | mpu6050[9];
	*gz = mpu6050[10] << 8 | mpu6050[11];
}


void Complementary_Filter(double ax,double ay,double az,double gx,double gy,double gz,double *angle_x,double *angle_y, double *angle_z)
{

	accel_x = ax;
	accel_y = ay;
	accel_z = az;

	gyro_x= gx/(double)131;
	gyro_y= gy/(double)131;
	gyro_z= gz/(double)131;
	//가속도 값 각도 변환
	angle_x_accel = atan(accel_y/(sqrt(accel_x*accel_x+accel_z*accel_z)))*(180/3.141592);
	angle_y_accel = atan(-accel_x/(sqrt(accel_y*accel_y+accel_z*accel_z)))*(180/3.141592);
	//angle_z_accel = atan((sqrt(accel_y*accel_y+accel_x*accel_x)/accel_z))*(180/3.141592);
	//가속도 센서로는 z축  회전각 계산이 불가능
	
	//자이로 값 각도 변환
	angle_x_gyro = (gyro_x*((double)1/1000)+*angle_x);
	angle_y_gyro = (gyro_y*((double)1/1000)+*angle_y);
	angle_z_gyro = (gyro_z*((double)1/1000)+*angle_z);
	//상보필터
	*angle_x = (double)0.95*angle_x_gyro+(1-(double)0.95)*angle_x_accel;
	*angle_y = (double)0.95*angle_y_gyro+(1-(double)0.95)*angle_y_accel;
	*angle_z = angle_z_gyro;
}
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

int main()
{
	int angle=0;
	TCCR1A=0x82;	//0b10000010  비교 일치 될때 OCnX 핀 0, TOP에서 1 출력
	TCCR1B=0x1A;	//0b00011010  TOP값을 ICRn으로 하는 FAST PWM모드 설정, 분주비 8
	ICR1=18430;  	// 주기 20msec 로 하는 카운터값 18431 TOP값으로 사용

	
	//0CR1A=PB5 PWM신호 출력  서보모터가 받음
	DDRB=0x2f;
	mpu6050_init();		//mpu설정
	serial_init(9600);		//UART통신 보레이트 9600설정
	
	sei();
	while(1)
	{
		mpu6050_getData(&ax,&ay,&az,&gx,&gy,&gz);
		
		Complementary_Filter(ax,ay,az,gx,gy,gz,&Roll,&Pitch,&Yaw);
		
		stdPID_roll(Roll);
		//if(roll_output_angle>2100) roll_output_angle=2100;
		//else if(roll_output_angle<650) roll_output_angle=650;
		
		angle=(int)(650+8*(roll_output_angle/2+90));
		//실수형 Roll,Pitch값을 정수형으로 강제변환후 정수형으로 배열에 저장
		//sprintf(Roll_Pitch_value,"roll=%d  Pitch=%d Yaw=%d\r\n",(int)Roll, (int)Pitch, (int)Yaw
		//sprintf(Roll_Pitch_value,"roll=%d  roll_output_angle=%d\r\n ",(int)Roll,(int)angle);
		//tx_string(Roll_Pitch_value);
		
		
		 OCR1A=angle;
		_delay_ms(20);
		
	}
}