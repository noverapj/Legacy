// ServerDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "cFile.h"
#include "LS_launcher.h"
#include "ServerDialog.h"
#include "InstallDialog.h"
#include "BackupDialog.h"
#include "cPacket.h"
#include "Server.h"
#include "WinMessage.h"
#include "MD5.h"
#include "Timer.h"
#include "../common/Protocol.h"
#include "LauncherTabCtrl.h"
#include "LS_launcherDlg.h"
#include "PerfMonDlg.h"



extern BOOL SelectFolder(HWND hWnd, CString &folder);


// CServerDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerDialog, CDialog)

CServerDialog::CServerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDialog::IDD, pParent), m_socket(NULL), m_sequence(0), m_launcherDlg(NULL), m_perfMonDlg(NULL),	m_syncComplete(TRUE), m_ready(FALSE)
	
{
}

CServerDialog::~CServerDialog()
{
}

void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NAME, m_nameStatic);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_status);
	DDX_Control(pDX, IDC_STATIC_LOG, m_log);
	DDX_Control(pDX, IDC_LIST_SERVICE, m_services);
	DDX_Control(pDX, IDC_PROGRESS_TRANSFER, m_progressBar);
	DDX_Control(pDX, IDC_STATIC_CPU, m_barCPU);
	DDX_Control(pDX, IDC_STATIC_RAM, m_barRAM);
	DDX_Control(pDX, IDC_STATIC_UPLOAD, m_barUpload);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD, m_barDownload);
	DDX_Control(pDX, IDC_STATIC_DISK, m_barDisk);
	DDX_Control(pDX, IDC_STATIC_DISK2, m_barDisk2);
}


BEGIN_MESSAGE_MAP(CServerDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CServerDialog::OnBnClickedButtonUpdate)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_MESSAGE(IDC_CONNECT, OnConnect)
	ON_MESSAGE(IDC_DISCONNECT, OnDisconnect)
	ON_MESSAGE(IDC_RECEIVE, OnReceive)
	ON_MESSAGE(IDC_SERVICE_PLAY, OnPlay)
	ON_MESSAGE(IDC_SERVICE_STOP, OnStop)
	ON_MESSAGE(IDC_SERVICE_PLAY_CONSOLE, OnPlayConsole)
	ON_MESSAGE(IDC_SERVICE_STOP_CONSOLE, OnStopConsole)
	ON_MESSAGE(IDC_SERVICE_UNINSTALL, OnUninstall)
	ON_MESSAGE(WM_AUTOMATIC,OnAutomatic)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CServerDialog::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_PLAYALL, &CServerDialog::OnBnClickedButtonPlayAll)
	ON_BN_CLICKED(IDC_BUTTON_STOPALL, &CServerDialog::OnBnClickedButtonStopAll)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CServerDialog::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &CServerDialog::OnBnClickedButtonBackup)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CServerDialog 메시지 처리기입니다.


void CServerDialog::Init(CString& title, CString& IP, UINT port, RECT pos)
{
	m_name		= title;
	m_tapRect	= pos;
	m_send.Resize(MAX_TRANSFER_SIZE*2);

	CreatePopup();
	CreateSocket(IP, port);

	InitControls();
	SetAuto(FALSE);

	m_msgbox.SetParent(this);

	m_launcherDlg = (CLS_launcherDlg*)GetParent()->GetParent();
	m_perfMonDlg = new CPerfMonDlg(NULL);
	m_perfMonDlg->Create(IDD_PERFMONDLG, NULL);
}

void CServerDialog::InitControls()
{
	
	m_statusIcon[STATUS_ON] = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_ON), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	m_statusIcon[STATUS_OFF] = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_OFF), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	
	m_status.SetBitmap(m_statusIcon[STATUS_OFF]);
	m_nameStatic.SetWindowText(_T("not connected"));

	// 리스트 컨트롤
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_ALL);
	m_imageList.Create(19, 18, ILC_COLOR24 | ILC_MASK, 4, 4);
	m_imageList.Add(&bmp, RGB(100, 100, 100));
	bmp.DeleteObject();

	m_services.CreatePopup();
	m_services.SetImageList(&m_imageList, LVSIL_SMALL);
	m_services.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_FULLROWSELECT);
	m_services.InsertColumn(0, _T("Service name"), LVCFMT_LEFT, 167);
	m_services.InsertColumn(1, _T("Run type"), LVCFMT_LEFT, 70);

	// CPU ProgressBar
	CRect rc;
	GetDlgItem(IDC_STATIC_CPU)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ctlCPU.Create(WS_VISIBLE | WS_CHILD, rc, this); 
	m_ctlCPU.SetRange( 0, 100 );
	m_ctlCPU.InitToolTip();

	// RAM ProgressBar
	GetDlgItem(IDC_STATIC_RAM)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ctlRAM.Create(WS_VISIBLE | WS_CHILD, rc, this); 
	m_ctlRAM.SetRange( 0, 100 );
	m_ctlRAM.InitToolTip();

	// Upload ProgressBar
	GetDlgItem(IDC_STATIC_UPLOAD)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ctlUpload.Create(WS_VISIBLE | WS_CHILD, rc, this);
	m_ctlUpload.SetRange( 0, 1024 );
	m_ctlUpload.InitToolTip();

	// Download ProgressBar
	GetDlgItem(IDC_STATIC_DOWNLOAD)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ctlDownload.Create(WS_VISIBLE | WS_CHILD, rc, this);
	m_ctlDownload.SetRange( 0, 1024 );
	m_ctlDownload.InitToolTip();

	// Disk ProgressBar
	GetDlgItem(IDC_STATIC_DISK)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ctlDisk.Create(WS_VISIBLE | WS_CHILD, rc, this);
	m_ctlDisk.SetRange( 0, 100 );
	m_ctlDisk.InitToolTip();

	GetDlgItem(IDC_STATIC_DISK2)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ctlDisk2.Create(WS_VISIBLE | WS_CHILD, rc, this);
	m_ctlDisk2.SetRange( 0, 100 );
	m_ctlDisk2.InitToolTip();
}

void CServerDialog::CreatePopup()
{
	// 팝업메뉴
	m_menu.DestroyMenu();
	m_menu.CreatePopupMenu();

	CString menu;

	//menu = _T("Monitor"); 2012-10-05 youngdie, 글로벌로 변경
	//m_menu.AppendMenu(MF_STRING, IDC_SERVICE_MONITOR, (LPCTSTR)menu);

	//menu = _T("Refresh(A)"); 2012-10-05 youngdie, 자동이나 F5키로 변경
	//m_menu.AppendMenu(MF_STRING, IDC_SERVICE_REFRESH, (LPCTSTR)menu);

	//m_menu.AppendMenu(MF_SEPARATOR);

	menu = _T("File Sync(A)");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_FILESYNC, (LPCTSTR)menu);

	menu = _T("File Remove");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_FILEREMOVE, (LPCTSTR)menu);

	menu = _T("File Backup");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_FILEBACKUP, (LPCTSTR)menu);

	m_menu.AppendMenu(MF_SEPARATOR);

	menu = _T("Play All(A)");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_PLAYALL, (LPCTSTR)menu);

	menu = _T("Stop All(A)");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_STOPALL, (LPCTSTR)menu);

	m_menu.AppendMenu(MF_SEPARATOR);

	menu = _T("Install");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_INSTALL, (LPCTSTR)menu);

	menu = _T("Reboot(A)");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_REBOOT, (LPCTSTR)menu);
}

