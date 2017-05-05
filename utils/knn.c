#include <stdio.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_sort_vector.h>

#define MAX_MAC_SIZE 18

typedef struct knn_prep{
	gsl_matrix *m;	//fingerprint
	int k;
	int class_col;
	int fp_size;
	char mac[3][MAX_MAC_SIZE];
}knn_t;

typedef struct calc_prep
{
	//agent_info_t *infos;
	int info_indexes[3];
	int rssi[3];
	int room;
}calc_prep_t;


void init_knn_prep(knn_t *prep, int k, int x, int y, int class_col)
{
	if(prep == NULL) return;
	if(k < 0) prep->k = 4;
	else prep->k = k;

	prep->m = gsl_matrix_alloc(x, y);
	prep->class_col = class_col;
}

void free_knn_prep(knn_t *prep)
{
	if(prep == NULL) return;

	if(prep->m != NULL)
	{
		gsl_matrix_free(prep->m);
	}
	free(prep);
}

void print_matrix(gsl_matrix *m)
{
	int i =0, j=0; 
	if(m==NULL) return;
	printf("-----\n");
	for(i=0;i<m->size1;i++)
	{
		for(j=0;j<m->size2; j++)
		{
			printf("%f\t",gsl_matrix_get(m, i, j));
		}
		printf("\n");
	}
}

void print_vector(gsl_vector *v)
{
	int i =0;
	if(v == NULL) return;
	printf("\n[");
	for(; i<v->size; i++)
	{
		printf("%f\t", gsl_vector_get(v, i));
	}
	printf("]\n");
}
/*
void file_reader(char *filename)
{
	FILE* fp;
	char* line = NULL;

	if(filename == NULL){
		printf("[ERROR] Cannot load data for KNN\n");
		exit(EXIT_FAILURE);
	}

	fp = fopen(filename, "r");
}*/

void knn(knn_t *k, calc_prep_t *prep)
{
	int i = 0, j = 0;
	if(k == NULL || prep == NULL) return;
	
	gsl_matrix *m = gsl_matrix_alloc(k->m->size1, k->m->size2);
	gsl_matrix *m1 = gsl_matrix_alloc(k->m->size1, k->m->size2);
	gsl_matrix_set_zero(m);
	gsl_matrix_set_all(m1, 1.0);

	gsl_vector *temp_vector = gsl_vector_alloc(k->m->size1);
	size_t p[k->k];

	//set rssi to the matrix
	for(; i<3; i++)
	{
		gsl_vector_set_all(temp_vector, -1.0*(double)prep->rssi[i]);
		gsl_matrix_set_col(m, i, temp_vector);
	}

	print_matrix(m);

	gsl_matrix_add(m, k->m);
	print_matrix(m);

	for(i=0;i<3;i++){
		gsl_matrix_get_col(temp_vector, m, i);
		gsl_matrix_set_col(m1, i, temp_vector);
	}

	gsl_matrix_mul_elements(m, m1); //square
	print_matrix(m);

	for(i = 0; i<m->size1; i++)
	{
		gsl_vector_set(temp_vector, i, gsl_matrix_get(m, i, 0)+gsl_matrix_get(m, i, 1)+gsl_matrix_get(m, i, 2));
	}
	print_vector(temp_vector);

	//sort the vector
	gsl_sort_vector_smallest_index(p, k->k, temp_vector);
	
	printf("===============result==============\n");
	for(i = 0; i<k->k;i++)
	{
		printf("%d\t",(int)p[i]);
	}
	printf("\n[DONE]\n");
	gsl_vector_free (temp_vector);
	gsl_matrix_free(m);
	gsl_matrix_free(m1);
}

int main (void)
{
	knn_t *k = malloc(sizeof(knn_t));
	int i=0, j=0;
	init_knn_prep(k, 3, 5, 4, 3);

	for(;i<k->m->size1;i++)
	{		
		for(j = 0;j<k->m->size2; j++)
		{
			gsl_matrix_set(k->m, i, j, (i+1)*(j+1));
		}
	}

	print_matrix(k->m);

	calc_prep_t prep;
	prep.rssi[0] = 4;
	prep.rssi[1] = 7;
	prep.rssi[2] = 11;

	knn(k, &prep);

	free_knn_prep(k);
	return 0;
}