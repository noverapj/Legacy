#include "stdafx.h"

#include "ioFTPThreadTaskMgr.h"

#include "../io3DEngine/ioRenderTexture.h"

ioFTPThreadTask::ioFTPThreadTask( ioHashString szServerAddress, DWORD dwPort, ioHashString szID, ioHashString szPassword, ioHashString szFileName, DWORD CreateID ) : 
	m_szServerAddress( szServerAddress ), m_dwPort( dwPort ), m_szID( szID ), m_szPassword( szPassword ), m_szFileName( szFileName ), m_dwCreateID( CreateID )
{
	m_CurrentState = FS_NONE;

	char szBuff[MAX_PATH];
	_fullpath( szBuff, m_szFileName.c_str(), MAX_PATH );
	m_szFileName = szBuff;
}

ioFTPThreadTask::~ioFTPThreadTask()
{
}

int  ioFTPThreadTask::GetPriority() const
{
	return 3;
}

void ioFTPThreadTask::Process()
{
	char szUploadFileName[MAX_PATH];

	//이름 생성 로직 or 지정된 이름 추가
	sprintf_s_e( szUploadFileName, "%s", "Test.jpg" );

	m_CurrentState = FS_UPLOAD;	
	FileUpLoad( szUploadFileName );
		
	if( ::remove( m_szFileName.c_str() ) != 0 && m_CurrentState == FS_UPLOAD_DONE )	
		LOG.PrintTimeAndLog(0 ,"ftp temp upload file remove failed", m_szFileName );
}

void ioFTPThreadTask::FileUpLoad( const char* szUploadFileName )
{
	HINTERNET hInternet	= NULL;
	HINTERNET hConnect	= NULL;

	m_CurrentState = FS_UPLOAD_FAIL;
	hInternet = InternetOpen("LostSaga", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if ( hInternet )
	{		
		hConnect = InternetConnect( hInternet, m_szServerAddress.c_str(), m_dwPort, m_szID.c_str(), m_szPassword.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL );
		if (hConnect)	
		{
			if( FtpPutFile( hConnect, m_szFileName.c_str(), szUploadFileName, FTP_TRANSFER_TYPE_BINARY, NULL ) )
			{				
				m_CurrentState = FS_UPLOAD_DONE;
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "file upload fail - not find file : %s", m_szFileName.c_str() );
			}
		}
	}

	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
}

void ioFTPThreadTask::Done()
{
	switch( m_CurrentState )
	{
	case FS_UPLOAD_DONE:
		g_FTPMgr.FileUpLoadDone( m_dwCreateID, ioFTPThreadTaskMgr::FTS_LOADED_OK );
		break;
	default :
		g_FTPMgr.FileUpLoadDone( m_dwCreateID, ioFTPThreadTaskMgr::FTS_LOADED_FAIL );
		break;
	}
}

bool ioFTPThreadTask::CreateFile()
{
	//파일 생성 or 파일 변환 작업등을 위한 함수
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ioMyCharShotTask::ioMyCharShotTask( ioHashString szServerAddress, DWORD dwPort, ioHashString szID, ioHashString szPassword, ioHashString szFileName, DWORD CreateID ) : 
 ioFTPThreadTask(szServerAddress, dwPort, szID, szPassword, szFileName, CreateID )
{
	m_pTextureSurf	= NULL;
	m_pUI3DRender	= NULL;

	m_pChar			= NULL;
}

ioMyCharShotTask::~ioMyCharShotTask()
{
	SAFEDELETE( m_pChar );
	SAFEDELETE( m_pUI3DRender );	
}

void ioMyCharShotTask::CharCreate( CharInfoData &rkInfo, CHARACTER pCharInfo )
{
	SAFEDELETE( m_pChar );
	m_pChar = m_pUI3DRender->CreateChar( pCharInfo, "LeaderShot", false );
	if( !m_pChar )	return;

	m_pChar->SetPosition( D3DXVECTOR3( 0.0f, -70.0f, 150.0f ) );
		
	float fAspect = (float)256/(float)256;
	m_pUI3DRender->SetProjection( FLOAT1, FLOAT1000, 45.0f, fAspect );

	CharEquipItem( rkInfo, pCharInfo );
	m_pChar->SetLoopAni( "public_delay.ani" );
	
}

void ioMyCharShotTask::CharEquipItem( CharInfoData &rkInfo, CHARACTER pCharInfo )
{
	if( !m_pChar )
		return;
		
	for( int i = 0; i < MAX_INVENTORY; i++ )
	{
		int nCostumeCode = pCharInfo.m_EquipCostume[i].m_CostumeCode;
		DWORD dwCostumeMaleCustom = pCharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
		DWORD dwCostumeFemaleCustom = pCharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

		if( rkInfo.m_EquipItemInfo[i].m_item_code != 0 )
		{
			if ( nCostumeCode <= 0 )
			{
				dwCostumeMaleCustom = rkInfo.m_EquipItemInfo[i].m_item_male_custom;
				dwCostumeFemaleCustom = rkInfo.m_EquipItemInfo[i].m_item_female_custom;
			}
			m_pChar->EquipItem( rkInfo.m_EquipItemInfo[i].m_item_code, dwCostumeMaleCustom, dwCostumeFemaleCustom, nCostumeCode );
		}
		else
		{
			if ( nCostumeCode <= 0 )
			{
				dwCostumeMaleCustom = 0;
				dwCostumeFemaleCustom = 0;
			}
			int iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + pCharInfo.m_class_type;
			m_pChar->EquipItem( iItemCode, 0, 0, nCostumeCode );
		}
	}
}

void ioMyCharShotTask::SetMyCharShot( CharInfoData &rkInfo, CHARACTER pCharInfo )
{
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "mycharshot_cam" );	
	CharCreate( rkInfo, pCharInfo );
}

