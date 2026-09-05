#include "StdAfx.h"
#include "cFile.h"
#include "MD5.h"
#include "cDemonLauncher.h"
#include "SessionProc.h"
#include "GameCommand.h"
#include "Utility.h"
#include "../include/IIocpContext.h"
#include <io.h>
#include <shlwapi.h>



extern void Tokenize(const std::tstring& str, TOKENS& tokens, const std::tstring& delimiters);
extern BOOL GetServices(STRINGS& services, STRINGS& files, STRINGS& scripts);
extern BOOL IsService(const TCHAR* serviceName);
extern BOOL CreateFolder(const TCHAR* path);
extern BOOL CreateConsole(const TCHAR* title, TCHAR* command, TCHAR* arguments);


void CPing::Run(IIocpContext* context)
{
	scPing packet;
	packet.code = rand();

	SessionProc::SendPacket(
		context, 
		S_PING, 
		reinterpret_cast<uint8*>(&packet),
		sizeof(packet));
}

void CServices::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CServices::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CServices::Begin(IIocpContext* context)
{
	if(GetServices(m_services, m_files, m_scripts))
	{
		Debug(_T("Service count :: %lu\n"), m_services.size());

		scServices packet;
		packet.count = m_services.size();

		cSerialize::Reset();
		cSerialize::Write(reinterpret_cast<uint8*>(&packet), sizeof(packet));

		Service temp;
		std::tstring service, file, script;
		for(uint32 n = 0 ; n < packet.count ; n++)
		{
			service = m_services[n];
			file	= m_files[n];
			script	= m_scripts[n];

			ZeroMemory(&temp, sizeof(temp));

			BOOL result = GetRunning(service.c_str(), temp.running, temp.console);
			if(result)
			{
				Debug(_T("Running :: %s\n"), service.c_str());
			}
			else
			{
				Debug(_T("Not Running :: %s\n"), service.c_str());
			}
			CopyMemory(temp.service, service.c_str(), sizeof(temp.service));
			cSerialize::Write(reinterpret_cast<uint8*>(&temp), sizeof(temp));
		}

		SessionProc::SendPacket(
			context, 
			S_SERVICES,
			cSerialize::GetBuffer(), 
			cSerialize::GetLength());
	}
	else
	{
		Debug(_T("Service count :: 0\n"));

		scServices packet;
		packet.count = 0;

		SessionProc::SendPacket(
			context, 
			S_SERVICES,
			reinterpret_cast<uint8*>(&packet), 
			sizeof(packet));
	}
}

BOOL CServices::GetRunning(const TCHAR* service, BOOL& running, BOOL& console)
{
	running = g_demonLauncher.IsRunningService(service);
	if(running)
	{
		console = FALSE;
		return TRUE;
	}

	running = g_demonLauncher.IsRunningConsole(service);
	if(running)
	{
		//PID = g_demonLauncher.GetPID(service);
		console = TRUE;
		return TRUE;
	}
	return FALSE;
}

void CTransfer::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CTransfer::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CTransfer::Begin(IIocpContext* context)
{
	BOOL result = FALSE;
	switch(m_packet->type)
	{
	case TRANSFER_BEGIN :
		result = TransferBegin();
		break;

	case TRANSFER_DO :
		result = TransferDo();
		break;

	case TRANSFER_END :
		result = TransferEnd();
		break;
	}

	if(result)
	{
		scTransfer packet;
		packet.result	= result;
		packet.type		= m_packet->type;
		packet.index	= m_packet->index;

		SessionProc::SendPacket(
			context, 
			S_TRANSFER,
			reinterpret_cast<uint8*>(&packet), 
			sizeof(packet));
	}
	else
	{
		scTransferFail packet;
		packet.result	= result;
		packet.type		= m_packet->type;
		packet.index	= m_packet->index;
		CopyMemory(packet.file, m_session->GetFile(), sizeof(packet.file));

		SessionProc::SendPacket(
			context, 
			S_TRANSFER,
			reinterpret_cast<uint8*>(&packet), 
			sizeof(packet));

		m_session->TransferEnd();
	}
}

