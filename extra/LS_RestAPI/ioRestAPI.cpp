#include "StdAfx.h"
#include "ioRestAPI.h"
#include "libcurl/curl.h"
#include <vector>

#define STR_MAX 4096
#define HTTP "http"
#define DEFAULT_TIMEOUT 7000

#pragma comment(lib, "Ws2_32.lib") 

void Trim(std::string& str)
{
	std::string::size_type pos = str.find_last_not_of(' ');

	if(pos != std::string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != std::string::npos) str.erase(0, pos);
	}

	else str.erase(str.begin(), str.end());
}

extern void Tokenize( const std::string str, std::vector<std::string>& tokens, const std::string delimiters )
{
	tokens.clear();

	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		std::string temp = str.substr(lastPos, pos - lastPos);
		
		Trim(temp);
		tokens.push_back(temp);

		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

ioRestAPI::ioRestAPI(void)
{
	InitData();	

	curl_global_init(CURL_GLOBAL_SSL);
}

ioRestAPI::~ioRestAPI(void)
{
	curl_global_cleanup();
}

void ioRestAPI::InitData()
{
	m_szURL.clear();
	m_vCustomHeader.clear();
	m_szPostParam.clear();
}

size_t ioRestAPI::write_data( void* ptr, size_t size, size_t nmemb, std::string* stream )
{
	if(stream || size*nmemb < STR_MAX)
	{
		stream->append((char*)ptr, 0, size*nmemb); 
	}
	else
		return -1;

	return size*nmemb;
}

int ioRestAPI::SetURL( char* szURL )
{
	if( CheckSize(strlen(szURL), STR_MAX) )
	{
		m_szURL = szURL;
	}

	return 0;
}

BOOL ioRestAPI::AddCustomHeader( char* szCustomHeader )
{
	if( CheckSize(strlen(szCustomHeader), STR_MAX) )
	{
		std::string szTemp = szCustomHeader;
		m_vCustomHeader.push_back(szTemp);
	}

	return TRUE;
}

BOOL ioRestAPI::SetPostParam( char* szPostParam )
{
	if( CheckSize(strlen(szPostParam), STR_MAX) )
	{
		m_szPostParam = szPostParam;
	}

	return TRUE;
}

int ioRestAPI::Perfrom(std::string& szResultData, std::string& szErrCode)
{
	int iReturnCode = 0;

	CURL* curl_handle = NULL;
	CURLcode resCode = CURL_LAST;
	int iRespCode = 0;
	curl_slist* customHeaders = NULL;

	curl_handle = curl_easy_init();

	curl_easy_setopt(curl_handle, CURLOPT_URL, m_szURL.c_str());
	
	if(CheckHTTPS(m_szURL) == TRUE)
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);

	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &szResultData);

	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, DEFAULT_TIMEOUT);

	if(!m_vCustomHeader.empty())
	{
		for(BYTE i=0; i<m_vCustomHeader.size(); i++)
		{
			customHeaders = curl_slist_append(customHeaders, m_vCustomHeader[i].c_str());
		}
	}
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, customHeaders) ;

	if(!m_szPostParam.empty())
	{
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, m_szPostParam.c_str());
	}
	

	resCode = curl_easy_perform(curl_handle);

	if(resCode == CURLE_OK)
	{
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &iRespCode);
	}
	else
	{
		szErrCode = curl_easy_strerror(resCode);
		iReturnCode =  -1;
	}

	curl_slist_free_all(customHeaders);
	curl_easy_cleanup(curl_handle);
	InitData();

	return iReturnCode;
}

BOOL ioRestAPI::CheckSize( DWORD dwInputSzie, DWORD dwLimit )
{
	if(dwInputSzie > dwLimit)
		return FALSE;

	return TRUE;
}

BOOL ioRestAPI::CheckHTTPS( std::string& szURL )
{
	std::vector<std::string> tokens;

	Tokenize(szURL, tokens, ":");

	if(tokens.empty())
		return FALSE;

	if(strcmp(HTTP, tokens[0].c_str()) == 0)
	{
		return FALSE;
	}
	else 
		return TRUE;
}
