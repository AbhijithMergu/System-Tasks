#define PROFILE_SIZE 120
#define DB_LINK "profile_data.bin"
typedef struct profile{
	int id;
	char first_name[20];
	char last_name[20];
	char email[60];
	char gender[6];

}profile;

int insert_profile(profile user){
	printf("DB INSERT\n");
	FILE* fp = fopen(DB_LINK, "ab");
	fwrite(&user, PROFILE_SIZE, 1, fp);
	fclose(fp);
	return 1;
}

profile* get_profile(int id){
	printf("DB GET\n");
	FILE* fp = fopen(DB_LINK, "rb");
	if (fp == NULL)
		return NULL; 
	fseek(fp, (id - 1)*PROFILE_SIZE, SEEK_SET);
	profile* user = (profile*) malloc(sizeof(profile));
	fread(user, PROFILE_SIZE, 1, fp);
	return user;
}