BOOL CTransfer::TransferBegin()
{
	if(m_session->DoingTransfer())
		return FALSE;

	TCHAR path[512];
	_stprintf_s(path, _countof(path), _T("%s\\%s"), m_begin->folder, m_begin->file);
	if(UpdateFile(path, m_begin->length, m_begin->digest))
	{
		// 파일이 존재하지 않거나 다른 버젼이므로 복사한다
		CreateFolder(m_begin->folder);
		CreateBuffer(path, m_begin->length);
		return TRUE;
	}

	// 파일이 존재하고 같은 버젼의 파일이다
	return FALSE;
}

BOOL CTransfer::TransferDo()
{
	if(m_session->TransferDo(m_buffer, m_length))
	{
		// 추가 성공
		if(m_session->GetLength() >= G_MAXFILESIZE)
		{
			cFileWriter file;
			if(!file.Open(m_session->GetFile(), OPEN_ALWAYS))
			{
				// 파일생성 실패
				Debug(_T("CTransfer - 파일생성 실패\n"));
				return FALSE;
			}

			file.Move(FILE_END, 0);
			if(!file.Write(m_session->GetBuffer(), m_session->GetLength()))
			{
				// 파일 쓰기 실패
				file.Close();
				Debug(_T("CTransfer - 파일쓰기 실패(%lu Bytes)\n"), m_session->GetLength());
				return FALSE;
			}

			// 종료
			file.Close();

			// 버퍼정리
			m_session->TransferWrite();
		}
		return TRUE;
	}

	// 실패
	Debug(_T("CTransfer - 데이타 전송실패\n"));
	return FALSE;
}

BOOL CTransfer::TransferEnd()
{
	if(!m_session->DoingTransfer())
		return FALSE;

	if(m_end->length == m_session->GetTransferLength())
	{
		// 미리 저장된 파일인지 확인
		if(m_session->GetTransferLength() == m_session->GetLength())
		{
			// 저장된 적이 없으면 기존 파일을 지운다
			DeleteFile(m_session->GetFile());
		}

		BOOL result = TRUE;

		cFileWriter file;
		if(!file.Open(m_session->GetFile(), OPEN_ALWAYS))
		{
			// 파일생성 실패
			result = FALSE;
			DWORD error = GetLastError();
			m_session->TransferEnd();
			Debug(_T("CTransfer - 파일생성 실패\n"));
			return FALSE;
		}

		file.Move(FILE_END, 0);
		if(!file.Write(m_session->GetBuffer(), m_session->GetLength()))
		{
			// 파일 쓰기 실패
			file.Close();
			result = FALSE;
			m_session->TransferEnd();
			Debug(_T("CTransfer - 파일쓰기 실패(%lu Bytes)\n"), m_session->GetLength());
			return FALSE;
		}

		// 종료
		file.Close();
		m_session->TransferEnd();

		Debug(_T("CTransfer - 파일 전송완료 (%0.1lfKB)\n"), m_end->length / 1024.0);
		return result;
	}

	// 실패
	m_session->TransferEnd();
	Debug(_T("CTransfer - 파일 전송실패\n"));
	return FALSE;
}

BOOL CTransfer::UpdateFile(const TCHAR* fileName, const uint32 fileLength, const BYTE* source)
{
	cFileReader file;
	BOOL result = file.Open(fileName);
	if(result)
	{
		if(file.GetFileSize() == fileLength)
		{
			m_temp.Erase();
			m_temp.Resize(fileLength);
			if(!file.Read(m_temp.GetBuffer(), fileLength))
			{
				file.Close();
				return TRUE;
			}
			m_temp.SetLength(fileLength);

			CMD5 MD5;
			MD5.Generate(m_temp.GetBuffer(), fileLength);

			BYTE digest[16];
			CopyMemory(digest, MD5.GetDigest(), sizeof(digest));
			for(uint32 n = 0 ; n < 16 ; n++)
			{
				if(digest[n] != source[n])
				{
					file.Close();
					return TRUE;
				}
			}
			file.Close();
			return FALSE;
		}
		else
		{
			file.Close();
			return TRUE;
		}
	}
	return TRUE;
}

BOOL CTransfer::CreateBuffer(const TCHAR* fileName, const uint32 length)
{
	m_session->TransferBegin(fileName, length);
	DeleteFile(m_session->GetFile());
	return TRUE;
}

