#include "StdAfx.h"
#include "CRCManager.h"

#include "../../OneCRCDlg.h"
#include "../FTP/FTPManager.h"
#include <urlmon.h>

CCRCManager *CCRCManager::g_Inst = NULL;

CCRCManager::CCRCManager(void) :
m_iProcessStep(_FILECHECK), m_strInfoIniPath(_T("")), m_strAutoExeFolderAddr(_T(""))
, m_strCrcReaderAddr(_T("")), m_strCopyFolderAddr(_T("")), m_strExportTxtAddr(_T("")), 
m_iServerCnt(SERVER_ONE), m_iConnectCnt(0)//, m_strChangeDataName(_T(""))
, m_strIP(_T("")), m_strID(_T("")), m_strPW(_T("")), m_strLog(_T(""))
{
}


CCRCManager::~CCRCManager(void)
{
}

CCRCManager* CCRCManager::GetInst()
{
	if(NULL == g_Inst)
		g_Inst = new CCRCManager;
	return g_Inst;
}

void CCRCManager::ReleaseInst()
{
	if(g_Inst)
		delete g_Inst;
	g_Inst = NULL;
}

void CCRCManager::RunProcess()
{

	//CString  strToken, strMakeDirName;
	

	switch(m_iProcessStep)
	{
	case _FILECHECK:
		PrintLog("서버 정보를 읽습니다.(info.ini)");
		CheckServerInfo(); //서버 갯수 확인
		LoadInfoINI();
		break;
	case _COPYnRUN:
		//필요한 파일을 원하는 경로에 복사 원본 파일, 파일명, 복사경로
		PrintLog("CrcReader.exe을 실행합니다");
		CopyFilePath(&m_strAutoExeFolderAddr, AUTO_UP_EXE); 
		CopyFilePath(&m_strCrcReaderAddr, CRC_EXE_FILE_NAME); 
		RunCRCnDelete(); //Reader 실행 및 불필요 파일 삭제
	case _MAKEFILE:
		PrintLog("Crc값을 통해 파일을 생성합니다.");
		CreateExportFolder();
		(*m_CRCDlg).SetConnectInfo(&m_strIP, &m_strID, &m_strPW);	//접속 정보 전달
		(*m_CRCDlg).SetUploadListPath(&m_vecUploadlist);			//업로드 리스트 보내주고
		(*m_CRCDlg).SetDownloadPath(&m_strExportTxtAddr);			//다운로드 경로 전달
		CFTPManager::GetInst()->DownDataChangeFolder(&m_vecOutFolderName[SD_ALPHA]); //ftp다운로드 전달
		break;
	}
	++m_iProcessStep;
	if(m_iProcessStep >= _WORKCOMPLETE-1)
	{
		PrintLog("Crc값 생성 완료");
		return;
	}
	RunProcess();
}

//경로안에 파일이 있는지 확인하는 함수입니다.
BOOL CCRCManager::CheckFileInFolder(CString* const strPath, char* ctext )
{
	CFileFind pFind;
	CString strSearchPath;
	strSearchPath = *strPath;
	strSearchPath += '\\';
	strSearchPath += ctext;
	return pFind.FindFile(strSearchPath);
}

//메시지 박스 출력
void CCRCManager::PrintMessage( int iType, char* ctext )
{
	TCHAR szText[MAX_PATH] = {0};
	ChangeFontString(szText, ctext);

	switch(iType)
	{
	case MC_FILE :
		MessageBoxW(g_hWnd, szText, L"NOT FOUND FILE", MB_OK);
		break;
	case MC_URL :
		MessageBoxW(g_hWnd, szText, L"NOT VALID URL", MB_OK);
		break;
	case MC_INTERNET :
		MessageBoxW(g_hWnd, szText, L"NOT CONNECTED INTERNET", MB_OK);
		break;
	case MC_ALERT :
		MessageBoxW(g_hWnd, szText, L"PROCESS ALERT", MB_OK);
		break;
	}
}

//폰트 타입 변경
void CCRCManager::ChangeFontString( TCHAR* pSour, char* pDest )
{
#if defined(UNICODE) | defined(_UNICODE)
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, 
		pDest, strlen(pDest), pSour, strlen(pDest));
#else
	strcpy(pSour, pDest);
#endif
}

