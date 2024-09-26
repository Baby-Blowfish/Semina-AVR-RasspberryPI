/* Hardware pwm
#include<wiringPi.h>
#include<stdio.h>
#include <errno.h>
#include <string.h>

#define SERVO 1


int main(void)
{
	if(wiringPiSetup()< 0)															     //wiringPi 준비 완료 확인
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));//라즈벨파이에서 표준출력(모니터)
		return 1;
	}

	pinMode(SERVO,PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(384);
	pwmSetRange(1000);
	
	while(1)
	{
		pwmWrite(SERVO,50);
		delay(3000);
		pwmWrite(SERVO,75);
		delay(3000);
		pwmWrite(SERVO,100);
		delay(3000);
	}
	return 0;
	
}
*/
/* Software pwm
#include<wiringPi.h>
#include<stdio.h>
#include <errno.h>
#include <string.h>
#include <softPwm.h>

#define SERVO 4


int main(void)
{
	if(wiringPiSetup()< 0)															     //wiringPi 준비 완료 확인
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));//라즈벨파이에서 표준출력(모니터)
		return 1;
	}

	pinMode(SERVO,PWM_OUTPUT);
	softPwmCreate(SERVO,0,200);
	
	
	while(1)
	{
		softPwmWrite(SERVO,6);
		delay(3000);
		softPwmWrite(SERVO,15);
		delay(3000);
		softPwmWrite(SERVO,24);
		delay(3000);
	}
	return 0;
	
}*/