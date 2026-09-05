#include "stdafx.h"

#include "../io3DEngine/ioWaveFile.h"

#include "ioCustomSoundMgr.h"

#include "Crc32Static.h"

CustomSoundData::CustomSoundData()
{
	m_pData        = NULL;
	m_dwSize       = 0;
	m_dwUnpacFileCRC = 0;
}

CustomSoundData::~CustomSoundData()
{
	Clear();
}

void CustomSoundData::Clear()
{
	SAFEDELETE( m_pData );
	m_dwSize         = 0;
	m_dwUnpacFileCRC = 0;
	m_vNetworkInfoList.clear();
	m_szKeyWord.Clear();
}

void CustomSoundData::ClearNetworkInfo()
{
	m_vNetworkInfoList.clear();
}

DWORD CustomSoundData::GetCurData( OUT CustomSoundPacket& rData , IN DWORD dwDataPos )
{
	if( dwDataPos >= m_dwSize ) return 0;

	DWORD dwCpySize = min( CUSTOM_SOUND_PACKET_SIZE, m_dwSize - dwDataPos );
	memcpy( rData.m_Data, &m_pData[dwDataPos], dwCpySize);
	rData.m_dwWrittenSize = dwCpySize;
	dwDataPos += dwCpySize;

	return dwDataPos;
}

bool CustomSoundData::OpenAndPac( const char *szFileName , DWORD dwCRC )
{
	SAFEDELETE( m_pData );

	if( !g_ResourceLoader.PacPieceFileDataInMem( szFileName, m_dwSize) )
		return false;
	
	m_pData = new BYTE[m_dwSize];
	if( !m_pData )
		return false;

	if( !g_ResourceLoader.GetPacPieceFileData( m_pData, m_dwSize ) )
		return false;	

	m_dwUnpacFileCRC = dwCRC;
	m_szFilePath     = szFileName;
	return true;
}

bool CustomSoundData::UnpacAndSave( const char *szFileName )
{
	return g_ResourceLoader.UnPacPieceFileByMem( szFileName, m_pData, m_dwSize );
}

void CustomSoundData::AddNetworkInfo( DWORD dwUserIndex )
{
	for(NetworkInfoList::iterator iter = m_vNetworkInfoList.begin(); iter != m_vNetworkInfoList.end(); ++iter)
	{
	    NetworkInfo &rInfo = *iter;
		if( rInfo.m_dwUserIndex == dwUserIndex )
			return;
	}

	NetworkInfo kInfo;
	kInfo.m_dwUserIndex = dwUserIndex;
	kInfo.m_dwDataPos   = 0;
	m_vNetworkInfoList.push_back( kInfo );
}

void CustomSoundData::DeleteNetworkInfo( DWORD dwUserIndex )
{
	for(NetworkInfoList::iterator iter = m_vNetworkInfoList.begin(); iter != m_vNetworkInfoList.end(); ++iter)
	{
		NetworkInfo &rInfo = *iter;
		if( rInfo.m_dwUserIndex == dwUserIndex )
		{
			m_vNetworkInfoList.erase( iter );
			return;
		}
	}
}


void CustomSoundData::SendDataToAll( DWORD dwIndex )
{
	for(NetworkInfoList::iterator iter = m_vNetworkInfoList.begin(); iter != m_vNetworkInfoList.end(); ++iter)
	{
		NetworkInfo &rInfo = *iter;
		CustomSoundPacket kData;
		DWORD dwDataPos = 0;
		dwDataPos = GetCurData( kData, rInfo.m_dwDataPos );
		if( rInfo.m_dwDataPos < dwDataPos )
			rInfo.m_dwDataPos = dwDataPos;
		if( kData.m_dwWrittenSize == 0 )
			continue;
		// send
		SP2Packet kPacket(CUPK_CUSTOM_SOUND_DATA);
		kPacket << g_MyInfo.GetUserIndex();
		kPacket << dwIndex;
		kPacket << kData; 
		P2PNetwork::SendToUserByIndex( rInfo.m_dwUserIndex, kPacket );
	}
}

void CustomSoundData::CheckAndDeleteNetworkInfo()
{
	NetworkInfoList::iterator iter = m_vNetworkInfoList.begin();
	while ( iter != m_vNetworkInfoList.end() )
	{
		NetworkInfo &rInfo = *iter;
		if( rInfo.m_dwDataPos < m_dwSize )
		{
			++iter;
		}
		else
		{
			iter = m_vNetworkInfoList.erase( iter );
		}
	}
}