BOOL CServerDialog::CreateSocket(CString& IP, UINT port)
{
	if(!m_socket)
	{
		m_socket = new CClientSocket(this->GetSafeHwnd(), IP, port);
	}

	if(!m_socket->Create())
	{
		return FALSE;
	}

	m_sequence = 0;
	return m_socket->Connect();
}

BOOL CServerDialog::CloseSocket()
{
	if(m_socket)
	{
		m_socket->Close();
		delete m_socket;
		m_socket = NULL;
	}

	m_sequence = 0;
	return TRUE;
}

BOOL CServerDialog::IsConnected()
{
	if(m_socket && m_socket->IsConnected())
	{
		return TRUE;
	}
	return FALSE;
}

void CServerDialog::SendPacket(const UINT command, const void* buffer, const UINT length)
{
	if(IsConnected())
	{
		if(m_sequence >= 0xff) m_sequence = 0;
		++m_sequence;

		if(!m_packet.Make(command, reinterpret_cast<const uint8*>(buffer), length, m_sequence))
		{
			MessageBox(_T("Network error"), _T("error"), MB_OK);
			return;
		}

		uint8* pkBuffer = m_packet.GetBuffer();
		if(NULL == pkBuffer)
		{
			MessageBox(_T("Packet Buffer Error"), _T("error"), MB_OK);
			return ;
		}

		if(NULL == m_socket)
		{
			MessageBox(_T("socket Error"), _T("error"), MB_OK);
			return ;
		}

		uint32	nLength = m_packet.GetLength();
		int bytes = 0, ret = 0;
		while(m_packet.GetLength() != bytes)
		{
			ret = m_socket->Send(pkBuffer+bytes, nLength-bytes);
			if(SOCKET_ERROR == ret)
			{
				if(WSAEWOULDBLOCK == GetLastError())
				{
					continue;
				}
				break;
			}
			else
			{
				bytes += ret;
			}
		}
	}
}

void CServerDialog::Reset()
{
	TRACE("Reset\n");
	m_files.RemoveAll();
	m_folders.RemoveAll();
	m_fileIndex		= 0;
	m_fileOffset	= 0;
	m_fileCount		= 0;
	m_begin			= GetTickCount();
}

void CServerDialog::Next(BOOL success)
{
	if(success)
		++m_fileCount;

	m_files.RemoveAt(0);
	m_folders.RemoveAt(0);
	m_fileIndex		= 0;
	m_fileOffset	= 0;

	int current = m_maxCount - m_files.GetCount()+1;
	if( IsSyncComplete() )
	{
		int percentage = current * 100 / m_maxCount;
		m_launcherDlg->BatchProgress(percentage);

		m_progressBar.SetPos(current);
	}
	else
	{
		int percentage = current * 100 / m_maxCount;
		m_launcherDlg->BatchProgress(percentage);

		m_progressBar.SetPos(current);
	}
}

BOOL CServerDialog::TransferFile()
{
	TRACE("TransferFile\n");
	if(0 == m_fileIndex)
	{
		// 파일전송 시작
		return TransferBegin();
	}
	//else if(1 == m_fileIndex)
	//{
	//	return TransferDo();
	//}
	else
	{
		return TransferDo();
	}
	return FALSE;
}

BOOL CServerDialog::TransferBegin()
{
	//TRACE("TransferBegin\n");
	if(m_files.GetCount() == 0)
	{
		if( IsSyncComplete() )	// SyncCommand일 경우엔 FALSE이다
		{
			DWORD elapse = GetTickCount() - m_begin;
			Log(_T("total : %lu,  elapsed : %lus"), m_fileCount, elapse / 1000);
			m_msgbox.MessageBox(_T("File sync finished"),_T("Lostsaga"),100,MB_OK | MB_ICONINFORMATION);
			//이때 다른 다이얼로그들에게 전송
			SetAuto(TRUE);
			FOLDERSTRING *sttmp = new FOLDERSTRING;
			sttmp->m_files.Copy( m_stlocal.m_files);
			sttmp->m_folder = m_stlocal.m_folder;
			sttmp->m_folders.Copy(m_stlocal.m_folders);
			SendMsgDlg(*sttmp);
		}
		else
		{
			SyncCommandEnd();
		}
		return FALSE;
	}

	TCHAR fileName[64], folderName[256];

	_stprintf_s(fileName, _countof(fileName), _T("%s"), m_files[0]);	
	_stprintf_s(folderName, _countof(folderName), _T("%s"), m_folders[0]);

	m_filePath.Format(_T("%s\\%s"), folderName, fileName);
	m_fileName = fileName;

	csTransferBegin packet;
	cFileReader file;

	if( file.Open(m_filePath) )
	{
		if( IsSyncComplete() )
		{
			OPTION()->WriteLog(_T("TRANSFER Staret PC:%s FileName:%s"), m_name.GetBuffer(), m_fileName.GetString());
			Log(_T("Transfer - %s"), fileName);
		}
		else
		{
			OPTION()->WriteLog(_T("TRANSFER BATCH:Staret PC:%s FileName:%s"), m_name.GetBuffer(), m_fileName.GetString());
			// Batch 메시지
			Log(_T("BATCH::Transfer - %s"), fileName);
		}

		m_fileLength = file.GetFileSize();

		// 버퍼 설정
		m_buffer.Erase();
		m_buffer.Resize(m_fileLength);
		if(!file.Read(m_buffer.GetBuffer(), m_fileLength))
		{
			file.Close();
			MessageBox(_T("File read failed"), _T("error"), MB_OK);
			return FALSE;
		}
		m_buffer.SetLength(m_fileLength);

		file.Close();

		CMD5 MD5;
		MD5.Generate(m_buffer.GetBuffer(), m_buffer.GetLength());

		//folderName[0] = 'C';
		if(!m_root.IsEmpty())
		{
			CString temp = folderName;
			temp.Replace(m_folder, m_root);
			_tcscpy_s(folderName, _countof(folderName), temp);
		}

		ZeroMemory(&packet, sizeof(packet));
		packet.type		= TRANSFER_BEGIN;
		packet.index	= m_fileIndex++;
		packet.length	= m_fileLength;
		CopyMemory(packet.file, fileName, sizeof(packet.file));
		CopyMemory(packet.folder, folderName, sizeof(packet.folder));
		CopyMemory(packet.digest, MD5.GetDigest(), sizeof(packet.digest));

		SendPacket(C_TRANSFER, &packet, sizeof(csTransferBegin));
		return TRUE;
	}
	return FALSE;
}

