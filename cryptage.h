#include <stdlib.h>
#include <stdio.h>

int bin(char *p){
	int str_len = strlen(p);
int a=1;
	int i, j, k = 8 ,num;
	int binary[8 * str_len];
	for(i = 0; i < 8*str_len; i++){
		binary[i] = 0;
	}
	for(i = 0; i < str_len; i++){
		j = k - 1;
		num = p[i];//num = *(p+i);
		while(num != 0){
			binary[j--] = num % 2;
			num /= 2;
		}
		k += 9; 
	}
	
	for(i = 1; i < 8*str_len; i++){
		


	}
	

}