//itype 값에 의해 info.ini에서 정보를 읽어 옵니다.
void CCRCManager::GetINIData( CString* const FilePath, char* SourFile, int iType, CString* DestData)
{
	CString strFilePath;
	strFilePath = *FilePath;
	strFilePath += '\\';
	strFilePath += SourFile;

	char szName[MAX_PATH]={0,};
	GetCurrentDirectory(MAX_PATH,(LPWSTR)szName); //현재 경로 얻기
	CString strTestPath;
	AddStringToChar(&strTestPath, szName);

	CString tchar;
	CString strToken, strAddr;
	switch (iType)
	{
	case GD_SERVERCNT:
		tchar = "ServerCount";
		break;
	case GD_INIPATH: //INI 읽어내
		tchar = "start.ini";
		break;
	case GD_CRCPATH :
		tchar = "CrcReader.exe";
		break;
	case GD_AUTOEXEPATH:
		tchar = "autoupgrade.exe";
		break;
	case GD_CPYFOLDER:
		tchar = "CopyFolder"; //복사폴더 설정시 info.ini에 넣어주셈
		break;
	case GD_IP:
		tchar = "IP"; 
		break;
	case GD_ID:
		tchar = "USER"; 
		break;
	case GD_PW:
		tchar = "Password";
		break;
	case GD_OTHERSVRCNT:
		tchar = "OtherSerCnt"; 
		break;
	case GD_BETACNT:
		tchar = "BETASVRCNT"; 
		break;
	case GD_LIVECNT:
		tchar = "LIVESVRCNT"; 
		break;
	}

	CStdioFile Txt_File;
	if(Txt_File.Open(strFilePath, CFile::modeRead | CFile::typeText))
	{
		CString str, strToken;
		while(Txt_File.ReadString(str))
		{
			if(str.Find((LPWSTR)(LPCWSTR)tchar)!= -1)
			{
				if(iType == GD_AUTOEXEPATH && str.Find(L"ChangeData")!= -1)
				{
					continue;
				}
				str += '='; //종료 문자열을 맨 뒤에 넣는다.
				AfxExtractSubString(strToken, str, 0, '=');
				AfxExtractSubString(strToken, str, 1, '=');
				strToken.TrimLeft(); //왼쪽 공백 제거
				*DestData += strToken;
				break;
			}
		}
	}
	Txt_File.Close();
}

//info.ini에서 기타 서버의 url 갯수만큼 정보를 읽어들입니다.
void CCRCManager::GetINIData(std::vector<CString>* vecAddr, char* SourFile, int iType, std::vector<CString>* DestData, int ServerCnt)
{
	CString cText, tStr;
	if(iType == GD_URL)
		cText = L"URL";

	Iter iter;
	iter = vecAddr->begin();
	CStdioFile Txt_File;
	for(int i = 0; i < m_iServerCnt; ++i)
	{
		if(Txt_File.Open((LPCTSTR)(LPCTSTR)(*iter), CFile::modeRead | CFile::typeText))
		{
			CString str, strToken;
			while(Txt_File.ReadString(str))
			{
				if(str.Find((LPWSTR)(LPCWSTR)cText)!= -1)
				{
					str += '='; //종료 문자열을 맨 뒤에 넣는다.
					AfxExtractSubString(strToken, str, 0, '=');
					AfxExtractSubString(strToken, str, 1, '=');
					strToken.TrimLeft(); //왼쪽 공백 제거
					DestData->push_back(strToken);
					++iter;
					break;
				}
			}
		}
		Txt_File.Close();
	}
}

