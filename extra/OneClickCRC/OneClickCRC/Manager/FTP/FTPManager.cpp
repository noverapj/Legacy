commandMap
#include "StdAfx.h"
#include "FTPManager.h"

#include "../CRC/CRCManager.h"

CFTPManager *CFTPManager::g_Inst = NULL;

CFTPManager::CFTPManager(void) :
m_strWorkDir(_T("")), m_strDownBackUpPath(_T(""))
{
	m_pConnection = NULL;
	m_pFileFind	  = NULL;
	m_CRCDlg	  = NULL;
	m_pSession = NULL;
}


CFTPManager::~CFTPManager(void)
{
	if(m_pConnection != NULL)
		CloseConnection();

	if(m_pSession != NULL)
		m_pSession->Close();
}

CFTPManager* CFTPManager::GetInst()
{
	if(NULL == g_Inst)
		g_Inst = new CFTPManager;
	return g_Inst;
}

void CFTPManager::ReleaseInst()
{
	if(g_Inst)
		delete g_Inst;
	g_Inst = NULL;
}

void CFTPManager::ConnectProcess(CString* strip, CString* strid, CString* strpw)
{
	if(!AfxSocketInit())
		AfxMessageBox(_T("소켓 초기화 실패"));

	CString strRemoteDirTxt;
	if(m_pConnection != NULL)
		CloseConnection();

	m_pSession = new CInternetSession(_T("New Session"));

	try
	{
		m_pConnection = m_pSession->GetFtpConnection(*strip, *strid, *strpw);
	}
	catch (CInternetException* e)
	{
		e->ReportError();
		m_pConnection = NULL;
		m_pSession->Close();
		delete m_pSession ;
		e->Delete();
	}
	//현재 디렉토리를 얻음
	m_pConnection->GetCurrentDirectory(strRemoteDirTxt);

	if(m_pFileFind) delete m_pFileFind; //사용시 삭제

	CString strDir;
	m_pFileFind = new CFtpFileFind(m_pConnection);
	m_pFileFind->FindFile(strDir);

	GetCurrentObjectList(&m_vecDir);
	//디렉토리 경로를 확인합니다.
	if(!CheckDirName(&m_vecDir, L"autoupgrade", &m_strWorkDir))
		AfxMessageBox(_T("폴더 경로(autoupgrade)를 확인해 주세요!"));

	//디렉토리 이동
	MoveDir(&m_strWorkDir);
	GetCurrentObjectList(&m_vecFile); //디렉토리 내 파일 목록 확인

	//백업하자 백업하자
	CreateDownloadDir();
	DownloadDataInFolder(&m_strDownBackUpPath, &m_strWorkDir, &m_vecFile);

	//변경할 CRC 데이터를 가져옵니다.
	CString CrcEXEData = CCRCManager::GetInst()->GetCrcData(DI_AUTOEXE); 
	CString CrcINFOData = CCRCManager::GetInst()->GetCrcData(DI_MGAMEINI); 
	//int		iChange = CCRCManager::GetInst()->GetChangeDataType(); //DI_AUTOEXE ~ DI_MGAMEINI
	ChangeMyCrcDataInList(&m_strDownBackUpPath, &m_vecFile, CrcEXEData, CrcINFOData); //autoexe값을 변경
	//업로드를 하자
	//업로드 경로는 백업하기 위해 지정된 AutoUpgrade\ 폴더에 덮어쓰기 됩니다.
	UploadFileNPath(m_pvecUpLoadList, &m_strWorkDir);
}

//vec에서 txt파일을 검사하여 분류하고, 
void CFTPManager::ChangeMyCrcDataInList( CString* sPath, pVecString vecFileList, CString strExe ,CString strInfo)
{
	//check Txt data
	Iter iterList = vecFileList->begin();
	std::vector<int> vecINum; //txt배열 정보 얻기
	int iType = TT_START;
	for(int i = 0; iterList <  (vecFileList->end()); ++iterList)
	{
		if((*iterList).Find(L".txt")!= -1) //vec에서 txt넘버를 얻는다.
			vecINum.push_back(i);
		if((*iterList).Find(L"mgame")!= -1) //내가 다운받은 자료는 모두 mgame.txt일것이다.
			iType = TT_MGAME;
		++i;
	}
	ChangeTXTdata(iType, vecFileList, &vecINum, strExe, strInfo, sPath);
}

void CFTPManager::ChangeTXTdata( int itype, pVecString veclist, std::vector<int>* vecInt, CString strExe, CString strInfo, CString* sPath)
{
	CString strPath = *sPath;
	Iter	iter = veclist->begin();
	int iMax = (*vecInt).size();
	if(itype == TT_MGAME) //mgame.txt에서
	{
		for(int j = 0 ; j < iMax ; ++j)
		{
			CString str;
			str = strPath;
			int iNum = (*vecInt)[j]; //벡터 결과 가져오고
			str += *(iter+iNum);	 //주소 값 가져오기
			ChangeCRCData(strExe, strInfo,str);
		}
	}
 	else if(itype == TT_START)
		AfxMessageBox(_T("start.txt 변경은 지원하지 않습니다."));
}

