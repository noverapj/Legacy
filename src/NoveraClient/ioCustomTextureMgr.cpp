#include "stdafx.h"

#include "ioCustomTextureMgr.h"
#include "ioCreateChar.h"

//D3DXSaveTextureToFile
class ioCustomTextureTask : public IThreadTask
{
protected:
	ioHashString m_FullName;
	ioHashString m_FullURL;

protected:
	TexDownLoadData m_DownData;

protected:
	bool m_bDone;

public:
	virtual void Process();
	virtual void Done();

public:
	virtual int  GetPriority() const { return 3; }

public:
	ioCustomTextureTask( const char *szFullName, const char *szFullURL, TexDownLoadData &kData );
	virtual ~ioCustomTextureTask();
};

ioCustomTextureTask::ioCustomTextureTask( const char *szFullName, const char *szFullURL, TexDownLoadData &kData ) : m_FullName( szFullName ), m_FullURL( szFullURL )
{
	m_DownData = kData;
	m_bDone    = false;

	//LOG.PrintTimeAndLog( 0, "ioCustomTextureTask URL : %s", szFullURL );
	//LOG.PrintTimeAndLog( 0, "ioCustomTextureTask Path : %s", szFullName );
}

ioCustomTextureTask::~ioCustomTextureTask()
{
}

void ioCustomTextureTask::Process()
{
	HRESULT hr = ::URLDownloadToFile( 0, m_FullURL.c_str(), m_FullName.c_str(),	0, NULL );

	if( FAILED(hr) )
		m_bDone = false;
	else
		m_bDone = true;
}

void ioCustomTextureTask::Done()
{
	g_CustomTextureMgr.FileDownLoadingDone( m_DownData );
}

template<> ioCustomTextureMgr* Singleton< ioCustomTextureMgr >::ms_Singleton = 0;
ioCustomTextureMgr::ioCustomTextureMgr()
{
	m_pPlayStage    = NULL;
}

ioCustomTextureMgr::~ioCustomTextureMgr()
{	
	m_DownDataList.clear();
}

ioCustomTextureMgr& ioCustomTextureMgr::GetSingleton()
{
	return Singleton< ioCustomTextureMgr >::GetSingleton();
}

void ioCustomTextureMgr::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_custom_texture.ini" );

	char szBuf[MAX_PATH];
	{
		kLoader.SetTitle_e( "common" );

		kLoader.LoadString_e( "storage_url", "", szBuf, MAX_PATH );
		m_szStorageURL = szBuf;

		kLoader.SetTitle_e( "extra_item" );
		int iMaxItem = kLoader.LoadInt_e( "item_cnt", 0 );
		for(int i = 0;i < iMaxItem;i++)
		{
			char szBuf[MAX_PATH] = "";
			char szKey[MAX_PATH] = "";
			TrainExtraItem kData;

			sprintf_e( szKey, "item%d_code", i + 1 );
			kData.m_dwItemCode = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "item%d_name", i + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kData.m_szItemName = szBuf;

			sprintf_e( szKey, "item%d_icon", i + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kData.m_szItemIcon = szBuf;

			m_TrainExtraItemList.push_back( kData );
		}
	}

	//경로 저장
	char szAppDir[MAX_PATH] = "";
	::GetCurrentDirectory( MAX_PATH, szAppDir );

	sprintf( szBuf, "%s/resource/", szAppDir );
	::CreateDirectory( szBuf, NULL );

	sprintf( szBuf, "%s/resource/texture/", szAppDir );
	::CreateDirectory( szBuf, NULL );
	m_szAppDirectory = szBuf;
}

bool ioCustomTextureMgr::IsFileCheck( const ioHashString &rkFileName )
{
	FILE *pFile = fopen( rkFileName.c_str(), "r" );
	if( pFile ) 
	{
		fclose( pFile );		
		return true;
	}
	return false;
}

bool ioCustomTextureMgr::IsExistenceFile( const ioHashString &rkOwnerName, const ioHashString &rkDstTexName, DWORD dwCustomIndex, DWORD dwSubIndex )
{
	if( rkOwnerName.IsEmpty() ) return false;
	if( rkDstTexName.IsEmpty() ) return false;
	if( dwCustomIndex == 0 || dwSubIndex == 0 ) return false;

	// 파일이 존재하는지 확인
	char szTextureDir[MAX_PATH] = "";
	sprintf( szTextureDir, "%s%d_%d.lsc", m_szAppDirectory.c_str(), dwCustomIndex, dwSubIndex );
	if( IsFileCheck( szTextureDir ) ) return true;
		
	TexDownLoadData kDownLoad = { rkOwnerName, rkDstTexName, dwCustomIndex, dwSubIndex };
	if( IsDownLoading( kDownLoad ) ) return false;

	// 저장소 URL 생성
	char szStorageURL[MAX_PATH] = "";
	sprintf( szStorageURL, "%s/%d_%d.lsc", m_szStorageURL.c_str(), dwCustomIndex, dwSubIndex );

	ioCustomTextureTask *pTask = new ioCustomTextureTask( szTextureDir, szStorageURL, kDownLoad );
	if( pTask )
	{
		m_DownDataList.push_back( kDownLoad );
		g_ThreadTaskMgr.AddTask( pTask );
	}
	return false;
}