//info.ini에서 정보를 읽어들입니다.
void	CCRCManager::GetINIData(CString* const FilePath, char* SourFile, int iType, std::vector<CString>* DestData, int ServerCnt)
{
	CString strFilePath;
	strFilePath = *FilePath;
	strFilePath += '\\';
	strFilePath += SourFile;
	CString tchar;

	switch (iType)
	{
	case GD_URL: //url 읽어내
		tchar = "URL";
		break;
	case GD_INIPATH: //INI 읽어내
		tchar = "start.ini";
		break;
	case GD_CPYFOLDER:
		tchar = "CopyFolder"; //복사폴더 설정시 info.ini에 넣어주셈
		break;
	}

	CStdioFile Txt_File;
	BOOL bGetText = FALSE;
	Iter iter = m_vecCreateFileName.begin();
	if(Txt_File.Open(strFilePath, CFile::modeRead | CFile::typeText))
	{
		CString str, strToken, strToken1;
		CString tFindKey, strNum, strChangeNum;
		int iCnt = 0 ;
		tFindKey += '[';
		strNum.Format(_T("%d"),iCnt+1);
		tFindKey += strNum;
		tFindKey += ']';
		while(Txt_File.ReadString(str))
		{
			if(iCnt > m_iServerCnt)
				break;
			if(!bGetText && str.Find(tFindKey)!= -1)
			{
				++iCnt;
				strNum.Format(_T("%d"), iCnt+1);
				strChangeNum.Format(_T("%d"), iCnt);
				tFindKey.Replace(strChangeNum, strNum);
				bGetText = TRUE;
			}
			if(bGetText && str.Find((LPWSTR)(LPCWSTR)tchar)!= -1)
			{
				str += '='; //종료 문자열을 맨 뒤에 넣는다.
				AfxExtractSubString(strToken, str, 0, '=');
				AfxExtractSubString(strToken, str, 1, '=');
				strToken.TrimLeft(); //왼쪽 공백 제거
				m_vecINIFolderAddr.push_back(strToken);
				if(iType == GD_INIPATH) //위치 조립
				{
					strToken += '\\';
					strToken += tchar;
				}
				(*DestData).push_back(strToken);
				bGetText = false; //다음꺼 읽자
				//생성할 출력 폴더 명을 얻습니다.
				str.MakeReverse();
				AfxExtractSubString(strToken1, str, 0, '\\');
				AfxExtractSubString(strToken1, str, 1, '\\');
				strToken1.MakeReverse();
				m_vecOutFolderName.push_back(strToken1);
				//생성할 출력파일 폴더명을 얻습니다.
				(*iter) += '\\';
				(*iter) += strToken1;
				(*iter) += '\\';
				++iter;
			}
		}
	}
	Txt_File.Close();
}

//crcReader을 실행하고, 값을 저장한 후 생성된 crc.txt파일을 삭제합니다.
void CCRCManager::RunCRCnDelete()
{
	//CRC 리더 실행 및 복사했던 파일 삭제
	Iter iter = m_vecINIFolderAddr.begin();
	//for(int i = 0; i < m_iServerCnt; ++i)
	{
		SetCurrentDirectory(*iter);
		CString DelStr = *iter;
		CString strCRCtxtPath = *iter;
		CString strCRCFolderPath = *iter;
		DelStr		  += '\\';
		strCRCtxtPath += '\\';
		strCRCtxtPath += CRC_EXE_FILE_NAME;

		int iResult = (int)::ShellExecute(NULL, TEXT("open"), strCRCtxtPath, NULL, NULL, SW_SHOW);
		//에러 체크
		if(iResult != TRUE)
			switch(iResult)
		{
			case ERROR_FILE_NOT_FOUND:
				MessageBoxW(g_hWnd,L"파일이 없습니다.",L"FAILED CRCReader",MB_OK);
				break;
		}
		WaitTime(1000); //1초간 기다립니다.
		LoadCRCTxtData(&strCRCFolderPath); //경로에서 CRCtxt의 값을 읽어옵니다.

		WaitTime(1500); //1.5초간 기다립니다.
		FindWindowNClose("Crc.txt - 메모장");
		DeletePathInFile(&DelStr, AUTO_UP_EXE);
		DeletePathInFile(&DelStr, CRC_EXE_FILE_NAME);
		DeletePathInFile(&DelStr, CRC_TXT_FILE_NAME);
		++iter;
	}
	
}

//대기 시간을 설정합니다.
void CCRCManager::WaitTime( DWORD dwMilliSec )
{
	DWORD dwStart = GetTickCount();
	while(GetTickCount() - dwStart < dwMilliSec)
	{
	}
}

