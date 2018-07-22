#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include "execution.h"

#define INTER_CODE_FILE "inter_code.txt"
#define SYMBOLS_FILE "symbols.txt"
#define LABELS_FILE "labels.txt"

typedef struct opcode{
	char* operation;
	int code;
}opcode;

typedef struct symbol_row{
	char* symbol;
	int offset;
	int size;
	bool is_const;
	int value;
}symbol_row;

typedef struct labels{
	char* label;
	int line;
}labels;

typedef struct code_line{
	int line_num;
	int opcode;
	int* parameters;
	int size;
}code_line;

opcode** opcodes;
int opcodes_size;
symbol_row** symbols;
int symbols_size;

code_line** inter_code;
int code_size;

labels** label_table;
int labels_size;

int* stack; 
int* sp;

/* Updates the parameter with opcodes from file and returns the number of opcodes */
int get_opcodes(){
	

	FILE* fp = fopen("opcodes.txt", "r");

	opcodes = (opcode**)calloc(20, sizeof(opcode*));
	for (int i = 0; i < 20; i++)
	{
		opcodes[i] = (opcode*)calloc(1, sizeof(opcode));
		opcodes[i]->operation = (char*)calloc(8, sizeof(char));
	}
		

	int count = 0;
	while (!feof(fp))
	{
		fscanf(fp, "%d,%s\n", &opcodes[count]->code, opcodes[count]->operation);
		count++;
	}

	return count;
}


/* Returns all the tokens splitting on delimiter */
char** get_tokens(char* string, char delimiter){
	
	char** tokens = (char**)calloc(6, sizeof(char*));
	for (int i = 0; i < 6; i++)
		tokens[i] = (char*)calloc(10 , sizeof(char));
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
		if (count!=0)
			tokens[token_count++] = token;
	}
	return tokens;
}


/* Checks if the given input represents an array. (By checking the format for []) */
bool is_array(char* string){
	
	for (int i = 0; string[i] != 0; i++)
		if (string[i] == '[')
			return true;
	return false;

}

/* Checks if the operand is a constant (By checking for the '=' presence.)*/
bool is_const(char* string){
	for (int i = 0; string[i] != 0; i++)
	{
		if (string[i] == '=')
			return true;
	}
	return false;
}

/* Inserts symbols into symbol table. */
void insert_symbol_data(char* operand)
{
	if (is_array(operand)){

		char* symbol = (char*)calloc(10, sizeof(char));
		int i;
		int count = 0;
		for (i = 0; operand[i] != '['; i++){
			symbol[count++] = operand[i];
		}
		symbol[count] = '\0';

		char* size_str = (char*)calloc(5 ,sizeof(char));
		count = 0;
		for (i = i + 1; operand[i] != ']'; i++){
			size_str[count++] = operand[i];
		}
		size_str[count] = '\0';
		
		int size = atoi(size_str);
		if (symbols_size == 0)
		{
			symbols[symbols_size]->offset = 0;
			
		}
		else
		{
			symbols[symbols_size]->offset = symbols[symbols_size-1]->offset+symbols[symbols_size-1]->size;
		}
		symbols[symbols_size]->size = size;
		symbols[symbols_size]->symbol = symbol;
		symbols[symbols_size]->is_const = false;
		symbols[symbols_size]->value = -1;
		symbols_size++;
		
	}
	else if (is_const(operand)){
		if (symbols_size == 0)
		{
			symbols[symbols_size]->offset = 0;

		}
		else
		{
			symbols[symbols_size]->offset = symbols[symbols_size - 1]->offset + symbols[symbols_size - 1]->size;
		}
		symbols[symbols_size]->size = 1;

		char** tokens = get_tokens(operand, '=');
		symbols[symbols_size]->symbol = tokens[0];
		symbols[symbols_size]->is_const = true;
		symbols[symbols_size]->value = atoi(tokens[1]);
		symbols_size++;
	}
	else
	{
		if (symbols_size == 0)
		{
			symbols[symbols_size]->offset = 0;

		}
		else
		{
			symbols[symbols_size]->offset = symbols[symbols_size - 1]->offset + symbols[symbols_size - 1]->size;
		}
		symbols[symbols_size]->size = 1;
		symbols[symbols_size]->symbol = operand;
		symbols[symbols_size]->is_const = false;
		symbols[symbols_size]->value = -1;
		symbols_size++;
	}
}

