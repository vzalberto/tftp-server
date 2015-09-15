#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

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
	
			else

				printf("\n%s\n",buffer);

	}	
	}
	
}
}