//결과 값을 변경합니다.
void CFTPManager::ChangeCRCData(CString strExe, CString strInfo, CString strPath )
{
	CString tFindKey;
// 	switch(itype) //변경할 데이터를 찾자~
// 	{
// 	case DI_AUTOEXE :
// 		tFindKey = "autoupgrade.exe?";
// 		break;
// 	case DI_MGAMEINI :
// 		tFindKey = "autoupgrade_info_mgame.ini?";
// 		break;
// 	case DI_AUTOINFO :
// 		tFindKey = "autoupgrade_info.ini?";
// 		break;
// 	}

	CString strWriteData;
	CStdioFile Txt_File;
	if(Txt_File.Open(strPath, CFile::modeRead |CFile::typeText))
	{
		CString str, strToken;
		while(Txt_File.ReadString(str))
		{
			//if(str.Find(tFindKey) != -1)
			{
				AfxExtractSubString(strToken, str, 0, '?'); //?앞까지 버리기
				AfxExtractSubString(strToken, str, 1, '?'); //특정 문자 자르기
				str.Replace(strToken, strExe);
				AfxExtractSubString(strToken, str, 2, '?'); //?앞까지 버리기
				AfxExtractSubString(strToken, str, 3, '?'); //특정 문자 자르기
				str.Replace(strToken, strInfo);
				strWriteData = str;
			}
		}
	}
	Txt_File.Close();
	CString cstr;
	strPath.MakeReverse();
	AfxExtractSubString(cstr, strPath, 0, '\\'); //?앞까지 버리기
	AfxExtractSubString(cstr, strPath, 1, '\\'); //특정 문자 자르기
	m_strChangeFolderName.MakeReverse();
	strPath.Replace(cstr, m_strChangeFolderName);
	m_strChangeFolderName.MakeReverse();
	strPath.MakeReverse();
	m_vecUpnDelList.push_back(strPath);
	if(Txt_File.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		Txt_File.WriteString(strWriteData);
	}
	Txt_File.Close();
}

void CFTPManager::CloseConnection()
{
	m_pConnection->Close();
	delete m_pConnection;
	m_pConnection = NULL;
}

void CFTPManager::GetCurrentObjectList( pVecString vec )
{
	if((*vec).empty()) //있으면 지워라~
		(*vec).clear();


	CString strFileName;
	BOOL	bFind = TRUE;
	while(bFind)
	{
		if(!m_pFileFind->FindNextFile())
			bFind = FALSE;
		strFileName = m_pFileFind->GetFileName();
		(*vec).push_back(strFileName);

	}
	return;
}

BOOL CFTPManager::CheckDirName( pVecString vec, CString sName, CString* strSetPath )
{
	Iter iter = vec->begin();
	CString tStr;
	for( ; iter < (vec->end()); ++iter)
	{
		tStr = (*iter);
		if(tStr.MakeUpper()== sName.MakeUpper())
		{
			*strSetPath = *iter;
			return TRUE;
		}
	}
	return FALSE;
}

void CFTPManager::MoveDir( CString* strPath )
{
	CString strTmpDir, strDir;
	strTmpDir += *strPath;
	strTmpDir += "\\";
	BOOL bSucess = m_pConnection->SetCurrentDirectory(strTmpDir);
	m_pFileFind->FindFile(strDir);
}

void CFTPManager::DownloadDataInFolder( CString* strDownLocalPath, CString* strRemotePath, pVecString vecDonwList )
{
	CString TempRemotePath;
	TempRemotePath += '\\';
	TempRemotePath += *strRemotePath;
	TempRemotePath += '\\';

	Iter iter = (*vecDonwList).begin();
	for(; iter < ((*vecDonwList).end()); ++iter)
	{
		m_pConnection->GetFile(TempRemotePath + (*iter), *strDownLocalPath+ (*iter), FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD | NULL);
	}
}

void CFTPManager::SetUseData( pVecString vec, CString* str )
{
	m_pvecUpLoadList = vec;
	m_pDownLoadPath = str;
}

void CFTPManager::CreateDownloadDir()
{
	CString strDate, strToken, strMakeDirName;
	COleDateTime NowDate;
	CFileFind CFileFinder;
	NowDate = COleDateTime::GetCurrentTime();

	strDate.Format( L"%04d-%02d-%02d|%02d:%02d:%02d"
		, NowDate.GetYear()
		, NowDate.GetMonth()
		, NowDate.GetDay()
		, NowDate.GetHour()
		, NowDate.GetMinute()
		, NowDate.GetSecond()
		);

	AfxExtractSubString(strToken, strDate, 0, '|'); //날짜까지 자름
	strMakeDirName = strToken;
	strMakeDirName += '_';
	strMakeDirName += m_strWorkDir;
	strMakeDirName += "_Backup";

	CString str = *m_pDownLoadPath; //경로 받아오자잉
	m_strDownBackUpPath = str;
	m_strDownBackUpPath += '\\';
	m_strDownBackUpPath += strMakeDirName;
	if(!CFileFinder.FindFile(m_strDownBackUpPath)) //디렉토리가 음슴
		CreateDirectory(m_strDownBackUpPath, NULL);
	m_strDownBackUpPath += '\\';
}

void CFTPManager::UploadFileNPath( pVecString vec, CString* strPath )
{
	Iter Upiter = (*m_pvecUpLoadList).begin();
	for(;Upiter < (*m_pvecUpLoadList).end(); ++Upiter)
		m_vecUpnDelList.push_back(*Upiter);
	Iter iter = m_vecUpnDelList.begin();
	for(;iter < m_vecUpnDelList.end(); ++iter)
	{
		CString tStr;
		(*iter).MakeReverse();
		AfxExtractSubString(tStr, (*iter), 0, '\\'); //?앞까지 버리기
		tStr.MakeReverse();
		tStr += L"test"; //임시 세팅용 데이터
		(*iter).MakeReverse();	
		m_pConnection->PutFile((*iter), tStr, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD, 0);
	}
}