/* Returns the opcode for the given operation based on the opcode file generated data.*/
int return_opcode(char* operation){
	for (int i = 0; i < opcodes_size; i++)
	{
		if (strcmp(opcodes[i]->operation, operation) == 0)
			return opcodes[i]->code;
	}
	return -1;
}

/* Returns the offset for the symbol. */
int get_symbol_offset(char* symbol)
{
	for (int i = 0; i < symbols_size; i++){
		if (strcmp(symbol, symbols[i]->symbol) == 0)
			return symbols[i]->offset;
	}
}

/* Returns the line number based on the label from the label table. */
int get_label_line(char* label){
	for (int i = 0; i < labels_size; i++)
		if (strcmp(label_table[i]->label, label) == 0)
			return label_table[i]->line;
	return -1;

}

/* Insert the obtained label into label table with the corresponding line number.*/
void insert_into_label_table(char* operand){
	if (operand[strlen(operand) - 1] == ':'){
		operand[strlen(operand) - 1] = '\0';
	}
	label_table[labels_size]->label = operand;
	label_table[labels_size]->line = code_size;
	labels_size++;
}

/* Function to insert READ operation into the intermediate code. */
void insert_read(char* operand)
{
	if (strlen(operand) != 2)
	{
		printf("Invalid variable for read");
		exit(0);
	}

	int opcode = return_opcode("READ");
	if (opcode == -1)
	{
		printf("Invalid operation!");
		exit(0);
	}
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = opcode;
	inter_code[code_size]->parameters = (int*)malloc(sizeof(int));
	inter_code[code_size]->parameters[0] = operand[0] - 'A';
	inter_code[code_size]->size = 1;
	code_size++;
}


/* Function to insert PRINT instruction into the intermediate code. */
void insert_print(char* operand){

	if (strlen(operand) != 2)
	{
		printf("Invalid variable for read");
		exit(0);
	}

	int opcode = return_opcode("PRINT");
	if (opcode == -1)
	{
		printf("Invalid operation!");
		exit(0);
	}
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = opcode;
	inter_code[code_size]->parameters = (int*)malloc(sizeof(int));
	inter_code[code_size]->parameters[0] = operand[0] - 'A';
	inter_code[code_size]->size = 1;
	code_size++;
}

/* Checks to which MOV operation the instruction belongs to. */
int check_mov(char* token1, char* token2)
{
	/* 
		finds the suitable MOV opcode]
		returns
		1 - MOV (Register to MEM)
		2 - MOV (MEM to Register)
	*/
	if (strlen(token1) == 2)
	{
		if (token1[0] >= 'A'&&token1[0] <= 'H'&&token1[1] == 'X')
			return 2;
	}
	return 1;
}

int return_index(char* operand,char** symbol_temp)
{
	
		char* symbol = (char*)calloc(10, sizeof(char));
		int i;
		int count = 0;
		for (i = 0; operand[i] != '['; i++){
			symbol[count++] = operand[i];
		}
		symbol[count] = '\0';

		char* size_str = (char*)calloc(5, sizeof(char));
		count = 0;
		for (i = i + 1; operand[i] != ']'; i++){
			size_str[count++] = operand[i];
		}
		size_str[count] = '\0';

		int size = atoi(size_str);
		
		*symbol_temp = symbol;
		return size;
}


/* Function to insert "MOV"(both) instruction into the intermediate code. */
void insert_mov(char* operand){
	char** tokens = get_tokens(operand, ',');
	int opcode = check_mov(tokens[0], tokens[1]);
	int symbol_offset, reg_offset;
	if (opcode == 1)
	{
		if (is_array(tokens[0]))
		{
			char* symbol = (char*)calloc(10, sizeof(char));
			int temp = return_index(tokens[0], &symbol);
			symbol_offset = get_symbol_offset(symbol)+temp;
		}
		else
		{
			symbol_offset = get_symbol_offset(tokens[0]);
		}
		
		reg_offset = tokens[1][0] - 'A';
		inter_code[code_size]->line_num = code_size + 1;
		inter_code[code_size]->opcode = opcode;
		inter_code[code_size]->parameters = (int*)malloc(2 * sizeof(int));
		inter_code[code_size]->parameters[0] = symbol_offset;
		inter_code[code_size]->parameters[1] = reg_offset;
		inter_code[code_size]->size = 2;
		
	}
	else
	{
		if (is_array(tokens[1]))
		{
			char* symbol = (char*)calloc(10, sizeof(char));
			int temp = return_index(tokens[1], &symbol);
			symbol_offset = get_symbol_offset(symbol) + temp;
		}
		else
		{
			symbol_offset = get_symbol_offset(tokens[1]);
		}
		//symbol_offset = get_symbol_offset(tokens[1]);
		reg_offset = tokens[0][0] - 'A';
		inter_code[code_size]->line_num = code_size + 1;
		inter_code[code_size]->opcode = opcode;
		inter_code[code_size]->parameters = (int*)malloc(2 * sizeof(int));
		inter_code[code_size]->parameters[0] = reg_offset;
		inter_code[code_size]->parameters[1] = symbol_offset;

		inter_code[code_size]->size = 2;
	}
	
	code_size++;
}

