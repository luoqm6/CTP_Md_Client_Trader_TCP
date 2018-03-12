#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <dlfcn.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiStruct.h"
#include "CMdSpi.h"
#include "Ini.h"

#define MAX_BUF_SIZE 1024 

using namespace std;

// argv para
char ch;
char* filePath;
char* cPort;

// socket para
int server_sockfd,client_sockfd, server_len, client_len;  
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
char buffer[MAX_BUF_SIZE];
FILE *fp;

//functions used
void readArgv(int argc,char* argv[]);
void readLoginFile(char* filePath);
void readSubFile(char* filePath);

int main(int argc,char* argv[])
{

	printf("-------------\n");

	readArgv(argc,argv);
    
	// CMdSpi
	CMdSpi* pMdUserSpi = new CMdSpi(filePath);
	pMdUserSpi->InitApi();

	// waiting for connect and login
	while(!pMdUserSpi->isLogin||!pMdUserSpi->isConnect)
	{
		//cout<<"waiting\n";
	}
	if(pMdUserSpi->isLogin&&pMdUserSpi->isConnect)
	{
		//recv order config.ini file
		filePath = "../doc/subConfig.ini";
		pMdUserSpi->readOrderFile(filePath);
		pMdUserSpi->sendOrder();
	}	
	
	pMdUserSpi->pMdUserApi->Join();

	//pMdUserApi->Release();
	return 0;
}

void readArgv(int argc,char* argv[])
{
	//get parameters
	if(argc < 2)
	{
		printf("Usage: ./Md -opt1 para1 -opt2 para2 ...\n");
		printf("Use \"./Md -h\" to see the options\n");
		exit(0);
	}

	while((ch = getopt(argc, argv, "f:h"))!= -1)
	{
		switch(ch)
		{

			case 'f':
				filePath=optarg;
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
}

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
	    char recPath[100];

    	bzero(buffer, MAX_BUF_SIZE); 

		socklen_t st = (socklen_t) server_len;// add 
		int writelength=0;
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
                break;
            }
            cout<<"writing"<<endl;
            writelength = fwrite(buffer,sizeof(char),st,fp);

            if(writelength <st)
            {
                printf("write error!\n");
                break;
            }
            bzero(buffer,MAX_BUF_SIZE); 
            //memset(buffer,0,sizeof(buffer));

            printf("recv finished!\n");
            fclose(fp);
        }
}