#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

#define BUFF_SIZE 4

struct student{
	int id;
	char name[32];
};

struct data_page_student{
	char page_type = 0;
	char table_id;
	char unused[18];
	struct student data[3];
};

struct marks{
	int sid;
	int smarks[4];
};


struct data_page_marks{
	char page_type = 0;
	char table_id;
	char unused[6];
	struct marks data[6];
};

struct table_page{
	char page_type = 2;
	char table_id;
	char unused[6];
	int root_page_id[10];
	char table_names[10][8];
};

struct page_index{
	char page_type = 1;
	char unused[3];
	int page_id[16];
	int key[15];
	
};

struct buffer{
	char free;
	char unused[7];
	int last_accessed_time;
	int page_id;
	char data[128];
};

struct  buffer buffer_pool[BUFF_SIZE];

void initialise_buffers(){
	for (int i = 0; i < BUFF_SIZE; i++)
	{
		buffer_pool[i].free = 't';
		buffer_pool[i].last_accessed_time = 0;
		buffer_pool[i].page_id = -1;
		//buffer_pool[i].data;
	}
}

int bp_alloc(){

	int min_time_stamp = INT_MAX;
	int index = -1;
	for (int i = 0; i < BUFF_SIZE; i++)
	{
		if (buffer_pool[i].free=='t'&&min_time_stamp>buffer_pool[i].last_accessed_time)
		{
			index = i;
			min_time_stamp = buffer_pool[i].last_accessed_time;
		}
	}
	buffer_pool[index].free = 'f';
	return index;
}

void bp_dealloc(int index){
	buffer_pool[index].free = 't';
	buffer_pool[index].last_accessed_time = (int)time(NULL);
}


void perform_join(char* file_name){

	FILE* fp = fopen(file_name, "rb");
	int bp_index1 = bp_alloc();
	fseek(fp, -128, SEEK_END);

	fread((void*)&buffer_pool[bp_index1].data[0], 128, 1, fp);

	struct table_page* tp = (struct table_page*)(&buffer_pool[bp_index1].data[0]);

	int t1_index = tp->root_page_id[0];
	int t2_index = tp->root_page_id[1];

	bp_dealloc(bp_index1);

	int sp_index; //student page index
	int mp_index; //marks page index

	bp_index1 = bp_alloc();
	fseek(fp, t1_index * 128, SEEK_SET);
	fread((void*)&buffer_pool[bp_index1].data[0], 128, 1, fp);
	struct page_index* dp1 = (struct page_index*) (&buffer_pool[bp_index1].data[0]);
	sp_index = dp1->page_id[0];
	bp_dealloc(bp_index1);

	bp_index1 = bp_alloc();
	fseek(fp, t2_index * 128, SEEK_SET);
	fread((void*)&buffer_pool[bp_index1].data[0], 128, 1, fp);
	dp1 = (struct page_index*) (&buffer_pool[bp_index1].data[0]);
	mp_index = dp1->page_id[0];
	bp_dealloc(bp_index1);

	printf("NAME   TOTAL MARKS\n===================================\n");
	for (int i = mp_index; mp_index != -1; i++)
	{
		bp_index1 = bp_alloc();
		fseek(fp, mp_index * 128, SEEK_SET);
		fread((void*)&buffer_pool[bp_index1].data[0], 128, 1, fp);
		struct data_page_marks* mp = (struct data_page_marks*) (&buffer_pool[bp_index1].data[0]);
		if (mp->page_type != 0)
		{
			bp_dealloc(bp_index1);
			break;
		}
		mp_index++;
		
		int bp_index2 = bp_alloc();
		fseek(fp, sp_index * 128, SEEK_SET);
		fread((void*)&buffer_pool[bp_index2].data[0], 128, 1, fp);
		struct data_page_student* sp1 = (struct data_page_student*)(&buffer_pool[bp_index2].data[0]);
		sp_index++;

		int bp_index3 = bp_alloc();
		fseek(fp, sp_index * 128, SEEK_SET);
		fread((void*)&buffer_pool[bp_index3].data[0], 128, 1, fp);
		struct data_page_student* sp2 = (struct data_page_student*)(&buffer_pool[bp_index3].data[0]);
		sp_index++;

		for (int j = 0; j < 3; j++)
		{
			int total_marks = 0;
			for (int k = 0; k < 4; k++)
				total_marks += mp->data[j].smarks[k];

			printf("%s  %d\n", sp1->data[j].name, total_marks);
		}

		for (int j = 0; j < 3; j++)
		{
			int total_marks = 0;
			for (int k = 0; k < 4; k++)
				total_marks += mp->data[j+3].smarks[k];

			printf("%s  %d\n", sp2->data[j].name, total_marks);
		}


		bp_dealloc(bp_index1);
		bp_dealloc(bp_index2);
		bp_dealloc(bp_index3);
	}
}


