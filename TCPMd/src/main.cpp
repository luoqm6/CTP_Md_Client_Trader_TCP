#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <dlfcn.h>
#include <cstdlib>
#include <unistd.h>
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiStruct.h"
#include "CMdSpi.h"
#include "Ini.h"

using namespace std;

//functions used
void readLoginFile(char* filePath);
void readSubFile(char* filePath);

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