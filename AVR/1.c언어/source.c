#include <stdio.h>

void buy_fruit(int monney);
void ascending_sort(void);
void GAME_369(int x);
void wear_bowtie(void);

int main(void)
{
	int problem, b, c, d = 0;
	while (1)
	{
		printf("-----------------------------------------------------------------------------------------\n");
		printf("몇번문제를 실행하시겠습니까?(1.과일과게 2.오름차순 정렬 3. 369게임 4.나비넥타이) : ");
		scanf("%d", &problem);
		printf("-----------------------------------------------------------------------------------------\n");
		switch (problem)
		{
		case 1: {
			
			int  x = 0;

			while (1) {
				printf("소지하고 있는 금액을 입력하세요 :");
				scanf("%d", &x);
                                                  rewind(stdin); //입력버퍼지우기

				if (x < 1000 || x>10000)
				{
					printf("1000~10000범위의 금액을 입력하세요.\n");

				}
				else if (x >= 1000 && x <= 10000)
					break;		
			} 
			buy_fruit(x); 
			break;
		}

		case 2:ascending_sort();
			break;

		case 3: {
			int n;

			while (1) {                                 //50이상의 수를 입력받기 위한루프
				printf("50이상의 수를 입력하세요 :");
				scanf("%d", &n);
				if (n < 50)
					printf("다시 입력하세요!\n");
				else if (n >= 50)
					break;
			}
			GAME_369(n);
			break;
		}

		case 4: wear_bowtie();
			break;

		default:
			printf("잘못 입력하셨습니다. 다시입력해 주십시오.\n");
			break;
		}
	}

	return 0;
}

	void buy_fruit(int monney) {
		int i, j, k, n, m, a = 0;
		for (i = 0; i < 20; i++)
		{
			a++;
			if (a * 500 > monney) {
				break;
			}

		}

		for (j = 0; j < a; j++)
		{
			int monney2 = monney, b = 0;
			monney2 = monney2 - (j * 500);

			for (k = 0; k < 10; k++)
			{
				b++;
				if (b * 1000 > monney2)
				{
					break;
				}

			}
			for (n = 0; n < b; n++)
			{
				int monney1 = monney2, change = 0, c = 0;

				monney1 = monney1 - (n * 1000);

				for (m = 0; m < 14; m++)
				{
					c++;
					if (c * 700 > monney1)
					{
						break;
					}

				}
				change = monney - (j * 500 + n * 1000 + (c - 1) * 700);
				printf("1.사과:%d, 2.딸기:%d, 3.복숭아:%d, 4.거스름돈:%d \n", j, n, c - 1, change);

			}
		}

		return;
	}
	
