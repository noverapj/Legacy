#pragma once


#include <vector>
#include <afxinet.h>	//ftp 를 위해 추가해야됨
#include <afxsock.h>

typedef std::vector<CString> VecString;
typedef std::vector<CString>* pVecString;
typedef std::vector<CString>::iterator Iter;
typedef std::vector<CString>::const_iterator CIter;


class OneCRCDlg;
class CFTPManager
{
	enum	TXT_TYPE		{ TT_START, TT_MGAME };
public:
	static CFTPManager*		GetInst(); //그닥 필요없지만 싱글톤..
	static void				ReleaseInst();
	CFTPManager(void);
	virtual ~CFTPManager(void);
	void					SetDlg(OneCRCDlg* dlg) { m_CRCDlg = dlg; };//SET
	void					SetUseData(pVecString vec, CString* str);
	void					SetUploadList(pVecString vec) {m_pvecUpload = vec; } ;
	void					setWorkDirPath(pVecString vec) { m_pvecWorkDirPath = vec; };
	void					ConnectProcess(CString* strip, CString* strid, CString* strpw);
	void					DownDataChangeFolder(CString* str) { m_strChangeFolderName = *str; };
private:
	void					ChangeCRCData(CString strExe, CString strInfo, CString strPath);
	void					ChangeTXTdata(int itype, pVecString veclist, std::vector<int>* vecInt, CString strExe, CString strInfo,CString* sPath);
	void					CloseConnection();
	void					GetCurrentObjectList(pVecString vec);
	void					MoveDir(CString* strPath);
	void					CreateDownloadDir();
	void					DownloadDataInFolder(CString* strDownLocalPath, CString* strRemotePath, pVecString vecDonwList);
	BOOL					CheckDirName(pVecString vec, CString sName, CString* strSetPath);
	void					UploadFileNPath(pVecString vec, CString* strPath);
	void					ChangeMyCrcDataInList(CString* sPath, pVecString vecFileList, CString strExe ,CString strInfo);
	
	static CFTPManager		*g_Inst; //인수

	//추가내용
	CInternetSession	*m_pSession;
	CFtpConnection		*m_pConnection;
	CFtpFileFind		*m_pFileFind;
	VecString			m_vecDir;
	CString				m_strWorkDir;
	VecString			m_vecFile;
	pVecString			m_pvecUpLoadList;
	pVecString			m_pvecWorkDirPath;
	pVecString			m_pvecUpload;
	CString*			m_pDownLoadPath;
	CString				m_strDownBackUpPath;
	OneCRCDlg*			m_CRCDlg;
	VecString			m_vecUpnDelList;
	CString				m_strChangeFolderName;
};