bool ioMyCharShotTask::CreateFile()
{
	ioRenderSystem* m_pSystem = g_App.GetRenderSystem();
	D3DRECT rcD3D;
	rcD3D.x1 = 0;
	rcD3D.y1 = 0;
	rcD3D.x2 = 512;
	rcD3D.y2 = 512;

	D3DVIEWPORT9 kNewView, kOldView;
	kOldView = m_pSystem->GetViewPort();

	kNewView.X		= rcD3D.x1;
	kNewView.Y		= rcD3D.y1;
	kNewView.Width  = rcD3D.x2 - rcD3D.x1;
	kNewView.Height = rcD3D.y2 - rcD3D.y1;
	kNewView.MinZ	= 0.0f;
	kNewView.MaxZ	= FLOAT1;

	ioRenderTexture* pRenderTexture = g_TextureMgr.CreateRTT( "LeaderCharShot", 512, 512, D3DFMT_A8R8G8B8, D3DFMT_D16, false );
	if( !pRenderTexture )
	{
		LOG.PrintTimeAndLog( 0, "CreateRTT failed" );
		return false;
	}

	m_pSystem->SetRenderTarget( pRenderTexture );
	m_pSystem->SetViewPort( kNewView );

	m_pSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, &rcD3D );
	if( m_pSystem->BeginScene() )
	{
		m_pUI3DRender->RenderNow( NULL, false );
		m_pSystem->EndScene();
	}

	m_pTextureSurf = pRenderTexture->GetTextureSurface();
	HRESULT hr = D3DXSaveSurfaceToFile( m_szFileName.c_str(), D3DXIFF_JPG, m_pTextureSurf, NULL, NULL );
	m_pSystem->SetViewPort( kOldView );
	m_pSystem->RestoreRenderTarget();
	SAFEDELETE( m_pChar );

	if( FAILED(hr) )
	{
		LOG.PrintTimeAndLog( 0, "leader char create file failed" );
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template<> ioFTPThreadTaskMgr* Singleton< ioFTPThreadTaskMgr >::ms_Singleton = 0;
ioFTPThreadTaskMgr& ioFTPThreadTaskMgr::GetSingleton()
{
	return Singleton< ioFTPThreadTaskMgr >::GetSingleton();
}

void ioFTPThreadTaskMgr::Initialize( const char* szResourcePath )
{
	ioINILoader_e kLoader( "config/sp2_ftp_info.ini" );

	kLoader.SetTitle_e( "common" );
	
	char szBuffer[MAX_PATH];
	kLoader.LoadString_e( "sever_address", "", szBuffer, MAX_PATH );
	m_szServerAddress = szBuffer;

	kLoader.LoadString_e( "id", "", szBuffer, MAX_PATH );
	m_szID = szBuffer;

	kLoader.LoadString_e( "pw", "", szBuffer, MAX_PATH );
	m_szPassword = szBuffer;

	kLoader.LoadString_e( "port", "", szBuffer, MAX_PATH );
	m_dwPort = atoi( szBuffer );

	_fullpath( szBuffer, szResourcePath, MAX_PATH );
	m_szResourcePath = szBuffer;
}

ioFTPThreadTaskMgr::ioFTPThreadTaskMgr()
{
	m_dwTaskCreate	= 0;
	m_dwPort		= 0;
}

ioFTPThreadTaskMgr::~ioFTPThreadTaskMgr()
{
}

ioFTPThreadTask* ioFTPThreadTaskMgr::CreateFTPTask( ioHashString szFileName )
{
	m_TaskStatusMap.insert( TaskStatusMap::value_type( ++m_dwTaskCreate, FTS_LOADING ) );

	ioFTPThreadTask* Task = new ioFTPThreadTask( m_szServerAddress, m_dwPort, m_szID, m_szPassword, szFileName, m_dwTaskCreate );	
	Task->SetState( ioFTPThreadTask::FS_CREATE );
	m_ReserveTaskVec.push_back( Task );
	return Task;
}

ioMyCharShotTask* ioFTPThreadTaskMgr::CreateShotTask( ioHashString szFileName )
{
	m_TaskStatusMap.insert( TaskStatusMap::value_type( ++m_dwTaskCreate, FTS_LOADING ) );

	ioMyCharShotTask* Task = new ioMyCharShotTask( m_szServerAddress, m_dwPort, m_szID, m_szPassword, szFileName, m_dwTaskCreate );	

	Task->SetState( ioFTPThreadTask::FS_CREATE );
	m_ReserveTaskVec.push_back( Task );		
	return Task;
}

ioFTPThreadTaskMgr::FTPTASKSTATUS ioFTPThreadTaskMgr::GetTaskStatus( DWORD dwID )
{
	TaskStatusMap::iterator iter = m_TaskStatusMap.find( dwID );
	if( iter == m_TaskStatusMap.end() )
		return FTS_LOADED_FAIL;

	return iter->second;
}

void ioFTPThreadTaskMgr::FileUpLoadDone( DWORD dwID, ioFTPThreadTaskMgr::FTPTASKSTATUS State )
{
	TaskStatusMap::iterator iter = m_TaskStatusMap.find( dwID );
	if( iter == m_TaskStatusMap.end() )
		return;

	iter->second = State;
}

void ioFTPThreadTaskMgr::Process()
{
	ReserveTaskVec::iterator iter = m_ReserveTaskVec.begin();
	for( ; iter != m_ReserveTaskVec.end(); )
	{
		ioFTPThreadTask* pTask = *iter;
		if( pTask && pTask->CreateFile() )
		{
			g_ThreadTaskMgr.AddTask( pTask );
			iter = m_ReserveTaskVec.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}