#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "ioCharacter.h"

#include ".\ioexercisecharindexmanager.h"

template<> ioExerciseCharIndexManager* Singleton< ioExerciseCharIndexManager >::ms_Singleton = 0;

ioExerciseCharIndexManager::ioExerciseCharIndexManager(void)
{
	m_vIndexAdd.reserve(1000);
	m_iStartAdd = 0;
	m_iEndAdd   = 0;
}

ioExerciseCharIndexManager::~ioExerciseCharIndexManager(void)
{
	m_vIndexAdd.clear();
}

void ioExerciseCharIndexManager::Init( const DWORD dwServerIndex )
{
	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "AddExerciseCharIndex" );

	int iMaxIndex = kLoader.LoadInt( "max_index", 1000 );
	m_iStartAdd = dwServerIndex * iMaxIndex;
	m_iEndAdd   = m_iStartAdd + (iMaxIndex - 1);

	if( m_iStartAdd == -1 || m_iEndAdd == -1 )
		return;

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : start:%d  /  end:%d", __FUNCTION__, m_iStartAdd, m_iEndAdd );

	for (int i = m_iStartAdd; i <  m_iEndAdd +1; i++)
		m_vIndexAdd.push_back(i);
}

DWORD ioExerciseCharIndexManager::Pop()
{
	if( m_vIndexAdd.empty() ) return 0;

	DWORD dwReturn = MIN_EXERCISE_CHAR_INDEX + m_vIndexAdd[0];

	m_vIndexAdd.erase( m_vIndexAdd.begin() + 0 );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s - INDEX:%u, ADD:%d", __FUNCTION__, dwReturn , dwReturn - MIN_EXERCISE_CHAR_INDEX);
	return dwReturn;
}

void ioExerciseCharIndexManager::Add( DWORD dwExerciseIndex )
{
	if( !COMPARE( dwExerciseIndex, MIN_EXERCISE_CHAR_INDEX, MAX_EXERCISE_CHAR_INDEX +1) )	
		return;

	int iAdd = dwExerciseIndex - MIN_EXERCISE_CHAR_INDEX;

	if( !COMPARE(iAdd, m_iStartAdd, m_iEndAdd+1) ) return;

	IntVec::iterator iter = std::find( m_vIndexAdd.begin(), m_vIndexAdd.end(), iAdd );
	if( iter == m_vIndexAdd.end() )
	{
		m_vIndexAdd.push_back( iAdd );
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s - INDEX:%u, ADD:%d", __FUNCTION__, MIN_EXERCISE_CHAR_INDEX + iAdd, iAdd );
	}
}

bool ioExerciseCharIndexManager::IsHave()
{
	if( m_vIndexAdd.empty() ) 
		return false;

	return true;
}

bool ioExerciseCharIndexManager::IsRight( DWORD dwIndex )
{
	if( COMPARE( dwIndex, MIN_EXERCISE_CHAR_INDEX, MAX_EXERCISE_CHAR_INDEX + 1 ) )
		return true;

	return false;
}