//info.ini에서 서버 정보를 읽어들이고 갯수만큼 vector를 예약합니다.
//현재 실행파일의 경로를 통해 상대적인 경로로 파일을 찾습니다.
void CCRCManager::CheckServerInfo()
{
	char szName[MAX_PATH]={0,};
	GetCurrentDirectory(MAX_PATH,(LPWSTR)szName); //현재 경로 얻기
	int i = 0;
	AddStringToChar(&m_strInfoIniPath, szName);

	//출력 폴더 지정
	m_strExportTxtAddr = m_strInfoIniPath;
	m_strExportTxtAddr += "\\Output";

	m_strInfoIniPath += "\\bin";
	CString	strUp = m_strInfoIniPath;
	strUp += '\\';
	strUp += AUTO_UP_EXE;
	m_vecUploadlist.push_back(strUp);

	//Info파일 있니?
	if(!CheckFileInFolder( &m_strInfoIniPath,"Info.ini"))
		PrintMessage(MC_FILE, "Info.ini");

	CString Tstr;
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_SERVERCNT, &Tstr);
	//GetINIData(&m_strInfoIniPath, "Info.ini", GD_CHAGEDATA, &m_strChangeDataName);
	//FTP정보 가져옴
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_IP, &m_strIP);
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_ID, &m_strID);
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_PW, &m_strPW);
	m_iServerCnt = _ttoi(Tstr);

	//서버 갯수로 vector 초기화
	m_vecOutFolderName.reserve(m_iServerCnt);
	m_vecINIAddrData.reserve(m_iServerCnt);
	m_vecURLAddr.reserve(m_iServerCnt);
	m_vecURLData.reserve(m_iServerCnt);
	m_vecAutoEXE.reserve(m_iServerCnt);
	m_vecAutoInfo.reserve(m_iServerCnt);
	m_vecMGameInfo.reserve(m_iServerCnt);
	m_vecCreateFileName.reserve(m_iServerCnt);
	for(int i = 0 ; i < m_iServerCnt; ++i)
	{
		m_vecCreateFileName.push_back(m_strExportTxtAddr);
	}
}

//info.ini정보를 읽습니다.
void CCRCManager::LoadInfoINI()
{
	//ini파일에서 정보 읽어 오자!
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_INIPATH, &m_vecINIAddrData, m_iServerCnt); //서버에 따른 start.ini의 경로를 로드합니다.
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_AUTOEXEPATH, &m_strAutoExeFolderAddr);	
	GetINIData(&m_strInfoIniPath, "Info.ini", GD_CRCPATH, &m_strCrcReaderAddr);
}

//파일 경로를 복사합니다.
void CCRCManager::CopyFilePath( CString* const strSour, char* ctext)
{
	Iter iter = m_vecINIFolderAddr.begin();
	for(int i = 0 ; i < m_iServerCnt ; ++i)
	{
		CString Sour, Dest;
		Sour = *strSour;
		Sour += '\\';
		AddStringToChar(&Sour, ctext);
		Dest = *iter;
		Dest += '\\';
		AddStringToChar(&Dest, ctext); //이제 복사 시작하면 된다
		CopyFile(Sour, Dest, FALSE);
		++iter;
	}
}

//CString에다가 char를 붙입니다.
void CCRCManager::AddStringToChar( CString* sSour, char* ctext )
{
	int i = 0;
	while(TRUE)
	{
		*sSour += (ctext + i);
		i+=2;
		if(*(ctext + 1) != NULL) //char 저장 형태에 때라 복사 방식이 다룸~~
			return;
		if(*(ctext + i) == NULL)
			return;
	}
}

//crc.txt파일에서 정보를 얻어들여 저장합니다.
void CCRCManager::LoadCRCTxtData( CString* sSour )
{
	CString szTempTxT = *sSour;
	szTempTxT += '\\';
	szTempTxT += CRC_TXT_FILE_NAME;
	int iCount = 0;
	CStdioFile Txt_File;
	char* pChar = NULL;
	int iValue = 2;
	WaitTime(1200);
	if(Txt_File.Open(szTempTxT, CFile::modeRead | CFile::typeText))
	{
		CString str, strData;
		while(Txt_File.ReadString(str))
		{
			pChar = LPSTR(LPCTSTR(str));
			if(str.Find(L"autoupgrade_info.ini") != -1 && iCount == 0)
			{
				for(;(*(pChar+iValue))!='\n'; pChar += iValue) //맨 마지막 까지 증가
				{
					if((*(pChar)=='0') || (*(pChar+iValue))=='\n') //값이 0이면 종료
						break;
					else if(*(pChar)>'0' && *(pChar)<='9') //0이 아니면
					{
						iValue = 1;
						break;
					}
				}
				strData.Format(L"%s",pChar);
				m_vecAutoInfo.push_back(strData);
				++iCount;
			}
			if(str.Find(L"autoupgrade.exe") != -1 && iCount == 1)
			{
				for(;(*(pChar+iValue))!='\n'; pChar += iValue) //맨 마지막 까지 증가
				{
					if((*(pChar)=='0') || (*(pChar+iValue))=='\n') //값이 0이면 종료
						break;
					else if(*(pChar)>'0' && *(pChar)<='9') //0이 아니면
					{
						iValue = 1;
						break;
					}
				}
				strData.Format(L"%s",pChar);
				m_vecAutoEXE.push_back(strData);
				++iCount;
			}
			if(str.Find(L"autoupgrade_info_mgame.ini") != -1 /*&& iCount == 2*/)
			{
				for(;(*(pChar+iValue))!='\n'; pChar += iValue) //맨 마지막 까지 증가
				{
					if((*(pChar)=='0') || (*(pChar+iValue))=='\n') //값이 0이면 종료
						break;
					else if(*(pChar)>'0' && *(pChar)<='9') //0이 아니면
					{
						iValue = 1;
						break;
					}
				}
				strData.Format(L"%s",pChar);
				m_vecMGameInfo.push_back(strData);
			}
		}
	}
}