/* Function to insert "ADD" instruction into the intermediate code. */
void insert_add(char* operand){

	int opcode = return_opcode("ADD");
	if (opcode == -1)
	{
		printf("Invalid operation!");
		exit(0);
	}
	char** tokens = get_tokens(operand, ',');
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = opcode;
	inter_code[code_size]->size = 3;
	inter_code[code_size]->parameters = (int*)malloc(3*sizeof(int));
	for (int i = 0; i < 3;i++)
		inter_code[code_size]->parameters[i] = tokens[i][0] - 'A';
	code_size++;
}

/* Function to insert "SUB" instruction into the intermediate code. */
void insert_sub(char* operand){

	int opcode = return_opcode("SUB");
	if (opcode == -1)
	{
		printf("Invalid operation!");
		exit(0);
	}
	char** tokens = get_tokens(operand, ',');
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = opcode;
	inter_code[code_size]->size = 3;
	inter_code[code_size]->parameters = (int*)malloc(3 * sizeof(int));
	for (int i = 0; i < 3; i++)
		inter_code[code_size]->parameters[i] = tokens[i][0] - 'A';
	code_size++;
}

/* Function to insert "MUL" instruction into the intermediate code. */
void insert_mul(char* operand){
	int opcode = return_opcode("MUL");
	if (opcode == -1)
	{
		printf("Invalid operation!");
		exit(0);
	}
	char** tokens = get_tokens(operand, ',');
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = opcode;
	inter_code[code_size]->size = 3;
	inter_code[code_size]->parameters = (int*)malloc(3 * sizeof(int));
	for (int i = 0; i < 3; i++)
		inter_code[code_size]->parameters[i] = tokens[i][0] - 'A';
	code_size++;

}


/* Helper function to push into the stack used for including if, else instructions */
void push_stack(int val){
	*sp = *sp + 1;
	stack[*sp] = val;
}

/* Helper function to pop the stack used for including if, else instructions */
int pop_stack(){
	if (*sp >=0)
	{
		int val = stack[*sp];
		*sp = *sp - 1;
		return val;
	}
}


/* Function to insert if instruction into intermediate code. */
void insert_if(char* operand){
	char** tokens = get_tokens(operand, ' ');
	int opcode = return_opcode(tokens[1]);
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = return_opcode("IF");
	inter_code[code_size]->size = 4;
	int* parameters = (int*)malloc(4 * sizeof(int));
	parameters[0] = tokens[0][0] - 'A';
	parameters[1] = tokens[2][0] - 'A';
	parameters[2] = opcode;
	inter_code[code_size]->parameters = parameters;
	push_stack(code_size + 1);
	code_size++;
}

/* Function to insert else instruction into intermediate code and the path for the failing condition for if statement. */
void insert_else(){
	int if_line = pop_stack();
	inter_code[if_line - 1]->parameters[3] = code_size + 2;
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = return_opcode("JMP");
	inter_code[code_size]->size = 1;
	int* parameters = (int*)malloc(sizeof(int));
	inter_code[code_size]->parameters = parameters;
	push_stack( code_size + 1);
	code_size++;
}

/* Function to insert endif instruction into intermediate code and the path for the failing condtion for else statement. */
void insert_endif(){
	int else_line = pop_stack();
	inter_code[else_line - 1]->parameters[0] = code_size + 2;
	inter_code[code_size]->line_num = code_size + 1;
	inter_code[code_size]->opcode = 0;
	inter_code[code_size]->size = 0;
	code_size++;
}

/* Function to insert jump instruction into intermediate code. */
void insert_jmp(char* op){
	for (int i = 0; i < labels_size; i++){
		if (strcmp(op, label_table[i]->label) == 0)
		{
			inter_code[code_size]->line_num = code_size + 1;
			inter_code[code_size]->opcode = return_opcode("JMP");
			inter_code[code_size]->size = 1;
			int* parameters = (int*)malloc(sizeof(int));
			parameters[0] = label_table[i]->line+1;
			inter_code[code_size]->parameters = parameters;
			code_size++;
			break;
		}
	}
}

