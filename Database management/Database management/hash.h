#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


struct lNode
{
	char* key;
	int val;
	struct lNode* next;
};
struct lNode** bucket;
int bucketLength=100;
int hashFunction(char* str)
{
	int len1 = strlen(str);

	int val = 0;
	for (int i = 0; i < len1; i++)
	{
		val += i*((int)str[i]);
	}
	return val%bucketLength;
}
struct lNode* newNode(char* key, int val)
{
	struct lNode* temp = (struct lNode*) malloc(sizeof(struct lNode));
	temp->key = key;
	temp->val = val;

	temp->next = NULL;
	return temp;
}
void insertIntoList(int index, char* key, int val)
{
	struct lNode* head = bucket[index];
	if (head == NULL)
		head = newNode(key, val);
	else
	{
		struct lNode* temp = head;
		int flag = 0;
		while (temp != NULL)
		{
			if (temp->next == NULL&&strcmp(temp->key, key) != 0)
			{
				break;
			}
			if (strcmp(temp->key, key) == 0)
			{
				temp->val = val;
				flag = 1;
				break;
			}
			temp = temp->next;
		}

		if (flag == 0)
		{
			temp->next = newNode(key, val);
		}
		//fwrite()
	}
	bucket[index] = head;
}
void insertIntoHash(char* str1, int val)
{
	int index = hashFunction(str1);
	insertIntoList(index, str1, val);
}

int* searchInList(char* str1, struct lNode* head)
{
	if (head == NULL)
		return NULL;
	struct lNode* temp = head;
	int* val = (int*)calloc(1,sizeof(int));
	//val = NULL;
	while (temp != NULL)
	{
		if (strcmp(temp->key, str1) == 0)
		{
			*val = temp->val;
			return val;
		}
		temp = temp->next;
	}
	return NULL;
}
int* getHashValue(char* str1)
{
	int index = hashFunction(str1);
	//return index;
	int* val = (int*)malloc(sizeof(int));
	val = searchInList(str1, bucket[index]);
	///*if (val == -1)
	//	return "not found";*/
	/////*else
	////{*/
	///*	return str;
	//}*/
	return val;
}

int* performSearchHash(struct lNode** newBuckets, char* str1){
	bucketLength = 100;
	bucket = newBuckets;
	return getHashValue(str1);
}
struct lNode** initialise(struct lNode** newBuckets)
{
	bucket = (struct lNode**) malloc(bucketLength * sizeof(struct lNode*));
	newBuckets = (struct lNode**) malloc(bucketLength * sizeof(struct lNode*));
	for (int i = 0; i < bucketLength; i++)
	{
		newBuckets[i] = (struct lNode*) malloc(sizeof(struct lNode));
		newBuckets[i] = NULL;
		bucket[i] = (struct lNode*) malloc(sizeof(struct lNode));
		bucket[i] = NULL;
	}
	return newBuckets;
}


void performInsertHash(struct lNode** newBuckets, char* key, int val)
{
	bucketLength = 100;
	bucket = newBuckets;
	insertIntoHash(key, val);
	newBuckets = bucket;

	//return newBuckets;
}
//char* returnValueForKey(struct lNode** newBuckets, int key)
//{
//	bucketLength = 6;
//	bucket = newBuckets;
//	int str = getHashValue(key);
//	return str;
//}
//bool performSearchHash(struct lNode** newBuckets, char* key, char* val)
//{
//	bucketLength = 6;
//	bucket = newBuckets;
//	int val = getHashValue(key);
//	int index = 0;
//	char* temp = getNextToken(&index,str);
//	while (temp != NULL)
//	{
//		if (strcmp(temp, val) == 0)
//			return true;
//		temp = getNextToken(&index, str);
//	}
//	return false;
//}