//윈도우 창을 닫습니다.
void CCRCManager::FindWindowNClose( char* ctext )
{
	CString tStr;
	AddStringToChar(&tStr, ctext);
	HWND hExit = NULL;
	hExit = ::FindWindow(NULL, (LPWSTR)(LPCWSTR)tStr); //창을 닫자!
	::SendMessage(hExit, WM_CLOSE,0,0);
}

//해당 경로에 파일을 삭제합니다.
void CCRCManager::DeletePathInFile( CString* sPath, char* ctext )
{
	//테스트용
	SetCurrentDirectory(*sPath);
	CStdioFile Txt_File;
	CString str;
	AddStringToChar(&str, ctext);
	Txt_File.Remove(str);
}

//배포할 폴더를 생성합니다.
void CCRCManager::CreateExportFolder()
{
	for(int i = 0 ; i < m_iServerCnt ; ++i)
	{
		MakeFolder(&m_strExportTxtAddr, &m_vecOutFolderName[i]);
	}
}

//폴더를 생성합니다.
void CCRCManager::MakeFolder( CString* strPath, CString* StrName )
{
	CFileFind CFileFinder;
	CString strExportFolderAddr;
	
	strExportFolderAddr += *strPath;
	strExportFolderAddr += '\\';
	strExportFolderAddr += *StrName;

	if(!CFileFinder.FindFile(strExportFolderAddr)) //디렉토리가 음슴
		CreateDirectory(strExportFolderAddr, NULL);
	else	//있음
		return;
}

//crcdata를 얻어옵니다.
CString CCRCManager::GetCrcData(int m_iChangeDateType)
{
	CString strData;
	//crc파일에서 변경할 데이터를 찾아 전달합니다.
	Iter cIAuto, cIAutoInfo, cIMGame;
	cIAuto		= m_vecAutoEXE.begin();
	cIAutoInfo	= m_vecAutoInfo.begin();
	cIMGame		= m_vecMGameInfo.begin();
	switch(m_iChangeDateType)
	{
	case DI_AUTOEXE:
		 return *cIAuto;
		break;
	case DI_AUTOINFO :
		return *cIAutoInfo;
		break;
	case DI_MGAMEINI:
		return *cIMGame;
		break;
	}
	
// 	//if(m_strChangeDataName.Find(L"autoupgrade.exe") != -1)
// 	{
// 		strData = m_vecAutoEXE[0];
// 		//m_iChangeDateType = DI_AUTOEXE;
// 	}
// 	//else if(m_strChangeDataName.Find(L"mgame.ini") != -1)
// 	{
// 		strData = m_vecMGameInfo[0];
// 		//m_iChangeDateType = DI_MGAMEINI;
// 	}
// 	//else {
// 		strData = m_vecAutoInfo[0];
// 		//m_iChangeDateType = DI_AUTOINFO;
// 	}

	return strData;
}

//베이스서버(ftp로 정보를 얻는 Alpha)외에 기타 서버의 정보를 변경할 때
//기타 서버의 하위 서버의 갯수를 파악합니다.
int CCRCManager::GetOtherServerCnt(int iOther)
{
	CString Tstr;
	GetINIData(&m_strInfoIniPath, "Info.ini", iOther, &Tstr);
	return _ttoi(Tstr);
}

//기타 서버의 url정보를 얻어옵니다.
void CCRCManager::GetOtherServerURL( pVecString vec, int iCnt, int iSvrName)
{
	GetURLData(&m_strInfoIniPath, "Info.ini", vec, iCnt, iSvrName);
}


