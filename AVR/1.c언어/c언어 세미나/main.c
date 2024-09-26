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
		printf("��������� �����Ͻðڽ��ϱ�?(1.���ϰ��� 2.�������� ���� 3. 369���� 4.�����Ÿ��) : ");
		scanf("%d", &problem);
		printf("-----------------------------------------------------------------------------------------\n");
		switch (problem)
		{
		case 1: {

			int  x = 0;

			while (1) {
				printf("�����ϰ� �ִ� �ݾ��� �Է��ϼ��� :");
				scanf("%d", &x);
				if (x < 1000 || x>10000)
				{
					printf("1000~10000������ �ݾ��� �Է��ϼ���.\n");

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

			while (1) {                                 //50�̻��� ���� �Է¹ޱ� ���ѷ���
				printf("50�̻��� ���� �Է��ϼ��� :");
				scanf("%d", &n);
				if (n < 50)
					printf("�ٽ� �Է��ϼ���!\n");
				else if (n >= 50)
					break;
			}
			GAME_369(n);
			break;
		}

		case 4: wear_bowtie();
			break;

		default:
			printf("�߸� �Է��ϼ̽��ϴ�. �ٽ��Է��� �ֽʽÿ�.\n");
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
			printf("1.���:%d, 2.����:%d, 3.������:%d, 4.�Ž�����:%d \n", j, n, c - 1, change);

		}
	}

	return;
}

void ascending_sort(void) {
	int input[] = { 0 };   //���ϴ� �������� ��� �迭(�ִ� 20������)
	int i, k, least, temp = 0, SIZE;
	printf("���ϴ� ������ �Է��Ͻÿ� : ");

	for (int i = 0; i < 20; i++) { //���ϴ� ��ŭ ���� �Է� �ݺ�

		scanf("%d", &input[i]);   //���� �迭�� �ֱ�
		SIZE = i;                // �迭������ ��-1
		if (getc(stdin) == '\n') { //���͸� ������ �ݺ� ����

			break;
		}

	}

	for (i = 0; i <= SIZE; i++) {   // �Է¹��� ���� ���

		printf("%d ", input[i]);
	}

	printf("\n");

	for (i = 0; i < SIZE; i++)       //������������ ����
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
	for (i = 0; i <= SIZE; i++) {    //������ �迭 ���

		printf("%d ", input[i]);
	}
	printf("\n");
	return;
}
void GAME_369(int x)
{
	int current = 1, remainder, temp, count;
	while (current <= x) //�Է¹��� ������ ������
	{
		count = 0; //x�� Ƚ��=0
		temp = current; //���� ��(���� or �ڼ�)�� �ӽð����� ����
		while (temp > 0)
		{
			remainder = temp % 10; //1�� �ڸ�
			if (remainder == 3 || remainder == 6 || remainder == 9)
				count++; //1�� �ڸ��� 3,6,9�̸� x���
			temp /= 10; //10�� �ڸ�, 100�� �ڸ�, ... Ȯ��
		}
		if (count == 0) //1�� �ڸ��� 3,6,9�� �ƴϸ�
		{
			printf("%d ", current); //���ڰ� ���
			if (current % 10 == 0) printf("\n"); // �ึ�� 10���� ���
		}
		else {
			printf("x "); //x ���
			if (current % 10 == 0) printf("\n"); // �ึ�� 10���� ���
		}

		current++; //���� ���� �Ǵ�

	}
	return;
}
void wear_bowtie(void) {

	int i, j, size, h_size;



	printf("�����Ÿ���� ũ�⸦ �Է��Ͻÿ� : ");
	scanf("%d", &size);

	// ¦���� Ȧ���� ����� �ٸ��Ƿ� ���� ��츦 ����
	if (size % 2 == 0)					     // ũ�Ⱑ ¦���� ���(���� �Ʒ��� ����) 

	{
		h_size = size / 2;                   // ��, �Ʒ��� ������ ����Ƚ��

		for (i = 1; i <= h_size; i++)        // �����κ� ��������
		{
			for (j = 0; j < i; j++)          // i�������� ó���κ��� * ����
				printf("*");
			for (j = h_size; i < j; j--)     // ���η� �ݳ������� ���� ����
				printf(" ");
			for (j = h_size; i < j; j--)     // ���η� �ݳ������� ���� ����
				printf(" ");
			for (j = 0; j < i; j++)          // i�������� �������κ��� *����
				printf("*");
			printf("\n");                    // �� �� �ϼ��� �ٹٲ���ߵ�
		}

		for (i = 1; i <= h_size; i++)        // �Ʒ��κ� ��������   
		{
			for (j = h_size; i <= j; j--)    // h_size+1���� ���� ó���κ� *����
				printf("*");
			for (j = 0; j < i - 1; j++)      // ���η� �ݳ������� ���� ����
				printf(" ");
			for (j = 0; j < i - 1; j++)      // ���η� �ݳ������� ���� ����
				printf(" ");
			for (j = h_size; i <= j; j--)    // h_size+1�������� �������κ��� *����
				printf("*");
			printf("\n");                    // �� �� �ϼ��� �ٹٲ���ߵ�
		}
	}
	else if (size != 1)// ũ�Ⱑ 1�̾ƴ� Ȧ���� ���(���� �߰� �Ʒ��� ����)
	{
		h_size = size / 2;                   // ��,�Ʒ� �κ��� �ݺ�Ƚ��

		for (i = 1; i <= h_size; i++)        // �����κ� ��������
		{
			for (j = 0; j < i; j++)          // i�������� ó���κ��� * ����
				printf("*");
			for (j = h_size; i < j; j--)     // ���η� �ݳ������� ���� ����
				printf(" ");

			printf(" ");                     // �߰�����

			for (j = h_size; i < j; j--)     // ���η� �ݳ������� ���� ����
				printf(" ");
			for (j = 0; j < i; j++)          // i�������� �������κ��� *����
				printf("*");
			printf("\n");                    // �� �� �ϼ��� �ٹٲ���ߵ�
		}

		for (i = 0; i < size; i++)         // �߰��κ� *����
		{
			printf("*");
		}
		printf("\n");                       // ���������� �ٹٲ�

		for (i = 1; i <= h_size; i++)        // �Ʒ��κ� ��������   
		{
			for (j = h_size; i <= j; j--)    // h_size+1���� ���� ó���κ� *����
				printf("*");
			for (j = 0; j < i - 1; j++)      // ���η� �ݳ������� ���� ����
				printf(" ");

			printf(" ");                     // �߰�����

			for (j = 0; j < i - 1; j++)      // ���η� �ݳ������� ���� ����
				printf(" ");
			for (j = h_size; i <= j; j--)    // h_size+1�������� �������κ��� *����
				printf("*");
			printf("\n");                    // �� �� �ϼ��� �ٹٲ���ߵ�
		}


	}
	else printf("*\n");                         // size�� 1�϶�

	return;
}