#include "stdafx.h"

#include "ioGuildMarkMgr.h"

template<> ioGuildMarkMgr* Singleton< ioGuildMarkMgr >::ms_Singleton = 0;

class ioGuildMarkTask : public IThreadTask
{
protected:
	ioHashString m_FullName;
	ioHashString m_FullURL;
	DWORD m_dwIndex;
	DWORD m_dwMark;

	bool m_bDone;

public:
	virtual void Process();
	virtual void Done();

public:
	virtual int  GetPriority() const { return 1; }

public:
	ioGuildMarkTask( const char *szFullName,
					 const char *szFullURL,
					 DWORD dwGuildIndex,
					 DWORD dwGuildMark );
	virtual ~ioGuildMarkTask();
};

ioGuildMarkTask::ioGuildMarkTask( const char *szFullName,
								  const char *szFullURL,
								  DWORD dwGuildIndex,
								  DWORD dwGuildMark )
: m_FullName( szFullName ),
  m_FullURL( szFullURL )
{
	m_dwIndex = dwGuildIndex;
	m_dwMark  = dwGuildMark;
	m_bDone = false;
}

ioGuildMarkTask::~ioGuildMarkTask()
{
}

void ioGuildMarkTask::Process()
{
	HRESULT hr = ::URLDownloadToFile( 0,
									  m_FullURL.c_str(),
									  m_FullName.c_str(),
									  0,
									  NULL );

	if( FAILED(hr) )
		m_bDone = false;
	else
		m_bDone = true;
}

void ioGuildMarkTask::Done()
{
	g_GuildMarkMgr.GuildMarkLoadingDone( m_dwIndex, m_dwMark, m_bDone );
}

ioGuildMarkMgr::ioGuildMarkMgr()
{
	m_pBigMarkNone		 = NULL;

	m_iMaxReserveMarkIndex = 100;
	m_iMaxDefaultMarkSize  = 0;
}

ioGuildMarkMgr::~ioGuildMarkMgr()
{	
	SAFEDELETE( m_pBigMarkNone );

	ClearGuildMarkList();
	ClearGuildLevelRes();
}

ioGuildMarkMgr& ioGuildMarkMgr::GetSingleton()
{
	return Singleton< ioGuildMarkMgr >::GetSingleton();
}

bool ioGuildMarkMgr::IsAlreadyLoadData( DWORD dwGuildIndex )
{
	LoadingMarkList::iterator iter = std::find( m_LoadingMarkList.begin(),
												m_LoadingMarkList.end(),
												dwGuildIndex );

	if( iter != m_LoadingMarkList.end() )
		return true;

	return false;
}

void ioGuildMarkMgr::GuildMarkLoadingDone( DWORD dwGuildIndex, DWORD dwGuildMark, bool bDone )
{
	m_LoadingMarkList.remove( dwGuildIndex );

	// 메인스레드인데 여기서 로딩하면 어떨까..;
}

void ioGuildMarkMgr::AddLoadGuildMark( DWORD dwGuildIndex, DWORD dwGuildMark, bool bLarge )
{
	if( dwGuildIndex == 0 || m_szLoadingMarkURL.IsEmpty() )
		return;

	if( IsAlreadyLoadData( dwGuildIndex ) )
		return;

	char szFullName[MAX_PATH] = "";
	char szFullURL[MAX_PATH] = "";

	if( bLarge )
	{
		sprintf( szFullName, "%s/%d_%d_l.jpg", m_szAppDirectory.c_str(), dwGuildIndex, dwGuildMark );
		sprintf( szFullURL, "%s%d_%d_l.jpg", m_szLoadingMarkURL.c_str(), dwGuildIndex, dwGuildMark );
	}
	else
	{
		sprintf( szFullName, "%s/%d_%d_s.jpg", m_szAppDirectory.c_str(), dwGuildIndex, dwGuildMark );
		sprintf( szFullURL, "%s%d_%d_s.jpg", m_szLoadingMarkURL.c_str(), dwGuildIndex, dwGuildMark );
	}

	ioGuildMarkTask *pTask = new ioGuildMarkTask( szFullName,
												  szFullURL,
												  dwGuildIndex,
												  dwGuildMark );
	if( pTask )
	{
		m_LoadingMarkList.push_back( dwGuildIndex );
		g_ThreadTaskMgr.AddTask( pTask );
	}
}