int get_total_data_pages(char*);


void dump_table_page(char* file_name,int table_count, int* table_offsets, char** table_names){
	struct table_page page;
	for (int i = 0; i < table_count; i++){
		page.page_type = 2;
		page.table_id = 3;
		strcpy(page.table_names[i], table_names[i]);
		page.root_page_id[i] = table_offsets[i];
	}
	for (int i = table_count; i < 10; i++)
	{
		strcpy(page.table_names[i],"");
		page.root_page_id[i] = -1;
	}
	/*FILE* fp = fopen(file_name, "rb");
	fseek(fp, 0, SEEK_END);
	printf("%ld\n", ftell(fp));
	fclose(fp);*/
	FILE* fp = fopen(file_name, "ab");
	fwrite(&page, 128, 1, fp);
	fclose(fp);
	return;
}


struct page_index create_page_index(){
	struct page_index index;
	for (int i = 0; i < 16; i++)
	{
		index.page_id[i] = i;
	}
	for (int i = 0; i < 15; i++){
		index.key[i] = 3 * i;
	}
	return index;
}

void dump_page_indexes(char* file_name, struct page_index** indexes,int final_offset){

	int total_pages = get_total_data_pages(file_name);
	total_pages -= final_offset;
	int total_levels = 0;

	int temp_offset = total_pages;
	while (temp_offset > (int)pow((double)16, total_levels)){
		total_levels++;
	}

	int first_level_nodes = (total_pages % 16) ? (total_pages / 16 + 1) : total_pages / 16;
   
	FILE* fp1 = fopen(file_name, "rb");
	fseek(fp1, 0, SEEK_END);
	printf("%ld\n", ftell(fp1));
	fclose(fp1);

	FILE* fp = fopen(file_name, "ab");
	for (int i = 0; i < total_levels; i++)
	{
		if (i == 0)
			fwrite((void*)indexes[0], sizeof(struct page_index), first_level_nodes, fp);
		else
			fwrite((void*)indexes[1], sizeof(struct page_index), (int)pow((double)16, (total_levels - i - 1)), fp);
	}
	fclose(fp);
}

//void dump_page_index(int no_of_records){
//	no_of_records = no_of_records / 3;
//	int no_of_levels = 0;
//	while (no_of_records < 3){
//		no_of_records = no_of_records / 16;
//		no_of_levels++;
//	}
//	for (int i = 0; i < no_of_levels; i++)
//	{
//
//	}/*
//	while ()*/
//}

/* Checks if there are pages, return true if pages are found, else returns false */
struct data_page_student* check_for_data_student(char* file_name,int offset){
	FILE* fp = fopen(file_name, "rb");
	fseek(fp, offset * 128, SEEK_SET);
	struct data_page_student* page=(struct data_page_student*)calloc(1,sizeof(struct data_page_student));
	fread((void*)page, sizeof(struct data_page_student), 1, fp);
	if (page->data[0].id == 0)
		return NULL;
	fclose(fp);
	return page;
}

