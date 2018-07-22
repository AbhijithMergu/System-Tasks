#include "server.h"
#define input_file "MOCK_DATA.csv"
pthread_t put_thread;
pthread_t get_thread;
FILE* fp;
int write_id = 1;
int read_id = 1;
int max_id = 500;
char** get_tokens(char* string, char delimiter)
{
	char** tokens = (char**)calloc(6, sizeof(char*));
	for (int i = 0; i < 6; i++)
		tokens[i] = (char*)calloc(100, sizeof(char));
	//tokens = NULL;
	int token_count = 0;
	for (int i = 0; string[i] != 0; i++)
	{
		int count = 0;
		char* token = (char*)calloc(100, sizeof(char));
		while (1)
		{
			if (string[i] == delimiter || string[i] == '\n')
			{
				token[count] = '\0';
				break;
			}
			else if (string[i] == '\0')
			{
				token[count] = '\0';
				break;
			}
			else
			{
				token[count++] = string[i];
			}
			i++;
		}
		tokens[token_count++] = token;
	}
	return tokens;
}


void* put_request(void*)
{
	while (1){
		if (write_id >= max_id)
		{
			printf("PUT REQUESTS DONE!\n");
			break;
		}
		char* string = (char*)malloc(130 * sizeof(char));
		fgets(string, 130, fp);
		char** tokens = get_tokens(string, ',');
		
		Sleep(3000);
		profile user;
		user.id = atoi(tokens[0]);
		strcpy(user.first_name, tokens[1]);
		strcpy(user.last_name, tokens[2]);
		strcpy(user.email, tokens[3]);
		strcpy(user.gender, tokens[4]);
		printf("\n\nPUT REQUEST SENT!\n");
		request rq;
		rq.type = PUT_RQ;
		rq.user = user;
		if(!client_request(rq))
			system("pause");
		write_id++;
		//Sleep(3000);
	}
	return NULL;
	
}

void* get_request(void*)
{
	while (1)
	{
		if (read_id >= max_id)
		{
			printf("GET REQUESTS DONE!\n");
			break;
		}
		Sleep(5000);
		request req;
		req.type = GET_RQ;
		req.id = read_id;
		printf("\n\nGET REQUEST SENT!\n");
		if(!client_request(req))
			system("pause");
		read_id++;
		//Sleep(15000);
	}
	return NULL;
}


void initiate_app()
{
	fp = fopen(input_file, "r");
	char* string = (char*)malloc(130 * sizeof(char));
	fgets(string,130,fp);

	pthread_create(&put_thread, NULL, put_request, NULL);
	pthread_create(&get_thread, NULL, get_request, NULL);

	pthread_join(put_thread, NULL);
	pthread_join(get_thread, NULL);

	fclose(fp);
}