void ioGuildMarkMgr::ClearGuildMarkList()
{
	GuildMarkMap::iterator iCreate;
	for( iCreate = m_GuildSmallMarkMap.begin() ; iCreate != m_GuildSmallMarkMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GuildSmallMarkMap.clear();

	for( iCreate = m_GuildBigMarkMap.begin() ; iCreate != m_GuildBigMarkMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GuildBigMarkMap.clear();

	vCustomMarkData_Iter iter,iEnd;

	iEnd = m_CustomBigMarkList.end();
	for(iter = m_CustomBigMarkList.begin();iter != iEnd;iter++)
	{
		CustomMarkData *pCustom = *iter;
		m_CustomBigMarkPool.push_back( pCustom );
	}
	m_CustomBigMarkList.clear();

	iEnd = m_CustomBigMarkPool.end();
	for(iter = m_CustomBigMarkPool.begin();iter != iEnd;iter++)
	{
		CustomMarkData *pCustom = *iter;
		SAFEDELETE( pCustom->m_pCustomMark );
		SAFEDELETE( pCustom );
	}
	m_CustomBigMarkPool.clear();	

	iEnd = m_CustomSmallMarkList.end();
	for(iter = m_CustomSmallMarkList.begin();iter != iEnd;iter++)
	{
		CustomMarkData *pCustom = *iter;
		m_CustomSmallMarkPool.push_back( pCustom );
	}
	m_CustomSmallMarkList.clear();

	iEnd = m_CustomSmallMarkPool.end();
	for(iter = m_CustomSmallMarkPool.begin();iter != iEnd;iter++)
	{
		CustomMarkData *pCustom = *iter;
		SAFEDELETE( pCustom->m_pCustomMark );
		SAFEDELETE( pCustom );
	}
	m_CustomSmallMarkPool.clear();	
}

void ioGuildMarkMgr::ClearGuildLevelRes()
{
	GuildLevelResMap::iterator iCreate;
	for( iCreate = m_GuildLevelResBlueMap.begin() ; iCreate != m_GuildLevelResBlueMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GuildLevelResBlueMap.clear();

	for( iCreate = m_GuildLevelResRedMap.begin() ; iCreate != m_GuildLevelResRedMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GuildLevelResRedMap.clear();
}

void ioGuildMarkMgr::Initialize()
{
	ClearGuildMarkList();
	ClearGuildLevelRes();

	ioINILoader_e kLoader( "config/sp2_guild.ini" );
	kLoader.SetTitle_e( "resource" );
	m_iMaxReserveMarkIndex = kLoader.LoadInt_e( "max_reserve_mark_index", 100 );

	m_iMaxDefaultMarkSize = kLoader.LoadInt_e( "max_mark",  0 );
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	int i;
	for(i = 0;i < m_iMaxDefaultMarkSize;i++)
	{
		sprintf_e( szKey, "def_%d_small", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		CreateGuildSmallMark( i, szBuf );

		sprintf_e( szKey, "def_%d_big", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		CreateGuildBigMark( i, szBuf );
	}	

	SAFEDELETE( m_pBigMarkNone );
	kLoader.LoadString_e( "big_none", "", szBuf, MAX_PATH );
	m_pBigMarkNone		 = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	if( m_pBigMarkNone )
	{
		m_pBigMarkNone->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBigMarkNone->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	}

	//경로 저장
	char szAppDir[MAX_PATH] = "";
	::GetCurrentDirectory( MAX_PATH, szAppDir );
	sprintf_e( szBuf, "%s/Guild", szAppDir );
	m_szAppDirectory = szBuf;
	::CreateDirectory( m_szAppDirectory.c_str(), NULL );

	// Guild Mark Loading
	kLoader.SetTitle_e( "custom" );
	kLoader.LoadString_e( "load_url", "", szBuf, MAX_PATH );
	m_szLoadingMarkURL = szBuf;

	//텍스쳐 풀 생성
	int iMaxMark = kLoader.LoadInt_e( "large_custom_count", 0 );
	for(i = 0;i < iMaxMark;i++)
	{
		CustomMarkData *pCustomData = new CustomMarkData;
		sprintf( szBuf, "Guildmark_Large#CL%.3d", i + 1 );
		pCustomData->m_pCustomMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );
		if( pCustomData->m_pCustomMark )
		{
			pCustomData->m_pCustomMark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			pCustomData->m_pCustomMark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_CustomBigMarkPool.push_back( pCustomData );
		}
		else
			SAFEDELETE( pCustomData );
	}

	iMaxMark = kLoader.LoadInt_e( "small_custom_count", 0 );
	for(i = 0;i < iMaxMark;i++)
	{
		CustomMarkData *pCustomData = new CustomMarkData;
		sprintf( szBuf, "Guildmark_Small#CS%.3d", i + 1 );
		pCustomData->m_pCustomMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );
		if( pCustomData->m_pCustomMark )
		{
			m_CustomSmallMarkPool.push_back( pCustomData );
		}
		else
			SAFEDELETE( pCustomData );
	}	

	//길드 레벨 마크
	kLoader.SetTitle_e( "guild_entry" );
	int iMaxEntry = kLoader.LoadInt_e( "entry_count", 0 );
	for(int i = 0;i < iMaxEntry;i++)
	{
		// 레벨 이미지
		sprintf_e( szKey, "entry_res_blue_%d", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_GuildLevelResBlueMap.insert( GuildLevelResMap::value_type( i, g_UIImageSetMgr.CreateImageByFullName( szBuf ) ) );

		sprintf_e( szKey, "entry_res_red_%d", i );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_GuildLevelResRedMap.insert( GuildLevelResMap::value_type( i, g_UIImageSetMgr.CreateImageByFullName( szBuf ) ) );
	}
}

void ioGuildMarkMgr::CreateGuildSmallMark( DWORD dwGuildMark, const ioHashString &szMarkName )
{
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szMarkName );
	m_GuildSmallMarkMap.insert( GuildMarkMap::value_type( dwGuildMark, pImg ) );
}

void ioGuildMarkMgr::CreateGuildBigMark( DWORD dwGuildMark, const ioHashString &szMarkName )
{
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szMarkName );		
	if( pImg )
	{
		pImg->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		pImg->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
	}
	m_GuildBigMarkMap.insert( GuildMarkMap::value_type( dwGuildMark, pImg ) );
}

ioUIRenderImage *ioGuildMarkMgr::GetGuildLevelRes( DWORD dwGuildLevel )
{
	GuildLevelResMap::iterator iter = m_GuildLevelResBlueMap.find( dwGuildLevel );
	if( iter != m_GuildLevelResBlueMap.end() )
		return iter->second;
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetSmallMarkImage( DWORD dwGuildIndex,  DWORD dwGuildMark )
{
	if( dwGuildIndex == 0 )
		return NULL;

	GuildMarkMap::iterator iter = m_GuildSmallMarkMap.find( dwGuildMark );
	if( iter != m_GuildSmallMarkMap.end() )
		return iter->second;
	else if( dwGuildMark <= (DWORD)GetMaxReserveMarkIndex() )
	{
		LOG.PrintTimeAndLog( 0, "ioGuildMarkMgr::GetSmallMarkImage Not Image : %d - %d", dwGuildIndex, dwGuildMark );
		return NULL;
	}

	// 커스텀 마크
	return GetSmallMarkImageCustom( dwGuildIndex, dwGuildMark );
}

ioUIRenderImage *ioGuildMarkMgr::GetSmallMarkImageDef( DWORD dwGuildMark )
{
	GuildMarkMap::iterator iter = m_GuildSmallMarkMap.find( dwGuildMark );
	if( iter != m_GuildSmallMarkMap.end() )
		return iter->second;
	else
	{
		LOG.PrintTimeAndLog( 0, "ioGuildMarkMgr::GetSmallMarkImageDef Not Image : %d", dwGuildMark );
	}

	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetSmallMarkImageCustom( DWORD dwGuildIndex, DWORD dwGuildMark )
{	
	{//텍스쳐에 있는지 확인
		ioUIRenderImage *pRenderImage = GetSmallMarkImageCustomList( dwGuildIndex, dwGuildMark );
		if( pRenderImage )
			return pRenderImage;
	}
	
	{//파일로 존재하는지 확인
		
		//전체경로
		char szBuf[MAX_PATH] = "";
		sprintf( szBuf, "%s/%d_%d_s.jpg", m_szAppDirectory.c_str(), dwGuildIndex, dwGuildMark );

		FILE *pFile = fopen( szBuf, "r" );
		if( pFile == NULL ) 
		{
			//다운로드 받아야한다.
			AddLoadGuildMark( dwGuildIndex, dwGuildMark, false );
			return NULL;
		}
		fclose( pFile );
	}

	{//텍스쳐 생성
		ioUIRenderImage *pRenderImage = GetSmallMarkImageCustomCreate( dwGuildIndex, dwGuildMark );
		if( pRenderImage )
			return pRenderImage;
	}
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetSmallMarkImageCustomList( DWORD dwGuildIndex, DWORD dwGuildMark )
{
	vCustomMarkData_Iter iter, iEnd;
	iEnd = m_CustomSmallMarkList.end();
	for(iter = m_CustomSmallMarkList.begin();iter != iEnd;iter++)
	{
		CustomMarkData *pCustom = *iter;
		if( pCustom->m_dwGuildIndex == dwGuildIndex )
		{
			if( pCustom->m_dwGuildMark == dwGuildMark )
			{
				m_CustomSmallMarkList.erase( iter );
				m_CustomSmallMarkList.push_back( pCustom );        //항상 마지막에 사용한 마크를 넣어준다.
				return pCustom->m_pCustomMark;
			}
			else        //마크가 변경되었다
			{
				m_CustomSmallMarkList.erase( iter );

				pCustom->Init();
				m_CustomSmallMarkPool.push_back( pCustom );
				return NULL;
			}
		}
	}
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetSmallMarkImageCustomCreate( DWORD dwGuildIndex, DWORD dwGuildMark )
{
	CustomMarkData *pCustom = NULL;
	if( m_CustomSmallMarkPool.empty() )     //리스트에서 하나 반환
	{
		if( m_CustomSmallMarkList.empty() )
			return NULL;

		vCustomMarkData_Iter iter = m_CustomSmallMarkList.begin();
		pCustom = *iter;
		m_CustomSmallMarkList.erase( iter );
		m_CustomSmallMarkList.push_back( pCustom );
	}
	else
	{
		vCustomMarkData_Iter iter = m_CustomSmallMarkPool.begin();
		pCustom = *iter;
		m_CustomSmallMarkPool.erase( iter );
		m_CustomSmallMarkList.push_back( pCustom );
	}

	if( pCustom )
	{
		pCustom->Init();
		pCustom->m_dwGuildIndex = dwGuildIndex;
		pCustom->m_dwGuildMark  = dwGuildMark;
		if( pCustom->m_pCustomMark  )
		{
			char szBuf[MAX_PATH] = "";
			sprintf( szBuf, "%s/%d_%d_s.jpg", m_szAppDirectory.c_str(), dwGuildIndex, dwGuildMark );
			pCustom->m_pCustomMark->FillFileImage( szBuf, 16, 16 );
			return pCustom->m_pCustomMark;
		}
	}	
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetBigMarkImage( DWORD dwGuildIndex,  DWORD dwGuildMark )
{
	if( dwGuildIndex == 0 )
		return m_pBigMarkNone;

	GuildMarkMap::iterator iter = m_GuildBigMarkMap.find( dwGuildMark );
	if( iter != m_GuildBigMarkMap.end() )
		return iter->second;
	else if( dwGuildMark <= (DWORD)GetMaxReserveMarkIndex() )
	{
		LOG.PrintTimeAndLog( 0, "ioGuildMarkMgr::GetBigMarkImage Not Image : %d - %d", dwGuildIndex, dwGuildMark );
		return NULL;
	}
	// 커스텀 마크
	return GetBigMarkImageCustom( dwGuildIndex, dwGuildMark );
}

ioUIRenderImage *ioGuildMarkMgr::GetBigMarkImageDef( DWORD dwGuildMark )
{
	GuildMarkMap::iterator iter = m_GuildBigMarkMap.find( dwGuildMark );
	if( iter != m_GuildBigMarkMap.end() )
		return iter->second;
	else
	{
		LOG.PrintTimeAndLog( 0, "ioGuildMarkMgr::GetBigMarkImageDef Not Image : %d", dwGuildMark );
	}
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetBigMarkImageCustom( DWORD dwGuildIndex, DWORD dwGuildMark )
{
	{//텍스쳐에 있는지 확인
		ioUIRenderImage *pRenderImage = GetBigMarkImageCustomList( dwGuildIndex, dwGuildMark );
		if( pRenderImage )
			return pRenderImage;
	}

	{//파일로 존재하는지 확인

		//전체경로
		char szBuf[MAX_PATH] = "";
		sprintf( szBuf, "%s/%d_%d_l.jpg", m_szAppDirectory.c_str(), dwGuildIndex, dwGuildMark );

		FILE *pFile = fopen( szBuf, "r" );
		if( pFile == NULL ) 
		{
			//다운로드 받아야한다.
			AddLoadGuildMark( dwGuildIndex, dwGuildMark, true );
			return NULL;
		}
		fclose( pFile );
	}
	
	{//텍스쳐 생성
		ioUIRenderImage *pRenderImage = GetBigMarkImageCustomCreate( dwGuildIndex, dwGuildMark );
		if( pRenderImage )
			return pRenderImage;
	}
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetBigMarkImageCustomList( DWORD dwGuildIndex, DWORD dwGuildMark )
{
	vCustomMarkData_Iter iter, iEnd;
	iEnd = m_CustomBigMarkList.end();
	for(iter = m_CustomBigMarkList.begin();iter != iEnd;iter++)
	{
		CustomMarkData *pCustom = *iter;
		if( pCustom->m_dwGuildIndex == dwGuildIndex )
		{
			if( pCustom->m_dwGuildMark == dwGuildMark )
			{
				m_CustomBigMarkList.erase( iter );
				m_CustomBigMarkList.push_back( pCustom );        //항상 마지막에 사용한 마크를 넣어준다.
				return pCustom->m_pCustomMark;
			}
			else        //마크가 변경되었다
			{
				m_CustomBigMarkList.erase( iter );

				pCustom->Init();
				m_CustomBigMarkPool.push_back( pCustom );
				return NULL;
			}
		}
	}
	return NULL;
}

ioUIRenderImage *ioGuildMarkMgr::GetBigMarkImageCustomCreate( DWORD dwGuildIndex, DWORD dwGuildMark )
{
	CustomMarkData *pCustom = NULL;
	if( m_CustomBigMarkPool.empty() )     //리스트에서 하나 반환
	{
		if( m_CustomBigMarkList.empty() )
			return NULL;

		vCustomMarkData_Iter iter = m_CustomBigMarkList.begin();
		pCustom = *iter;
		m_CustomBigMarkList.erase( iter );
		m_CustomBigMarkList.push_back( pCustom );
	}
	else
	{
		vCustomMarkData_Iter iter = m_CustomBigMarkPool.begin();
		pCustom = *iter;
		m_CustomBigMarkPool.erase( iter );
		m_CustomBigMarkList.push_back( pCustom );
	}

	if( pCustom )
	{
		pCustom->Init();
		pCustom->m_dwGuildIndex = dwGuildIndex;
		pCustom->m_dwGuildMark  = dwGuildMark;
		if( pCustom->m_pCustomMark  )
		{
			char szBuf[MAX_PATH] = "";
			sprintf( szBuf, "%s/%d_%d_l.jpg", m_szAppDirectory.c_str(), dwGuildIndex, dwGuildMark );
			pCustom->m_pCustomMark->FillFileImage( szBuf, 128, 128 );
			return pCustom->m_pCustomMark;
		}
	}	
	return NULL;
}

int ioGuildMarkMgr::GetGuildSmallMarkSize( DWORD dwGUildIndex, DWORD dwGuildMark )
{
	if( dwGUildIndex == 0 )
		return 0;

	ioUIRenderImage *pRenderImage = GetSmallMarkImageDef( 0 );
	if( !pRenderImage )
		return 0;
	return pRenderImage->GetWidth();
}

void ioGuildMarkMgr::RenderSmallMark( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetSmallMarkImage( dwGuildIndex, dwGuildMark );
	if( pRenderImage )
		pRenderImage->Render( fXPos, fYPos, eRenderType );
}

void ioGuildMarkMgr::RenderSmallMarkDefault( DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetSmallMarkImageDef( dwGuildMark );
	if( pRenderImage )
		pRenderImage->Render( fXPos, fYPos, eRenderType );
}

void ioGuildMarkMgr::RenderSmallMarkOutImg( ioUIRenderImage *m_pImg, float fXPos, float fYPos, UIRenderType eRenderType /*= UI_RENDER_NORMAL */ )
{
	if( m_pImg )
		m_pImg->Render( fXPos, fYPos, eRenderType );
}

void ioGuildMarkMgr::RenderMarkSize( int iSizeW, int iSizeH, DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */)
{
	ioUIRenderImage *pRenderImage = GetBigMarkImage( dwGuildIndex, dwGuildMark );
	if( pRenderImage )
	{
		pRenderImage->SetSize( iSizeW, iSizeH );
		pRenderImage->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}

void ioGuildMarkMgr::RenderMark56x56( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetBigMarkImage( dwGuildIndex, dwGuildMark );
	if( pRenderImage )
	{
		pRenderImage->SetSize( 56, 56 );
		pRenderImage->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}

void ioGuildMarkMgr::RenderMark71x71( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetBigMarkImage( dwGuildIndex, dwGuildMark );
	if( pRenderImage )
	{
		pRenderImage->SetSize( 71, 71 );
		pRenderImage->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}

void ioGuildMarkMgr::RenderMark86x86( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetBigMarkImage( dwGuildIndex, dwGuildMark );
	if( pRenderImage )
	{
		pRenderImage->SetSize( 86, 86 );
		pRenderImage->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}

void ioGuildMarkMgr::RenderMark96x96( DWORD dwGuildIndex, DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetBigMarkImage( dwGuildIndex, dwGuildMark );
	if( pRenderImage )
	{
		pRenderImage->SetSize( 96, 96 );
		pRenderImage->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}

void ioGuildMarkMgr::RenderMark80x80Default( DWORD dwGuildMark, float fXPos, float fYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */ )
{
	ioUIRenderImage *pRenderImage = GetBigMarkImageDef( dwGuildMark );
	if( pRenderImage )
	{
		pRenderImage->SetSize( 80, 80 );
		pRenderImage->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}

void ioGuildMarkMgr::RenderMark96x96OutImg( ioUIRenderImage *m_pImg, float fXPos, float fYPos, UIRenderType eRenderType /*= UI_RENDER_NORMAL */ )
{
	if( m_pImg )
	{
		m_pImg->SetSize( 96, 96 );
		m_pImg->Render( fXPos, fYPos, eRenderType, TFO_BILINEAR );
	}
}