void CCRCManager::GetURLData( CString* sPath, char* SourFile, pVecString pDest, int iSvrNum ,int iSvrName)
{
	CString str;
	str += *sPath;
	str += '\\';
	AddStringToChar(&str, SourFile);
	CString strStartText;
	switch(iSvrName)
	{
	case GD_BETACNT :
		strStartText = "[BETA]";
		break;
	case GD_LIVECNT :
		strStartText = "[LIVE]";
		break;
	case GD_DEVCNT :
		strStartText = "[DEV]";
		break;
	}

	CStdioFile Txt_File;
	BOOL	bSearchStart = FALSE;

	if(Txt_File.Open(str, CFile::modeRead | CFile::typeText))
	{
		CString str, strToken;
		while(Txt_File.ReadString(str))
		{
			if(str.Find((LPWSTR)(LPCWSTR)strStartText)!= -1)
				bSearchStart = TRUE;

			if(str.Find(L"URL")!= -1 && bSearchStart)
			{
				str += '='; //종료 문자열을 맨 뒤에 넣는다.
				AfxExtractSubString(strToken, str, 0, '=');
				AfxExtractSubString(strToken, str, 1, '=');
				strToken.TrimLeft(); //왼쪽 공백 제거
				pDest->push_back(strToken);
			}
			if(str.Find(L"END")!= -1 && bSearchStart)
				break;
		}
	}
	Txt_File.Close();

}

//url을 통해 소스를 읽습니다. 문제가 생겨 봉인
void CCRCManager::GetSourceURLData( pVecString vecURL, pVecString vecDest, int iCnt )
{
	Iter iter= (*vecURL).begin();
	m_iConnectCnt = 0;
	for(int i = 0 ; i < iCnt; )
	{
		//웹브라우져 소스 획득
		IWebBrowser2	*pWebBrowser = NULL;
		HRESULT			hr;
		hr = OleInitialize(NULL);
		CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (LPVOID*)&pWebBrowser);

		BSTR	bstrURL = SysAllocString(*iter);
		if(pWebBrowser)
		{
			VARIANT	vEmpty;
			VariantInit(&vEmpty);

			hr = pWebBrowser->Navigate(bstrURL, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
		}

		WaitTime(500); //1초간 접속 기다림

		IHTMLDocument2* pHTMLDocument;
		LPDISPATCH		lpDispatch;
		HRESULT			hrDispatch;
		hrDispatch = pWebBrowser->get_Document(&lpDispatch);

		if(m_iConnectCnt > 20)
		{
			PrintMessage(MC_INTERNET,"lpDispatch 값이 입력되지 않았습니다. 대기 시간을 수정해주세요.\n(최대 대기시간 10초)");
		}
		if(!hrDispatch || lpDispatch == NULL )
		{
			++m_iConnectCnt;
			continue;
		}	

		if(lpDispatch)
		{
			HRESULT hr;
			hr = lpDispatch->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pHTMLDocument);
			lpDispatch->Release();
			WaitTime(500); //0.5초간 접속 기다림
			IHTMLElement*	pBody;
			hr = pHTMLDocument->get_body(&pBody);
			BSTR bstr;
			pBody->get_innerHTML(&bstr);
			int nLen;
			char* szTarget;
			CString Tstr;
			nLen = (int)wcslen(bstr) * 2 + 1;
			szTarget = new char[nLen];
			::WideCharToMultiByte(CP_ACP, 0, bstr, -1, szTarget, nLen, NULL, NULL);
			Tstr = szTarget;
			if(*szTarget == NULL)
			{
				PrintMessage(MC_INTERNET, "인터넷 경로를 확인해 주세요."); //경로 값을 읽어오지 못합니다.
			}
			(*vecDest).push_back(Tstr);
			Iter iterData= (*vecDest).end();
			--iterData;
			(*iterData).Replace(L"<pre>",L"");
			(*iterData).Replace(L"</pre>",L"");
			SysFreeString(bstr);
			pBody->Release();

			 ++i, ++iter;

			delete szTarget;
		}

		pWebBrowser->Quit();
		SysFreeString(bstrURL);
		pWebBrowser->Release();
		OleUninitialize();	
	}
}

