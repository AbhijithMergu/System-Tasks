#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include "hash.h"
typedef struct table_col{
	void* value;
	int current_version;
	struct table_col* next_version;

}table_col;

typedef struct table_row{
	char*  row_id;
	int current_version;
	int committed_version;
	table_col** cols;
}table_row;

typedef struct col_type{
	char* name;
	char* type;
}col_type;

typedef struct table_schema{
	char* table_name;
	col_type** col_types;
	int no_of_cols;
	table_row** table_rows;
	int table_size;
	struct lNode** hashMap;
}table_schema;


//int no_of_cols;
//col_type** col_types;
//
//int table_size;
//table_row** table_rows;
//
//struct lNode** hashMap;
//

table_schema** tables;
int table_count;

char* current_dir = "E:\\mac's stuff\\Summer\\Database management\\Database management\\appdata\\";
void initialise_row_file(char* table_name, char* row_id, int col_index, void* value, int version);
int get_table_index(char* table_name){
	for (int i = 0; i < table_count; i++)
		if (strcmp(table_name, tables[i]->table_name) == 0)
			return i;
	return -1;
}

int get_col_index(int table_index, char* col_name){
	
	for (int i = 0; i < tables[table_index]->no_of_cols; i++)
	{
		if (strcmp(col_name, tables[table_index]->col_types[i]->name) == 0)
			return i;
	}
}