bool ioCustomTextureMgr::IsDownLoading( TexDownLoadData &rkData )
{
	TexDownLoadDataList::iterator iter = std::find( m_DownDataList.begin(), m_DownDataList.end(), rkData );

	if( iter != m_DownDataList.end() )
		return true;

	return false;
}

void ioCustomTextureMgr::FileDownLoadingDone( TexDownLoadData &rkData )
{
	m_DownDataList.remove( rkData );

//	LOG.PrintTimeAndLog( 0, "FileDownLoadingDone : %s - %s - %d - %d", rkData.m_szOwnerName.c_str(), rkData.m_szDstTexName.c_str(), 
//							rkData.m_dwCustomIndex, rkData.m_dwSubIndex );
	// 파일 다운로드가 완료되면 여기서 처리한다.
	
	// UI 캐릭터 처리.
	if( rkData.m_szOwnerName == g_MyInfo.GetPublicID() )
	{
		for(int i = 0;i < (int)m_dwUICharPtrList.size();i++)
		{
			ioCreateChar *pUIChar = (ioCreateChar *)m_dwUICharPtrList[i];
			if( pUIChar )
			{
				pUIChar->CustomTextureDownLoadComplete( rkData.m_szDstTexName, rkData.m_dwCustomIndex, rkData.m_dwSubIndex );
			}
		}
	}
	
	// 필드 캐릭터 처리
	if( m_pPlayStage )
	{
		ioBaseChar *pBaseChar = m_pPlayStage->GetBaseChar( rkData.m_szOwnerName );
		if( pBaseChar )
		{
			pBaseChar->CustomTextureDownLoadComplete( rkData.m_szDstTexName, rkData.m_dwCustomIndex, rkData.m_dwSubIndex );
		}
	}
}

void ioCustomTextureMgr::CreateNewUIChar( DWORD dwCharPtr )
{
	m_dwUICharPtrList.push_back( dwCharPtr );
}

void ioCustomTextureMgr::DestroyUIChar( DWORD dwCharPtr )
{
	for(int i = 0;i < (int)m_dwUICharPtrList.size();i++)
	{
		if( m_dwUICharPtrList[i] == dwCharPtr )
		{
			m_dwUICharPtrList.erase( m_dwUICharPtrList.begin() + i );
			break;
		}
	}
}

bool ioCustomTextureMgr::IsUICharPtrCheck( DWORD dwCharPtr )
{
	for(int i = 0;i < (int)m_dwUICharPtrList.size();i++)
	{
		if( m_dwUICharPtrList[i] == dwCharPtr )
		{
			return true;
		}
	}
	return false;
}

void ioCustomTextureMgr::CompleteCustomItem( DWORD dwItemCode, DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	if( dwItemCode == 0 ) return;

	for(int i = 0;i < (int)m_dwUICharPtrList.size();i++)
	{
		ioCreateChar *pUICharacter = (ioCreateChar *)m_dwUICharPtrList[i];
		if( !pUICharacter ) continue;

		pUICharacter->ReloadEquipItem( dwItemCode, dwMaleCustom, dwFemaleCustom );
	}
}

DWORD ioCustomTextureMgr::GetTrainExtraItem( int iArray )
{
	if( COMPARE( iArray, 0, (int)m_TrainExtraItemList.size() ) )
		return m_TrainExtraItemList[iArray].m_dwItemCode;
	return 0;
}

const ioHashString &ioCustomTextureMgr::GetTrainExtraItemName( DWORD dwItemCode )
{
	static ioHashString szReturn = "None";
	for(int i = 0;i < (int)m_TrainExtraItemList.size();i++)
	{
		if( m_TrainExtraItemList[i].m_dwItemCode == dwItemCode )
			return m_TrainExtraItemList[i].m_szItemName;
	}
	return szReturn;
}

const ioHashString &ioCustomTextureMgr::GetTrainExtraItemIcon( DWORD dwItemCode )
{
	static ioHashString szReturn = "None";
	for(int i = 0;i < (int)m_TrainExtraItemList.size();i++)
	{
		if( m_TrainExtraItemList[i].m_dwItemCode == dwItemCode )
			return m_TrainExtraItemList[i].m_szItemIcon;
	}
	return szReturn;
}