void CRemove::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CRemove::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CRemove::Begin(IIocpContext* context)
{
	BOOL result = RemoveFolder();

	scRemove packet;
	packet.result = result;

	SessionProc::SendPacket(
		context, 
		S_REMOVE,
		reinterpret_cast<uint8*>(&packet), 
		sizeof(packet));
}

BOOL CRemove::RemoveFolder()
{
	SHFILEOPSTRUCT fileop = {0};

	fileop.hwnd		= NULL;
	fileop.wFunc	= FO_DELETE; // 삭제 속성 설정
	fileop.pFrom	= m_packet->folder;
	fileop.pTo		= NULL;
	fileop.fFlags	= FOF_NOCONFIRMATION | FOF_NOERRORUI; 
	fileop.fAnyOperationsAborted	= FALSE;
	fileop.hNameMappings			= NULL;
	fileop.lpszProgressTitle		= NULL;
	
	int result = SHFileOperation(&fileop);
	return (0 == result) ? TRUE : FALSE;
}

void CInstall::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CInstall::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CInstall::Begin(IIocpContext* context)
{
	BOOL result = FALSE;
	if(m_packet->install)
	{
		result = DoInstall(m_install->service, m_install->file, m_install->script);
	}
	else
	{
		result = DoUninstall(m_uninstall->service);
		if(result)
		{
			Remove(m_uninstall->clear);
		}
	}

	scInstall packet;
	packet.result = result;

	SessionProc::SendPacket(
		context, 
		S_INSTALL,
		reinterpret_cast<uint8*>(&packet), 
		sizeof(packet));
}

BOOL CInstall::DoInstall(const TCHAR* service, const TCHAR* file, const TCHAR* script)
{
	if(CreateRegistry(service, file, script))
	{
		TCHAR command[512];
		_stprintf_s(command, _countof(command), _T("/c \"%s -i %s %s\""), file, service, service);
		ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE); 
		return IsService(service);
	}
	return FALSE;
}

BOOL CInstall::DoUninstall(const TCHAR* service)
{
	if(DeleteRegistry(service))
	{
		TCHAR command[512];
		_stprintf_s(command, _countof(command), _T("/c \"sc delete %s\""), service);
		ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE); 
		return !IsService(service);
	}
	return FALSE;
}

void CInstall::Remove(const BOOL b)
{
	if(b)
	{
		// 파일을 모두 지운다
	}
}

BOOL CInstall::CreateRegistry(const TCHAR* service, const TCHAR* file, const TCHAR* script)
{
	TCHAR regKey[512];
	_stprintf_s(regKey, _countof(regKey), _T("software\\LostSaga\\%s"), service);
	
	HKEY key;
	DWORD disp = 0;
	LONG ret = RegCreateKeyEx(HKEY_CURRENT_USER, regKey, 
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp);
	if(ret != ERROR_SUCCESS) return FALSE;

	uint32 length = _tcsclen(service);
	length = length*sizeof(TCHAR);
	RegSetValueEx(key, _T("service"), 0, REG_SZ, (LPBYTE)service, length);

	length = _tcsclen(file);
	length = length*sizeof(TCHAR);
	RegSetValueEx(key, _T("file"), 0, REG_SZ, (LPBYTE)file, length);

	length = _tcsclen(script);
	length = length*sizeof(TCHAR);
	RegSetValueEx(key, _T("script"), 0, REG_SZ, (LPBYTE)script, length);

	RegCloseKey(key);
	return TRUE;
}

BOOL CInstall::DeleteRegistry(const TCHAR* service)
{
	TCHAR regKey[512];
	_stprintf_s(regKey, _countof(regKey), _T("software\\LostSaga\\%s"), service);
	
	DWORD disp = 0;
	LONG ret = SHDeleteKey(HKEY_CURRENT_USER, regKey);
	if(ret != ERROR_SUCCESS) return FALSE;
	return TRUE;
}

void CPlay::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CPlay::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CPlay::Begin(IIocpContext* context)
{
	BOOL result = FALSE;
	if(m_packet->play)
	{
		result = g_demonLauncher.Play(m_packet->service, m_packet->isDemon);
	}
	else
	{
		result = g_demonLauncher.Stop(m_packet->service, m_packet->isDemon);
	}

	scPlay packet;
	packet.result = result;

	SessionProc::SendPacket(
		context, 
		S_PLAY,
		reinterpret_cast<uint8*>(&packet), 
		sizeof(packet));
}