BOOL CServerDialog::TransferDo()
{
	//TRACE("TransferDo\n");
	UINT size = MAX_TRANSFER_SIZE;
	UINT remain = (m_buffer.GetLength() > m_fileOffset) ? (m_buffer.GetLength() - m_fileOffset) : 0;
	if(0 == remain)
	{
		TransferEnd();
		return TRUE;
	}

	if(remain < MAX_TRANSFER_SIZE)
	{
		size = remain;
	}

	double percentage = (m_fileOffset * 100.0 / m_buffer.GetLength());
	
	if( IsSyncComplete() )
	{
		Log(_T("Transfer - %s(%0.1lf%%)"), m_fileName, percentage);
	}
	else
	{
		Log(_T("BATCH::Transfer - %s(%0.1lf%%)"), m_fileName, percentage);
	}

	csTransferDo packet;
	packet.type		= TRANSFER_DO;
	packet.index	= m_fileIndex++;
	packet.length	= size;

	m_send.Erase();
	m_send.Append(reinterpret_cast<uint8*>(&packet), sizeof(csTransferDo));
	m_send.Append(m_buffer.GetBuffer(m_fileOffset), size);
	m_fileOffset += size;

	SendPacket(C_TRANSFER, m_send.GetBuffer(), m_send.GetLength());
	return TRUE;
}

BOOL CServerDialog::TransferEnd()
{
	OPTION()->WriteLog(_T("TRANSFER SUCCESS PC:%s FileName:%s"), m_name.GetBuffer(), m_fileName.GetString());

	//TRACE("TransferEnd\n");
	csTransferEnd packet;
	packet.type		= TRANSFER_END;
	packet.index	= m_fileIndex++;
	packet.length	= m_buffer.GetLength();
	SendPacket(C_TRANSFER, &packet, sizeof(csTransferEnd));

	Next();
	return TRUE;
}

void CServerDialog::OnBnClickedButtonRefresh()
{
	if(!IsConnected())
	{
		Log(_T("error - server is closed"));
		return;
	}

	// 서비스목록 요청
	RequestServices();
	SendMsgDlg(ECOMMAND::REFRESH);
}

void CServerDialog::OnBnClickedButtonUpdate()
{
	RequestFileSync();
}

void CServerDialog::RequestServices()
{
	SendPacket(C_SERVICES, NULL, 0);
}

void CServerDialog::RequestPeformances()
{
	SendPacket(C_RESOURCE, NULL, 0);
}

void CServerDialog::RequestReboot()
{
	int result = MessageBox(_T("Are you sure? Reboot"), _T("Question"), MB_YESNO);
	if(IDYES == result)
	{	
		csReboot packet;
		packet.option = 5; // 5sec

		SendPacket(C_REBOOT, &packet, sizeof(csReboot));

		Log(_T("Reboot..."));

		SendMsgDlg(ECOMMAND::REBOOT);
	}
}

void CServerDialog::RequestInstall()
{
	if(IsConnected())
	{
		CInstallDialog dlg(m_folder);
		if(IDOK == dlg.DoModal())
		{
			CString service = dlg.GetService();
			CString demon = dlg.GetDemon();
			CString script = dlg.GetScript();

			if(service.IsEmpty()) return;
			if(demon.IsEmpty()) return;
			if(script.IsEmpty()) return;

			csInstall packet;
			packet.install = TRUE;
			CopyMemory(packet.service, service, service.GetLength()*sizeof(TCHAR));
			CopyMemory(packet.file, demon, demon.GetLength()*sizeof(TCHAR));
			CopyMemory(packet.script, script, script.GetLength()*sizeof(TCHAR));

			SendPacket(C_INSTALL, &packet, sizeof(csInstall));
		}
	}
}

void CServerDialog::RequestPlayAll()
{
	int result = MessageBox(_T("Are you sure to play all?"), _T("Question"), MB_YESNO);
	if(IDYES == result)
	{	
		TCHAR serviceName[64];
		for(int i = 0 ; i < m_services.GetItemCount() ; i++)
		{
			if(m_services.GetItemText(i, 0, serviceName, _countof(serviceName)))
			{
				ServicePlay(serviceName, OPTION()->GetOption(OPTION_CONSOLE));
			}
		}

		Log(_T("Play all services"));
		
		SendMsgDlg(ECOMMAND::PLAYALL);
	}
}

void CServerDialog::RequestStopAll()
{
	int result = MessageBox(_T("Are you sure to stop all?"), _T("Question"), MB_YESNO);
	if(IDYES == result)
	{	
		//postmessage 부분 
		TCHAR serviceName[64];
		for(int i = 0 ; i < m_services.GetItemCount() ; i++)
		{
			if(m_services.GetItemText(i, 0, serviceName, _countof(serviceName)))
			{
				ServiceStop(serviceName, OPTION()->GetOption(OPTION_CONSOLE));
			}
		}

		Log(_T("Stop all services"));
		SendMsgDlg(ECOMMAND::STOPALL);
	}
}

void CServerDialog::RequestFileSync()
{
	/*if( IsSyncComplete() )
	{
		// command중일땐 실행하지 않는다.
		return ;
	}*/
	
	if(OPTION()->GetOption(OPTION_APPLYALL) == TRUE)
	{
		CLauncherTabCtrl* dlg = (CLauncherTabCtrl*)GetParent();
		for(unsigned int i=0; i < dlg->m_dialogs.size();++i)
		{
			dlg->m_dialogs.at(i)->SetAuto( FALSE );
		}

		if(SelectFolder(this->GetSafeHwnd(), m_folder))
		{
			SetCurrentDirectory(m_folder);
			Reset();
			FindFiles(m_folder, m_files, m_folders);
			m_stlocal.m_folder = m_folder;
			m_stlocal.m_folders.Copy(m_folders);
			m_stlocal.m_files.Copy(m_files);
		}

		FOLDERSTRING *sttmp = new FOLDERSTRING;
		sttmp->m_files.Copy( m_stlocal.m_files);
		sttmp->m_folder = m_stlocal.m_folder;
		sttmp->m_folders.Copy(m_stlocal.m_folders);
		SendMsgDlg(*sttmp);
		return;
	}
		 
	if(!IsConnected())
	{
		Log(_T("error - server is closed"));
		return;
	}

	// 서버 업데이트
	UpdateRoot();
	if(SelectFolder(this->GetSafeHwnd(), m_folder))
	{
		SetCurrentDirectory(m_folder);

		Reset();

		FindFiles(m_folder, m_files, m_folders);
		m_stlocal.m_folder = m_folder;
		m_stlocal.m_folders.Copy(m_folders);
		m_stlocal.m_files.Copy(m_files);
		//kyg m_folder,files,foldes 모두 복사하여 다른 다이얼로그에 전달 해야됨 
		//다이얼로그는 이 상위 다이얼로그에서 포인터 얻어와서 postmessage 하면 될듯 
		//해당 기능들은 
		m_maxCount = static_cast<UINT>(m_files.GetCount());
		m_progressBar.SetRange(1, m_maxCount);

		TransferFile();
	}
}

void CServerDialog::RequestFileRemove()
{
	if(!IsConnected())
	{
		Log(_T("error - server is closed"));
		return;
	}

	// 데이타 삭제
	UpdateRoot();

	CString folder;
	if(SelectFolder(this->GetSafeHwnd(), folder))
	{
		//folder.SetAt(0, 'C');
		if(!m_root.IsEmpty())
		{
			folder = m_root;
		}

		csRemove packet;
		CopyMemory(&packet.folder, folder, sizeof(packet));
		packet.folder[folder.GetLength()] = 0;
		packet.folder[folder.GetLength()+1] = 0;

		SendPacket(C_REMOVE, &packet, sizeof(csRemove));

		Log(_T("Start removing..."));
	}
}

