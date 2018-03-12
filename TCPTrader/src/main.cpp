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

#define PORT 1235
#define MAX_BUF_SIZE 1024 

// argv para
char ch;
char* filePath;
char* cPort;

// socket para
int server_sockfd,client_sockfd, server_len, client_len;  
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;

// the path of the received file 
char recPath[100];

bool receiveFile();
bool socketBind();
bool socketAccept();
  
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

	while((ch = getopt(argc, argv, "f:p:h"))!= -1){
		switch(ch){

			case 'f':
				filePath = optarg;
				printf("%s\n", filePath);
				break;

			case 'p':
				cPort = optarg;
				printf("%s\n", cPort);
				break;

			case 'h':
				printf("Options:\n");
				printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
				printf("-p: port\n");
				printf("-f: login config filePath\n");
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


	socketBind();

   	//TraderSpi
    CTraderSpi *pTradeUserSpi = new CTraderSpi(filePath);

    pTradeUserSpi->InitApi();



    // waiting for confirm
    while(!pTradeUserSpi->isConfirm){}

    // waiting for UDPC
    while(pTradeUserSpi->isConfirm){

    	socketAccept();



		// bzero(buffer, MAX_BUF_SIZE); 

		// socklen_t st = (socklen_t) server_len;// add 
		// int writeLength=0;
		// memset(recPath,'\0',sizeof(recPath));

		// st = recv(client_sockfd,recPath,MAX_BUF_SIZE,0);
		// printf("filePath :*%s*\n",recPath);

		// if(st<0)
		// {
		//     printf("recv error!\n");
		// }
		// fp = fopen(recPath,"w");
		// if(fp!=NULL)
		// {
		//     st =recv(client_sockfd,buffer,MAX_BUF_SIZE,0);
		//     if(st<0)
		//     {
		//         printf("recv error!\n");
		//         break;
		//     }
		//     cout<<"writing"<<endl;
		//     writeLength = fwrite(buffer,sizeof(char),st,fp);

		//     if(writeLength <st)
		//     {
		//         printf("write error!\n");
		//         break;
		//     }
		//     bzero(buffer,MAX_BUF_SIZE); 
		//     //memset(buffer,0,sizeof(buffer));

		//     printf("recv finished!\n");
		//     fclose(fp);
		// }

		receiveFile();

		// sent order
		pTradeUserSpi->ReqOrderInsertBy(pTradeUserSpi->ReadOrderFieldIni(recPath));

		// waitin for return message
		while(!pTradeUserSpi->isRtnMsg){} 
		send(client_sockfd, pTradeUserSpi->rtnMsg, MAX_BUF_SIZE, 0); // addrlen to st 
		// sendto(sockfd, pTradeUserSpi->rtnMsg, MAX_BUF_SIZE, 0,(struct sockaddr *)(&server_addr), st); // addrlen to st 
		printf("Server endpoint input message %s\n", pTradeUserSpi->rtnMsg); 
		pTradeUserSpi->isRtnMsg = 0;
    }

    pTradeUserSpi->JoinApi();

	//pTradeUserApi->Release();
	close(server_sockfd);
	close(client_sockfd);
	exit(EXIT_SUCCESS);  
}

// int SetNonBlock(int iSock)
// {
//     int iFlags;

//     iFlags = fcntl(iSock, F_GETFL, 0);
//     iFlags |= O_NONBLOCK;
//     iFlags |= O_NDELAY;
//     int ret = fcntl(iSock, F_SETFL, iFlags);
//     return ret;
// }

bool socketBind()
{
	// server socket
	server_sockfd = socket(PF_INET, SOCK_STREAM, 0);  
	if (server_sockfd < 0)  
	{  
	  fprintf(stderr, "socket failed\n");  
	  exit(EXIT_FAILURE);  
	}

	// bind
	server_len = sizeof(struct sockaddr_in);  
	bzero(&server_addr, server_len);  
	server_addr.sin_family = PF_INET;  
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
	int iPort = atoi(cPort);
	server_addr.sin_port = htons(iPort);
	if (bind(server_sockfd, (struct sockaddr*)(&server_addr), server_len) < 0)  
	{  
	  fprintf(stderr, "bind fail\n");  
	  exit(EXIT_FAILURE);
	}  
	printf("bind success\n");
	return 1;
}

bool socketAccept()
{
	// listen 
	listen(server_sockfd,5);
	printf("server waiting for connect\n");

	// accept server
	client_len = sizeof(client_addr);
	client_sockfd = accept(server_sockfd,(struct sockaddr * )&server_addr,(socklen_t *)&server_len);

}

bool receiveFile()
{
	char buffer[MAX_BUF_SIZE];
	FILE *fp;

	bzero(buffer, MAX_BUF_SIZE); 

	socklen_t st;// add 
	int writeLength=0;
	memset(recPath,'\0',sizeof(recPath));

	st = recv(client_sockfd,recPath,MAX_BUF_SIZE,0);
	printf("filePath :*%s*\n",recPath);

	if(st<0)
	{
	    printf("recv error!\n");
	}
	fp = fopen(recPath,"w");
	if(fp!=NULL)
	{
	    st =recv(client_sockfd,buffer,MAX_BUF_SIZE,0);
	    if(st<0)
	    {
	        printf("recv error!\n");
	        return 0;
	    }
	    cout<<"writing"<<endl;
	    writeLength = fwrite(buffer,sizeof(char),st,fp);

	    if(writeLength <st)
	    {
	        printf("write error!\n");
	        return 0;
	    }
	    bzero(buffer,MAX_BUF_SIZE); 
	    //memset(buffer,0,sizeof(buffer));

	    printf("recv finished!\n");
	    fclose(fp);
	}
}