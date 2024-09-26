#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


volatile int push_count=0;
int fd = 0;
int main(void)
{
	
	
	char data=0;
	char push_count_s[16]={};
	if(wiringPiSetup()< 0)															     //wiringPi 준비 완료 확인
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));//라즈벨파이에서 표준출력(모니터)
		return 1;
	}

	if((fd = serialOpen("/dev/ttyAMA1", 9600)) < 0)                                    //포트 열기
	{
		fprintf(stderr, "unable to open serial device: %s\n", strerror(errno));
		return -1;
	}
	
	pinMode(14,INPUT);
	pinMode(30,INPUT);	
	printf("\n Rasberry Pi UART Test \n");
    serialPuts(fd,"Raspberry Test \r");
	while(1)
	{
		
		if(serialDataAvail(fd)>0) //데이터가 putty에서 ras Rx로 들어오면 실행
		{
			data = serialGetchar(fd);                                                                   //데이터 받기                                                                   //문자 출력
			serialPutchar(fd, data);
			serialPuts(fd,"\r\n");
			printf("x\n");
			fflush(stdout);
			              	
		}
		else continue;;
		
		
		/*if(digitalRead(13)==0)
		{
			//fgets(str,sizeof(str),stdin); //15개의 문자열+null문자 저장
			//serialPuts(fd, str);
			//serialPutchar(fd,'\r');
			scanf("%s",data);
			serialPutchar(fd, data);
			serialFlush(fd);
		}
		
		if(digitalRead(14)==0) //ras에서 putty로 데이터 보내기
		{
			delay(100);
			push_count++;
			printf("%d",push_count); // 출력스트림을 통해서 ras 모니터로 출력
			fflush(stdout);			  // 출력버퍼에 남은 데이터를 모니터에 모두 내보냄
			sprintf(push_count_s,"%d",push_count);
			serialPuts(fd,push_count_s);  // 들어온 데이터를 putty에 표현
			while(digitalRead(14)==0);
			delay(100);
		}
		
		
		
		
			serialPutchar(fd,'1');
			serialPuts(fd, "\r\n");
			data=getchar();  //문자 엔터 미포함
			putchar(data);  
			
			scanf("%s",DATA); 문자열 엔터미포함
			printf("%s",DATA);
			
			fgets(DATA,sizeof(DATA), stdin); 무자열 엔터 포함
			puts(DATA);
			
			
			fflush(stdout);*/
		
	}
	return 0;
}