void CServerDialog::RequestFileBackup()
{
	if(!IsConnected())
	{
		Log(_T("error - server is closed"));
		return;
	}

	CBackupDialog dlg;
	if(IDOK == dlg.DoModal())
	{
		CString source, target;
		if(dlg.GetBackupPath(source, target))
		{
			//source.SetAt(0, 'C');
			//target.SetAt(0, 'C');
			//여기서 부터 postmessage ㄱㄱ 
			csBackup packet;
			CopyMemory(packet.source, source, source.GetLength()*sizeof(TCHAR));
			CopyMemory(packet.target, target, target.GetLength()*sizeof(TCHAR));

			packet.source[source.GetLength()] = 0;
			packet.source[source.GetLength()+1] = 0;
			packet.target[target.GetLength()] = 0;
			packet.target[target.GetLength()+1] = 0;

			SendPacket(C_BACKUP, &packet, sizeof(csBackup));

			Log(_T("Start backup..."));
		}
	}
}

void CServerDialog::FindFiles(CString folder, CStringArray& files, CStringArray& folders)
{
	BOOL recursive = OPTION()->GetOption(OPTION_RECURSIVE);

    CString wildCard = folder + _T("\\*.*");
    
	CFileFind finder;
    BOOL working = finder.FindFile(wildCard);
	while(working)
    {
        working = finder.FindNextFile();
        if(finder.IsDots() || finder.IsHidden()) continue;

        // 디렉토리이면 재귀 호출
        if(finder.IsDirectory())
		{
			if(recursive)
				FindFiles(finder.GetFilePath(), files, folders);
		}
		else
		{
			folders.Add(folder);
			files.Add(finder.GetFileName());
		}
    }
}

void CServerDialog::UpdateRoot()
{
	GetDlgItem(IDC_EDIT_ROOT)->GetWindowText(m_root);
}

void CServerDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TIMER_CONNECTION :
		{
			if(!IsConnected())
			{
				m_socket->Connect();
			}
			
		}
		break;

	case TIMER_PING :
		{
			if(IsConnected())
			{
				SendPacket(C_PING, NULL, 0);				
			}
		}
		break;

	case TIMER_MONITOR :
		{
			RequestPeformances();
		}
		break;

	default:
		{
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CServerDialog::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer(TIMER_PING);
	CloseSocket();
	CDialog::OnClose();
}

LRESULT CServerDialog::OnConnect( WPARAM wParam, LPARAM lParam )
{
	//printf("serverOnConnect\n");

	KillTimer(TIMER_CONNECTION);
	m_status.SetBitmap(m_statusIcon[STATUS_ON]);
	m_nameStatic.SetWindowText(_T("connected"));

	// 서비스목록 요청
	RequestServices();

	// 핑
	SetTimer(TIMER_PING, 1000 * 60, NULL);
	return 0;
}

LRESULT CServerDialog::OnDisconnect( WPARAM wParam, LPARAM lParam )
{
	KillTimer(TIMER_PING);
	KillTimer(TIMER_CONNECTION);

	m_status.SetBitmap(m_statusIcon[STATUS_OFF]);
	m_nameStatic.SetWindowText(_T("not connected"));

	m_sequence = 0;

	if(m_launcherDlg->GetBatchManager()->IsConsoleMode())
	{
		_tprintf(_T("not connected %s \n"), m_name);
	}
	
	SetTimer(TIMER_CONNECTION, 1000, NULL);
	return 0;
}

LRESULT CServerDialog::OnReceive( WPARAM wParam, LPARAM lParam )
{
	switch(wParam)
	{
	case S_PING :
		{
			scPing* packet = (scPing*)(lParam);
			TRACE("S_PING : %lu\n", packet->code);
		}
		break;

	case S_SERVICES :
		{
			m_ready = TRUE;

			m_services.Save();
			m_services.DeleteAllItems();

			scServices* packet = (scServices*)(lParam);
			if(0 == packet->count)
			{
				Log(_T("no services"));
				break;
			}
			
			for(int i = 0 ; i < packet->count ; i++)
			{
				Service* service = (Service*)(lParam+sizeof(scServices)+sizeof(Service)*i);
				TRACE(_T("service : %s\n"), service->service);

				BYTE bitmapId = 0;
				CString runType;
				if(service->running) 
				{
					bitmapId = 1;
					runType = _T("");
				}
				if(service->console)
				{
					bitmapId = 1;
					runType = _T("console");
				}

				//if(service->running && service->demonPID) bitmapId = 3;

				m_services.InsertItem(i, _T(""), bitmapId);
				m_services.SetItemText(i, 0, service->service);
				m_services.SetItemText(i, 1, runType);
			}

			m_services.Restore();
		}
		break;

	case S_TRANSFER :
		{
			scTransfer* packet = (scTransfer*)(lParam);
			if(FALSE == packet->result)
			{
				scTransferFail* fail = (scTransferFail*)(lParam);
				switch(packet->type)
				{
				case TRANSFER_BEGIN :
					if( IsSyncComplete() )
					{
						Log(_T("exist file(%s)"), fail->file);
						OPTION()->WriteLog(_T("TRANSFER SUCCESS PC:%s FileName:%s"), m_name.GetBuffer(), m_fileName.GetString());
					}
					else
					{
						// Batch 메시지
					}					
					Next(FALSE);
					TransferFile();
					break;

				case TRANSFER_DO :
					if( IsSyncComplete() )
					{
						Log(_T("전송실패 file(%s)"), fail->file);
						OPTION()->WriteLog(_T("TRANSFER FAILED PC:%s FileName:%s"), m_name.GetBuffer(), m_fileName.GetString());
					}
					else
					{
						// Batch 메시지
					}					
					Next(FALSE);
					TransferFile();

					break;

				case TRANSFER_END :
					if( IsSyncComplete() )
					{
						Log(_T("전송실패 file(%s)"), fail->file);
						OPTION()->WriteLog(_T("TRANSFER FAILED PC:%s FileName:%s"), m_name.GetBuffer(), m_fileName.GetString());
					}
					else
					{
						// Batch 메시지
					}
					TransferFile();

					//올머신 하지 않음 
					break;

				}
			}
			else
			{
				switch(packet->type)
				{
				case TRANSFER_BEGIN :
					TransferFile();
					break;

				case TRANSFER_DO :
					TransferFile();
					break;

				case TRANSFER_END :
					TransferFile();
					///kyg 이떄 자기 자신을 제외한 dailog에게 보내는데 
					//벡터중에 자기가 아닌 벡터 실행하면됨 그리고 자기는 상태체크 하나 해주고 하면 될듯

					break;

				}
				//Log("전송성공 remain:%lu, type:%lu, index:%lu", m_files.GetCount(), packet->type, packet->index);
			}
		}
		break;

	case S_REMOVE :
		{
			scRemove* packet = (scRemove*)(lParam);
			if(packet->result)
			{
				Log(_T("Remove completed"));
				MessageBox(_T("Remove completed"), _T("Lostsaga"), MB_OK);
			}
			else
			{
				Log(_T("Remove failed"));
				MessageBox(_T("Remove failed"), _T("Lostsaga"), MB_OK);
			}
		}
		break;

	case S_INSTALL :
		{
			scInstall* packet = (scInstall*)(lParam);
			if(packet->result)
			{
				OnBnClickedButtonRefresh();
			}
		}
		break;

	case S_PLAY :
		{
			scPlay* packet = (scPlay*)(lParam);
			if(packet->result)
			{
				OnBnClickedButtonRefresh();
			}
		}
		break;

	case S_BACKUP :
		{
			scBackup* packet = (scBackup*)(lParam);
			if(packet->result)
			{
				Log(_T("Backup completed"));
				MessageBox(_T("Backup completed"), _T("Lostsaga"), MB_OK);
			}
			else
			{
				Log(_T("Backup failed"));
				MessageBox(_T("Backup failed"), _T("Lostsaga"), MB_OK);
			}
		}
		break;

	case S_REBOOT :
		{
			scReboot* packet = (scReboot*)(lParam);
			if(packet->result)
			{
				Log(_T("Reboot completed"));
				m_msgbox.MessageBox(_T("Reboot completed"), _T("Lostsaga"), 300, MB_OK);
			}
			else
			{
				Log(_T("Reboot failed"));
				m_msgbox.MessageBox(_T("Reboot failed"), _T("Lostsaga"), 300, MB_OK);
			}
		}
		break;

	case S_RESOURCE :
		{
			scResource* packet = (scResource*)(lParam);
			if(packet->result)
			{
				uint32 segment = sizeof(scResource);
				UNITS cpus, disks, memorys, networks;
				for(uint32 i = 0 ; i < packet->cpu ; i++)
				{
					uint32 value = 0;
					CopyMemory(&value, reinterpret_cast<void*>(lParam + segment), sizeof(uint32));
					cpus.push_back( value );

					segment += sizeof(uint32);
				}
				for(uint32 i = 0 ; i < packet->memory ; i++)
				{
					uint32 value = 0;
					CopyMemory(&value, reinterpret_cast<void*>(lParam + segment), sizeof(uint32));
					memorys.push_back( value );

					segment += sizeof(uint32);
				}
				for(uint32 i = 0 ; i < packet->disk ; i++)
				{
					uint32 value = 0;
					CopyMemory(&value, reinterpret_cast<void*>(lParam + segment), sizeof(uint32));
					disks.push_back( value );

					segment += sizeof(uint32);
				}
				for(uint32 i = 0 ; i < packet->network ; i++)
				{
					uint32 value = 0;
					CopyMemory(&value, reinterpret_cast<void*>(lParam + segment), sizeof(uint32));
					networks.push_back( value );

					segment += sizeof(uint32);
				}

				SetCPU( cpus );
				SetMemory( memorys );
				SetNetwork( networks );
				SetDisk( disks );

				//if(!m_perfMonDlg->IsInited())
				//{
				//	m_perfMonDlg->SetConfig(this, m_name);
				//}

				//m_perfMonDlg->ShowResult();
			
			}
			else
			{
				m_msgbox.MessageBox(_T("S_RESOURCE failed"), _T("Lostsaga"),300, MB_OK);
			}
		}
		break;
	}
	return 0;
}

