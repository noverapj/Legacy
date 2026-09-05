#include "stdafx.h"
#include "WordFilterManager.h"

WordFilterManager::WordFilterManager()
{
	Init();
}

WordFilterManager::~WordFilterManager()
{
}

void WordFilterManager::Init()
{
	m_vBlockWords.clear();

	InsertBlockWords();
}

void WordFilterManager::Destroy()
{
}

void WordFilterManager::InsertBlockWords()
{
	m_vBlockWords.push_back('\n');
}

BOOL WordFilterManager::IsSpecialLetters(const char szWord)
{
	int iSize = m_vBlockWords.size();

	for( int i = 0; i < iSize; i++ )
	{
		if( m_vBlockWords[i] == szWord )
			return TRUE;
	}

	return FALSE;
}

BOOL WordFilterManager::CheckSpecialLetters(const char *szWords, const int iLen)
{
	if(!szWords)
		return FALSE;

	for( int i = 0; i < iLen; i++ )
	{
		if( IsSpecialLetters(szWords[i]) )
			return FALSE;
	}
	
	return TRUE;
}