DWORD CustomSoundData::SetCurData( IN const CustomSoundPacket &rData, IN DWORD dwDataPos )
{
	if( dwDataPos >= m_dwSize ) return 0;

	DWORD dwCpySize = min( rData.m_dwWrittenSize, m_dwSize - dwDataPos );
	memcpy( &m_pData[dwDataPos], rData.m_Data, dwCpySize);

	dwDataPos += dwCpySize;
	return dwDataPos;
}

void CustomSoundData::NewData( DWORD dwSize )
{
	Clear();
	m_pData  = new BYTE[dwSize];
	m_dwSize =  dwSize;
}

DWORD CustomSoundData::GetNetworkDataPos( DWORD dwUserIndex )
{
	for(NetworkInfoList::iterator iter = m_vNetworkInfoList.begin(); iter != m_vNetworkInfoList.end(); ++iter)
	{
		NetworkInfo &rInfo = *iter;
		if( rInfo.m_dwUserIndex == dwUserIndex )
			return rInfo.m_dwDataPos;
	}

	return 0;
}

void CustomSoundData::SetNetworkDataPos( DWORD dwUserIndex, DWORD dwDataPos )
{
	for(NetworkInfoList::iterator iter = m_vNetworkInfoList.begin(); iter != m_vNetworkInfoList.end(); ++iter)
	{
		NetworkInfo &rInfo = *iter;
		if( rInfo.m_dwUserIndex == dwUserIndex )
		{
			rInfo.m_dwDataPos = dwDataPos;
		}
	}
}