LRESULT CServerDialog::OnPeformanceMonitor(WPARAM wParam, LPARAM lParam)
{
	if(!IsConnected())
	{
		Log(_T("error - server is closed"));
		return 0;
	}

	int hotItem = (int)wParam;

	// 서버 성능모니터 정보를 얻어옴
	SendPacket(C_RESOURCE, NULL, 0);
	return 0;
}

LRESULT CServerDialog::OnPlay(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;

	TCHAR serviceName[64];
	if(m_services.GetItemText(index, 0, serviceName, _countof(serviceName)))
	{
		ServicePlay(serviceName, FALSE);

		Log(_T("Play : %s"), serviceName);

		SendMsgDlg(serviceName, ECOMMAND::PLAYSERVICE);
	}
	return 0;
}

LRESULT CServerDialog::OnStop(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;

	TCHAR serviceName[64];
	if(m_services.GetItemText(index, 0, serviceName, _countof(serviceName)))
	{
		ServiceStop(serviceName, FALSE);

		Log(_T("Stop : %s"), serviceName);

		SendMsgDlg(serviceName, ECOMMAND::STOPSERVICE);
	}
	return 0;
}

LRESULT CServerDialog::OnPlayConsole(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;

	TCHAR serviceName[64];
	if(m_services.GetItemText(index, 0, serviceName, _countof(serviceName)))
	{
		ServicePlay(serviceName, TRUE);
		Log(_T("Play : %s"), serviceName);
	}
	return 0;
}

LRESULT CServerDialog::OnStopConsole(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;

	TCHAR serviceName[64];
	if(m_services.GetItemText(index, 0, serviceName, _countof(serviceName)))
	{
		ServiceStop(serviceName, TRUE);
		Log(_T("Stop : %s"), serviceName);
	}
	return 0;
}

LRESULT CServerDialog::OnUninstall(WPARAM wParam, LPARAM lParam)
{
	if(IsConnected())
	{
		int index = (int)wParam;

		csUninstall packet;
		TCHAR serviceName[64];
		if(m_services.GetItemText(index, 0, serviceName, _countof(serviceName)))
		{
			ZeroMemory(&packet, sizeof(packet));

			packet.install = FALSE;
			CopyMemory(packet.service, serviceName, sizeof(serviceName));

			SendPacket(C_INSTALL, &packet, sizeof(csUninstall));
			Log(_T("Uninstall : %s"), serviceName);
		}
	}
	return 0;
}

void CServerDialog::Log(const TCHAR *format, ...)
{
	TCHAR message[512];

	va_list marker; 
	va_start(marker, format); 
	_vstprintf_s(message, _countof(message), format, marker); 
	va_end(marker);

	m_log.SetWindowText(message);
}

int CServerDialog::GetIndex(const TCHAR* service)
{
	CString name;
	for(int i = 0; i < m_services.GetItemCount(); i++)
	{
		name = m_services.GetItemText(i, 0);
		if(name == service)
			return i;
	}
	return -1;
}

void CServerDialog::SetCPU(UNITS& units)
{
	//m_perfMonDlg->SetCPU( units );

	m_CPUs = units;

	if(m_CPUs.size() >= 1)
	{
		m_ctlCPU.DrawGraph( units[0] );

		if(maxResource[RESOURCE_TYPE_CPU] < units[0])
			maxResource[RESOURCE_TYPE_CPU] = units[0];
		
		CString text;
		text = "CPU(";
		for(uint32 n = 1 ; n < m_CPUs.size() ; n++)
		{
			CString param;
			if(n == 1)
				param.Format( _T("%d%%"), m_CPUs[n] );
			else
				param.Format( _T(", %d%%"), m_CPUs[n] );

			text.AppendFormat( _T("%s"), param);
		}
		text.AppendFormat( _T(")"));
		m_ctlCPU.SetToolTip( text );
		
	}
}