/* Checks if the given input is a label (Format - X: ) (where 'X' is considered a label). */
bool is_label(char* label){
	int i;
	for (i = 0; label[i] != 0; i++)
		if (label[i] == ' ')
			return false;

	if (label[strlen(label) - 1] == ':')
		return true;
}


/* Checks the instructions and calls the corresponding functions for generating intermediate code. */
char* get_operands(char* op, char* operand_string){

	if (operand_string[strlen(operand_string) - 1] == '\n'){
		operand_string[strlen(operand_string) - 1] = '\0';
	}
	if (strcmp(op, "DATA") == 0){
		if (operand_string[strlen(operand_string) - 1] == '\n')
			operand_string[strlen(operand_string) - 1] = '\0';
		insert_symbol_data(operand_string);
	}
	else if (strcmp(op, "CONST") == 0){
		insert_symbol_data(operand_string);
	}
	else if (strcmp(op, "MOV") == 0){
		insert_mov(operand_string);

	}
	else if (strcmp(op, "ADD") == 0){
		insert_add(operand_string);
	}
	else if (strcmp(op, "SUB") == 0){
		insert_sub(operand_string);
	}
	else if (strcmp(op, "MUL") == 0){
		insert_mul(operand_string);
	}
	else if (strcmp(op, "IF") == 0){
		insert_if(operand_string);
	}
	else if (strcmp(op, "ELSE") == 0){
		insert_else();
	}
	else if (strcmp(op, "JMP") == 0){
		insert_jmp(operand_string);
	}
	else if (strcmp(op, "ENDIF") == 0){
		insert_endif();
	}
	else if (strcmp(op, "READ") == 0){
		insert_read(operand_string);
	}
	else if (strcmp(op, "PRINT") == 0){
		insert_print(operand_string);
	}
	else if (is_label(op)){
		insert_into_label_table(op);
	}
	return NULL;
}


/* A function which reads the instructions sent by the read file and executes each instruction. */
char** get_all_tokens(char* string)
{
	int len = strlen(string);
	char* op = (char*)malloc(6 * sizeof(char));

	int i;
	for ( i = 0; string[i] == ' '||string[i]=='\t'; i++);
	int count = 0;
	for (i; string[i] != ' '&&string[i]!=0&&string[i]!='\n'&&string[i]!='\t'; i++)
	{
		op[count++] = string[i];
	}
	op[count] = '\0';
	if (string[i] != '\0')
		get_operands(op, string + i + 1);
	else
		get_operands(op, NULL);

	return NULL;
}

/* A wrapper function which reads the input file and generates the intermediate code. */
void read_file()
{
	char* file_name = (char*)malloc(20 * sizeof(char));
	scanf("%s", file_name);

	FILE* fp = fopen(file_name, "r");
	while (!feof(fp)){

		char* instruction = (char*)calloc(50, sizeof(char));
		fgets(instruction, 50, fp);
		get_all_tokens(instruction);
	}
}

/* A method to initialise all the globals declared. */
void initialise_globals()
{
	opcodes_size = get_opcodes();
	symbols = (symbol_row**)calloc(50, sizeof(symbol_row*));
	for (int i = 0; i < 50; i++)
	{
		symbols[i] = (symbol_row*)calloc(1, sizeof(symbol_row));
		//symbols[i]->symbol = (char*)calloc(10, sizeof(char));
	}
	symbols_size = 0;
	inter_code = (code_line**)calloc(50, sizeof(code_line*));
	for (int i = 0; i < 50; i++){
		inter_code[i] = (code_line*)calloc(1, sizeof(code_line));
	}
	code_size = 0;

	labels_size = 0;
	label_table = (labels**)calloc(50, sizeof(labels*));
	for (int i = 0; i < 50; i++)
		label_table[i] = (labels*)calloc(1, sizeof(labels));

	stack = (int*)malloc(20 * sizeof(int));
	sp = (int*)malloc(sizeof(int));
	*sp = -1;

}

/* Dumps the generated intermediate code into a txt file. */
void dump_intermediate_code()
{
	FILE* fp = fopen(INTER_CODE_FILE, "w");
	for (int i = 0; i < code_size; i++)
	{
		fprintf(fp, "%d,%d,%d", inter_code[i]->line_num, inter_code[i]->opcode, inter_code[i]->size);
		for (int j = 0; j < inter_code[i]->size; j++){
			fprintf(fp, ",%d", inter_code[i]->parameters[j]);
		}
		if (i!=code_size-1)
		fprintf(fp, "\n");
	}
	fclose(fp);
}