bool CustomSoundData::IsCompletedData( DWORD dwUserIndex )
{
	for(NetworkInfoList::iterator iter = m_vNetworkInfoList.begin(); iter != m_vNetworkInfoList.end(); ++iter)
	{
		NetworkInfo &rInfo = *iter;
		if( rInfo.m_dwUserIndex == dwUserIndex )
		{
			if( rInfo.m_dwDataPos == m_dwSize )
				return true;
			else
				return false;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
template<> ioCustomSoundMgr* Singleton< ioCustomSoundMgr >::ms_Singleton = 0;

char g_szFilterChar[]={'\t',' ','_','-','.',':','^','/','\\',29,28,NULL}; /* 29:ctrl+] , 28: ctrl+\ */

ioCustomSoundMgr::ioCustomSoundMgr(void)
{
	m_iMaxCustomData    = 0;
	m_dwDelayTimePlayMy = 0;
	m_dwLastTimePlayMy  = 0;
	m_dwMaxPlayLength   = 0;
	m_iCurDisableChangeArray = 0;
	m_bSaveDisableList       = false;
}

ioCustomSoundMgr::~ioCustomSoundMgr(void)
{
	SaveINIForDisable();

	Clear();
	m_vDisableList.clear();
}

ioCustomSoundMgr& ioCustomSoundMgr::GetSingleton()
{
	return Singleton< ioCustomSoundMgr >::GetSingleton();
}


void ioCustomSoundMgr::CustomSoundLoop()
{
	if( m_vCustomSoundInfoList.empty() )
		return;

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return;

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.begin();
	for(; iter != m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end(); ++iter)
	{
		CustomSoundData *pData = iter->second;
		if( !pData )
			continue;
		pData->SendDataToAll( iter->first );
		pData->CheckAndDeleteNetworkInfo();
	}
}

void ioCustomSoundMgr::LoadINI( DWORD dwUserIndex, const ioHashString &rszPublicID )
{
	if(dwUserIndex == 0 ) return;
	if( rszPublicID.IsEmpty() ) return;

	Clear();

	CustomSoundInfo *pInfo = new CustomSoundInfo;
	if( !pInfo ) return;
	pInfo->m_dwUserIndex = dwUserIndex;
	pInfo->m_szPublicID  = rszPublicID;

	ioINILoader_e kLoader( "config/sp2_custom_sound.ini" );
	kLoader.SetTitle_e( "Info" );
	m_iMaxCustomData = kLoader.LoadInt( "Max", 10 );
	char szBuf[MAX_PATH]="";
	kLoader.LoadString_e( "DefaultChatIcon", "", szBuf, MAX_PATH );
	m_szDefaultChatIcon = szBuf;

	m_dwDelayTimePlayMy = kLoader.LoadInt_e( "DelayTimePlayMy", 0 );
	m_dwMaxPlayLength   = kLoader.LoadInt_e( "MaxPlayLength", 0 );
	
	char szKeyName[MAX_PATH]="";
	char szININame[MAX_PATH]="";
	StringCbPrintf( szININame, sizeof(szININame), "Save\\%s\\CustomSound.ini", rszPublicID.c_str() );

	kLoader.SetFileName( szININame , false );
	kLoader.SetTitle_e( "Info" );
	
	for(int i=0; i < m_iMaxCustomData; i++ )
	{
		CustomSoundData *pData = new CustomSoundData;
		if( !pData )
			continue;
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuf, sizeof( szBuf ) );
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "Text%d", i+1);
		kLoader.LoadString( szKeyName, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "") == 0 )
		{
			SAFEDELETE( pData );
			continue;
		}
		pData->SetKeyWord( szBuf );
		
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		ZeroMemory( szBuf, sizeof( szBuf ) );
		StringCbPrintf_e(szKeyName, sizeof(szKeyName), "File%d", i+1);
		kLoader.LoadString( szKeyName, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "") == 0 )
		{
			SAFEDELETE( pData );
			continue;
		}

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "FileCRC%d", i+1);
		DWORD dwCRC = kLoader.LoadInt( szKeyName, 0 );
		if( dwCRC == 0 )
		{
			SAFEDELETE( pData );
			continue;
		}

		
		char szCurPath[MAX_PATH*2]="";
		GetCurrentDirectory( sizeof( szCurPath ), szCurPath );

		char szFullPath[MAX_PATH*2]="";
		StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\%s", szCurPath, szBuf );
		int iFileSize = Help::GetFileSize( szFullPath );
		if( iFileSize < 0 )
		{
			SAFEDELETE( pData );
			continue;
		}

		if( iFileSize > MAX_WAV_SIZE )
		{
			SAFEDELETE( pData );
			continue;
		}

		DWORD dwCurCRC = 0;
		CCrc32Static::FileCrc32Streams((LPCTSTR)szFullPath, dwCurCRC);
		if( dwCRC != dwCurCRC )
		{
			SAFEDELETE( pData );
			continue;
		}

		if( !pData->OpenAndPac( szFullPath, dwCurCRC) )
		{
			SAFEDELETE( pData );
			continue;
		}

		pInfo->m_CustomSoundDataMap.insert( CustomSoundDataMap::value_type( i+1, pData) );
	}

	m_vCustomSoundInfoList.push_back( pInfo );

	LoadINIForDisable( kLoader );
}

void ioCustomSoundMgr::Clear()
{
	for(CustomSoundInfoList::iterator iter = m_vCustomSoundInfoList.begin(); iter != m_vCustomSoundInfoList.end(); ++iter)
	{
		CustomSoundInfo *pInfo = *iter;
		if( !pInfo ) continue;

		for(CustomSoundDataMap::iterator iter2 = pInfo->m_CustomSoundDataMap.begin(); iter2 != pInfo->m_CustomSoundDataMap.end(); ++iter2)
		{
			SAFEDELETE( iter2->second );
		}
		pInfo->m_CustomSoundDataMap.clear();

		SAFEDELETE( *iter );
	}
	m_vCustomSoundInfoList.clear();
}

void ioCustomSoundMgr::GetCustomSoundInfo( IN const ioHashString & rszChat , OUT DWORD &rdwIndex, OUT DWORD &rdwCRC , OUT DWORD &rdwSize )
{
	rdwIndex = 0;
	rdwCRC   = 0;

	if( rszChat.IsEmpty() )
		return;
	
	if( m_vCustomSoundInfoList.empty() )
		return;

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return;

	char string[MAX_PATH*2];
	memset( string, 0, MAX_PATH*2 );
	FilterChar( rszChat.c_str(), string, g_szFilterChar);

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.begin();
	for(; iter != m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end(); ++iter)
	{
		CustomSoundData *pData = iter->second;
		if( !pData )
			continue;
		char *pResult = strstr( string, pData->GetKeyWord().c_str() );
		if( pResult )
		{
			rdwIndex = iter->first;
			rdwCRC   = pData->GetUnpacCRC();
			rdwSize  = pData->GetSize();
			DWORD dwCurCRC = 0;
			CCrc32Static::FileCrc32Streams((LPCTSTR)pData->GetFilePath().c_str(), dwCurCRC);
			if( dwCurCRC != rdwCRC )
			{
				rdwIndex = 0;
				rdwCRC   = 0;
				rdwSize  = 0;
			}
			return;
		}	
	}
}