void CServerDialog::SetMemory(UNITS& units)
{
	//m_perfMonDlg->SetMemory( units );

	m_Memorys = units;

	if(m_Memorys.size() >= 1)
	{
		m_ctlRAM.DrawGraph( units[0] );
		if(maxResource[RESOURCE_TYPE_RAM] < units[0])
			maxResource[RESOURCE_TYPE_RAM] = units[0];

		CString text;
		text.Format(_T("%d%%"), m_Memorys[0]);
		text.Format(_T("Memory(%dM / %dM)"), (units[1]-units[2])/1024, units[1]/1024);
		m_ctlRAM.SetToolTip( text );
	}
}

void CServerDialog::SetNetwork(UNITS& units)
{
	//	m_perfMonDlg->SetNetwork( units );

	m_Networks = units;

	if(m_Networks.size() >= 1)
	{
		CString downlabel, upLabel;
		double download = units[0];
		double updload = units[1];

		if(maxResource[RESOURCE_TYPE_UPLOAD] < updload)
			maxResource[RESOURCE_TYPE_UPLOAD] = (int)updload;
		if(maxResource[RESOURCE_TYPE_DOWNLOAD] < download)
			maxResource[RESOURCE_TYPE_DOWNLOAD] = (int)download;

		// download
		if(download < (1024 * 1.3))
		{
			downlabel = "KB";
			m_ctlDownload.SetPlotColor(RGB(128, 255, 128));
		}
		else
		{
			downlabel = "MB";
			download = download / 1024.0;
			m_ctlDownload.SetPlotColor(RGB(255, 255, 64));
		}

		// upload
		if(updload < (1024 * 1.3))
		{
			upLabel = "KB";
			m_ctlUpload.SetPlotColor(RGB(128, 255, 128));
		}
		else
		{
			upLabel = "MB";
			updload = updload / 1024.0;
			m_ctlUpload.SetPlotColor(RGB(255, 255, 64));
		}

		m_ctlUpload.DrawGraph( (UINT)updload );
		m_ctlDownload.DrawGraph( (UINT)download );

		CString text;
		text.Format(_T("Download(%0.1lf %s)"), download, downlabel, maxResource[RESOURCE_TYPE_DOWNLOAD]);
		m_ctlDownload.SetToolTip( text );

		text.Format(_T("Upload(%0.1lf %s)"), updload, upLabel);
		m_ctlUpload.SetToolTip( text );
	}
}

void CServerDialog::SetDisk(UNITS& units)
{
	//m_perfMonDlg->SetDisk( units );

	m_Disks = units;

	if(m_Disks.size() > 0 )
	{
		int spaceC = m_Disks[0]/1024, spaceD = m_Disks[2]/1024, totalC = m_Disks[1]/1024, totalD = m_Disks[3]/1024;
		double usingSpace = (double)(totalC-spaceC) / totalC * 100.0;
		m_ctlDisk.DrawGraph( (UINT)usingSpace );
		CString text;
		text.Format(_T("Disk(%0.1lf GB / %0.1lf GB)"), (totalC-spaceC)/1024.0, totalC/1024.0);
		m_ctlDisk.SetToolTip( text );

		usingSpace = (double)(totalD-spaceD) / totalD * 100.0;
		m_ctlDisk2.DrawGraph( (UINT)usingSpace );
		text.Format(_T("Disk(%0.1lf GB / %0.1lf GB)"), (totalD-spaceD)/1024.0, totalD/1024.0);
		m_ctlDisk2.SetToolTip( text );
	}
}

void CServerDialog::SyncCommandStart( CString& syncFolder, BOOL recursive )
{
	m_syncComplete = FALSE;
	Reset();

	m_folder = syncFolder;
	m_folders.RemoveAll();
	m_files.RemoveAll();

	SyncFindFiles( recursive );

	m_stlocal.m_folder = m_folder;
	m_stlocal.m_folders.Copy( m_folders );
	m_stlocal.m_files.Copy( m_files );
	m_maxCount = static_cast<UINT>(m_files.GetCount());
	m_progressBar.SetRange(1, m_maxCount);	

	TransferFile();
}

void CServerDialog::SyncCommandEnd()
{
	m_syncComplete = TRUE;
}

void CServerDialog::SyncFindFiles( BOOL recursive )
{
	BOOL prevRecursive = OPTION()->GetOption( OPTION_RECURSIVE );
	OPTION()->SetOption( OPTION_RECURSIVE, recursive );

	FindFiles( m_folder, m_files, m_folders );
	OPTION()->SetOption( OPTION_RECURSIVE, prevRecursive );
}

BOOL CServerDialog::CommandFileSync( CString& syncFolder, BOOL recursive )
{
	if( GetAuto() != FALSE )
	{
		return FALSE;
	}
	
	if( !IsConnected() )
	{
		return FALSE;
	}

	SyncCommandStart( syncFolder, recursive );
	return TRUE;
}

BOOL CServerDialog::CommandInstall( CString& service, CString& demon, CString& script )
{
	if( IsConnected() )
	{
		if(service.IsEmpty())	return FALSE;
		if(demon.IsEmpty())		return FALSE;
		if(script.IsEmpty())	return FALSE;

		csInstall packet;
		packet.install = TRUE;
		CopyMemory(packet.service, service, service.GetLength()*sizeof(TCHAR));
		CopyMemory(packet.file, demon, demon.GetLength()*sizeof(TCHAR));
		CopyMemory(packet.script, script, script.GetLength()*sizeof(TCHAR));
		
		SendPacket(C_INSTALL, &packet, sizeof(csInstall));

		Log(_T("BATCH::Install : %s"), service);
		return TRUE;
	}

	return FALSE;
}

BOOL CServerDialog::CommandUninstall( CString& service )
{
	if( !IsConnected() )
		return FALSE;

	int index = FindServiceIndex( service );
	if( index != -1 )
	{
		csUninstall packet;
		ZeroMemory( &packet, sizeof(packet) );

		packet.install = FALSE;
		CopyMemory( packet.service, service.GetBuffer(), sizeof(TCHAR)*(service.GetLength()+1) );

		SendPacket( C_INSTALL, &packet, sizeof(csUninstall) );

		// OnUninstall 방식 : 왜 sizeof( csInstall )?? READ_VIOLATION이 그동안 없었나?
		// SendPacket( C_INSTALL, &packet, sizeof(csinstall) );		

		Log(_T("BATCH::Unistall : %s"), service);
		return TRUE;
	}

	//return FALSE;
	return TRUE;	// 서비스가 없으면 Uninstall로 취급
}

BOOL CServerDialog::CommandPlay( CString& service )
{
	int index = FindServiceIndex( service );

	if( index != -1 )
	{
		ServicePlay( service.GetBuffer(), FALSE );
		Log(_T("BATCH::Play : %s"), service);
		return TRUE;
	}
	return FALSE;
}

BOOL CServerDialog::CommandStop( CString& service )
{
	int index = FindServiceIndex( service );

	if( index != -1 )
	{
		ServiceStop( service.GetBuffer(), FALSE );
		Log(_T("BATCH::Stop : %s"), service);
		return TRUE;
	}
	return FALSE;
}