struct data_page_marks* check_for_data_marks(char* file_name, int offset){
	FILE* fp = fopen(file_name, "rb");
	fseek(fp, offset * 128, SEEK_SET);
	struct data_page_marks* page = (struct data_page_marks*)calloc(1, sizeof(struct data_page_marks));
	fread((void*)page, sizeof(struct data_page_marks), 1, fp);
	if (page->data[0].sid == 0)
		return NULL;
	fclose(fp);
	return page;
}

int get_total_data_pages(char* file_name){

	FILE* fp = fopen(file_name, "r");
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	fclose(fp);
	int total_pages = bytes / 128;
	return total_pages;
}

//void dump_base_nodes(char* file_name){
//	int offset = get_total_data_pages(file_name);
//	
//	int nodes_count = 0;
//	int data_offset = 0;
//
//	int total_levels = 0;
//	
//	
//	/*while (temp_offset>16){
//		temp_offset /= 16;
//		total_levels++;
//	}*/
//
//	int total_nodes = offset / 16 + offset % 16 + (((int)pow(16.0, total_levels - 1) - 1) / 15);
//
//	int temp_offset = offset;
//	while (temp_offset <= (int)pow((double)16, total_levels)){
//		total_levels++;
//	}
//
//
//	/*struct page_index** indexes = (struct page_index**)
//	struct page_index* indexes = (struct page_index*) calloc(offset,sizeof(struct page_index));
//	if (data_offset!=0)
//	{
//		for (int i = 0; i < offset / 16 + 1; i++){
//
//		}
//
//		while ()
//		int previous_level_index = offset+1;
//		for (int i = 2; i <= total_levels;)
//		{
//
//		}
//
//	}*/
//}

struct page_index** creating_whole_indexes(char* file_name,char* type,int* final_offset){

	int total_pages = get_total_data_pages(file_name);

	total_pages -= (*final_offset);
	int total_levels = 0;
	
	int temp_offset = total_pages;
	while (temp_offset > (int)pow((double)16, total_levels)){
		total_levels++;
	}

	/* Stores all the indexes at all levels */
	struct page_index** indexes = (struct page_index**) malloc(total_levels*sizeof(struct page_index*));
	for (int i = 0; i < total_levels; i++){
		if (i == 0)
			indexes[i] = (struct page_index*) malloc((total_pages / 16 + 1)*sizeof(struct page_index));
		else
			indexes[i] = (struct page_index*) malloc((int) pow((double)16,(total_levels-i-1))*sizeof(struct page_index));
	}
	int previous_level_offset = total_pages + 1;
	temp_offset = 0;
	if (*final_offset!=0)
		temp_offset = *final_offset;

	if (total_levels > 0){ 
		
		/*To load first base level*/
		int first_level_nodes = (total_pages % 16) ? (total_pages / 16 + 1) : total_pages/16;
		for (int i = 0; i < first_level_nodes; i++){
			

			indexes[0][i].page_type = 1;
			for (int j = 0; j < 16; j++){
				if (strcmp(type, "students")==0){
					struct data_page_student* page = check_for_data_student(file_name, temp_offset);
					if (page == NULL)
						break;
					indexes[0][i].page_id[j] = temp_offset;
					if (j != 16)
						indexes[0][i].key[j] = page->data[0].id;
				}
				else if (strcmp(type, "marks")==0){
					struct data_page_marks* page = check_for_data_marks(file_name, temp_offset);
					if (page == NULL){
						for (j = j; j < 16; j++){
							indexes[0][i].page_id[j] = -1;
							if (j!=16)
							indexes[0][i].key[j] = -1;
							
						}
						break;
						
					}
					else{
						indexes[0][i].page_id[j] = temp_offset;
						if (j != 16)
							indexes[0][i].key[j] = page->data[0].sid;
					}
					
					
				}
				
				temp_offset++;
			}
			

		}
		if (total_levels > 1)
		{
			int count = 0;
			/* To load level 2 */
			for (int i = 0; i < (int)pow((double)16, (total_levels - 2)); i++){
				indexes[1][i].page_type = 1;
				for (int j = 0; j < 16; j++){
					if (count == first_level_nodes){
						indexes[1][i].page_id[j] = -1;
						break;
					}
					indexes[1][i].key[j] = indexes[0][i].key[j];
					indexes[1][i].page_id[j] = previous_level_offset + i * 16 + j;
					count++;
				}
			}
			previous_level_offset += total_pages / 16;

		}


		/*To load later levels*/
		for (int i = 1; i < total_levels; i++){
			for (int j = 0; j < (int)pow((double)16, (total_levels - i - 1)); j++)
			{
				/*if (j == 0){
					indexes[i][0].key[0] = indexes[i-1][0].key[0];
					indexes[i][j].page_id[0] = previous_level_offset;
					previous_level_offset = temp_offset;
				}*/
				indexes[1][i].page_type = 1;
				for (int k = 0; k < 16; k++){

					indexes[i][j].key[k] = indexes[i - 1][j].key[k];
					indexes[i][j].page_id[k] = previous_level_offset+j*16+k;
				}
			}
			previous_level_offset += (int)pow((double)16, (total_levels - 2));
		}
	}