void ioCustomSoundMgr::FilterChar( const char *src, char *dst, char *szFilterChar )
{
	if(szFilterChar == NULL) return;

	bool bFiltered = false;
	char *srcp = (char *) src;
	char *dstp = dst;

	while (*srcp) 
	{
		if (strchr(szFilterChar, *srcp) == NULL) 
		{
			*dstp = *srcp;   
			dstp += 1;
		}
		else
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, *srcp ) )
#else
			if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, *srcp) ) 
#endif

#else
			if( IsDBCSLeadByte(*srcp)) 
#endif

			{
				*dstp = *srcp;   
				dstp += 1;
			}
			else
			{
				bFiltered = true;
			}
		}
		srcp += 1;
	}
	*dstp = 0;

	// 앞에 글자로 NULL이 변형되는 것을 방지하기 위해서
	if( bFiltered )
	{
		dstp += 1;
		*dstp = 0;
	}
}

void ioCustomSoundMgr::SendCustomSoundInfo( DWORD dwIndex , DWORD dwCRC ,DWORD dwSize, bool bAll , TeamType eTeamType )
{
	if( dwIndex == 0 ) return;
	if( dwCRC   == 0 ) return;

	// 전투로 연결된경우에는 udp connection이 없으므로 전투연결시에는 커스텀 음성을 보내지 않는다.
	// 나중에 개선 생각해 볼것. 
	SP2Packet kPacket(CUPK_CUSTOM_SOUND_INFO);
	kPacket << HAVE_CUSTOM_SOUND;
	kPacket << g_MyInfo.GetUserIndex();
	kPacket << dwIndex; 
	kPacket << dwCRC;
	kPacket << dwSize;
	if( bAll )
		P2PNetwork::SendToAllPlayingUser( kPacket );
	else
		P2PNetwork::SendToTeam( kPacket, eTeamType );
}

void ioCustomSoundMgr::PlayMy( DWORD dwCustomSoundIndex , DWORD dwCRC )
{
 	if( m_vCustomSoundInfoList.empty() )
 		return;
 
 	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
 		return;
 
 	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );
 	
 	if( iter == m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
		return;
 	
	CustomSoundData *pData = iter->second;
	if( !pData )
		return;

	DWORD dwCurCRC = 0;
	CCrc32Static::FileCrc32Streams((LPCTSTR)pData->GetFilePath().c_str(), dwCurCRC);
	if( dwCurCRC != dwCRC )
		return;

	g_SoundMgr.PlayPieceSound( pData->GetFilePath() );
	m_dwLastTimePlayMy = FRAMEGETTIME();
}

void ioCustomSoundMgr::PlayByPath( const char *szPath )
{
	g_SoundMgr.PlayPieceSound( ioHashString(szPath) );
}

void ioCustomSoundMgr::AddSendNetworkInfo( DWORD dwCustomSoundIndex, DWORD dwUserIndex )
{
	if( m_vCustomSoundInfoList.empty() )
		return;

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return;

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );

	if( iter!=m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
	{
		CustomSoundData *pData = iter->second;
		if( !pData )
			return;
		pData->AddNetworkInfo( dwUserIndex );
	}
}

