//#define THREAD_COUNT 8
#define MAT_SIZE 1000

typedef struct argument{
	int start;
	int end;
	int** mat;
}argument;

int** mat1;
int** mat2;
pthread_t threads[100];

int calc_element(int** mat1, int** mat2, int r, int c, int n){
	int total = 0;
	for (int i = 0; i < n; i++)
	{
		total += mat1[r][i] * mat2[i][c];
	}
	return total;
}

int** matrix_multiplication(int** mat1, int** mat2, int n){
	int** mat3 = (int**)malloc(n*sizeof(int*));
	for (int i = 0; i < n; i++)
		mat3[i] = (int*)malloc(n*sizeof(int));

	for (int ri = 0; ri < n; ri++) //ri - row index
	{
		for (int ci = 0; ci < n; ci++)//ci - column index
		{
			mat3[ri][ci] = calc_element(mat1, mat2, ri, ci, n);
		}
	}
	return mat3;
}

void* matrix_multiplication_thread(void* args){

	argument* arg = (argument*)args;


	for (int ri = arg->start; ri < arg->end; ri++) //ri - row index
	{
		for (int ci = 0; ci < MAT_SIZE; ci++)//ci - column index
		{
			arg->mat[ri][ci] = calc_element(mat1, mat2, ri, ci, MAT_SIZE);
		}
	}
	return NULL;
}


void print_matrix(int** mat, int n)
{
	printf("\n\n\n");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

void initialise_matrices(){
	mat1 = (int**)malloc(MAT_SIZE*sizeof(int*));
	for (int i = 0; i < MAT_SIZE; i++)
		mat1[i] = (int*)malloc(MAT_SIZE*sizeof(int));


	mat2 = (int**)malloc(MAT_SIZE*sizeof(int*));
	for (int i = 0; i < MAT_SIZE; i++)
		mat2[i] = (int*)malloc(MAT_SIZE*sizeof(int));

	for (int i = 0; i < MAT_SIZE; i++)
	{
		for (int j = 0; j < MAT_SIZE; j++)
		{
			mat1[i][j] = i + j;
			mat2[i][j] = i*j;
		}
	}
}

int** matrix_mul_threads(int count){ //count - number of threads

	int** mat3 = (int**)malloc(MAT_SIZE*sizeof(int*));
	for (int i = 0; i < MAT_SIZE; i++)
		mat3[i] = (int*)malloc(MAT_SIZE*sizeof(int));



	argument arguments[1000];

	int block_size = MAT_SIZE / count;
	int start = 0;
	int end = block_size;
	for (int i = 0; i < count; i++)
	{
		arguments[i].mat = mat3;
		arguments[i].start = start;
		arguments[i].end = end;
		start += block_size;
		end += block_size;

	}
	clock_t a = clock();
	for (int i = 0; i < count; i++)
	{
		pthread_create(&threads[i], NULL, matrix_multiplication_thread, (void*)&arguments[i]);
	}

	for (int i = 0; i < count; i++)
	{
		pthread_join(threads[i], NULL);
	}
	clock_t b = clock() - a;

	double d = (double)b / CLOCKS_PER_SEC;
	printf("THREAD COUNT : %d\tTIME TAKEN : %f\n", count, d);
	return mat3;

}

void matrix_mul(){
	initialise_matrices();

	for (int i = 2; i < 12; i++)
		int** result_mat = matrix_mul_threads(i);
}