	//printf("a");
	*final_offset = previous_level_offset-1;
	return indexes;
}


//struct student search_record_student(char* file_name, int key,struct page_index index){
//	
//	int i = 0;
//	while (i<16&&key>index.key[i])
//		i++;
//	int page_offset = index.page_id[i - 1];
//	
//	FILE* fp = fopen(file_name, "rb");
//	struct data_page_student* temp = (struct data_page_student*) calloc(1,sizeof(struct data_page_student));
//	fread((void*)temp, sizeof(struct data_page_student),1,fp);
//
//	fclose(fp);
//	return temp->data[(key-1) % 3];
//
//}

struct student search_record_student(char* file_name, int key){
	FILE* fp = fopen(file_name, "rb");
	fseek(fp, -128, SEEK_END);
	
	struct page_index base_index;
	fread((void*)&base_index, sizeof(struct page_index), 1, fp);
	int i;
	for ( i = 0; i<16&&key > base_index.key[i]; i++);
	struct data_page_student* temp = (struct data_page_student*) calloc(1, sizeof(struct data_page_student));
	
	if (i != 0)
		fseek(fp, base_index.page_id[i - 1] * 128, SEEK_SET);
	else
		fseek(fp, 0, SEEK_SET);
	fread((void*)temp, sizeof(struct data_page_student), 1, fp);
	fclose(fp);
	return temp->data[(key - 1) % 3];
}
void print_student_list(struct student* list, int size){
	for (int i = 0; i < size; i++)
		printf("%d,%s\n", list[i].id, list[i].name);
}
struct student* get_students_in_range(char* file_name, int gt, int lt,int* result_size){

	int init_offset, final_offset;

	FILE* fp = fopen(file_name, "rb");
	fseek(fp, -128, SEEK_END);
	
	struct page_index base_index;
	fread((void*)&base_index, sizeof(struct page_index), 1, fp);
	int i;
	init_offset = 15;
	for (int i = 0; i < 16; i++)
	{
		if (gt<base_index.key[i]){
			if (i == 0)
				init_offset = 0;
			else
				init_offset = base_index.page_id[i - 1];
			break;
		}
			
	}
	final_offset = 15;
	for (int i = 0; i < 16; i++){
		if (lt < base_index.key[i]){
			final_offset = base_index.page_id[i - 1];
			break;
		}
	}

	struct student* result_set = (struct student*) malloc((lt - gt - 1)*sizeof(struct student));
	//init offset
	struct data_page_student temp;// = (struct data_page_student*) calloc(1, sizeof(struct data_page_student));
	fseek(fp, init_offset * 128, SEEK_SET);
	fread((void*)&temp, sizeof(struct data_page_student), 1, fp);