/* Dumps the symbols from the symobls table into a txt file. */
void dump_symbols(){
	
	FILE* fp = fopen(SYMBOLS_FILE, "w");
	for (int i = 0; i < symbols_size; i++)
	{
		fprintf(fp, "%s,%d,%d,%d,%d", symbols[i]->symbol, symbols[i]->offset,symbols[i]->size, symbols[i]->is_const, symbols[i]->value);
		if (i != symbols_size - 1)
			fprintf(fp, "\n");
	}
	fclose(fp);
}

void dump_labels()
{
	FILE* fp = fopen(LABELS_FILE, "w");
	for (int i = 0; i < labels_size; i++)
	{
		fprintf(fp, "%s,%d", label_table[i]->label, label_table[i]->line);
		if (i != labels_size)
			fprintf(fp, "\n");
	}
	fclose(fp);

}


/* C CODE GENERATION LOGIC*/
//
//char* arr;
//
//FILE* fp;
////
////char* num_to_str(int num){
////	char* s = (char*)calloc(10, sizeof(char));
////	sprintf(s, "%d", num);
////	return s;
////}
////void load_variables(){
////	for (int i = 0; i < symbols_size; i++)
////	{
////		vars[i]->name = symbols[i]->symbol;
////		vars[i]->size = symbols[i]->size;
////		vars[i]->
////	}
////}
//
//void get_index_size(int offset, int index, int size){
//
//}
//void mova(int* parameters){
//	/*reg to mem*/
//	/*int size, index;*/
//
//	arr[parameters[0]] = registers[parameters[1]];
//}
//void movb(int* parameters){
//	/* mem to reg */
//	registers[parameters[0]] = arr[parameters[1]];
//}
//void print(int* parameters){
//
//	printf("Value of %cX = %d\n", 'A' + parameters[0], registers[parameters[0]]);
//}
//
//void read(int* parameters){
//	printf("Enter value of %cX: ", 'A' + parameters[0]);
//	scanf("%d", &registers[parameters[0]]);
//}
//
//void add(int* parameters){
//	registers[parameters[0]] = registers[parameters[1]] + registers[parameters[2]];
//}
//
//void sub(int* parameters){
//	registers[parameters[0]] = registers[parameters[1]] - registers[parameters[2]];
//}
//
//void mul(int* parameters){
//	registers[parameters[0]] = registers[parameters[1]] * registers[parameters[2]];
//}
//
//bool execute_condition(int opcode, int op1, int op2){
//	int a = registers[op1];
//	int b = registers[op2];
//	if (opcode == 9)
//		return a == b;
//	else if (opcode == 10)
//		return a > b;
//	else if (opcode == 11)
//		return a < b;
//	else if (opcode == 12)
//		return a >= b;
//	else if (opcode == 13)
//		return a <= b;
//	return false;
//}
//void if_func(int* parameters, int* i){
//	fprintf(fp,"\nif(");
//	if (execute_condition(parameters[2], parameters[0], parameters[1]))
//		return;
//	*i = parameters[3] - 2;
//	fprintf(fp, "){\n");
//}
//void jmp_func(int* parameters, int* i){
//	*i = parameters[0] - 2;
//}
//
//void call_function(int opcode, int* parameters){
//	switch (opcode){
//	case 1:
//		mova(parameters);
//		break;
//	case 2:
//		movb(parameters);
//		break;
//	case 3:
//		add(parameters);
//		break;
//	case 4:
//		sub(parameters);
//		break;
//	case 5:
//		mul(parameters);
//		break;
//
//	case 6:
//		read(parameters);
//		break;
//	case 7:
//		print(parameters);
//		break;
//	}
//
//}
//
//void perform_operations(){
//
//	fprintf(fp, "#include<stdio.h>\nint main(){");
//	for (int i = 0; i < code_size; i++){
//		if (inter_code[i]->opcode == 8)
//			if_func(inter_code[i]->parameters, &i);
//		else if (inter_code[i]->opcode == 14)
//			i = inter_code[i]->parameters[0] - 2;
//		else{
//			call_function(inter_code[i]->opcode, inter_code[i]->parameters);
//		}
//	}
//	fprintf(fp, "\n}");
//}



//int main()
//{
//	initialise_globals();
//	
//	read_file();
//	
//	dump_intermediate_code();
//
//	dump_symbols();
//
//	system("pause");
//	return 0;
//}