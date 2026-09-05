#pragma once


#include <string>
#include <vector>
#include <WTypes.h>

class ioRestAPI
{
public:
	ioRestAPI(void);
	virtual ~ioRestAPI(void);

public:
	static size_t write_data(void* ptr, size_t size, size_t nmemb, std::string* stream);

protected:
	void InitData();

public:
	int SetURL(char* szURL); //return 1==http 2==https -1==error
	BOOL AddCustomHeader(char* szCustomHeader);
	BOOL SetPostParam(char* szPostParam);

public:
	int Perfrom(std::string& szResultData, std::string& szErrCode);//return -1 is error

protected:
	BOOL CheckSize(DWORD dwInputSzie, DWORD dwLimit);
	BOOL CheckHTTPS(std::string& szURL);

protected:
	std::string m_szURL;
	std::vector<std::string> m_vCustomHeader;
	std::string m_szPostParam;

};