	int  result_count = 0;
	for (int i = 0; i < 3; i++)
	{
		if (temp.data[i].id > gt)
		{
			result_set[result_count++] = temp.data[i];
			//printf("%d,%s\n", temp.data[i].id, temp.data[i].name);
		}
	}
	for (int i = init_offset+1; i < final_offset; i++)
	{
		fseek(fp, i * 128, SEEK_SET);
		//temp = (struct data_page_student*) calloc(1, sizeof(struct data_page_student));
		fread((void*)&temp, sizeof(struct data_page_student), 1, fp);
		for (int j = 0; j < 3; j++)
		{
			result_set[result_count++] = temp.data[j];
			//printf("%d,%s\n", temp.data[j].id, temp.data[j].name);
		}
	}


	//final offset

	fseek(fp, final_offset * 128, SEEK_SET);
	fread((void*)&temp, sizeof(struct data_page_student), 1, fp);
	for (int i = 0; i < 3; i++)
	{
		if (temp.data[i].id < lt)
		{
			result_set[result_count++] = temp.data[i];
			//printf("%d,%s\n", temp.data[i].id, temp.data[i].name);
		}
	}
	*result_size = result_count;
	return result_set;

}

struct student* filter_range_list_on_name(struct student* input_list, int* size,char* string)
{
	struct student* result_list = (struct student*) calloc((*size),sizeof(struct student));
	int count = 0;
	for (int i = 0; i < *size; i++)
	{
		if (strstr(input_list[i].name, string))
		{
			result_list[count++] = input_list[i];
		}
	}
	*size = count;
	return result_list;
}

/* Helper function that Returns all the tokens splitting the given string on delimiter */
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
			if (string[i] == delimiter||string[i]=='\n')
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


/* Loads data from file into page structures and returns them */
struct data_page_student* load_student_data(char* file_name, int no_of_lines)
{
	FILE*fp = fopen(file_name, "r");

	char* bash = (char*)malloc(150 * sizeof(char));
	fgets(bash, 150, fp);

	int page_count = no_of_lines / 3 ;
	(no_of_lines % 3) ? page_count += 1 : page_count += 0;
	struct data_page_student* pages = (struct data_page_student*) malloc(page_count*sizeof(struct data_page_student));
	int record_count = 0;
	for (int i = 0; i < no_of_lines; i++)
	{
		struct student temp;
		char* str = (char*)calloc(150, sizeof(char));
		fgets(str, 150, fp);
		char** tokens = get_tokens(str, ',');
		temp.id = atoi(tokens[0]);
		strcpy(temp.name, tokens[1]);
		pages[i / 3].page_type = 0;
		pages[i/3].data[record_count] = temp;
		record_count++;
		if (record_count == 3)
			record_count = 0;
	}

	return pages;
}

struct data_page_marks* load_marks_data(char* file_name, int no_of_lines)
{
	FILE*fp = fopen(file_name, "r");

	char* bash = (char*)malloc(150 * sizeof(char));
	fgets(bash, 150, fp);

	int page_count = no_of_lines / 6;
	(no_of_lines % 6) ? page_count += 1 : page_count += 0;
	struct data_page_marks* pages = (struct data_page_marks*) malloc(page_count*sizeof(struct data_page_marks));
	int record_count = 0;
	for (int i = 0; i < no_of_lines; i++)
	{

		struct marks temp;
		char* str = (char*)calloc(150, sizeof(char));
		fgets(str, 150, fp);
		char** tokens = get_tokens(str, ',');
		temp.sid = atoi(tokens[0]);
		temp.smarks[0] = atoi(tokens[1]);
		temp.smarks[1] = atoi(tokens[2]);
		temp.smarks[2] = atoi(tokens[3]);
		temp.smarks[3] = atoi(tokens[4]);
 		//strcpy(temp.name, tokens[1]);
		pages[i / 6].page_type = 0;
		pages[i / 6].data[record_count] = temp;
		record_count++;
		if (record_count == 6)
			record_count = 0;
	}

	return pages;
}

