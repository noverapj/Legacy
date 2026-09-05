#include "stdafx.h"
#include "ioExtendSoundManager.h"

template<> ioExtendSoundManager* Singleton< ioExtendSoundManager >::ms_Singleton = 0;
ioExtendSoundManager& ioExtendSoundManager::GetSingleton()
{
	return Singleton< ioExtendSoundManager >::GetSingleton();
}

ioExtendSoundManager::ioExtendSoundManager() : m_CurrSoundID( -1 ), m_bCutIgnore( false ), m_bIgnoreNextSound( false )
{
}

ioExtendSoundManager::~ioExtendSoundManager()
{
}

void ioExtendSoundManager::Initialize()
{
	m_ExSoundList.clear();

	ioINILoader_e kLoader( "config/sp2_ex_sound.ini" );
	kLoader.SetTitle_e( "common" );
	int nCount = kLoader.LoadInt_e("max_sound", 0 );

	char szBuf[MAX_PATH];
	for( int i = 0; i < nCount; ++i )
	{
		wsprintf_e( szBuf, "sound%d", i+1 );
		kLoader.SetTitle( szBuf );

		ExSound Sound;
		kLoader.LoadString_e( "sound_name", "", szBuf, MAX_PATH );
		Sound.m_SoundName			= szBuf;
		Sound.m_SoundType			= static_cast<ExSound::ExSoundType>( kLoader.LoadInt_e( "sound_type", 0 ) );
		Sound.m_bThread				= kLoader.LoadBool_e( "thread_sound", false );
		Sound.m_bIgnoreNextSound	= kLoader.LoadBool_e( "ignore_next_sound", false );
		Sound.m_bCutPrevSound		= kLoader.LoadBool_e( "cut_prev_sound", true );
		Sound.m_bCutIgnore			= kLoader.LoadBool_e( "cut_ignore", false );

		m_ExSoundList.push_back( Sound );
	}
}

bool ioExtendSoundManager::IsSound( ExSound::ExSoundType eSound )
{
	if( !Setting::IsPlayExtendSound() )
		return false;

	for( auto iter = m_ExSoundList.begin(); iter != m_ExSoundList.end(); ++iter )
	{
		const ExSound& Sound = *iter;
		if( Sound.m_SoundType == eSound )
			return true;
	}

	return false;
}

void ioExtendSoundManager::PlaySound( ExSound::ExSoundType eSound )
{
	//재생할 사운드 매니저가 없다면 플레이 하지 않음
	if( ioSoundManager::GetSingletonPtr() == NULL )
		return;

	//사운드 재생 옵션이 꺼져있으면 플레이 하지 않음
	if( !Setting::IsPlayExtendSound() )
		return;

	//재생할 사운드 리스트를 구성
	ExSoundList Buffer;
	for( auto iter = m_ExSoundList.begin(); iter != m_ExSoundList.end(); ++iter )
	{
		const ExSound& Sound = *iter;
		if( Sound.m_SoundType == eSound )
		{
			Buffer.push_back( Sound );
		}
	}

	//재생 할 사운드가 없으면 패스~
	if( Buffer.empty() )
		return;

	//랜덤으로 재생할 사운드 중 하나만 재생
	srand( time( NULL ) );
	int Index = rand() % (int)Buffer.size();
	if( !COMPARE( Index, 0, (int)Buffer.size() ) )
		Index = 0;

	//재생 중인 사운드가 새로 재생 할 사운드를 무시하는 기능이 있다면 리턴
	if( !m_CurrSoundName.IsEmpty() )
	{	
		if( g_SoundMgr.IsPlayingSound( m_CurrSoundName, m_CurrSoundID ) && m_bIgnoreNextSound )
			return;
	}

	//이전에 재생중인 사운드가 있다면 자르고 재생
	if( Buffer[Index].m_bCutPrevSound && !m_CurrSoundName.IsEmpty() && !m_bCutIgnore )
	{
		g_SoundMgr.StopSound( m_CurrSoundName, m_CurrSoundID );
	}

	if( Buffer[Index].m_bThread )
		m_CurrSoundID	= g_SoundMgr.PlaySound( Buffer[Index].m_SoundName, DSBVOLUME_MAX, PSM_THREAD );
	else
		m_CurrSoundID	= g_SoundMgr.PlaySound( Buffer[Index].m_SoundName, DSBVOLUME_MAX, PSM_NONE );

	m_CurrSoundName		= Buffer[Index].m_SoundName;
	m_bCutIgnore		= Buffer[Index].m_bCutIgnore;
	m_bIgnoreNextSound	= Buffer[Index].m_bIgnoreNextSound;
}