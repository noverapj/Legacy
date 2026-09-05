#include "stdafx.h"
#include "WinHttpClient.h"
#include "ioHTTP.h"



ioHTTP::ioHTTP(void)
{
	Init();
}

ioHTTP::~ioHTTP(void)
{
	Destroy();
}

void ioHTTP::Init()
{
	 
}

void ioHTTP::Destroy()
{
 
}

#if !defined( USE_GA )
DWORD Ansi2Wide(const char * szANSI, wchar_t* szWide, int iWideLen)
{
   int iRet = MultiByteToWideChar(	GetACP(),
                                    0,
									szANSI,
                                    -1,
                                    szWide,
                                    iWideLen);
    return (iRet);
}

DWORD Wide2Ansi(const wchar_t* szWide, char * szANSI, int iAnsiLen)
{
	ZeroMemory(szANSI, iAnsiLen);
	int iRet = WideCharToMultiByte(CP_ACP, 0, szWide, -1, szANSI, iAnsiLen, NULL, NULL);
	return iRet;
}
#endif

DWORD Wide2Utf8(const wchar_t* szWide, char * szANSI, int iAnsiLen)
{
	ZeroMemory(szANSI, iAnsiLen);
	int iRet = WideCharToMultiByte(CP_UTF8, 0, szWide, -1, szANSI, iAnsiLen, NULL, NULL);
	return iRet;
}

BOOL ioHTTP::GetResultData(const CHAR* URL, const char* POST, char* result, const int length)
{
	WinHttpClient wHTTP;
	std::wstring wURL;

	wURL = CA2W(URL);

	std::wstring verb = CA2W(POST);
	wHTTP.UpdateUrl(wURL);

 	int postLen = strlen(POST);

	//// Set post data.
	if( strcmp("GET",POST) != 0) //post 规侥老锭 
	{
		verb = L"POST";

		wHTTP.SetAdditionalDataToSend((BYTE *)POST, postLen);

		wchar_t szSize[50] = L"";
		swprintf_s(szSize, L"%d", postLen);
		std::wstring headers = L"Content-Length: ";
		headers += szSize;
		headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\n";
		wHTTP.SetAdditionalRequestHeaders(headers);
	}
	else
		verb = L"GET";
		
	if(!wHTTP.SendHttpRequest(verb))
		return FALSE;

	std::wstring responseHeader	= wHTTP.GetResponseHeader();
	std::wstring responseContent = wHTTP.GetResponseContent();

	Wide2Utf8( responseContent.c_str(), result, length );

	return TRUE;
}

BOOL ioHTTP::GetResultData(const CHAR* URL, const char* POST, char* HEADER, char* result, const int length)
{
	WinHttpClient wHTTP;
	std::wstring wURL, wHeader;

	wURL	= CA2W(URL);
	wHeader	= CA2W(HEADER);

	std::wstring verb = CA2W(POST);
	wHTTP.UpdateUrl(wURL);

 	int postLen = strlen(POST)+2;

	//// Set post data.
	if( strcmp("GET",POST) != 0) //post 规侥老锭 
	{
		verb = L"POST";

		wHTTP.SetAdditionalDataToSend((BYTE *)POST, postLen);

		wchar_t szSize[50] = L"";
		swprintf_s(szSize, L"%d", postLen);
		std::wstring headers = wHeader;
		headers += L"\r\n";
		headers += L"Content-Length: ";
		headers += szSize;
		headers += L"\r\n";
		wHTTP.SetAdditionalRequestHeaders(headers);
	}
	else
		verb = L"GET";
		
	if(!wHTTP.SendHttpRequest(verb))
		return FALSE;
	 
	std::wstring responseHeader	= wHTTP.GetResponseHeader();
	std::wstring responseContent = wHTTP.GetResponseContent();

	Wide2Utf8( responseContent.c_str(), result, length );

	return TRUE;
}