/* Writes obtained page structures to file */
void write_data_page_students(char* file_name, struct data_page_student* pages, int no_of_lines,int offset){
	int page_count = (no_of_lines / 3) ;
	(no_of_lines % 3) ? page_count += 1:page_count+=0;

	FILE* fp1 = fopen(file_name, "wb");
	/*FILE* fp1 = fopen(file_name, "rb");
	fseek(fp1, 0, SEEK_END);
	long bytes = ftell(fp1);*/
	//fseek(fp1, offset * 128, SEEK_SET);
	fwrite((void*)pages, sizeof(struct data_page_student), page_count, fp1);
	fclose(fp1);
}

void write_data_page_marks(char* file_name, struct data_page_marks* pages, int no_of_lines,int offset){
	int page_count = (no_of_lines / 6);
	(no_of_lines % 6) ? page_count += 1 : page_count += 0;

	FILE* fp1 = fopen(file_name, "ab");
	/*FILE* fp1 = fopen(file_name, "rb");
	fseek(fp1, 0, SEEK_END);
	long bytes = ftell(fp1);*/

	//fseek(fp1, offset * 128, SEEK_SET);
	fwrite((void*)pages, sizeof(struct data_page_marks), page_count, fp1);
	fclose(fp1);
}

/* Reads the data from file allocated for pages*/
struct data_page_student* read_data_page_students(char* file_name, int no_of_lines){
	int page_count = (no_of_lines / 3) + 1;
	FILE* fp1 = fopen(file_name, "rb");
	struct data_page_student* pages = (struct data_page_student*) malloc(page_count*sizeof(struct data_page_student));
	fread((void*)pages, sizeof(struct data_page_student), page_count, fp1);
	fclose(fp1);
	return pages;
}

struct data_page_marks* read_data_page_marks(char* file_name, int no_of_lines){
	int page_count = (no_of_lines / 6) + 1;
	FILE* fp1 = fopen(file_name, "rb");
	struct data_page_marks* pages = (struct data_page_marks*) malloc(page_count*sizeof(struct data_page_marks));
	fread((void*)pages, sizeof(struct data_page_marks), page_count, fp1);
	fclose(fp1);
	return pages;
}


/* Prints the data in data page structures */
void print_data_page_students(struct data_page_student* pages, int no_of_lines){
	int page_count = (no_of_lines / 3) + 1;
	for (int i = 0; i < no_of_lines; i++){
		printf("%d,%s\n", pages[i / 3].data[i % 3].id, pages[i / 3].data[i % 3].name);
	}
	return;
}

void print_data_page_marks(struct data_page_marks* pages, int no_of_lines){
	int page_count = (no_of_lines / 6) + 1;
	for (int i = 0; i < no_of_lines; i++){
		printf("%d,%d,%d,%d,%d\n", pages[i / 6].data[i % 6].sid, pages[i / 6].data[i % 6].smarks[0], pages[i / 6].data[i % 6].smarks[1], pages[i / 6].data[i % 6].smarks[2], pages[i / 6].data[i % 6].smarks[3]);
	}
	return;
}
 
/* Overall indexes of all the tables */
void dump_indexes_global(char* file_name){

	int table_count = 0;

	int* table_offsets = (int*)malloc(table_count*sizeof(int));
	
	struct data_page_student* student_pages = load_student_data("randomuserdata.csv", 48);

	write_data_page_students("output6.bin", student_pages, 48,0);

	//pages = read_data_page_students("output6.bin", 48);

	//print_data_page_students(student_pages, 48);

	int table1_index = 0;
	int present_offset = 0;
	struct page_index** student_index = creating_whole_indexes("output6.bin", "students", &table1_index);

	dump_page_indexes("output6.bin", student_index, present_offset);

	struct data_page_marks* marks_pages = load_marks_data("marksdata.csv", 48);

	write_data_page_marks("output6.bin", marks_pages, 48,table1_index);

	//marks_pages = read_data_page_marks("output6.bin", 48);

	//print_data_page_marks(marks_pages, 48);

	int table2_index = table1_index+1;
	present_offset = table1_index;
	struct page_index** marks_index = creating_whole_indexes("output6.bin", "marks",&table2_index);
	table2_index += table1_index;
	dump_page_indexes("output6.bin", marks_index, present_offset);
	table_offsets[0] = table1_index;
	table_offsets[1] = table2_index+1;


	char** table_names = (char**)malloc(2 * sizeof(char));
	for (int i = 0; i < 2; i++)
		table_names[i] = (char*)malloc(10 * sizeof(char));

	table_names[0] = "students";
	table_names[1] = "marks";

	dump_table_page(file_name, 2, table_offsets, table_names);

}