void CBackup::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CBackup::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CBackup::Begin(IIocpContext* context)
{
	BOOL result = DoBackup(m_packet->source, m_packet->target);

	scBackup packet;
	packet.result = result;

	SessionProc::SendPacket(
		context, 
		S_BACKUP,
		reinterpret_cast<uint8*>(&packet), 
		sizeof(packet));
}

BOOL CBackup::DoBackup(const TCHAR* source, const TCHAR* target)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	TCHAR backup[512];
	ZeroMemory(backup, _countof(backup));
	_stprintf_s(backup, _countof(backup), 
		_T("%s\\%04d%02d%02d-%02d%02d"),
		target, 		
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute);

	if(!CreateFolder(backup))
		return FALSE;

	SHFILEOPSTRUCT fileop = {0};

	fileop.hwnd		= NULL;
	fileop.wFunc	= FO_COPY; // 삭제 속성 설정
	fileop.pFrom	= source;
	fileop.pTo		= backup;
	fileop.fFlags	= FOF_NOCONFIRMATION | FOF_NOERRORUI; 
	fileop.fAnyOperationsAborted	= FALSE;
	fileop.hNameMappings			= NULL;
	fileop.lpszProgressTitle		= NULL;
	
	int result = SHFileOperation(&fileop);
	return (0 == result) ? TRUE : FALSE;
}

void CReboot::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CReboot::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CReboot::Begin(IIocpContext* context)
{
	BOOL result = DoReboot(m_packet->option);

	scReboot packet;
	packet.result = result;

	SessionProc::SendPacket(
		context, 
		S_REBOOT,
		reinterpret_cast<uint8*>(&packet), 
		sizeof(packet));
}

BOOL CReboot::DoReboot(const uint32 option)
{
	TCHAR command[512];
	_stprintf_s(command, _countof(command), _T("/c \"shutdown.exe /f /r /t %lu\""), option);
	ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE); 
	return TRUE;
}

void CResource::Run(IIocpContext* context)
{
	if(!Ready(context)) 
		return;
	
	Begin(context);
}

BOOL CResource::Ready(IIocpContext* context)
{
	m_session = SessionProc::GetSession(context->GetEntity());
	if(!m_session) return FALSE;

	return TRUE;
}

void CResource::Begin(IIocpContext* context)
{
	UNITS cpus, mems, diskC, diskD, networks;
	DWORD error;
	
	m_perfomance.GetCPU(cpus, error);
	m_perfomance.GetMemory(mems, error);
	m_perfomance.GetDisk(_T("c:"), diskC, error);
	m_perfomance.GetDisk(_T("d:"), diskD, error);
	m_perfomance.GetNetwork(networks, error);

	scResource packet;
	packet.result = TRUE;
	packet.cpu		= cpus.size();
	packet.memory	= mems.size();
	packet.disk		= diskC.size() + diskD.size();
	packet.network	= networks.size();

	cSerialize::Reset();
	cSerialize::Write(reinterpret_cast<uint8*>(&packet), sizeof(packet));

	for(unsigned int i = 0 ; i < cpus.size() ; i++)
	{
		cSerialize::Write(static_cast<uint32>(cpus[i]));
	}
	for(unsigned int i = 0 ; i < mems.size() ; i++)
	{
		cSerialize::Write(static_cast<uint32>(mems[i]));
	}
	for(unsigned int i = 0 ; i < diskC.size() ; i++)
	{
		cSerialize::Write(static_cast<uint32>(diskC[i]));
	}
	for(unsigned int i = 0 ; i < diskD.size() ; i++)
	{
		cSerialize::Write(static_cast<uint32>(diskD[i]));
	}
	for(unsigned int i = 0 ; i < networks.size() ; i++)
	{
		cSerialize::Write(static_cast<uint32>(networks[i]));
	}

	SessionProc::SendPacket(
		context, 
		S_RESOURCE,
		cSerialize::GetBuffer(), 
		cSerialize::GetLength());
}
