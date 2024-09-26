#include <wiringPi.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFF_SIZE 1024 
#define motor_pin 23



int server_socket;              //서버 디스크립터
int client_socket;              //클라이언트 디스크립터
int client_addr_size,option;
int change_duty;
struct sockaddr_in server_addr; //소켓에 주소와 포트를 할당하기 위해 struct 구조체 이용
struct sockaddr_in client_addr;
char buff_receive[BUFF_SIZE+5];
char duty_receive[BUFF_SIZE+5];
char buff_send[BUFF_SIZE+5];

void motor()
{
	pinMode(motor_pin, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);  // WiringPi는 PWM 신호 생성시 기본적으로 Balanced 모드이며 PWM_MODE_MS로 변경
 
	pwmSetClock(384);   //라즈베리 베이스 클럭 19.2 * 10^6,
	pwmSetRange(1000);  // 50Hz(20ms) 주기를 만들기 위해 펄스 주기 설정 Main 주파수 / CLOCK / RANGE   19.2 * 10^6 / 384 / 1000 = 50 Hz
}

void *wri()
{
	while(1)
	{
		if(getchar() == '\n')
		{
			printf("server : ");
			fgets(buff_send, sizeof(buff_send), stdin);		
			write(client_socket, buff_send, strlen(buff_send)+1);
		}
	}
}

	
int main(void)
{
	if(wiringPiSetup()< 0)															     //wiringPi 준비 완료 확인
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));//라즈벨파이에서 표준출력(모니터)
		return 1;
	}
	
	pthread_t p_thread[1];      // 쓰레드 식별자
    int thread_id;
    int status;
	
	motor();
	
	
	thread_id = pthread_create(&p_thread[0], NULL, wri, NULL);
    if(thread_id < 0)
	{
		perror("thread create error : ");
		exit(0);
	}
	
	memset(&server_addr, 0, sizeof(server_addr)); //메모리의 크기를 변경할 포인터, 초기화 값, 초기화 길이
	server_addr.sin_family = AF_INET;   //IPv4 인터넷 프로토콜
	server_addr.sin_port = htons(4000); //사용할 port 번호 4000
	server_addr.sin_addr.s_addr= htonl(INADDR_ANY); //32bit IPv4주소
	//htonl( INADDR_ANY) 는 주소를 지정해 주는 것 inet_addr("내 시스템의 IP ")로도 지정할 수 있습니다. 
	//그러나 프로그램이 실행되는 시스템 마다 IP 가 다를 것이므로 주소 지정을 고정 IP로 하지 않고 htonl(INADDR_ANY) 를 사용하는 것이 편리합니다.
	// wodud INADDR_ANY는 매크로 따라가보면 0.0.0.0을 가르킨다 0.0.0.0은 임의로 자신의 ip를 가르키는 것이다.
	// 즉 0.0.0.0으로 선언하면 자신의 호스트에 들어오는 패킷을 무조건 수신하게 된다.
		 
	//소켓 생성
	server_socket = socket(AF_INET, SOCK_STREAM, 0);    //TCP/IP에서는 SOCK_STREAM 을 UDP/IP에서는 SOCK_DGRAM을 사용
	if(-1 == server_socket) 
	{
		printf("socket() error\n"); 
		exit(1); //오류 확인
	}
	
	//소켓의 바인드 오류를 해결해주기위한 함수
	option=1;
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));

	//bind()함수를 이용하여 소켓에 server socket에 필요한 정보를 할당하고 커널에 등록
	if(-1 == bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
	{
		printf("bind error\n");
		exit(1);
	}
	
	//클라이언트 접속 요청 확인
	if( -1 == listen(server_socket,5)) 
	{
		printf("listen error\n");
		exit(1);
	}
	
	client_addr_size = sizeof(client_addr); //client 주소 크기 대입
	client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
	//accept()함수는 클라이언트의 접속 요청을 받아드리고 클라이언트와 통신하는 전용 소켓을 생성합니다
	//accept()로 접속요청을 허락시, 클라이언트와 통신을 하기 위해 커널이 자동으로 소켓 생성= client_socket
	if(-1 == client_socket) 
	{
		
		printf("클라이언트 연결 수락 실패\n");
		exit(1);
	}
		
	while(1)
	{	
		read(client_socket, buff_receive, BUFF_SIZE);
		printf("client : %s", buff_receive);
		if((strcmp(buff_receive,"m\n"))==0)
		{
		read(client_socket, buff_receive, BUFF_SIZE);
		printf("duty ratio : %s", buff_receive); 
		change_duty=(atoi(buff_receive))*10;
		pwmWrite(motor_pin,change_duty);
		}
	}
	
	return 0; 
}