void ioCustomSoundMgr::AddCustomSoundData( DWORD dwUserIndex, const ioHashString &rszPublicID, DWORD dwCustomSoundIndex, DWORD dwCustomSoundSize )
{
	// check
	if( dwCustomSoundIndex == 0 )
		return;
	if( dwCustomSoundSize == 0 )
		return;

	CustomSoundInfo *pInfo = GetCustomSoundInfoPtr( dwUserIndex );
	if( pInfo )
	{
		CustomSoundDataMap::iterator iter2 = pInfo->m_CustomSoundDataMap.find( dwCustomSoundIndex );
		if( iter2 == pInfo->m_CustomSoundDataMap.end() && 
			(int)pInfo->m_CustomSoundDataMap.size() < m_iMaxCustomData )
		{
			CustomSoundData *pData = new CustomSoundData;
			if( !pData )
				return;

			pData->NewData( dwCustomSoundSize );
			pData->AddNetworkInfo( dwUserIndex );
			pInfo->m_CustomSoundDataMap.insert( CustomSoundDataMap::value_type(dwCustomSoundIndex, pData) );
		}
		return;
	}

	pInfo = new CustomSoundInfo;
	if( !pInfo )
		return;

	pInfo->m_dwUserIndex = dwUserIndex;
	pInfo->m_szPublicID  = rszPublicID;

	CustomSoundData *pData = new CustomSoundData;
	if( !pData )
		return;

	pData->NewData( dwCustomSoundSize );
	pData->AddNetworkInfo( dwUserIndex );
	pInfo->m_CustomSoundDataMap.insert( CustomSoundDataMap::value_type(dwCustomSoundIndex, pData) );

	m_vCustomSoundInfoList.push_back( pInfo );
}

void ioCustomSoundMgr::ClearExceptMe()
{
	CustomSoundInfoList::iterator iter = m_vCustomSoundInfoList.begin();
	while ( iter != m_vCustomSoundInfoList.end() )
	{
		CustomSoundInfo *pInfo = *iter;
		if( !pInfo ) continue;

		if( pInfo->m_dwUserIndex == g_MyInfo.GetUserIndex() )
		{
			for(CustomSoundDataMap::iterator iter2 = pInfo->m_CustomSoundDataMap.begin(); iter2 != pInfo->m_CustomSoundDataMap.end(); ++iter2)
			{
				CustomSoundData *pData = iter2->second;
				if( !pData )
					continue;
				pData->ClearNetworkInfo();
			}
			++iter;
		}
		else
		{
			for(CustomSoundDataMap::iterator iter3 = pInfo->m_CustomSoundDataMap.begin(); iter3 != pInfo->m_CustomSoundDataMap.end(); ++iter3)
			{
				SAFEDELETE( iter3->second );
			}
			pInfo->m_CustomSoundDataMap.clear();
			SAFEDELETE( *iter );
			iter = m_vCustomSoundInfoList.erase( iter );
		}
	}
}

bool ioCustomSoundMgr::UpdateCustomSoundData( DWORD dwUserIndex, DWORD dwCustomSoundIndex, const CustomSoundPacket &rData )
{
	// check
	if( dwUserIndex == 0 )
		return false;
	if( dwCustomSoundIndex == 0 )
		return false;

	for(CustomSoundInfoList::iterator iter = m_vCustomSoundInfoList.begin(); iter != m_vCustomSoundInfoList.end(); ++iter)
	{
		CustomSoundInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_dwUserIndex != dwUserIndex )
			continue;

		CustomSoundDataMap::iterator iter2 = pInfo->m_CustomSoundDataMap.find( dwCustomSoundIndex );
		if( iter2==pInfo->m_CustomSoundDataMap.end() )
			return false;

		CustomSoundData *pData = iter2->second;
		if( !pData )
			return false;
		DWORD dwDataPos = pData->GetNetworkDataPos( dwUserIndex );
		DWORD dwNextPos = pData->SetCurData( rData, dwDataPos );
		pData->SetNetworkDataPos( dwUserIndex, dwNextPos );
		if( !pData->IsCompletedData( dwUserIndex ) )
			return false;
		
		char szRootPath[MAX_PATH]="";
		GetCurrentDirectory( MAX_PATH, szRootPath );
		char szPath[MAX_PATH*2]="";
		StringCbPrintf(szPath, sizeof(szPath),"%s\\Usersound\\_Usersoundother\\%s\\%d.wav",szRootPath,pInfo->m_szPublicID.c_str(),dwCustomSoundIndex);
		SetFileAttributes(szPath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(szPath);
		g_SoundMgr.DeleteSound( ioHashString( szPath ) );
		pData->UnpacAndSave( szPath );
		pData->DeleteNetworkInfo( dwUserIndex );
		SAFEDELETE( iter2->second );
		pInfo->m_CustomSoundDataMap.erase( iter2 );
		if( pInfo->m_CustomSoundDataMap.empty() )
		{
			SAFEDELETE( *iter );
			m_vCustomSoundInfoList.erase( iter );
		}
		return true;
	}

	return false;
}