void CCRCManager::ChangeURLData( pVecString vUrlData, int iChangeType, int iSvrCnt, int iChangeValue )
{
	iChangeType = DI_AUTOEXE;
	pVecString pVec;
	Iter iter = (*vUrlData).begin();
	switch(iChangeType)
	{
		case DI_AUTOEXE:
			pVec = &m_vecAutoEXE;
		break;
		case DI_MGAMEINI:
			pVec = &m_vecMGameInfo;
			break;
		case DI_AUTOINFO:
			pVec = &m_vecAutoInfo;
			break;
	}
	CString strChangeValue = (*pVec)[iChangeValue];
	CString tStr;

	int i = (*vUrlData).size();
	if(i != iSvrCnt)
	{
		iSvrCnt = i;
		PrintMessage(MC_INTERNET, "URL Data를 제대로 가져오지 못했습니다. 결과를 확인해주세요");
	}

	for(int i = 0 ; i < iSvrCnt ; ++i, ++iter)
	{
		if(iChangeType == DI_AUTOEXE &&(*iter).Find(L"autoupgrade.exe?") != -1) //문자열 찾았으니 시작
		{
			AfxExtractSubString(tStr, (*iter), 0, '?'); //?앞까지 버리기
			AfxExtractSubString(tStr, (*iter), 1, '?'); //특정 문자 자르기
			(*iter).Replace(tStr, strChangeValue);
		}
		if(iChangeType == DI_MGAMEINI &&(*iter).Find(L"autoupgrade_info_mgame.ini?") != -1) //문자열 찾았으니 시작
		{
			AfxExtractSubString(tStr, (*iter), 0, '?'); //?앞까지 버리기
			AfxExtractSubString(tStr, (*iter), 1, '?'); //특정 문자 자르기
			(*iter).Replace(tStr, strChangeValue);
		}
		if(iChangeType == DI_AUTOINFO &&(*iter).Find(L"autoupgrade_info.ini?") != -1) //문자열 찾았으니 시작
		{
			AfxExtractSubString(tStr, (*iter), 0, '?'); //?앞까지 버리기
			AfxExtractSubString(tStr, (*iter), 1, '?'); //특정 문자 자르기
			(*iter).Replace(tStr, strChangeValue);
		}
	}
}

void CCRCManager::CreateTxtFileInFolder( pVecString vURLAddr, pVecString vURLData, pVecString vOutputPath, int iSvrCnt, int iType )
{
	Iter iterAddr, iterData;
	CStdioFile Txt_File;
	iterAddr = (*vURLAddr).begin();
	iterData = (*vURLData).begin();
	CString strPath;
	CString strOutPath = (*vOutputPath)[iType];
	int i = (*vURLData).size();
	if(i != iSvrCnt)
	{
		iSvrCnt = i;
		PrintMessage(MC_INTERNET, "URL Data를 제대로 가져오지 못했습니다. 결과를 확인해주세요");
	}
	for(int i = 0 ; i < iSvrCnt ; ++i)
	{
		//생성할 파일명을 찾습니다.
		CString strMakeFileName;
		strPath = strOutPath;
		(*iterAddr).MakeReverse();
		AfxExtractSubString(strMakeFileName, (*iterAddr), 0,'/');
		(*iterAddr).MakeReverse();
		strMakeFileName.MakeReverse();
		strPath += '\\';
		strPath += strMakeFileName;
		//파일을 만든다
		if(Txt_File.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			Txt_File.WriteString(*iterData);
		}
		Txt_File.Close();
		if(i < iSvrCnt-1 )
		{
			++iterData;
			++iterAddr;
		}
		//끗
	}
	
}

void CCRCManager::OtherServerProcess()
{
	int iOtherServerCount = 0;
	int iServerNumInOtherServer[SERVER_MAX];
	//CString strCrcData = GetCrcData();		  //Crc값
	//int		iChangeType= GetChangeDataType(); //DI_AUTOEXE ~ DI_MGAMEINI

	if(m_iServerCnt > 1) //서버 갯수가 하나가 이나라면 작업해이이잉~~
	{
		iOtherServerCount = GetOtherServerCnt(GD_OTHERSVRCNT);
	}
	//각 Other서버의 안에 있는 서버의 갯수를 얻어옵니다.
	
	for(int i = 0 ; i < iOtherServerCount ; ++i)
	{
		if(i == SERVER_ZERO )
		{
			GetOtherServerURL(&m_vecOtherSvrURL1, iServerNumInOtherServer[i], GD_BETACNT+SERVER_ZERO);
			DownloadWebFileInFolder(&m_vecCreateFileName, &m_vecOtherSvrURL1, SD_BETA, 3);
			CreateChangeDataTxtFile(&m_vecOtherFilePath, SD_BETA); //파일에서 읽고 변경할 값으로 변경한다. 
			//CreateTxtFileInFolder(&m_vecOtherSvrURL1, &m_vecOtherSvrURLData1, &m_vecCreateFileName, iServerNumInOtherServer[i], SD_BETA ); //값 변경
		}
		if(i == SERVER_ONE )
		{
			GetOtherServerURL(&m_vecOtherSvrURL2, iServerNumInOtherServer[i], GD_BETACNT+SERVER_ONE);
			DownloadWebFileInFolder(&m_vecCreateFileName, &m_vecOtherSvrURL2, SD_LIVE, 3);
			CreateChangeDataTxtFile(&m_vecOtherFilePath, SD_LIVE); //파일에서 읽고 변경할 값으로 변경한다. 
		}
		if(i == SERVER_THREE )
		{
			GetOtherServerURL(&m_vecOtherSvrURL3, iServerNumInOtherServer[i], GD_BETACNT+SERVER_TWO);
			DownloadWebFileInFolder(&m_vecCreateFileName, &m_vecOtherSvrURL3, SD_DEV, 3);
			CreateChangeDataTxtFile(&m_vecOtherFilePath, SD_DEV); //파일에서 읽고 변경할 값으로 변경한다. 
		}
	}
}

