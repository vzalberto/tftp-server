#include <stdio.h>

int main()
{
	int i, n;
	FILE* fp, *dp;

	fp = fopen("babe.jpeg", "r");
	dp = fopen("newbabe.jpeg", "w+");

	while(n != EOF){
		for(i = 0; i < 512; i++){
		n = fgetc(fp);
		printf("%c",n);
		fputc(n, dp);
	}
	}

	fclose(fp);
	fclose(dp);

	return 0;
}