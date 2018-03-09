#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<unistd.h>  
#include<stdlib.h>  
#include<stdio.h>  
#include<string.h>  
#include<netdb.h>  
#include<errno.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <dlfcn.h>
#include <cstdlib>
#include <unistd.h>
#include "ThostFtdcTraderApi.h"
#include "CTraderSpi.h"
#include "Ini.h"

using namespace std;

#define PORT 12346  
#define MAX_MSG_SIZE 1024 
  
int main(int argc,char* argv[])  
{  
	printf("-------------\n");

	//get parameters
	if(argc < 2)
	{
		printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
		printf("Use \"./Md -h\" to see the options\n");
		exit(0);
	}

	char ch;

	char* filePath;

	while((ch = getopt(argc, argv, "f:h"))!= -1){
		switch(ch){

			case 'f':
				filePath = optarg;
				printf("%s\n", filePath);
				break;

			case 'h':
				printf("Options:\n");
				printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
				// printf("-b: Broker ID\n");
				// printf("-i: Investor ID\n");
				// printf("-p: Investor Password\n");
				printf("-f: login config filepath\n");
				// printf("-n: Sum of instrument\n");
				printf("-h: Help to list the options\n");
				exit(0);
				break;

			default:
				printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
				printf("Use \"./Md -h\" to see the options\n");
				exit(0);
				break;
		}
	}

	int sockfd, addrlen, n;  
	struct sockaddr_in addr;  
	char msg[MAX_MSG_SIZE];

	// prepare UDP
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);  
	if (sockfd < 0)  
	{  
	  fprintf(stderr, "socket failed\n");  
	  exit(EXIT_FAILURE);  
	}  
	addrlen = sizeof(struct sockaddr_in);  
	bzero(&addr, addrlen);  
	addr.sin_family = AF_INET;  
	addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	addr.sin_port = htons(PORT);  
	if (bind(sockfd, (struct sockaddr*)(&addr), addrlen) < 0)  
	{  
	  fprintf(stderr, "bind fail\n");  
	  exit(EXIT_FAILURE);  
	}  
	puts("bind success");  

   	//TraderSpi
    CTraderSpi *pTradeUserSpi = new CTraderSpi(filePath);

    pTradeUserSpi->InitApi();

    // waiting for confirm
    while(!pTradeUserSpi->isConfirm){}

    // waiting for UDPC
    while(pTradeUserSpi->isConfirm){

    	bzero(msg, MAX_MSG_SIZE); 
		
		socklen_t st = (socklen_t) addrlen;// add 

		n = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)(&addr), &st);// addrlen to st

		msg[strlen(msg)-1] = '\0';// add
		fprintf(stdout, "Recevie message from client is *%s*\n", msg);
		pTradeUserSpi->ReqOrderInsertBy(pTradeUserSpi->ReadOrderFieldIni(msg));

		// waitin for return message
		while(!pTradeUserSpi->isRtnMsg){}
		printf("Server endpoint input message %s\n", pTradeUserSpi->rtnMsg);  
		sendto(sockfd, pTradeUserSpi->rtnMsg, n, 0,(struct sockaddr *)(&addr), st); // addrlen to st 
		pTradeUserSpi->isRtnMsg = 0;
		
    }

    pTradeUserSpi->JoinApi();

	//pTradeUserApi->Release();
	close(sockfd);
	exit(EXIT_SUCCESS);  
}  



// int main(int argc,char* argv[])
// {
// 	printf("-------------\n");

// 	//get parameters
// 	if(argc < 2)
// 	{
// 		printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
// 		printf("Use \"./Md -h\" to see the options\n");
// 		exit(0);
// 	}

// 	char ch;

// 	char* filePath;

// 	while((ch = getopt(argc, argv, "f:h"))!= -1){
// 		switch(ch){

// 			case 'f':
// 				filePath = optarg;
// 				printf("%s\n", filePath);
// 				break;

// 			case 'h':
// 				printf("Options:\n");
// 				printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
// 				// printf("-b: Broker ID\n");
// 				// printf("-i: Investor ID\n");
// 				// printf("-p: Investor Password\n");
// 				printf("-f: login config filepath\n");
// 				// printf("-n: Sum of instrument\n");
// 				printf("-h: Help to list the options\n");
// 				exit(0);
// 				break;

// 			default:
// 				printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
// 				printf("Use \"./Md -h\" to see the options\n");
// 				exit(0);
// 				break;
// 		}
// 	}

//     //TraderSpi
//     CTraderSpi *pTradeUserSpi = new CTraderSpi(filePath);

//     pTradeUserSpi->InitApi();

//     while(!pTradeUserSpi->isConfirm){}
//     while(pTradeUserSpi->isConfirm){
//     	printf("Please Enter orderConfig.ini path:\n");
//     	scanf("%s",filePath);
//     	pTradeUserSpi->ReqOrderInsertBy(pTradeUserSpi->ReadOrderFieldIni(filePath));
//     }

//     pTradeUserSpi->JoinApi();

// 	//pTradeUserApi->Release();
// 	return 0;
// }
