#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

int min(int a, int b, int c) {
	int tmp = (a < b)? a : b;
	return (tmp < c)? tmp: c;



}

void stredit(int **table, char *s1, char *s2, int len1, int len2) {
	int i, j, d;
	/* do dynamic programming */
	for(i = 0; i < len2 + 1; i++)
		for(j = 0; j < len1 + 1; j++) {
			if(j == 0)
				printf("\n");
			printf("%d   ", table[i][j]);
		}
	printf("----------------------------------");

	for(i = 1; i < len2 + 1; i++)
		for(j = 1; j < len1 + 1;j++) {
		   if(s1[j - 1] == s2[i - 1])
				d = 0;
		   else
			   d = 1;
		   table[i][j] = min(table[i - 1][j - 1] + d, table[i - 1][j] + 1,
				   			 table[i][j - 1] + 1);
		}
}

void main(int argc, char **argv) {
	char *s1 = argv[1];			//input
	char *s2 = argv[2];			//template
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	int **table;
	int i, j;
	table = Malloc(int *,len2 + 1);
	for(i = 0; i < len2 + 1; i++)
		table[i] = Malloc(int, len1 + 1);
	for(i = 0; i < len1 + 1; i++)
		table[0][i] = i;
	for(i = 0; i < len2 + 1; i++)
		table[i][0] = i;
	
	stredit(table, s1, s2, len1, len2);
	
	for(i = 0; i < len2 + 1; i++)
		for(j = 0; j < len1 + 1; j++) {
			if(j == 0) 
				printf("\n");
			printf("%d   ",table[i][j]);
	}
	for(i = 0; i < len2 + 1; i++) 
		free(table[i]);
	free(table);




}