//void print_name_marks(char* file_name){
//	FILE* students_fp = fopen(file_name, "rb");
//	FILE* marks_fp = fopen(file_name, "rb");
//	fseek(students_fp, -128, SEEK_SET);
//	struct table_page table_index;
//	fread(&table_index, sizeof(struct table_page), 1, students_fp);
//
//
//	fseek(students_fp, table_index.root_page_id[0] * 128, SEEK_CUR);
//	fseek(marks_fp, table_index.root_page_id[1] * 128, SEEK_CUR);
//
//	struct page_index student_index;
//	struct page_index mark_index;
//
//	for (int i = 0; mark_index.page_id[i] != -1; i++)
//	{
//		struct marks marks;
//		fseek(marks_fp, mark_index.page_id[i] * 128, SEEK_CUR);
//		fread(&marks, sizeof(struct marks), 1, marks_fp);
//		struct student student;
//		fseek(students_fp, student_index.page_id[i]*128,SEEK_CUR);
//		fread(&student, sizeof(struct student),1, students_fp);
//
//		int j = 0;
//		for (j = j; j < 3; j++)
//		{
//			int total = 0;
//			for (int k = 0; k < 4; k++)
//				total += marks.smarks[k];
//		}
//	}
//	fseek(marks_fp, -128, SEEK_SET);
//
//
//}


int main()
{
	/*struct data_page_student* pages = load_student_data("randomuserdata.csv", 48);

	write_data_page_students("output6.bin", pages, 48);

	pages = read_data_page_students("output6.bin", 48);

	print_data_page_students(pages, 48);
	
	int table1_index=0;
	struct page_index** student_index = creating_whole_indexes("output6.bin","students",&table1_index);

	dump_page_indexes("output6.bin", student_index,table1_index);
*/

	

	//dump_indexes_global("output6.bin");
	/*struct page_index* p = (struct page_index*) malloc(sizeof(page_index));
	FILE* fp = fopen("output6.bin", "rb");
	fseek(fp, 2048, SEEK_SET);
	fread(p, sizeof(page_index),1, fp);
	fclose(fp);*/


	
	initialise_buffers();
	perform_join("output6.bin");

	//while (1){
	//	printf("Enter student id to get the record: ");
	//	int key;
	//	scanf("%d", &key);
	//	struct student s = search_record_student("output6.bin", key);
	//	printf("found record : %d,%s\n", s.id, s.name);
	//}
	


	//printf("Enter range : ");
	//int lt, gt;
	//printf("\ngreater than : ");
	//scanf("%d", &gt);
	//printf("\nless than : ");
	//scanf("%d", &lt);
	//int result_size;
	//struct student* result_set = get_students_in_range("output6.bin", gt, lt,&result_size);
	//print_student_list(result_set, result_size);
	//printf("Enter substring : ");
	//char s[20];
	////free(result_set);
	//scanf("%s", s);
	////int count = result_size;
	//result_set = filter_range_list_on_name(result_set, &result_size, s);
	//print_student_list(result_set, result_size);

	/*struct data_page_marks* pages = load_marks_data("marksdata.csv", 48);

	write_data_page_marks("output6.bin", pages, 48);

	pages = read_data_page_marks("output6.bin", 48);

	print_data_page_marks(pages, 48);





	struct page_index** marks_index = creating_whole_indexes("output7.bin", "marks");
*/

	//while (1){
	//	printf("Enter record id to search : ");
	//	int record;

	//	scanf("%d", &record);

	//	struct student result = search_record("output4.bin", record, index);

	//	printf("Obtained record:\n%d,%s\n", result.id, result.name);
	//}
	


	system("pause");
	
	return 0;
}