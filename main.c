#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

char* parseFileName(char* buffer) {
  return strdup(buffer);
}

unsigned short getOpcode(unsigned char* msg){
	unsigned short opcode;
	opcode = msg[1];
	return opcode;
}

unsigned short getBlockNum(unsigned char* msg){
	unsigned short blockNum;

	blockNum = (msg[2] << 8) + ((unsigned short)msg[3]);

	return blockNum;
}

void setBlockNum(unsigned char* msg, unsigned short blockNum){
	unsigned int b0,b1;

	b0 = (blockNum);
	b1 = (blockNum >> 8);

	memset(msg + 3, b0, 1);
	memset(msg + 2, b1, 1);

}

void sendACK(int socket, struct sockaddr_in* addr, int ref){
	unsigned char* msg = malloc(4);
	memset(msg, 0, 1);
	memset(msg + 1, 4, 1);
	setBlockNum(msg, ref);

	sendto(socket, msg, 4, 0, (struct sockaddr*)addr, sizeof(*addr));

	free(msg);
}

void sendDataPacket(int socket, struct sockaddr_in* addr, unsigned short ref, unsigned char* data, int dataSize, unsigned char* msg){

	memset(msg, 0, 1);
	memset(msg + 1, 3, 1);
	setBlockNum(msg, ref);
	memcpy(msg + 4, data, dataSize);

	sendto(socket, msg, dataSize, 0, (struct sockaddr*) addr, sizeof(*addr));
	
}

int hasArrived(int socket, struct sockaddr_in* addr, unsigned short ref){
	unsigned char msg [512];
	struct sockaddr_in* cliente;

	int n;
	n = sizeof(addr);

	if(recvfrom(socket, msg, 512, 0, (struct sockaddr*) cliente, &n) <0){
		perror("Error al recibir");
		return -1;
	}

	if(cliente == addr){
		if(getBlockNum(msg) == ref){
			
			return 1;
		}
	}

	return 0;
}

void sendZero(int socket, struct sockaddr_in* addr, unsigned short nblock, unsigned char* msg){
	unsigned char data[512];

	memset(data, 0, 512);
	sendDataPacket(socket, addr, nblock, data, 512, msg);
}

void sendData(int socket, struct sockaddr_in* addr, FILE* fp, int blockSize){
	int n = 1, ack, i, sentBytes=0, sendNext = 1;
	unsigned short nblock=1;

	unsigned char* data = malloc(512);
	unsigned char* msg = malloc(516);

	while(n != EOF){
		n = fread(data, 1, 512, fp);
		if(n == 0) break;

		if(sendNext){

			sendDataPacket(socket, addr, nblock, data, (4 + n), msg);

			sentBytes+=n;

		}
		
		sendNext = hasArrived (socket, addr, nblock++);
	}
		if(sentBytes % 512 == 0) sendZero(socket, addr, nblock, msg);
		printf("\nsent: %d bytes to \n",sentBytes);

		free(data);
		free(msg);
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

					FILE* fp = fopen(fileName, "rb");

					if(fp != NULL){

						sendData(sock_udp, &cliente, fp, 512);

					}
					else
						//send opcode 5 (File not found)
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