void CCRCManager::DownloadWebFileInFolder( pVecString vecPath, pVecString vecURL, int iType, int iTryCnt)
{
	m_vecOtherFilePath.clear();
	CString strFolderAddr = (*vecPath)[iType];
	Iter iter= (*vecURL).begin();
	HRESULT hr;
	CString str, strPath, strToken, strFileName, dwUrl, strBefore;
	str = (*iter);
	str.MakeReverse();
	AfxExtractSubString(strToken, str, 0, '/'); //'/'앞까지 버리기
	strToken.MakeReverse();

	for(int i =0 ; i < SERVER_MAX; ++i)
	{
		strBefore = strToken; //start.txt 파일이름 저장
		dwUrl = (*iter); //기존 url 받고
		strFileName = strFolderAddr;
		if(i != 0)
		{
			CString tStr;
			tStr.Format(L"%d", i); //1~2,3
			tStr +=".txt";
			strBefore.Replace(L".txt", tStr); //start1.txt 식으로 숫자 넣어주자
		}
		dwUrl.Replace(strToken, strBefore); //텍스트명 변경
		strFileName += strBefore;
		for(int j = 0 ; j < iTryCnt; ++j)
		{
			hr = URLDownloadToFile(NULL, dwUrl, strFileName, 0, NULL);
			if(hr == S_OK)
			{
				DeleteUrlCacheEntry(dwUrl);
				m_strLog = L"";
				m_strLog += strFileName;
				m_strLog += L"다운로드 완료!";
				PrintLog(&m_strLog);
				m_vecOtherFilePath.push_back(strFileName);
				break; //다운 받았으면 종료해잉!
			}
		}
		if(hr == E_FAIL)
			PrintLog("다운로드 실패 접속경로를 확인해주세요");
	}
}

void CCRCManager::PrintLog( CString* str )
{
	(*m_CRCDlg).PrintWorkLog(str);
}

void CCRCManager::PrintLog( char* str )
{
	CString Tstr(str);
	(*m_CRCDlg).PrintWorkLog(&Tstr);
}

void CCRCManager::CreateChangeDataTxtFile(pVecString vecPath , int iType)
{
	//변경할 CRC 데이터를 가져옵니다.
	CString strToken;
	Iter iterPath = (*vecPath).begin(); //읽어들이고 저장할 파일이름

	CStdioFile Txt_File;
	
	for(;iterPath < (*vecPath).end(); ++iterPath)
	{
		CString strSaveData;
		if(Txt_File.Open(*iterPath, CFile::modeRead | CFile::typeText))
		{
			CString strTXT;
			while(Txt_File.ReadString(strTXT))
			{
				if(strTXT.Find(L"autoupgrade.exe?") != -1) //문자열 찾았으니 시작
				{
					AfxExtractSubString(strToken, strTXT, 0, '?'); //?앞까지 버리기
					AfxExtractSubString(strToken, strTXT, 1, '?'); //특정 문자 자르기
					strTXT.Replace(strToken, GetCrcData(DI_AUTOEXE));
				}

				AfxExtractSubString(strToken, strTXT, 2, '?'); //?앞까지 버리기
				AfxExtractSubString(strToken, strTXT, 3, '?'); //특정 문자 자르기
				if(iType == SD_BETA) //문자열 찾았으니 시작
					strTXT.Replace(strToken, GetCrcData(DI_MGAMEINI));
				if(iType == SD_LIVE) //문자열 찾았으니 시작
					strTXT.Replace(strToken, GetCrcData(DI_AUTOINFO));
				strSaveData = strTXT;
			}	
		}
		Txt_File.Close();

		//파일을 만든다
		if(Txt_File.Open(*iterPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			Txt_File.WriteString(strSaveData);
		}
		Txt_File.Close();
	}
}


