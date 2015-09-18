#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

void printBuffer(char * buffer){
	int i;
	printf("\n");
	for(i = 0; i < 512; i++){
		printf("%.2x ", buffer[i]);
		if(i%16 == 0 && i!=0)
			printf("\n");
	}
}

char * parseFileName(char * buffer){
	int i;
	char * fileName;
	for(i = 0; buffer[i] != '\0'; i++)
		*(fileName + i) = buffer[i];
	*(fileName + i) = '\0';

	printf("\n");
	return fileName;
}

int main(){
	int sock_udp, tam;
	char buffer[512];
	
	struct sockaddr_in servidor, cliente;
	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_udp == -1){
		perror("Error al abrir");
		exit(0);
	}

	else{
		printf("\nSocket abierto\n");
		memset(&servidor, 0x00, sizeof(servidor));
		servidor.sin_family = AF_INET;
		servidor.sin_port = htons(69);
		servidor.sin_addr.s_addr = INADDR_ANY;

		if( bind(sock_udp, (struct sockaddr *)&servidor, sizeof(servidor)) == -1)
		{
			perror("Error en bind");
			exit('0');
		}
		else
		{
		while(1){
		int n = sizeof(cliente);
			tam = recvfrom(sock_udp, buffer, 512,0,(struct sockaddr*)&cliente,&n);
			if(tam == -1)
				printf("Error al recibir");
	
			else{

				if(buffer[1] == 1){     //Opcode: Read request
					printf("Leer\n");

					char * fileName = parseFileName(buffer + 2);
printf("size: %d\n", sizeof(fileName));
					FILE * fp = fopen(fileName, "w");
					fputc('c',fp);
					fclose(fp);
				}
				
				if(buffer[1] == 2){		//Opcode: Write request
					printf("Escribir\n");

				}
			}

	}	
	}
	
}
}