void ascending_sort(void) {
	int input[20] = { 0 };   //원하는 정수들을 담는 배열(최대 20개까지)
	int i, k, least, temp = 0, SIZE;
	printf("원하는 정수를 입력하시오 : ");

	for (int i = 0; i < 20; i++) { //원하는 만큼 정수 입력 반복

		scanf("%d", &input[i]);   //정수 배열에 넣기
		SIZE = i;                // 배열원소의 수-1
		if (getc(stdin) == '\n') { //엔터를 누르면 반복 종료

			break;
		}

	}

	for (i = 0; i <= SIZE; i++) {   // 입력받은 정수 출력

		printf("%d ", input[i]);
	}

	printf("\n");

	for (i = 0; i < SIZE; i++)       //오름차순으로 정렬
	{
		least = i;
		for (k = i + 1; k <= SIZE; k++)
		{
			if (input[k] < input[least])
				least = k;
		}
		temp = input[i];
		input[i] = input[least];
		input[least] = temp;


	}
	for (i = 0; i <= SIZE; i++) {    //정렬한 배열 출력

		printf("%d ", input[i]);
	}
	printf("\n");
	return;
}
void GAME_369(int x)
{
	int current = 1, remainder, temp, count;
	while (current <= x) //입력받은 값까지 루프함
	{
		count = 0; //x의 횟수=0
		temp = current; //현재 값(숫자 or 박수)을 임시공간에 저장
		while (temp > 0)
		{
			remainder = temp % 10; //1의 자리
			if (remainder == 3 || remainder == 6 || remainder == 9)
				count++; //1의 자리가 3,6,9이면 x출력
			temp /= 10; //10의 자리, 100의 자리, ... 확인
		}
		if (count == 0) //1의 자리가 3,6,9가 아니면
		{
			printf("%d ", current); //숫자값 출력
			if (current % 10 == 0) printf("\n"); // 행마다 10열로 출력
		}
		else {
			printf("x "); //x 출력
			if (current % 10 == 0) printf("\n"); // 행마다 10열로 출력
		}

		current++; //다음 숫자 판단

	}
	return;
}
void wear_bowtie(void) {

	int i, j, size, h_size;



	printf("나비넥타이의 크기를 입력하시오 : ");
	scanf("%d", &size);

	// 짝수와 홀수의 모양이 다르므로 둘의 경우를 나눔
	if (size % 2 == 0)					     // 크기가 짝수인 경우(위에 아래로 나눔) 

	{
		h_size = size / 2;                   // 위, 아래의 각각의 루프횟수

		for (i = 1; i <= h_size; i++)        // 위에부분 루프시작
		{
			for (j = 0; j < i; j++)          // i번쨰행의 처음부분의 * 생성
				printf("*");
			for (j = h_size; i < j; j--)     // 세로로 반나눴을때 앞쪽 공백
				printf(" ");
			for (j = h_size; i < j; j--)     // 세로로 반나눴을때 뒷쪽 공백
				printf(" ");
			for (j = 0; j < i; j++)          // i번쨰행의 마지막부분의 *생성
				printf("*");
			printf("\n");                    // 한 행 완성후 줄바꿔줘야됨
		}

		for (i = 1; i <= h_size; i++)        // 아래부분 루프시작   
		{
			for (j = h_size; i <= j; j--)    // h_size+1번쨰 행의 처음부분 *생성
				printf("*");
			for (j = 0; j < i - 1; j++)      // 세로로 반나눴을때 앞쪽 공백
				printf(" ");
			for (j = 0; j < i - 1; j++)      // 세로로 반나눴을때 뒷쪽 공백
				printf(" ");
			for (j = h_size; i <= j; j--)    // h_size+1번쨰행의 마지막부분의 *생성
				printf("*");
			printf("\n");                    // 한 행 완성후 줄바꿔줘야됨
		}
	}
	else if (size != 1)// 크기가 1이아닌 홀수인 경우(위에 중간 아래로 나눔)
	{
		h_size = size / 2;                   // 위,아래 부분의 반복횟수

		for (i = 1; i <= h_size; i++)        // 위에부분 루프시작
		{
			for (j = 0; j < i; j++)          // i번쨰행의 처음부분의 * 생성
				printf("*");
			for (j = h_size; i < j; j--)     // 세로로 반나눴을때 앞쪽 공백
				printf(" ");

			printf(" ");                     // 중간공백

			for (j = h_size; i < j; j--)     // 세로로 반나눴을때 뒷쪽 공백
				printf(" ");
			for (j = 0; j < i; j++)          // i번쨰행의 마지막부분의 *생성
				printf("*");
			printf("\n");                    // 한 행 완성후 줄바꿔줘야됨
		}

		for (i = 0; i < size; i++)         // 중간부분 *생성
		{
			printf("*");
		}
		printf("\n");                       // 다음행위해 줄바꿈

		for (i = 1; i <= h_size; i++)        // 아래부분 루프시작   
		{
			for (j = h_size; i <= j; j--)    // h_size+1번쨰 행의 처음부분 *생성
				printf("*");
			for (j = 0; j < i - 1; j++)      // 세로로 반나눴을때 앞쪽 공백
				printf(" ");

			printf(" ");                     // 중간공백

			for (j = 0; j < i - 1; j++)      // 세로로 반나눴을때 뒷쪽 공백
				printf(" ");
			for (j = h_size; i <= j; j--)    // h_size+1번쨰행의 마지막부분의 *생성
				printf("*");
			printf("\n");                    // 한 행 완성후 줄바꿔줘야됨
		}


	}
	else printf("*\n");                         // size가 1일때

	return;
}