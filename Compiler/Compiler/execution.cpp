#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define INTER_CODE_FILE "inter_code.txt"
#define SYMBOLS_FILE "symbols.txt"

typedef struct symbol_row{
	char* symbol;
	int offset;
	int size;
	bool is_const;
	int value;
}symbol_row;

typedef struct code_line{
	int line_num;
	int opcode;
	int* parameters;
	int size;
}code_line;

symbol_row** _symbols;
int _symbols_size;

code_line** _inter_code;
int _code_size;

int* registers;

typedef struct variables{
	char* name;
	int* values;
	int size;
}variables;

char* arr;


/* Returns all the tokens splitting on delimiter */
char** _get_tokens(char* string, char delimiter){

	char** tokens = (char**)calloc(10, sizeof(char*));
	for (int i = 0; i < 10; i++)
		tokens[i] = (char*)calloc(10, sizeof(char));
	//tokens = NULL;
	int token_count = 0;
	for (int i = 0; string[i] != 0; i++)
	{
		int count = 0;
		char* token = (char*)calloc(10, sizeof(char));
		while (1)
		{
			if (string[i] == delimiter || string[i] == '\n')
			{
				token[count] = '\0';
				break;
			}
			else if (string[i] == ' ')
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
		if (count != 0)
			tokens[token_count++] = token;
	}


	return tokens;
}


/* A method to initialise all the globals declared. */
void _initialise_globals()
{
	_symbols = (symbol_row**)calloc(50, sizeof(symbol_row*));
	for (int i = 0; i < 50; i++)
	{
		_symbols[i] = (symbol_row*)calloc(1, sizeof(symbol_row));
		//_symbols[i]->symbol = (char*)calloc(10, sizeof(char));
	}
	_symbols_size = 0;
	_inter_code = (code_line**)calloc(50, sizeof(code_line*));
	for (int i = 0; i < 50; i++){
		_inter_code[i] = (code_line*)calloc(1, sizeof(code_line));
	}
	_code_size = 0;

	registers = (int*)calloc(8, sizeof(int));

	int len = 0;
	if (_symbols_size>0)
		len = _symbols[_symbols_size - 1]->offset + _symbols[_symbols_size - 1]->size;
	arr = (char*)malloc((len + 1));

}

/* Loads intermediate code into globlas from the file*/
void load_inter_code()
{
	FILE* fp = fopen(INTER_CODE_FILE, "r");
	while (!feof(fp))
	{
		char* string = (char*)malloc(100 * sizeof(char));
		fgets(string, 100, fp);
		char** tokens = _get_tokens(string, ',');
		_inter_code[_code_size]->line_num = atoi(tokens[0]);
		_inter_code[_code_size]->opcode = atoi(tokens[1]);
		int size = atoi(tokens[2]);
		_inter_code[_code_size]->size = size;
		int* parameters = (int*)malloc(size*sizeof(int));
		for (int i = 0; i < size; i++)
		{
			parameters[i] = atoi(tokens[3 + i]);
		}
		_inter_code[_code_size]->parameters = parameters;
		_code_size++;
	}
	fclose(fp);
}

/* Loads _symbols into globlas from the file*/
void load_symbols(){

	FILE* fp = fopen(SYMBOLS_FILE, "r");
	while (!feof(fp))
	{
		char* string = (char*)malloc(50 * sizeof(int));
		fgets(string, 100, fp);
		char** tokens = _get_tokens(string, ',');
		_symbols[_symbols_size]->symbol = tokens[0];
		_symbols[_symbols_size]->offset = atoi(tokens[1]);
		_symbols[_symbols_size]->size = atoi(tokens[2]);
		_symbols[_symbols_size]->is_const = atoi(tokens[3]);
		_symbols[_symbols_size]->value = atoi(tokens[4]);
		_symbols_size++;
	}
	fclose(fp);
}

void print_intermediate(){
	printf("INTERMEDIATE CODE : \n=======================================\n");
	for (int i = 0; i < _code_size; i++)
	{
		printf("%d,%d", _inter_code[i]->line_num, _inter_code[i]->opcode);
		for (int j = 0; j < _inter_code[i]->size; j++)
			printf(",%d", _inter_code[i]->parameters[j]);
		printf("\n");
	}
}

void print_symbol_table(){
	printf("SYMBOL TABLE : \n=======================================\n");
	for (int i = 0; i < _symbols_size; i++)
	{
		printf("%s,%d,%d,%d,%d\n", _symbols[i]->symbol, _symbols[i]->offset, _symbols[i]->size, _symbols[i]->is_const, _symbols[i]->value);
	}
}


void get_index_size(int offset, int index, int size){

}
void mova(int* parameters){
	/*reg to mem*/
	/*int size, index;*/

	arr[parameters[0]] = registers[parameters[1]];
}
void movb(int* parameters){
	/* mem to reg */
	registers[parameters[0]] = arr[parameters[1]];
}
void print(int* parameters){

	printf("Value of %cX = %d\n", 'A' + parameters[0], registers[parameters[0]]);
}

void read(int* parameters){
	printf("Enter value of %cX: ", 'A' + parameters[0]);
	scanf("%d", &registers[parameters[0]]);
}

void add(int* parameters){
	registers[parameters[0]] = registers[parameters[1]] + registers[parameters[2]];
}

void sub(int* parameters){
	registers[parameters[0]] = registers[parameters[1]] - registers[parameters[2]];
}

void mul(int* parameters){
	registers[parameters[0]] = registers[parameters[1]] * registers[parameters[2]];
}

bool execute_condition(int opcode, int op1, int op2){
	int a = registers[op1];
	int b = registers[op2];
	if (opcode == 9)
		return a == b;
	else if (opcode == 10)
		return a > b;
	else if (opcode == 11)
		return a < b;
	else if (opcode == 12)
		return a >= b;
	else if (opcode == 13)
		return a <= b;
	return false;
}
void if_func(int* parameters, int* i){

	if (execute_condition(parameters[2], parameters[0], parameters[1]))
		return;
	*i = parameters[3] - 2;
}
void jmp_func(int* parameters, int* i){
	*i = parameters[0] - 2;
}

void call_function(int opcode, int* parameters){
	switch (opcode){
	case 1:
		mova(parameters);
		break;
	case 2:
		movb(parameters);
		break;
	case 3:
		add(parameters);
		break;
	case 4:
		sub(parameters);
		break;
	case 5:
		mul(parameters);
		break;

	case 6:
		read(parameters);
		break;
	case 7:
		print(parameters);
		break;
	}

}

void perform_operations(){

	for (int i = 0; i < _code_size; i++){
		if (_inter_code[i]->opcode == 8)
			if_func(_inter_code[i]->parameters, &i);
		else if (_inter_code[i]->opcode == 14)
			i = _inter_code[i]->parameters[0] - 2;
		else{
			call_function(_inter_code[i]->opcode, _inter_code[i]->parameters);
		}
	}
}

void load_constants(){
	for (int i = 0; i < _symbols_size; i++)
	{
		if (_symbols[i]->is_const)
		{
			arr[_symbols[i]->offset] = _symbols[i]->value;
		}
	}
}

int main()
{
	_initialise_globals();
	load_inter_code();
	load_symbols();

	print_intermediate();
	printf("\n\n");
	print_symbol_table();

	
	
	load_constants();

	perform_operations();
}