char** get_tokens(char* string, char delimiter){
	/* Returns all the tokens splitting on delimiter */

	char** tokens = (char**)calloc(6, sizeof(char*));
	for (int i = 0; i < 6; i++)
		tokens[i] = (char*)calloc(10, sizeof(char));
	//tokens = NULL;
	int token_count = 0;
	for (int i = 0; string[i] != 0; i++)
	{
		int count = 0;
		char* token = (char*)calloc(10, sizeof(char));
		while (1)
		{
			if (string[i] == delimiter)
			{
				token[count] = '\0';
				break;
			}
			else if (string[i] == '\0')
			{
				token[count] = '\0';
				break;
			}
			else if (string[i] == '\n')
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

void* scanf_type(char* col_type,char* name){
	/* Takes input for corresponding data type */
	if (strcmp(col_type, "int") == 0){
		int* input = (int*)malloc(sizeof(int));
		printf("Enter %s: ",name);
		scanf("%d", input);
		return (void*)input;
	}
	else if (strcmp(col_type, "char") == 0){
		char* input = (char*)malloc(sizeof(char));
		printf("Enter %s: ",name); 
		fflush(stdin);
		scanf("%c", input);
		return (void*)input;
	}
	else if (strcmp(col_type, "string") == 0){
		char* input = (char*)calloc(30,sizeof(char));
		printf("Enter %s: ",name);
		fflush(stdin);
		scanf("%s", input);
		return (void*)input;
	}
}

void insert_row(char* table_name){
	/*Insert rows by taking input from console */
	//printf("Enter table name : ");
	//char* table_name = (char*)calloc(20, sizeof(char));
	//scanf("%s", table_name);
	int table_index = get_table_index(table_name);
	printf("Enter row id : ");
	
	char* id = (char*)calloc(20, sizeof(char));
	scanf("%s", id);
	performInsertHash(tables[table_index]->hashMap,id, tables[table_index]->table_size);
	for (int i = 0; i < tables[table_index]->no_of_cols;i++)
	{
		table_col* col = (table_col*)calloc(1, sizeof(table_col));
		col->value = scanf_type(tables[table_index]->col_types[i]->type, tables[table_index]->col_types[i]->name);
		col->next_version = NULL;
		col->current_version = 0;
		tables[table_index]->table_rows[tables[table_index]->table_size]->cols[i] = col;
	}
	tables[table_index]->table_rows[tables[table_index]->table_size]->row_id = id;
	tables[table_index]->table_size = tables[table_index]->table_size+1;
}

void print_type(int table_index,int row_index,int col_index){
	/*Prints data with corresponding data type */
	char* col_type = tables[table_index]->col_types[col_index]->type;

	table_col* temp = tables[table_index]->table_rows[row_index]->cols[col_index];
	while (temp->next_version != NULL)
		temp = temp->next_version;

	if (strcmp(col_type, "int") == 0){
		
		printf("%d\t", *((int*)temp->value));
	}
	else if (strcmp(col_type, "char") == 0){
		printf("%c\t", *((char*)temp->value));
	}
	else if (strcmp(col_type, "string") == 0){
		printf("%s\t", ((char*)temp->value));
	}
}

void print_row(int table_index,int row_index){
	for (int i = 0; i < tables[table_index]->no_of_cols; i++)
	{
		print_type(table_index,row_index, i);
		//printf("\n");
	}
	printf("\n");

}

void print_table(char* table_name){
	/*char* table_name = (char*)calloc(20, sizeof(char));
	printf("Enter table name : ");
	scanf("%s", table_name);*/
	int table_index = get_table_index(table_name);
	if (table_index == -1){
		printf("Table Not found!\n");
		return;
	}
	printf("TABLE:\n==========================\n");
	printf("ROWID\t");
	for (int i = 0; i < tables[table_index]->no_of_cols; i++)
		printf("%s\t", tables[table_index]->col_types[i]->name);
	printf("\n----------------------------------------------------------\n");
	printf("\n");
	for (int row_index = 0; row_index < tables[table_index]->table_size; row_index++)
	{
		printf("%s\t", tables[table_index]->table_rows[row_index]->row_id);
		print_row(table_index, row_index);
	}

}


void print_id_row(char* table_name)
{
	/*char* table_name = (char*)calloc(20, sizeof(char));
	printf("Enter table name : ");
	scanf("%s", table_name);*/
	int table_index = get_table_index(table_name);
	char* id = (char*)calloc(20, sizeof(int));
	printf("Enter id : ");
	scanf("%s", id);
	int* a = performSearchHash(tables[table_index]->hashMap,id);
	print_row(table_index,*a);
}

//void update_col_api(int table_index, int row_index, int col_index, void* value){
//	table_col* new_version = (table_col*)calloc(1, sizeof(table_col));
//	new_version->current_version = tables[table_index]->table_rows[row_index]->current_version;
//	new_version->value = value;
//	new_version->next_version =NULL;
//	//tables[table_index]->table_rows[row_index]->cols[col_index] = new_version;
//	table_col* temp = tables[table_index]->table_rows[row_index]->cols[col_index];
//	if (temp->next_version == NULL)
//	{
//		temp->next_version = new_version;
//	}
//	else
//	{
//		while (temp->next_version != NULL)
//		{
//			temp = temp->next_version;
//		}
//		temp->next_version = new_version;
//	}
//}

void update_col_api_w_version(int table_index, int row_index, int col_index, void* value, int version){
	/* Helper function for update api call, which accepts version and other parameters and updates columns */
	table_col* new_version = (table_col*)calloc(1, sizeof(table_col));
	new_version->current_version = version;
	new_version->value = value;
	new_version->next_version = NULL;
	//tables[table_index]->table_rows[row_index]->cols[col_index];
	table_col* temp = tables[table_index]->table_rows[row_index]->cols[col_index];
	if (temp == NULL)
	{
		tables[table_index]->table_rows[row_index]->cols[col_index] = new_version;
	}
	else if (temp->next_version == NULL)
	{
		temp->next_version = new_version;
	}
	else
	{
		while (temp->next_version != NULL)
		{
			temp = temp->next_version;
		}
		temp->next_version = new_version;
	}
	//tables[table_index]->table_rows[row_index]->cols[col_index] = new_version;
}

void update_col(int table_index,int row_index,int col_index){
	table_col* new_version = (table_col*)calloc(1, sizeof(table_col));
	new_version->current_version = tables[table_index]->table_rows[row_index]->current_version;
	new_version->value = scanf_type(tables[table_index]->col_types[col_index]->type, tables[table_index]->col_types[col_index]->name);
	new_version->next_version = NULL;// tables[table_index]->table_rows[row_index]->cols[col_index];
	//tables[table_index]->table_rows[row_index]->cols[col_index] = new_version;
	table_col* temp = tables[table_index]->table_rows[row_index]->cols[col_index];
	if (temp->next_version == NULL)
	{
		temp->next_version = new_version; 
	}
	else
	{
		while (temp->next_version != NULL)
		{
			temp = temp->next_version;
		}
		temp->next_version = new_version;
	}


}
//void update_row(){
//	printf("Enter row number to update: ");
//	int row_index;
//
//	scanf("%d", &row_index);
//	table_rows[row_index]->current_version = table_rows[row_index]->current_version + 1;
//	printf("Enter number of cols to update: ");
//	int count;
//	scanf("%d", &count);
//	for (int i = 0; i < count; i++)
//	{
//		printf("Enter col number to update: ");
//		int col_index;
//		scanf("%d", &col_index);
//		update_col(row_index, col_index);
//
//	}
//}

void print_columns(int table_index){
	printf("\nCOLUMNS:\n");
	for (int i = 0; i < tables[table_index]->no_of_cols; i++)
		printf("%d. %s\n", i + 1, tables[table_index]->col_types[i]->name);
}
void update_id_row(char* table_name){
	//char* table_name = (char*)calloc(20, sizeof(char));
	//scanf("%s", table_name);
	int table_index = get_table_index(table_name);
	char* id = (char*)calloc(20, sizeof(int));
	printf("Enter id : ");
	scanf("%s", id);
	int* a = performSearchHash(tables[table_index]->hashMap,id);
	if (a == NULL){
		printf("Row not found!\n");
		return;
	}
	int row_index = *a;
	tables[table_index]->table_rows[row_index]->current_version = tables[table_index]->table_rows[row_index]->current_version + 1;
	print_columns(table_index);
	printf("Enter number of cols to update: ");
	int count;
	scanf("%d", &count);
	for (int i = 0; i < count; i++)
	{
		printf("Enter col number to update: ");
		int col_index;
		scanf("%d", &col_index);
		update_col(table_index,row_index, col_index-1);

	}
	//tables[table_index]->table_rows[row_index]->committed_version = tables[table_index]->table_rows[row_index]->current_version;
}


bool _put_(char* table_name,char* row_id, table_col* column,int col_num,int commit_version){
	int table_index = get_table_index(table_name);

	int* a = performSearchHash(tables[table_index]->hashMap,row_id);
	if (a == NULL){
		printf("Row not found!\n");
		return false;
	}

	for (int i = 0; i < col_num; i++)
	{
		update_col(table_index, *a, col_num);
	}
	
}

void* get_value_from_string(char* value, char* col_type)
{
	if (strcmp(col_type, "int") == 0)
	{
		int* a = (int*)malloc(sizeof(int));
		*a = atoi(value);
		return (void*)a;
	}
	else if (strcmp(col_type, "char") == 0)
	{
		return (void*)value;
	}
	else if (strcmp(col_type, "string") == 0)
		return (void*)value;
	return NULL;
}
bool _put_strings(char* table_name, char* row_id, char** col_names, char** col_values, int col_num, int commit_version){
	int table_index = get_table_index(table_name);

	int* a = performSearchHash(tables[table_index]->hashMap, row_id);
	if (a == NULL){
		printf("Row not found!\n");
		
		return false;
	}
	for (int i = 0; i < col_num; i++)
	{
		int col_index = get_col_index(table_index, col_names[i]);
		if (a==NULL)
			initialise_row_file(table_name, row_id, col_index, get_value_from_string(col_values[i], tables[table_index]->col_types[col_index]->type), commit_version);
		else
			update_col_api_w_version(table_index, *a, col_index, get_value_from_string(col_values[i],tables[table_index]->col_types[col_index]->type),commit_version);
	}
	return true;
}


table_row* _get_(char* table_name,char* id){
	int table_index = get_table_index(table_name);

	int* a = performSearchHash(tables[table_index]->hashMap,id);
	if (a == NULL){
		printf("Row not found!\n");
		return NULL;
	}
	int row_index = *a;
	return tables[table_index]->table_rows[row_index];
}

bool _delete_(char* table_name,char* id){
	int table_index = get_table_index(table_name);
	int* a = performSearchHash(tables[table_index]->hashMap,id);
	if (a == NULL){
		printf("Row not found!\n");
		return false;
	}
	int row_index = *a; 
	for (int i = row_index; i < tables[table_index]->table_size; i++)
		tables[table_index]->table_rows[i] = tables[table_index]->table_rows[i + 1];
	tables[table_index]->table_size=tables[table_index]->table_size-1;
	return true;
}
void delete_id_row(char* table_name){
	//char* table_name = (char*)calloc(20, sizeof(char));
	//printf("Enter table name : ");
	//scanf("%s", table_name);
	int table_index = get_table_index(table_name);
	char* id = (char*)calloc(20, sizeof(int));
	printf("Enter id : ");
	fflush(stdin);
	scanf("%s", id);

	if (_delete_(table_name,id))
	{
		printf("Deleted Successfully!\n");
	}
	else
	{
		printf("Delete unsuccessful\n");
	}
	
}

void create_table(char* table_name,int no_of_cols, col_type** col_types){

	tables[table_count] = (table_schema*)calloc(1,sizeof(table_schema));
	
	tables[table_count]->table_name = table_name;
	tables[table_count]->col_types = col_types;
	lNode** hashMap=NULL;
	tables[table_count]->hashMap = initialise(hashMap);
	tables[table_count]->table_size = 0;
	tables[table_count]->no_of_cols = no_of_cols;

	table_row** table_rows = (table_row**)calloc(50, sizeof(table_row*));
	for (int i = 0; i < 50; i++)
	{
		table_rows[i] = (table_row*)calloc(1, sizeof(table_row));
		table_rows[i]->committed_version = -1;
		table_rows[i]->current_version = 0;
		table_rows[i]->cols = (table_col**)calloc(no_of_cols, sizeof(table_col*));
	}
	tables[table_count]->table_rows = table_rows;
	table_count++;
}


void create_table_strings(char*  table_name, int no_of_cols, char** col_names, char** col_data_types){
	tables[table_count] = (table_schema*)calloc(1,sizeof(table_schema));
	tables[table_count]->table_name = table_name;
	tables[table_count]->no_of_cols = no_of_cols;
	col_type** col_types = (col_type**)calloc(no_of_cols, sizeof(col_type*));

	for (int i = 0; i < no_of_cols; i++)
		col_types[i] = (col_type*)calloc(1,sizeof(col_type));
	for (int i = 0; i < no_of_cols; i++)
	{
		col_types[i]->name = col_names[i];
		col_types[i]->type = col_data_types[i];
	}
	tables[table_count]->col_types = col_types;
	lNode** hashMap=NULL;
	tables[table_count]->hashMap = initialise(hashMap);
	tables[table_count]->table_size = 0;

	table_row** table_rows = (table_row**)calloc(50, sizeof(table_row*));
	for (int i = 0; i < 50; i++)
	{
		table_rows[i] = (table_row*)calloc(1, sizeof(table_row));
		table_rows[i]->committed_version = -1;
		table_rows[i]->current_version = 0;
		table_rows[i]->cols = (table_col**)calloc(no_of_cols, sizeof(table_col*));
	}
	tables[table_count]->table_rows = table_rows;
	table_count++;


}


void create_tables_from_file(){
	FILE* fp = fopen("meta_data2.txt","r");
	int no_of_tables;
	fscanf(fp, "%d", &no_of_tables);
	for (int i = 0; i < no_of_tables; i++)
	{
		char* table_name = (char*)calloc(20, sizeof(char));
		int no_of_cols;
		char* col_names_string = (char*)calloc(100, sizeof(char));
		char* col_types_string = (char*)calloc(100, sizeof(char));

		fscanf(fp, "%s", table_name);
		fscanf(fp, "%d", &no_of_cols);
		fscanf(fp, "%s", col_names_string);
		fscanf(fp, "%s", col_types_string);

		char** col_names = get_tokens(col_names_string, ',');
		char** col_types = get_tokens(col_types_string, ',');

		create_table_strings(table_name, no_of_cols, col_names, col_types);
	}
	fclose(fp);
}

void initialise_row_file(char* table_name,char* row_id,int col_index,void* value,int version){

	int table_index = get_table_index(table_name);
	performInsertHash(tables[table_index]->hashMap, row_id, tables[table_index]->table_size);
	table_col* col = (table_col*)calloc(1, sizeof(table_col));
	col->value = value;
	col->next_version = NULL;
	col->current_version = version;
	
	tables[table_index]->table_rows[tables[table_index]->table_size]->cols[col_index] = col;
	tables[table_index]->table_rows[tables[table_index]->table_size]->row_id = row_id;
	//tables[table_index]->table_rows[tables[table_index]->table_size]->
	tables[table_index]->table_size = tables[table_index]->table_size + 1;
}

void get_all_of_col_from_file(char* table_name, col_type* col_type_, int col_index )
{
	int table_index = get_table_index(table_name);
	char* file_path = (char*)calloc(200, sizeof(char));
	strcat(file_path, current_dir);
	strcat(file_path, table_name);
	strcat(file_path, "\\");
	strcat(file_path, col_type_->name);
	strcat(file_path, ".txt");

	FILE* fp = fopen(file_path, "r");
	while (!feof(fp)){
		char* row = (char*)calloc(50, sizeof(char));
		fscanf(fp, "%s\n", row);
		char** tokens = get_tokens(row, ',');
		int* row_index = performSearchHash(tables[table_index]->hashMap, tokens[0]);
		void* value = get_value_from_string(tokens[1], col_type_->type);
		if (row_index == NULL)
		{
			initialise_row_file(table_name, tokens[0], col_index, value, atoi(tokens[2]));
		}
		else
		{
			update_col_api_w_version(table_index, *row_index, col_index, value, atoi(tokens[2]));
		}
		
		
		//update_col_api()
	}
	fclose(fp);
	
}
void load_rows_from_file(char* table_name){

	int table_index = get_table_index(table_name);
	for (int i = 0; i < tables[table_index]->no_of_cols; i++){
		get_all_of_col_from_file(table_name, tables[table_index]->col_types[i], i);
	}

	//int version = tables[table_index]->table_rows[]
	for (int i = 0; i < tables[table_index]->table_size; i++)
	{
		int version = 0;
		for (int j = 0; j < tables[table_index]->no_of_cols; j++)
		{
			table_col* temp = tables[table_index]->table_rows[i]->cols[j];
			if (temp == NULL)
				continue;
			while (temp->next_version != NULL)
				temp = temp->next_version;

			if (temp->current_version > version){
				version = tables[table_index]->table_rows[i]->cols[j]->current_version;
			}
		}
		tables[table_index]->table_rows[i]->current_version = version;
		tables[table_index]->table_rows[i]->committed_version = version;
	}
}

void dump_all_cols_into_file(FILE*fp,char* table_name, char* row_id, col_type* col_type_,void* value, int version){

	int table_index = get_table_index(table_name);
	fprintf(fp,"%s,", row_id);
	if (strcmp(col_type_->type, "int")==0){
		fprintf(fp, "%d", *((int*)value));
	}
	else if (strcmp(col_type_->type, "char")==0){
		fprintf(fp, "%c", *((char*)value));
	}
	else if (strcmp(col_type_->type, "string")==0){
		fprintf(fp, "%s", (char*)value);
	}
	fprintf(fp,",%d\n", version);
}

void dump_rows_into_file(char* table_name){
	int table_index = get_table_index(table_name);
	for (int i = 0; i < tables[table_index]->no_of_cols; i++)
	{
		char* file_path = (char*)calloc(200, sizeof(char));
		strcat(file_path, current_dir);
		strcat(file_path, table_name);
		strcat(file_path, "//");
		CreateDirectory(file_path, NULL);
		strcat(file_path, tables[table_index]->col_types[i]->name);
		strcat(file_path, ".txt");

		FILE* fp = fopen(file_path, "a");

		for (int j = 0; j < tables[table_index]->table_size; j++){
			
			table_col* col = tables[table_index]->table_rows[j]->cols[i];

			while (col!=NULL&&col->current_version <= tables[table_index]->table_rows[j]->committed_version)
			{
				col = col->next_version;
			}
			while (col!=NULL)
			{
				dump_all_cols_into_file(fp, table_name, tables[table_index]->table_rows[j]->row_id, tables[table_index]->col_types[i], col->value, col->current_version);
				col=col->next_version;
			}
		}
		fclose(fp);
	}
	for (int i = 0; i < tables[table_index]->table_size; i++)
	{
		int version = 0;
		for (int j = 0; j < tables[table_index]->no_of_cols; j++)
		{
			table_col* temp = tables[table_index]->table_rows[i]->cols[j];
			if (temp == NULL)
				continue;
			while (temp->next_version != NULL)
				temp = temp->next_version;

			if (temp->current_version > version){
				version = tables[table_index]->table_rows[i]->cols[j]->current_version;
			}
		}
		//tables[table_index]->table_rows[i]->current_version = version;
		tables[table_index]->table_rows[i]->committed_version = version;
	}

}

void dump_table_meta(){
	FILE* fp = fopen("meta_data2.txt", "w");

	fprintf(fp, "%d\n", table_count);
	for (int i = 0; i < table_count; i++)
	{
		fprintf(fp, "%s\n", tables[i]->table_name);
		fprintf(fp, "%d\n", tables[i]->no_of_cols);
		for (int j = 0; j < tables[i]->no_of_cols; j++){
			if (j != tables[i]->no_of_cols - 1)
				fprintf(fp, "%s,", tables[i]->col_types[j]->name);
			else
				fprintf(fp, "%s", tables[i]->col_types[j]->name);
		}
		fprintf(fp, "\n");
		for (int j = 0; j < tables[i]->no_of_cols; j++){
			if (j != tables[i]->no_of_cols - 1)
				fprintf(fp, "%s,", tables[i]->col_types[j]->type);
			else
				fprintf(fp, "%s", tables[i]->col_types[j]->type);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}

void get_cols(char* table_name){
	int no_of_cols;
	printf("Enter the number of columns : ");
	scanf("%d", &no_of_cols);
	col_type** col_types = (col_type**)calloc(no_of_cols, sizeof(col_type*));
	for (int i = 0; i < no_of_cols; i++)
	{
		col_types[i] = (col_type*)calloc(1, sizeof(col_type));
		col_types[i]->name = (char*)calloc(20, sizeof(char));
		col_types[i]->type = (char*)calloc(20, sizeof(char));
	}


	for (int i = 0; i < no_of_cols; i++)
	{
		printf("Enter col name : ");
		scanf("%s", col_types[i]->name);
		printf("Enter col type: ");
		scanf("%s", col_types[i]->type);
	}

	create_table(table_name, no_of_cols, col_types);
}



void delete_table(char* table_name){
	int table_index = get_table_index(table_name);
	for (int i = table_index; i < table_count; i++)
		tables[i] = tables[i + 1];
	table_count--;
}

void print_table_names(){
	printf("\n\nTABLES\n-------------\n");
	for (int i = 0; i < table_count; i++)
		printf("%s\n", tables[i]->table_name);
	printf("\n\n");
}

void perform_table_operations(char* table_name){
	int table_index = get_table_index(table_name);
	while (1){
		printf("\n\nOPTIONS\n-1.LOAD TABLE\n0.DUMP TABLE\n1.INSERT ROW\n2.UPDATE VALUES\n3.PRINT TABLE\n4.GET ROW\n5.DELETE ROW\n6.GO BACK\n7.EXIT\n");
		printf("Enter option : ");
		int choice;
		scanf("%d", &choice);
		switch (choice)
		{
		case -1:
			load_rows_from_file(table_name);
			break;
		case 0:
			dump_rows_into_file(table_name);
			break;
		case 1:
			insert_row(table_name);
			break;
		case 2:
			update_id_row(table_name);
			break;
		case 3:
			print_table(table_name);
			break;
		case 4:
			print_id_row(table_name);
			break;
		case 5:
			delete_id_row(table_name);
			break;
		case 6:
			return;
		case 7:
			exit(1);
		default:
			printf("Invalid Option!\n\n");
		}
	}
	
}


int main()
{
	
	/*printf("Enter number of tables: ");
	scanf("%d", &count);
	for (int i = 0;i<)
	hashMap = initialise(hashMap);
	

	table_rows = (table_row**)calloc(50, sizeof(table_row*));
	for (int i = 0; i < 50; i++)
	{
		table_rows[i] = (table_row*)calloc(1,sizeof(table_row));
		table_rows[i]->committed_version = 0;
		table_rows[i]->current_version = 0;
		table_rows[i]->cols = (table_col**)calloc(no_of_cols, sizeof(table_col*));
	}
*/
	tables = (table_schema**)calloc(10, sizeof(table_schema*));
	table_count = 0;
	int choice;
	while (1)
	{
		printf("\n\nOPTIONS\n-1.DUMP META\n0. LOAD DB\n1. VIEW ALL TABLE NAMES\n2. CREATE TABLE\n3. SELECT TABLE\n4. DELETE TABLE\n5. PRINT TABLE\n6. EXIT\n");
		printf("Enter option: ");
		scanf("%d", &choice);
		switch (choice){	
		case -1:
			dump_table_meta();
			break;
		case 0:
			create_tables_from_file();
			break;
		case 1:
			print_table_names();
			break;
		case 2:
		{
			printf("Enter table name: ");
			char* table_name = (char*)calloc(20, sizeof(char));
			scanf("%s", table_name);
			get_cols(table_name);
			break;
		}
			
		case 3:
		{
			printf("Enter table name : ");
			char* table_name = (char*)calloc(20, sizeof(char));
			scanf("%s", table_name);
			perform_table_operations(table_name);
			break;

		}
			
		case 4:
		{
			printf("Enter table name: ");
			char* table_name = (char*)calloc(20, sizeof(char));
			scanf("%s", table_name);
			delete_table(table_name);
			free(table_name);
			break;
		}
		case 5:
		{
			printf("Enter table name: ");
			char* table_name = (char*)calloc(20, sizeof(char));
			scanf("%s", table_name);
			print_table(table_name);
			free(table_name);
			break;
		}
		case 6:
			exit(1);
			break;
		default:
			printf("Invalid option try again!");
		}
		
			
	}
	/*lNode** hashMap=NULL;
	hashMap = initialise(hashMap);
	performInsertHash(hashMap, "abhijith", 4);
	
	int* b = performSearchHash(hashMap, "abhijith");
	printf("%d", *b);*/
	system("pause");
	return 0;

}