void ioCustomSoundMgr::GetMyKeywordAndPath( DWORD dwCustomSoundIndex, OUT ioHashString &rszKeyword, OUT ioHashString &rszPath )
{
	if( m_vCustomSoundInfoList.empty() )
		return;

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return;

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );
	if( iter==m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
		return;
	
	CustomSoundData *pData = iter->second;
	if( !pData )
		return;
	
	rszKeyword = pData->GetKeyWord();
	rszPath    = pData->GetFilePath();
}

void ioCustomSoundMgr::DeleteMyCustomSoundData( DWORD dwCustomSoundIndex )
{
	if( m_vCustomSoundInfoList.empty() )
		return;

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return;

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );
	if( iter!=m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
	{
		SAFEDELETE( iter->second );
		m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.erase( iter );
	}
}

bool ioCustomSoundMgr::AddMyCustomSoundData( DWORD dwCustomSoundIndex, const char *szKeyword, const char *szFileName )
{
	if( strcmp( szKeyword , "" ) == 0 )
		return false;

	if( strcmp( szFileName, "" ) == 0 )
		return false;

	if( m_vCustomSoundInfoList.empty() )
	{
		CustomSoundInfo *pInfo = new CustomSoundInfo;
		if( !pInfo )
			return false;

		pInfo->m_dwUserIndex = g_MyInfo.GetUserIndex();
		pInfo->m_szPublicID  = g_MyInfo.GetPublicID();
		m_vCustomSoundInfoList.push_back( pInfo );
	}

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return false;

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );
	if( iter!=m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
		return false;

	CustomSoundData *pData = new CustomSoundData;
	if( !pData )
		return false;

	pData->SetKeyWord( szKeyword );

	char szCurPath[MAX_PATH*2]="";
	GetCurrentDirectory( sizeof( szCurPath ), szCurPath );

	char szFullPath[MAX_PATH*2]="";
	StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\%s", szCurPath, szFileName );

	int iFileSize = Help::GetFileSize( szFullPath );
	if( iFileSize < 0 )
	{
		SAFEDELETE( pData );
		return false;
	}

	if( iFileSize > MAX_WAV_SIZE )
	{
		SAFEDELETE( pData );
		return false;
	}

	DWORD dwCurCRC = 0;
	CCrc32Static::FileCrc32Streams((LPCTSTR)szFullPath, dwCurCRC);
	if( dwCurCRC == 0 )
	{
		SAFEDELETE( pData );
		return false;
	}

	if( !pData->OpenAndPac( szFullPath, dwCurCRC) )
	{
		SAFEDELETE( pData );
		return false;
	}

	m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.insert( CustomSoundDataMap::value_type( dwCustomSoundIndex, pData) );
	return true;
}

void ioCustomSoundMgr::UpdateMyKeyword( DWORD dwCustomSoundIndex, const char *szKeyword )
{
	if( m_vCustomSoundInfoList.empty() )
		return;

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
		return;

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );
	if( iter == m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
		return;
	
	CustomSoundData *pData = iter->second;
	if( !pData )
		return;

	pData->SetKeyWord( szKeyword );
}

void ioCustomSoundMgr::SaveINI( DWORD dwCustomSoundIndex )
{
	if( m_vCustomSoundInfoList.empty() )
	{
		SaveINIBlank( dwCustomSoundIndex );
		return;
	}

	if( m_vCustomSoundInfoList[0]->m_dwUserIndex != g_MyInfo.GetUserIndex() )
	{
		SaveINIBlank( dwCustomSoundIndex );
		return;
	}

	CustomSoundDataMap::iterator iter = m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.find( dwCustomSoundIndex );
	if( iter==m_vCustomSoundInfoList[0]->m_CustomSoundDataMap.end() )
	{
		SaveINIBlank( dwCustomSoundIndex );
		return;
	}
	
	CustomSoundData *pData = iter->second;
	if( !pData )
	{
		SaveINIBlank( dwCustomSoundIndex );
		return;
	}

	char szININame[MAX_PATH]="";
	StringCbPrintf( szININame, sizeof(szININame), "Save\\%s\\CustomSound.ini", m_vCustomSoundInfoList[0]->m_szPublicID.c_str() );

	ioINILoader kLoader( szININame, false );
	kLoader.SetTitle_e( "Info" );

	char szKeyName[MAX_PATH]="";
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Text%d", dwCustomSoundIndex );
	kLoader.SaveString( szKeyName, pData->GetKeyWord().c_str() );

	ZeroMemory( szKeyName, sizeof(szKeyName) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName), "File%d", dwCustomSoundIndex );
	char szName[MAX_PATH]="";
	char szExt[MAX_PATH]="";
	_splitpath( pData->GetFilePath().c_str(), NULL, NULL, szName, szExt );
	char szNameExt[MAX_PATH]="";
	StringCbPrintf( szNameExt, sizeof(szNameExt), "%s%s", szName, szExt );
	kLoader.SaveString( szKeyName, szNameExt );

	ZeroMemory( szKeyName, sizeof(szKeyName) );
	StringCbPrintf( szKeyName, sizeof( szKeyName), "FileCRC%d", dwCustomSoundIndex );
	kLoader.SaveInt( szKeyName, pData->GetUnpacCRC() );
}

