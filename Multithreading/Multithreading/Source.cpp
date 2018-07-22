#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<Windows.h>
#include<time.h>
//#include "matrix_mul.h"
#include "client.h"
//// A normal C function that is executed as a thread 
//// when its name is specified in pthread_create()
//void *myThreadFun(void *vargp)
//{
//	Sleep(10000);
//	return NULL;
//}

//
//pthread_cond_t      condA = PTHREAD_COND_INITIALIZER;
//pthread_cond_t      condB = PTHREAD_COND_INITIALIZER;
//pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
//
//void* even_thread(void* args){
//
//	int i = 0;
//	while (i < 10){
//	//	pthread_mutex_lock(&mutex);
//		printf("even thread : %d\n", i);
//
//	//	pthread_mutex_unlock(&mutex);
//		pthread_cond_signal(&condA);
//		while (pthread_cond_wait(&condA, &mutex) != 0){
//
//		}
//		i += 2;
//	}
//	return NULL;
//}
//
//void* odd_thread(void* args){
//	int i = 1;
//
//	while (i < 10){
//		//pthread_mutex_lock(&mutex);
//		printf("odd thread : %d\n", i);
//
//		//pthread_mutex_unlock(&mutex);
//		pthread_cond_signal(&condA);
//		while (pthread_cond_wait(&condA, &mutex) != 0){
//
//		}
//		i += 2;
//	}
//
//	return NULL;
//}

//
//void* print_thread(void* args){
//	int* a = (int*)args;
//	int i = 0;
//	pthread_mutex_lock(&mutex);
//	printf("%d\n", a[i]);
//	i++;
//	pthread_mutex_unlock
//}

int main()
{
	//matrix_mul(); //Matrix Multiplication Problem

	head = NULL;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&server, NULL, handle_requests, NULL);
	printf("SERVER IS ON!\n-------------------------------\n");
	/*for (int i = 0; i < THREAD_COUNT; i++)
	{
		pthread_create(&help_threads[i],NULL,)
	}*/
	initiate_app();
	//switch_server_on();
	pthread_join(server, NULL);
	pthread_mutex_destroy(&mutex);
	system("pause");
	exit(0);
}