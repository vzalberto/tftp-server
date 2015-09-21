#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

void printBuffer(char* buffer){
	int i;
	printf("\n");
	for(i = 0; i < 512; i++){
		printf("%.2x ", buffer[i]);
		if(i%16 == 0 && i!=0)
			printf("\n");
	}
}

/*char * parseFileName(char * buffer){
	int i;
	char * fileName;
	for(i = 0; buffer[i] != '\0'; i++)
		*(fileName + i) = buffer[i];
	*(fileName + i) = '\0';

	printf("\n");
	return fileName;
}*/

char* parseFileName(char* buffer) {
  return strdup(buffer);
}

void setBlockNum(unsigned char* msg, unsigned short blockNum){
	unsigned int b0,b1;

	b0 = (blockNum);
	b1 = (blockNum >> 8);

	memset(msg + 3, b0, 1);
	memset(msg + 2, b1, 1);

}

void sendData(int socket, struct sockaddr_in* addr, FILE* fp, int blockSize){
	int n = 1, ack;
	unsigned short nblock=1;
	unsigned char* msg = malloc(516);
	unsigned char* data = malloc(512);

	memset(msg, 0, 1);
	memset(msg + 1, 3, 1);

	while(n != EOF){
		n=fread (data, 1, blockSize, fp);
		if (n == 0)
				break;

		setBlockNum(msg, nblock++);
		memcpy(msg + 4, data, 512);


	//if(ack == 0)
	sendto(socket, msg, 516, 0, (struct sockaddr*) addr, sizeof(*addr));

	}

	free(data);
	free(msg);
}

void sendACK(int socket, struct sockaddr_in* addr, int prevACK){
	//sendto();
}

void sendErr(int socket, struct sockaddr_in* addr){
	unsigned char* msg = malloc(2);
	memset(msg, 0, 1);
	memset(msg + 1, 5, 1);
	sendto(socket, msg, sizeof(msg), 0, (struct sockaddr*) addr, sizeof(*addr));
	free(msg);
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
			tam = recvfrom(sock_udp, buffer, 512,0,(struct sockaddr*)&cliente,(socklen_t*)&n);
			if(tam == -1)
				printf("Error al recibir");
	
			else{

				if(buffer[1] == 1){     //Opcode: Read request
					printf("GET ");

					char* fileName = parseFileName(buffer + 2);
					printf("%s\n", fileName);

					FILE* fp = fopen(fileName, "r");

					if(fp != NULL){

						unsigned char* dataPacket = malloc(516);

						memset(dataPacket, 0, 1);
						memset(dataPacket + 1, 3, 1);
						memcpy(dataPacket + 2, fileName, 8);

						sendData(sock_udp, &cliente, fp, 512);

						free(dataPacket);

					}
					else
						//send opcode 5
						sendErr(sock_udp, &cliente);

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
