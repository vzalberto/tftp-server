#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void copy(FILE *sf, FILE *df,int blockSize);

int main(int argc, char *argv[]){

	FILE *sf;
	FILE *df;
	int opt=0,l;
	int blockSize=512,op=0;

	if(argc < 3){
		printf(" cp SOURCE DEST [option]\n\n Where option could be\n\n");
		printf("\t -b\t: especifies the blockSize (512 bytes default)\n\n");
	}
	else{

		while ((opt=getopt(argc,argv,"b:")) != -1){

			switch(opt){
				case 'b':
					blockSize=atoi(optarg);
					break;
				case '?':
					if(optopt=='b'){
						printf(" You need to define a blockSize\n");
					}
					else{
						printf("Invalid options\n");
					}
			}
		}

		sf=fopen(argv[argc-2],"rb");
		df=fopen(argv[argc-1],"wb");
		copy(sf,df,blockSize);

		fclose(df);
		fclose(sf);
	}

	return 0;
}

void copy(FILE *sf, FILE *df,int blockSize){

	int n=1,nblock=1;
	char *buffer = (char*) malloc (blockSize);
	while (n != EOF){
		n=fread (buffer,1,blockSize,sf);
		if (n == 0)
				break;
		fwrite(buffer,1,n,df);
	}

}