void ioCustomSoundMgr::SaveINIBlank( DWORD dwCustomSoundIndex )
{
	char szININame[MAX_PATH]="";
	StringCbPrintf( szININame, sizeof(szININame), "Save\\%s\\CustomSound.ini", g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( szININame, false );
	kLoader.SetTitle_e( "Info" );

	char szKeyName[MAX_PATH]="";
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Text%d", dwCustomSoundIndex );
	kLoader.SaveString( szKeyName, "" );

	ZeroMemory( szKeyName, sizeof(szKeyName) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName), "File%d", dwCustomSoundIndex );
	kLoader.SaveString( szKeyName, "" );

	ZeroMemory( szKeyName, sizeof(szKeyName) );
	StringCbPrintf( szKeyName, sizeof( szKeyName), "FileCRC%d", dwCustomSoundIndex );
	kLoader.SaveInt( szKeyName, 0 );
}

bool ioCustomSoundMgr::CheckWaveFile( const char *szFullPath )
{
	int iSize = Help::GetFileSize( szFullPath );
	if( iSize < 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1));
		return false;
	}

	if( iSize > MAX_WAV_SIZE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), MAX_WAV_SIZE );
		return false;
	}

	ioWaveFile *pWave = new ioWaveFile;
	if( !pWave )
	{
		return false;
	}

	HRESULT hr = pWave->Open( szFullPath );
	if( FAILED( hr ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		SAFEDELETE( pWave );
		return false;
	}
	WAVEFORMATEX *pwfx =  pWave->GetFormat();
	if( !pwfx )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		SAFEDELETE( pWave );
		return false;
	}

	if( pwfx->wFormatTag != WAVE_FORMAT_PCM )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		SAFEDELETE( pWave );
		return false;
	}

	DWORD dwBytePerSec = pwfx->nSamplesPerSec * ( pwfx->wBitsPerSample/8 ) * pwfx->nChannels; // /8은 비트를 바이트로 변경하기 위해
	DWORD dwPlayLength = iSize/dwBytePerSec;
	if( dwPlayLength > m_dwMaxPlayLength )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), m_dwMaxPlayLength );
		SAFEDELETE( pWave );
		return false;
	}

	SAFEDELETE( pWave );
	return true;
}

void ioCustomSoundMgr::DeleteCustomSoundInfo( const ioHashString &rszPubicID )
{
	CustomSoundInfoList::iterator iter = m_vCustomSoundInfoList.begin();
	while ( iter != m_vCustomSoundInfoList.end() )
	{
		CustomSoundInfo *pInfo = *iter;
		if( !pInfo ) continue;

		if( pInfo->m_szPublicID == rszPubicID )
		{
			for(CustomSoundDataMap::iterator iter2 = pInfo->m_CustomSoundDataMap.begin(); iter2 != pInfo->m_CustomSoundDataMap.end(); ++iter2)
			{
				SAFEDELETE( iter2->second );
			}
			pInfo->m_CustomSoundDataMap.clear();
			SAFEDELETE( *iter );
			m_vCustomSoundInfoList.erase( iter );
			return;
		}
		else
			++iter;
	}
}

