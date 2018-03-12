#include <iostream>
#include <fstream>
#include <cstring>
//#include <unordered_map>
#include "CMdSpi.h"
#include "Ini.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiStruct.h"


CMdSpi::CMdSpi(char* filePath){
	readLoginFile(filePath);
}

/// ctp_api callback function
// response for connected successfully
void CMdSpi::OnFrontConnected()
{
	std::cout<<"------Network connected successfully------"<<std::endl;
	// begin to login
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq,0,sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvestorID);
	strcpy(loginReq.Password, gInvestorPassword);
	static int requestID = 0;
	int rt = pMdUserApi -> ReqUserLogin(&loginReq,requestID);
	if(!rt)
		std::cout << ">>>>>>Sent login request successfully" << std::endl;
	else 
		std::cout << "--->>>Failed to send login request" << std::endl;
	isConnect = 1;
}

// imform the disconnection
void CMdSpi::OnFrontDisconnected(int nReason)
{
	std::cout << "------Network disconnected------" << std::endl;
	std::cout << "error code: " << nReason << std::endl;
}

// heartbeat warning
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cout << "------Network heartbeat overtime------" << std::endl;
	std::cout << "The interval from last time: " << nTimeLapse << std::endl;
}

// login response
void CMdSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo -> ErrorID != 0);
	if (!bResult)
	{
		std::cout << "------Account login successfully------" << std::endl;
		std::cout << "Market day: " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "Login time: " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "Broker: " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "User: " << pRspUserLogin->UserID << std::endl;
		isLogin = 1;
	}
	else
		std::cout << "return error--->>> ErrorID = " << pRspInfo->ErrorID << std::endl << "ErrorMsg = " << pRspInfo->ErrorMsg << std::endl;
}

// logout response
void CMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "------Account logout successfully------" << std::endl;
		std::cout << "Broker: " << pUserLogout->BrokerID << std::endl;
		std::cout << "User: " << pUserLogout->UserID << std::endl;
	}
	else
		std::cout << "return error--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cout << "return error--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

void CMdSpi::SubscribeMarketData(char* ppInstrumentID[],int instrumentNum)
{
	cout<<"subscribe\n";

	int rt = pMdUserApi->SubscribeMarketData(ppInstrumentID, instrumentNum);
	if(!rt)
		std::cout << ">>>>>>Sent subscribe MarketData request successfully" << std::endl;
	else 
		std::cout << "--->>>Failed to send subscribe MarketData request" << std::endl;
}

void CMdSpi::UnSubscribeMarketData(char* ppInstrumentID[],int instrumentNum)
{
	cout<<"UnSubscribe\n";

	int rt = pMdUserApi->UnSubscribeMarketData(ppInstrumentID, instrumentNum);
	if(!rt)
		std::cout << ">>>>>>Sent Unsubscribe MarketData request successfully" << std::endl;
	else 
		std::cout << "--->>>Failed to send Unsubscribe MarketData request" << std::endl;
}

void CMdSpi::SubscribeForQuoteRsp(char* ppInstrumentID[],int instrumentNum)
{
	int iResult = pMdUserApi->SubscribeForQuoteRsp(ppInstrumentID, instrumentNum);
	std::cout << "--->>> Send quotation subscribe: " << ((iResult == 0) ? "successful" : "failed") << std::endl;
}

void CMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID !=0 );
	if(!bResult)
	{
		std::cout << "------Subscribe MarketData successfully------" << std::endl;
		std::cout << "InstrumentID: " << pSpecificInstrument->InstrumentID << std::endl;
		// add csv
		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_market_data.csv", pSpecificInstrument->InstrumentID);
		std::ofstream outFile;
		outFile.open(filePath, std::ios::out); // open a new csv file
		outFile << "InstrumentID" << ","
				<< "UpdateTime" << ","
				<< "LastPrice" << ","
				<< "Volume" << ","
				<< "BidPrice1" << ","
				<< "BidVolume1" << ","
				<< "AskPrice1" << ","
				<< "AskVolume1" << ","
				<< "OpenInterest" << ","
				<< "Turnover" <<std::endl;
		outFile.close();

	}
	else 
		std::cout << "return error--->>> ErrorID=" << pRspInfo->ErrorID << ",ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

void CMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "OnRspSubForQuoteRsp" << std::endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID !=0 );
	if(!bResult)
	{
		std::cout << "------UnSubscribe MarketData successfully------" << std::endl;
		std::cout << "InstrumentID: " << pSpecificInstrument->InstrumentID << std::endl;

	}
	else 
		std::cout << "return error--->>> ErrorID=" << pRspInfo->ErrorID << ",ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

void CMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "OnRspUnSubForQuoteRsp" << std::endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// cout the market data
	std::cout << "OnRtnDepthMarketData" << std::endl;
	std::cout << "------get depth market data------" << std::endl;
	std::cout << "TradingDay: " << pDepthMarketData->TradingDay << std::endl;
	std::cout << "ExchangeID: " << pDepthMarketData->ExchangeID << std::endl;
	std::cout << "InstrumentID: " << pDepthMarketData->InstrumentID << std::endl;
	std::cout << "ExchangeInstID: " << pDepthMarketData->ExchangeInstID << std::endl;
	std::cout << "Last price: " << pDepthMarketData->LastPrice << std::endl;
	std::cout << "Volume: " << pDepthMarketData->Volume << std::endl;
	char filePath[100] = {'\0'};
	sprintf(filePath, "%s_market_data.csv", pDepthMarketData->InstrumentID);
	std::ofstream outFile;
	outFile.open(filePath, std::ios::app); // OnRspSubMarketData
	outFile << pDepthMarketData->InstrumentID << ","
			<< pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << ","
			<< pDepthMarketData->LastPrice << ","
			<< pDepthMarketData->Volume << ","
			<< pDepthMarketData->BidPrice1 << ","
			<< pDepthMarketData->BidVolume1 << ","
			<< pDepthMarketData->AskPrice1 << ","
			<< pDepthMarketData->AskVolume1 << ","
			<< pDepthMarketData->OpenInterest << ","
			<< pDepthMarketData->Turnover/1000000 << std::endl;
	std::cout << pDepthMarketData->Turnover/100000000 << std::endl;
	outFile.close();
}

void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	std::cout << "OnRtnForQuoteRsp" << std::endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// if ErrorID != 0, it means receiving error response
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		std::cout << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
	return bResult;
}

void CMdSpi::readOrderFile(char* filePath)
{
	//read config.ini file
	CIni ini;

	ini.openFile(filePath,"r");

	orderType = ini.getInt("Order","OrderType");

	instrumentNum = ini.getInt("InstrumentNum", "InstrumentNum");
	printf("instrumentNum=%d\n", instrumentNum);

	char InstrumentCode[6] = {'C','o','d','e','0','\0'};
	string sInstrumentID[instrumentNum];
	for(int i=0;i<instrumentNum;i++)
	{
		InstrumentCode[4] = (char)(i+48);
		cout<<"InstrumentCode="<<InstrumentCode<<endl;
		sInstrumentID[i] = ini.getStr("InstrumentCode", InstrumentCode);
		cout<<"sInstrumentID[i]="<<sInstrumentID[i]<<endl;
	}
	for(int i=0;i<instrumentNum;i++)
	{
		ppInstrumentID[i] = new char [20];
		strcpy(ppInstrumentID[i] ,sInstrumentID[i].data());
	}
	for(int i=0;i<instrumentNum;i++)
	{
		cout << "ppInstrumentID[" << i << "]=" << ppInstrumentID[i] << endl;
	}
}
void CMdSpi::readLoginFile(char* filePath)
{
	//read config.ini file
	CIni ini;

	ini.openFile(filePath,"r");
	cout<<"filePath"<<filePath<<endl;

	char* brokerId = ini.getStr("Broker","ID");
	sprintf(gBrokerID,"%s",brokerId);
	printf("gBrokerID=%s\n", gBrokerID);
	char* investorId = ini.getStr("Investor","ID");
	sprintf(gInvestorID,"%s",investorId);
	printf("gInvestorID=%s\n", gInvestorID);
	char* password = ini.getStr("Investor","Password");
	sprintf(gInvestorPassword,"%s",password);
	printf("gInvestorPassword=%s\n", gInvestorPassword);

	char frontAddr[]="tcp://180.168.146.187:10031";// "tcp://180.168.146.187:100010" 31
	gMdFrontAddr = new char [strlen(frontAddr)];
	sprintf(gMdFrontAddr,"%s",frontAddr);
}

void CMdSpi::sendOrder()
{
	cout<<"orderType = "<<orderType<<endl;
	if(orderType == 0)
	{
		SubscribeMarketData(ppInstrumentID,instrumentNum);
	}
	else if(orderType == 1)
	{
		
		UnSubscribeMarketData(ppInstrumentID,instrumentNum);
	}
}
