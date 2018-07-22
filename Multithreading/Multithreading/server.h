#include "database.h";
#define READ_RQ 1
#define WRITE_RQ 2
#define THREAD_COUNT 4
#define MAX_REQUESTS 100
#define GET_RQ 1
#define PUT_RQ 2

pthread_t server;
pthread_t help_threads[THREAD_COUNT];
int thread_status[THREAD_COUNT];
typedef struct help_thread_info{
	int type;
	int id;
	profile user;
	int index;
}help_thread_info;

pthread_mutex_t mutex;

int rq_count = 0;
typedef struct request_node{
	int id;
	profile user;
	int request_type; //1 - READ, 2- WRITE
	struct request_node* next;
}rn;

typedef struct request{
	int type;
	int id;
	profile user;
}request;

typedef struct get_help_thread{
	int id;
	int index;
}get_help_thread;

typedef struct put_help_thread{
	profile user;
	int index;
};

rn* head;

bool create_user(profile user){
	if (rq_count > MAX_REQUESTS)
	{
		printf("ERROR : SERVER FULL!\n");
		return false;
	}
	rn* request = (rn*)malloc(sizeof(rn));
	request->request_type = WRITE_RQ;
	request->user = user;
	request->next = NULL;

	if (head == NULL)
	{
		head = request;
	}
	else
	{
		rn* temp = head;
		while (temp->next != NULL)
			temp = temp->next;
	}
	rq_count++;
	return true;
}

 bool get_user_profile(int id){
	 if (rq_count > MAX_REQUESTS)
	 {
		 printf("ERROR : SERVER FULL!\n");
		 return false;
	 }
	 rn* request = (rn*)malloc(sizeof(rn));
	 request->request_type = READ_RQ;
	 request->id = id;
	 request->next = NULL;

	 if (head == NULL)
	 {
		 head = request;
	 }
	 else
	 {
		 rn* temp = head;
		 while (temp->next != NULL)
			 temp = temp->next;
	 }
	 rq_count++;
	 return true;
}

 bool client_request(request req){
	 if (req.type == GET_RQ)
	 {
		 if (get_user_profile(req.id))
			 return true;
		 else
			 return false;
	 }
	 else if (req.type == PUT_RQ)
	 { 
		 if (create_user(req.user))
			 return true;
		 else
			 return false;
	 }
 }

 void* handle_get(void* info){
	 get_help_thread a = *((get_help_thread*)info);
	 int user_id = a.id;
	 profile* user = (profile*)malloc(sizeof(profile));
	 pthread_mutex_lock(&mutex);
	 printf("SERVER FETCHING DATA.....\n");
	 user = get_profile(user_id);
	 if (user == NULL)
	 {
		 printf("DATA FETCH ERROR!\n");
		 return NULL;
	 }
	 thread_status[a.index] = 0;
	 pthread_mutex_unlock(&mutex);
	 printf("Fetched user profile:\nId: %d\nFirst Name: %s\nLast Name: %s\nEmail : %s\nGender : %s\n", user->id, user->first_name, user->last_name, user->email, user->gender);
	 printf("THREAD ID: %d\n", pthread_self());
	 return NULL;
 }

 void* handle_put(void* info){
	 
	 pthread_mutex_lock(&mutex);
	 put_help_thread a = *((put_help_thread*)info);
	 profile user = a.user;
	 printf("SERVER INSERTING DATA.......\n");
	 int x = insert_profile(user);
	 thread_status[a.index] = 0;
	 pthread_mutex_unlock(&mutex);
	 printf("PROFILE WITH ID : %d : INSERTED SUCCESSFULLY\n", user.id);
	 printf("THREAD ID: %d\n", pthread_self());
	 return NULL;
 }

void* handle_requests(void*){
	//printf("THREAD ID: %d\n", pthread_self());
	while (1){

		while (head == NULL){
			/*Sleep(100);*/
		}
		rn* temp = head;
		head = temp->next;
		
		int i;
		rq_count--;
		for ( i = 0; i < THREAD_COUNT; i++)
		{
			if (thread_status[i]==0){
				thread_status[i] = 1;
				if (temp->request_type == WRITE_RQ){
					put_help_thread x;
					x.index = i;
					x.user = temp->user;
					pthread_create(&help_threads[i], NULL, handle_put, (void*)&x);
					
					//pthread_join(help_threads[i], NULL);
				}
				else{
					get_help_thread x;
					x.index = i;
					x.id = temp->id;
					pthread_create(&help_threads[i], NULL, handle_get, (void*)&x);
					//pthread_join(help_threads[i], NULL);
				}
				break;
					
			}
		}
		free(temp);
		if (i == 4){
			//Sleep(100);
		}
			

	}
	return NULL;
}

//void switch_server_on()
//{
//	printf("SERVER IS ON!\n-------------------------------\n");
//	head = NULL;
//	pthread_mutex_init(&mutex, NULL);
//	pthread_create(&server, NULL, handle_requests, NULL);
//	pthread_join(server, NULL);
//	pthread_mutex_destroy(&mutex);
//}