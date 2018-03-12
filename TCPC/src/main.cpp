#include<stdio.h>  
#include<unistd.h>  
#include<stdlib.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<arpa/inet.h>
#include<netinet/in.h>  
#include<string.h>  
#include<iostream>
using namespace std;

#define MAX_BUF_SIZE 1024  
#define PORT 1235

//functions used
bool sendFile();
bool socketConnect();

// sokect para
int sockfd = -1, addrlen, result;  
char buffer[MAX_BUF_SIZE];  
struct sockaddr_in addr;

//get parameters
char ch;
char* filePath;
char* cPort;
char* cnetAddr;

FILE *fp;

int fileTrans=0;

int main(int argc,char* argv[])   
{  


	while((ch = getopt(argc, argv, "f:p:i:h"))!= -1){
		switch(ch){

			case 'f':
				filePath = optarg;
				printf("%s\n", filePath);
				break;

			case 'p':
				cPort = optarg;
				printf("%s\n", cPort);
				break;

			case 'i':
				cnetAddr = optarg;
				printf("%s\n",cnetAddr );
				break;

			case 'h':
				printf("Options:\n");
				printf("Usage: ./ -opt1 para1 -opt2 para2 ...\n");
				printf("-p: port\n");
				printf("-f: login config filePath\n");
				printf("-i: ip address\n");
				printf("-h: Help to list the options\n");
				exit(0);
				break;

			default:
				printf("Usage: ./ -opt1 para1 -opt2 para2 ...\n");
				printf("Use \"./ -h\" to see the options\n");
				exit(0);
				break;
		}
	}

	socketConnect();

	sendFile();

    //printf("waiting for rsp\n");
    //n=recv(sockfd,buffer,MAX_BUF_SIZE,0);
   	result =recv(sockfd,buffer,MAX_BUF_SIZE,0);
   	printf("receive msg:--%s\n",buffer );
	close(sockfd);  
	exit(0);  
	return 0;  
}  

bool socketConnect()
{
	// prepare sokect
	sockfd = socket(PF_INET, SOCK_STREAM, 0);  
	if (sockfd < 0)
	{  
	  printf( "socket falied\n");
	  exit(EXIT_FAILURE);  
	}  
	addrlen = sizeof(struct sockaddr_in);  
	bzero(&addr, addrlen);  
	addr.sin_family = PF_INET; 
	int iPort = atoi(cPort);
	addr.sin_port = htons(iPort);

	printf("%s\n",cnetAddr );
	addr.sin_addr.s_addr = inet_addr(cnetAddr);

	result = connect(sockfd,(struct sockaddr*) &addr,addrlen);

	if(result == -1)
	{
		cout<<"client connect error\n";
		return 0;
	}

	printf("socket success\n");
	return 1;
}

bool sendFile()
{
	bzero(buffer, MAX_BUF_SIZE);
	
	// add 
	socklen_t st = (socklen_t) addrlen ;

	printf("input file path:%s\n",filePath);

    result = sendto(sockfd,filePath,MAX_BUF_SIZE,0,(struct sockaddr*)(&addr),st);
    //result = send(sockfd,filePath,strlen(filePath),0);// put file path to sever 
    if(result<0)
    {
        printf("filePath send error!\n");
    }

    fp = fopen(filePath,"r");//opne file
    usleep(30);
    fileTrans = fread(buffer,sizeof(char),MAX_BUF_SIZE,fp);
    if(fileTrans > 0)
    {
        printf("fileTrans =%d\n",fileTrans);
        result = sendto(sockfd,buffer,MAX_BUF_SIZE,0,(struct sockaddr *)(&addr), st);
        if(result<0)
        {
            printf("send failed!\n");
        }
        bzero(buffer,MAX_BUF_SIZE);
        //memset(buffer,0,sizeof(buffer));  
    }
    fclose(fp);
}