BOOL CServerDialog::CommandPlayAll()
{
	if( m_services.GetItemCount() == 0 )
	{
		//return FALSE;
		return TRUE;				// 서비스가 없으면 모두 실행으로 취급(?)
	}

	TCHAR serviceName[64];
	for( int i = 0 ; i < m_services.GetItemCount() ; i++ )
	{
		if( m_services.GetItemText(i, 0, serviceName, sizeof(serviceName) ) )
		{
			ServicePlay( serviceName, FALSE );
		}
	}

	Log(_T("BATCH::Play all"));
	return TRUE;
}

BOOL CServerDialog::CommandStopAll()
{
	if( m_services.GetItemCount() == 0 )
	{
		//return FALSE;
		return TRUE;			// 서비스가 없으면 모두 실행으로 취급(?)
	}

	TCHAR serviceName[64];
	for( int i = 0 ; i < m_services.GetItemCount() ; i++ )
	{
		if( m_services.GetItemText(i, 0, serviceName, sizeof(serviceName) ) )
		{
			ServiceStop( serviceName, FALSE );
		}
	}

	Log(_T("BATCH::Stop all"));
	return TRUE;
}

BOOL CServerDialog::CommandUninstallAll()
{
	if( !IsConnected() )
		return FALSE;

	csUninstall packet;
	ZeroMemory( &packet, sizeof(packet) );
	packet.install = FALSE;

	TCHAR serviceName[64];
	for(int i = 0 ; i < m_services.GetItemCount() ; i++ )
	{
		if( m_services.GetItemText(i, 0, serviceName, sizeof(serviceName) ) )
		{
			ZeroMemory( packet.service, sizeof(packet.service) );
			CopyMemory( packet.service, serviceName, sizeof(serviceName) );

			SendPacket( C_INSTALL, &packet, sizeof(csUninstall) );
		}
	}

	Log(_T("BATCH::Uninstall all"));
	return TRUE;
}

int CServerDialog::FindServiceIndex( CString& service )
{
	TCHAR serviceName[64];
	for( int i = 0 ; i < m_services.GetItemCount() ; i++ )
	{
		m_services.GetItemText(i, 0, (LPTSTR)serviceName, sizeof(serviceName) );
		if( _tcscmp( serviceName, service.GetBuffer() ) == 0 )
		{
			return i;
		}
	}

	return -1;
}

