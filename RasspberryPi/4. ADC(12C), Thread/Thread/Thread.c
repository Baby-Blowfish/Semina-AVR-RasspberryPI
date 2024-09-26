#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <pthread.h>
#include <wiringSerial.h>

#define CS_MCP3208 8
#define SPI_CHANNEL 0
#define SPI_SPEED 1000000

int adcValue=0,adcValue_1 = 0,adcValue_2 = 0;
int fd = 0;
char dat= -1;

int read_mcp3208_adc(unsigned char adcChannel)
{
      unsigned char buff[3];

      buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
      buff[1] = ((adcChannel & 0x07) << 6);
      buff[2] = 0x00;

     digitalWrite(CS_MCP3208, 0);            // Low : CS Active

     wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);

     buff[1] = 0x0f & buff[1];               //12비트 중 상위 4비트
     adcValue = ( buff[1] << 8) | buff[2];   //12비트 중 상위 4비트 + 12비트 중 하위 8비트

     digitalWrite(CS_MCP3208, 1);            // High : CS Inactive

     return adcValue;
}
void *serial()
{
	
	while(1)
	{
		if(serialDataAvail(fd)>0) //데이터가 putty에서 ras Rx로 들어오면 실행
		{
			dat= serialGetchar(fd); //들어온 데이터를 data에 저장
			serialPutchar(fd, dat);  // 들어온 데이터를 putty에 표현	
			serialPuts(fd, "\r\n");  // 들어온 데이터를 putty에 표현
		}
		else NULL;
	
		if(dat=='1')
		{
			printf("adcValue_1 = %d\n", adcValue_1);
			fflush(stdout);			  // 출력버퍼에 남은 데이터를 모니터에 모두 내보냄
			delay(300);
		}
		else if(dat=='2')
		{
			printf("adcValue_2 = %d\n", adcValue_2);
			fflush(stdout);			  // 출력버퍼에 남은 데이터를 모니터에 모두 내보냄
			delay(300);
		}
		else 
		{
			printf("Choose a adc channel(0,1) and write it down\n ");
			fflush(stdout);			  // 출력버퍼에 남은 데이터를 모니터에 모두 내보냄
			delay(300);
		}
    }

}



void *adc_1()
{
	while(1)
	{
		adcValue_1 = read_mcp3208_adc(0);
	}
	return NULL;
}

void *adc_2()
{
	while(1)
	{
		adcValue_2 = read_mcp3208_adc(1);
	}
	return NULL;
}

int main(void)
{
	pthread_t p_thread[3];      // 쓰레드 ID
	int thread_id=0;
    int status;

	
	if(wiringPiSetup()< 0)		//wiringPi 준비 완료 확인
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));//라즈베리파이에서 표준출력(모니터)
		return -1;
	}
	else NULL;
	
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) 
	{
		fprintf(stdout, "Unable to start wiringPiSPI: %s\n", strerror(errno));//라즈베리파이에서 표준출력(모니터)
		return -1;       //spi 사용 준비 확인
	}
	else NULL;
	
	if((fd = serialOpen("/dev/ttyAMA1", 9600)) < 0)                                    //포트 열기
	{
		fprintf(stderr, "unable to open serial device: %s\n", strerror(errno));
		return -1;
	}
	else NULL;
	
	pinMode(CS_MCP3208, OUTPUT);             //cs 출력                  
	
	thread_id = pthread_create(&p_thread[0], NULL, adc_1, NULL);
    if(thread_id < 0)
	{
		perror("thread create error : adc_1");
		exit(1);
	}

    thread_id = pthread_create(&p_thread[1], NULL, adc_2, NULL);
    if(thread_id < 0)
	{
		perror("thread create error : adc_2");
		exit(1);
	}
    
	thread_id = pthread_create(&p_thread[2], NULL, serial, NULL);
    if(thread_id < 0)
	{
		perror("thread create error : serial");
		exit(1);
	}
	
    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);
    pthread_join(p_thread[2], (void **)&status);
	
	return 0;
}