ioCustomSoundMgr::CustomSoundInfo *ioCustomSoundMgr::GetCustomSoundInfoPtr( DWORD dwUserIndex )
{
	if( m_vCustomSoundInfoList.empty() )
		return NULL;

	for(CustomSoundInfoList::iterator iter = m_vCustomSoundInfoList.begin(); iter != m_vCustomSoundInfoList.end(); ++iter)
	{
		CustomSoundInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_dwUserIndex == dwUserIndex )
			return pInfo;
	}

	return NULL;
}

void ioCustomSoundMgr::InsertDisable( const ioHashString &rszPublicID )
{
	for(ioHashStringVec::iterator iter = m_vDisableList.begin(); iter != m_vDisableList.end(); ++iter)
	{
	    ioHashString &rszPublicIDInList = *iter;
		if( rszPublicIDInList.IsEmpty() )
			continue;
		if( rszPublicIDInList == rszPublicID )
			return;
	}

	int iSize = m_vDisableList.size();
	if( iSize >= MAX_DISABLE_LIST )
	{
		if( !COMPARE( m_iCurDisableChangeArray, 0, MAX_DISABLE_LIST ) )			
			m_iCurDisableChangeArray = 0;

		m_vDisableList[m_iCurDisableChangeArray] = rszPublicID;
		m_iCurDisableChangeArray++;
		m_bSaveDisableList = true;
	}
	else
	{
		m_vDisableList.push_back( rszPublicID );
		m_bSaveDisableList = true;
	}
}

void ioCustomSoundMgr::RemoveDisable( const ioHashString &rszPublicID )
{
	for(ioHashStringVec::iterator iter = m_vDisableList.begin(); iter != m_vDisableList.end(); ++iter)
	{
		ioHashString &rszPublicIDInList = *iter;
		if( rszPublicIDInList.IsEmpty() )
			continue;
		if( rszPublicIDInList == rszPublicID )
		{
			m_vDisableList.erase( iter );
			return;
		}
	}
}

bool ioCustomSoundMgr::IsDisable( const ioHashString &rszPublicID )
{
	for(ioHashStringVec::iterator iter = m_vDisableList.begin(); iter != m_vDisableList.end(); ++iter)
	{
		ioHashString &rszPublicIDInList = *iter;
		if( rszPublicIDInList.IsEmpty() )
			continue;
		if( rszPublicIDInList == rszPublicID )
			return true;
	}

	return false;
}

void ioCustomSoundMgr::GetFilteredKeyword( IN const char *szKeyword, OUT ioHashString &rszFiltered )
{
	char string[MAX_PATH*2];
	memset( string, 0, MAX_PATH*2 );
	FilterChar( szKeyword, string, g_szFilterChar);
	rszFiltered = string;
}

bool ioCustomSoundMgr::IsCanTimePlayMy() const
{
	if( FRAMEGETTIME() - m_dwLastTimePlayMy > m_dwDelayTimePlayMy )
		return true;

	return false;
}

void ioCustomSoundMgr::LoadINIForDisable( ioINILoader &rkLoader )
{
	m_vDisableList.clear();

	rkLoader.SetTitle_e( "DisableList" );
	int iMax = rkLoader.LoadInt( "Max", 0 );
	m_iCurDisableChangeArray = rkLoader.LoadInt_e( "CurChangeArray", 0 );

	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof(szKeyName), "ID%d", i+1);
		char szBuf[MAX_PATH]="";
		rkLoader.LoadString( szKeyName, "", szBuf, sizeof( szBuf ) );
		m_vDisableList.push_back( ioHashString( szBuf ) );
	}
}

void ioCustomSoundMgr::SaveINIForDisable()
{
	if( !m_bSaveDisableList ) return;
	if( g_MyInfo.GetPublicID().IsEmpty() ) return;

	char szININame[MAX_PATH]="";
	StringCbPrintf( szININame, sizeof(szININame), "Save\\%s\\CustomSound.ini", g_MyInfo.GetPublicID().c_str() );

	int iMax = m_vDisableList.size();

	ioINILoader kLoader( szININame, false );
	kLoader.SetTitle_e( "DisableList" );
	kLoader.SaveInt( "Max", iMax );
	kLoader.SaveInt( "CurChangeArray", m_iCurDisableChangeArray );

	for (int i = 0; i < iMax ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof(szKeyName), "ID%d", i+1);
		kLoader.SaveString( szKeyName, m_vDisableList[i].c_str() );
	}
}