BOOL CServerDialog::IsServiceOn( int index )
{
	LVITEMW	lvi;
	ZeroMemory( &lvi, sizeof(lvi) );
	lvi.mask  = LVIF_IMAGE;
	lvi.iItem = index;
	m_services.GetItem(&lvi);

	if( lvi.iImage == 1 )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CServerDialog::IsServiceOn( CString& service )
{
	int index = FindServiceIndex( service );
	if( index == -1 )
		return FALSE;

	return IsServiceOn( index );
}

BOOL CServerDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)
	{
	case WM_KEYDOWN :
		{
			if(VK_RETURN == pMsg->wParam)	return FALSE;
			if(VK_ESCAPE == pMsg->wParam)	return FALSE;
			if(VK_F5 == pMsg->wParam)
			{
				TRACE(_T("F5\n"));
				RequestServices();
				return FALSE;
			}
			if(pMsg->wParam == 'C' && GetAsyncKeyState(VK_CONTROL))
			{
				m_launcherDlg->ConsoleStop();
				m_launcherDlg->Quit();
			}
		}
		break;
	case WM_SYSKEYDOWN:
		{
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CServerDialog::OnBnClickedButtonPlayAll()
{
	RequestPlayAll();
}

void CServerDialog::OnBnClickedButtonStopAll()
{
	RequestStopAll();
}

void CServerDialog::OnBnClickedButtonRemove()
{
	RequestFileRemove();
}

void CServerDialog::OnBnClickedButtonBackup()
{
	RequestFileBackup();
}

BOOL CServerDialog::IsMouseOverTab(LONG x, LONG y)
{
	if(m_tapRect.top <= y && m_tapRect.bottom >= y)
	{
		if(m_tapRect.left <= x && m_tapRect.right >= x)
			return TRUE;
	}
	return FALSE;
}

LRESULT CServerDialog::OnAutomatic( WPARAM wParam, LPARAM lParam )
{
	switch(lParam)
	{
	case ECOMMAND::WATCHDOG:
		{
			TCHAR serviceName[64];
			for(int i = 0 ; i < m_services.GetItemCount() ; i++)
			{
				LVITEM item;
				ZeroMemory(&item, sizeof(item));

				item.iItem	= i;
				item.mask	= LVIF_IMAGE;
				if(!m_services.GetItem(&item)) continue;;

				if(item.iImage == 0)
				{
					if(m_services.GetItemText(i, 0, serviceName, _countof(serviceName)))
					{
						ServicePlay(serviceName, OPTION()->GetOption(OPTION_CONSOLE));

						Log(_T("Restart : %s"), serviceName);

						OPTION()->WriteLog(_T("Send RestartServer PC:%s serviceName:%s"), m_name.GetBuffer(), serviceName);
					}

					
				}
			}
		}
		break;

	case ECOMMAND::MONITOR:
		{
			RequestPeformances();
		}
		break;

	case ECOMMAND::REBOOT:
		{
			if(!IsConnected())
			{
				Log(_T("error - server is closed"));
				return 0;
			}
			csReboot packet;
			packet.option = 5; // 5sec

			SendPacket(C_REBOOT, &packet, sizeof(csReboot));

			Log(_T("ECOMMAND::Reboot..."));
			ResetApplyAll();
		}
		break;

	case ECOMMAND::FILESYNC:
		{
			if( !IsSyncComplete() )
			{
				SetAuto( TRUE );
				break;
			}

			OPTION()->SetOption(OPTION_APPLYALL, TRUE);
			FOLDERSTRING* stdata = (FOLDERSTRING*)wParam;
	
			if(stdata)
			{
				CLauncherTabCtrl* dlg = (CLauncherTabCtrl*)GetParent();
				for(unsigned int i=0; i < dlg->m_dialogs.size(); ++i)
				{
					if(dlg->m_dialogs.at(i) == this)
					{
						dlg->UserFocus(i);
						break;
					}
				}
				if(!IsConnected())
				{
					Log(_T("error - server is closed"));
					return 0;
				}
				Reset();

				//Todo 
				m_files.Copy(stdata->m_files);
				m_folders.Copy(stdata->m_folders);
				m_folder = stdata->m_folder;
				m_stlocal.m_files.Copy(m_files);
				m_stlocal.m_folders.Copy(m_folders);
				m_stlocal.m_folder = m_folder;
				m_maxCount = static_cast<UINT>(m_files.GetCount());
				m_progressBar.SetRange(1, m_maxCount);

				TransferFile();

				delete stdata;
			}
			 
		}
		break;
	case ECOMMAND::PLAYALL:
		{
			TCHAR serviceName[64];
			for(int i = 0 ; i < m_services.GetItemCount() ; i++)
			{
				if(m_services.GetItemText(i, 0, serviceName, _countof(serviceName)))
				{
					ServicePlay(serviceName, OPTION()->GetOption(OPTION_CONSOLE));
				}
			}

			Log(_T("ECOMMAND::Play all services"));
		}
		break;
	case ECOMMAND::STOPALL:
		{
			TCHAR serviceName[64];
			for(int i = 0 ; i < m_services.GetItemCount() ; i++)
			{
				if(m_services.GetItemText(i, 0, serviceName, _countof(serviceName)))
				{
					ServiceStop(serviceName, OPTION()->GetOption(OPTION_CONSOLE));
				}
			}

			Log(_T("ECOMMAND::Stop all services"));
			ResetApplyAll();
		}
		break;
	case ECOMMAND::REFRESH:
		{
			// 서비스목록 요청
			RequestServices();
		}
		break;
	case ECOMMAND::PLAYSERVICE:
		{
			//스트링으로 전송하고 할듯
			CString* cstmp = (CString*)wParam;
			TCHAR serviceName[64];

			for(int i=0; i<m_services.GetItemCount(); ++i)
			{
				m_services.GetItemText(i, 0, (LPTSTR)serviceName, sizeof(serviceName));
				if(_tcscmp(serviceName,cstmp->GetBuffer()) == 0)
				{
					ServicePlay(serviceName, OPTION()->GetOption(OPTION_CONSOLE));
					Log(_T("ECOMMAND::Play : %s"), serviceName);

				}				
			}
			ResetApplyAll();
		}
		break;
	case ECOMMAND::STOPSERVICE:
		{
			//스트링으로 전송하고 할듯 
			CString* cstmp = (CString*)wParam;
			TCHAR serviceName[64];

			for(int i=0; i<m_services.GetItemCount(); ++i)
			{
				m_services.GetItemText(i, 0, (LPTSTR)serviceName, sizeof(serviceName));
				if(_tcscmp(serviceName,cstmp->GetBuffer()) == 0)
				{
					ServiceStop(serviceName, OPTION()->GetOption(OPTION_CONSOLE));
					Log(_T("ECOMMAND::Stop : %s"), serviceName);
				}				
			}
			 
			ResetApplyAll();
		}
		break;

	}
	return 0;
}

void CServerDialog::SendMsgDlg( TCHAR * serviceName, int cmd )
{
	if(OPTION()->GetOption(OPTION_APPLYALL) == FALSE)
		return;

	CLauncherTabCtrl* tabCtrl = (CLauncherTabCtrl*)GetParent();
 
	for(unsigned int i=0; i < tabCtrl->m_dialogs.size(); ++i)
	{
		CString* cstmp = new CString;
		CServerDialog* dlg = tabCtrl->m_dialogs.at(i);
		if(this != dlg)
		{
			cstmp->Format(_T("%s"), serviceName);
			dlg->OnAutomatic((WPARAM)cstmp, cmd);
		}
		delete cstmp;
	}
	ResetApplyAll();
}

void CServerDialog::SendMsgDlg(FOLDERSTRING& stdata)
{
	if(OPTION()->GetOption(OPTION_APPLYALL) == FALSE)
	{
		delete &stdata;
		return;
	}

	CLauncherTabCtrl* dlg = (CLauncherTabCtrl*)GetParent();
 
	for(unsigned int i=0; i < dlg->m_dialogs.size(); ++i)
	{
		CServerDialog* serverDlg = dlg->m_dialogs.at(i);
		if(serverDlg)
		{	 
			if(serverDlg->GetAuto() == false)
			{
				serverDlg->PostMessage(WM_AUTOMATIC,(WPARAM)&stdata, ECOMMAND::FILESYNC);
				break;
			}			 
		}
	}
	ResetApplyAll();
}

void CServerDialog::SendMsgDlg( int cmd )
{
	if(OPTION()->GetOption(OPTION_APPLYALL) == FALSE)
		return;

	CLauncherTabCtrl* dlg = (CLauncherTabCtrl*)GetParent();
	for(unsigned int i=0; i < dlg->m_dialogs.size(); ++i)
	{
		auto adata = dlg->m_dialogs.at(i);
		if(this != adata)
		{
			adata->OnAutomatic(NULL,cmd);
		}
	}
	ResetApplyAll();
}

void CServerDialog::ResetApplyAll()
{
	if(OPTION()->GetOption(OPTION_APPLYALL) == TRUE)
	{
		CLauncherTabCtrl* dlg = (CLauncherTabCtrl*)GetParent();
		if(dlg->m_dialogs[dlg->m_dialogs.size() -1 ] == this)
		{
			OPTION()->SetOption(OPTION_APPLYALL, FALSE);
			m_launcherDlg->m_chk_applyall.SetCheck(FALSE);
		}
	}
}

void CServerDialog::ServicePlay(TCHAR* serviceName, BOOL console)
{ 
	csPlay packet;
	ZeroMemory(&packet, sizeof(packet));

	packet.play		= TRUE;
	packet.isDemon	= console;
	_tcscpy_s(packet.service, serviceName);
	//CopyMemory(packet.service, serviceName, _tcslen(serviceName) );
	SendPacket(C_PLAY, &packet, sizeof(csPlay));
}

void CServerDialog::ServiceStop(TCHAR* serviceName, BOOL console)
{
	csStop packet;
	ZeroMemory(&packet, sizeof(packet));

	packet.play		= FALSE;
	packet.isDemon	= console;
	_tcscpy_s(packet.service, serviceName);
	//CopyMemory(packet.service, serviceName, sizeof(packet.service));
	SendPacket(C_PLAY, &packet, sizeof(csStop));
}

void CServerDialog::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 마우스 위치에 팝업메뉴를 출력한다.
	CPoint pos;
	GetCursorPos(&pos);

	m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pos.x, pos.y, this);

	CDialog::OnRButtonUp(nFlags, point);
}

BOOL CServerDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDC_SERVICE_MONITOR :
		{
			RequestPeformances();

			UINT state = m_menu.GetMenuState(IDC_SERVICE_MONITOR, MF_BYCOMMAND);
			if (state & MF_CHECKED)
			{
				KillTimer(TIMER_MONITOR);
				m_menu.CheckMenuItem(IDC_SERVICE_MONITOR, MF_UNCHECKED);
			}
			else
			{
				SetTimer(TIMER_MONITOR, 2000, NULL);
				m_menu.CheckMenuItem(IDC_SERVICE_MONITOR, MF_CHECKED);
			}
		}
		break;

	case IDC_SERVICE_REFRESH :
		RequestServices();
		SendMsgDlg(ECOMMAND::REFRESH);
		break;

	case IDC_SERVICE_FILESYNC :
		RequestFileSync();
		break;

	case IDC_SERVICE_FILEBACKUP :
		RequestFileBackup();
		break;

	case IDC_SERVICE_FILEREMOVE :
		RequestFileRemove();
		break;

	case IDC_SERVICE_PLAYALL :
		RequestPlayAll();
		break;

	case IDC_SERVICE_STOPALL :
		RequestStopAll();
		break;

	case IDC_SERVICE_INSTALL :
		RequestInstall();
		break;

	case IDC_SERVICE_REBOOT :
		RequestReboot();
		break;
	}

	return CDialog::OnCommand(wParam, lParam);
}
