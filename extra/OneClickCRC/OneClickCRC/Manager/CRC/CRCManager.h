#pragma once
#include <vector>


typedef std::vector<CString> VecString;
typedef std::vector<CString>* pVecString;
typedef std::vector<CString>::iterator Iter;
typedef std::vector<CString>::const_iterator CIter;

class OneCRCDlg;
class CCRCManager
{
	enum	WORK_PROC		{ _FILECHECK, _COPYnRUN, _MAKEFILE,_WORKCOMPLETE };
	enum	MESSAGE_CODE	{ MC_FILE, MC_URL, MC_INTERNET, MC_ALERT};
	enum	GET_DATA		{ GD_INIPATH, GD_SERVERCNT,GD_AUTOEXEPATH, GD_CRCPATH, GD_CPYFOLDER, GD_CHAGEDATA,GD_URL, GD_IP, GD_ID, GD_PW,GD_LOCAL, 
						GD_OTHERSVRCNT, GD_BETACNT, GD_LIVECNT , GD_DEVCNT, GD_BETAURL, GD_LIVEURL};
	enum	SERVER_ID		{ SERVER_ZERO, SERVER_ONE, SERVER_TWO, SERVER_THREE ,SERVER_MAX};
	enum	SERVER_DATA		{ SD_ALPHA, SD_BETA, SD_LIVE ,SD_DEV};
	
#define CRC_EXE_FILE_NAME	"CrcReader.exe" //실행파일
#define CRC_TXT_FILE_NAME	"Crc.txt"		//삭제파일

public:
	static CCRCManager*		GetInst();		
	static void				ReleaseInst();	
	
	void					RunProcess();
	CString					GetCrcData(int iDataType);
	//int						GetChangeDataType() { return m_iChangeDateType; };
	void					SetDlg(OneCRCDlg* dlg) { m_CRCDlg = dlg; };				//SET
	void					OtherServerProcess();
	CCRCManager(void);
	virtual ~CCRCManager(void);
private:
	//함수
	void					PrintMessage(int iType, char* ctext);					//텍스터 출력
	void					ChangeFontString(TCHAR* pSour, char* pDest);			//문자열 변경
	void					CopyFilePath(CString* const strSour, char* ctext);		
	void					RunCRCnDelete();
	void					WaitTime( DWORD dwMilliSec );							//타이머
	void					AddStringToChar(CString* sSour, char* ctext);
	void					LoadCRCTxtData(CString* sSour);							//CRC 데이터 로드
	void					FindWindowNClose(char* ctext);							//창 찾아 닫기
	void					DeletePathInFile(CString* sPath, char* ctext);			//파일을 삭제하자
	//CHECK
	BOOL					CheckFileInFolder(CString* const strPath, char* ctext ); //파일   체크
	void					CheckServerInfo();	//데이터를 생성할 서버에 갯수를 파악합니다.
	//CREATE
	void					CreateExportFolder();									 //Output\출력폴더 생성
	void					MakeFolder(CString* strPath, CString* StrName);			 //출력폴더 경로 설정

	//GET
	void					LoadInfoINI();
	void					GetINIData(CString* const FilePath, char* SourFile, int iType, CString* DestData);
	void					GetINIData(CString* const FilePath, char* SourFile, int iType, std::vector<CString>* DestData, int ServerCnt);
	void					GetINIData(std::vector<CString>* vecAddr, char* SourFile, int iType, std::vector<CString>* DestData, int ServerCnt);

	//
	int						GetOtherServerCnt(int iOther); //씀
	void					GetOtherServerURL(pVecString vec, int iCnt, int iSvrName); //씀
	void					GetURLData(CString* sPath, char* SourFile, pVecString pDest, int iSvrNum, int iSvrName); //안씀
	void					GetSourceURLData(pVecString vecURL, pVecString vecDest, int iCnt);
	void					GetSourceOfURL(CString urlAddr);
	void					ChangeURLData(pVecString vUrlData, int iChangeType, int iSvrCnt, int iChangeValue);
	void					CreateTxtFileInFolder(pVecString vURLAddr, pVecString vURLData, pVecString vOutputPath, int iSvrCnt, int iType);

	//새로운 other 서버 관련 작업
	void					DownloadWebFileInFolder(pVecString vecPath, pVecString vecURL, int iType, int iTryCnt);
	void					PrintLog(CString* str);
	void					PrintLog( char* str );
	void					CreateChangeDataTxtFile(pVecString vecPath, int iType);
//변수
	static CCRCManager		*g_Inst; //인수
	int						m_iProcessStep;
	//CString					m_strChangeDataName;	//변경할 데이터 이름
	CString					m_strInfoIniPath;		//D:\Jal_overseas\Indonesia\Tool\OneclickCRC\bin
	CString					m_strAutoExeFolderAddr;
	CString					m_strCopyFolderAddr;
	CString					m_strCrcReaderAddr;
	CString					m_strExportTxtAddr;	//D:\Jal_overseas\Indonesia\Tool\OneclickCRC\Output
	VecString				m_vecOutFolderName;	//출력 폴더 명 저장
	VecString				m_vecINIAddrData;	//INI 파일경로 저장
	VecString				m_vecURLAddr;		//URL에 접속하기 위한 주소 정보
	VecString				m_vecURLData;		//URL에 접속하여 가져온 데이터
	VecString				m_vecCreateFileName;//생성 파일 이름
	VecString				m_vecINIFolderAddr; //INI 폴더 경로 저장
	VecString				m_vecAutoEXE;		//URL에서 읽은 정보
	VecString				m_vecAutoInfo;		//URL에서 읽은 정보
	VecString				m_vecMGameInfo;		//URL에서 읽은 정보
	VecString				m_vecUploadlist;	//업로드할 파일(경로)

	int						m_iServerCnt;		//확인된 서버 갯수
	int						m_iConnectCnt;		//접속 카운트
	OneCRCDlg*				m_CRCDlg;
	CString					m_strIP;			//FTP 접속 정보
	CString					m_strID;			//FTP 접속 정보
	CString					m_strPW;			//FTP 접속 정보
	//int						m_iChangeDateType; //다운받은 txt파일에서 변경할 타입을 확인합니다.
	VecString				m_vecOtherSvrURL1; //베타
	VecString				m_vecOtherSvrURL2; //라이브
	VecString				m_vecOtherSvrURL3; //나중에 쓸거
	VecString				m_vecOtherSvrURLData1;
	VecString				m_vecOtherSvrURLData2;
	VecString				m_vecOtherSvrURLData3;
	VecString				m_vecOtherFilePath;
	CString					m_strLog;

};

