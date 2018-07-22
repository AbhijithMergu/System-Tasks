#include<stdio.h>
#include<stdlib.h>
#include "manager.h"

int main()
{
	//char* val = (char*)mac_alloc(1000);
	//printf("%d\n", val);

	//char* val5 = (char*)mac_alloc(20);
	//printf("%d\n", val5);

	////mac_free(val5);

	//char* val6 = (char*)mac_alloc(20);
	//printf("%d\n", val6);

	//char* val2 = (char*)mac_alloc(500);
	//printf("%d\n", val2);

	//char* val3 = (char*)mac_alloc(500);
	//printf("%d\n", val3);

	////mac_free(val);
	//val = (char*)mac_alloc(400);
	//printf("%d\n", val);
	//
	//char* val4 = (char*)mac_alloc(400);
	//printf("%d\n", val4);
	//
	//char* val8 = (char*)mac_alloc(1000);
	//printf("%d\n", val8);

	//char* val9 = (char*)mac_alloc(1000);
	//printf("%d\n", val9);
	//char* val10 = (char*)mac_alloc(1000);
	//printf("%d\n", val10);
	//char* val11 = (char*)mac_alloc(1000);
	//printf("%d\n", val11);
	//char* val12 = (char*)mac_alloc(1000);
	//printf("%d\n", val12);
	//char* val13 = (char*)mac_alloc(1000);
	//printf("%d\n", val13);


	char** a = (char**)mac_alloc(10 * sizeof(char*));
	for (int i = 0; i < 15; i++)
	{
		a[i] = (char*)mac_alloc(1000 * sizeof(char));
		printf("%d\n", a[i]);
		mac_free(a[i]);
	}
	system("pause");
	return 0;
}