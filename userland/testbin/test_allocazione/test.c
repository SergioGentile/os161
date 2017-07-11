#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	/*int *ptr;
	ptr = (int*)malloc(2*sizeof(int));
	ptr[0] = 1;
	ptr[1] = 2;
	printf("Coor: %d,%d\n", ptr[0], ptr[1]);
	free(ptr);*/
	int *ptr = malloc(2*sizeof(int));
	ptr[0]=1;
	ptr[1]=7;
	//printf("ciao %d\n", ptr[0]+ptr[1]);
	//printf("ciao\n");
	free(ptr);
	printf("ciao\n");
	return 0;

}
