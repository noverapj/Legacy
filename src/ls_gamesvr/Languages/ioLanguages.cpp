#include "StdAfx.h"
#include "ioLanguages.h"

ioLanguages::ioLanguages(void) : m_language(LN_KOR)
{
	Init();
}

ioLanguages::~ioLanguages(void)
{
	Destroy();
}

void ioLanguages::Init()
{
}

void ioLanguages::Destroy()
{
}

BOOL ioLanguages::Load(const TCHAR* file)
{
	Generate(file);

	if(!LoadCurrent())			return FALSE;
	if(!LoadWords( _T("kor") )) return FALSE;

	return TRUE;
}
	
const TCHAR* ioLanguages::GetWord(const UINT index)
{
	LANGUAGES::iterator it = m_languages.find(m_language);
	if(it != m_languages.end())
	{
		WORDS* words = it->second;
		if(words)
		{
			if(index < words->size())
			{
				TCHAR* word = (*words)[index];
				return word;
			}
		}
	}
	return NULL;
}

BOOL ioLanguages::LoadCurrent()
{
	TCHAR buffer[512] = {0};
	int length = GetPrivateProfileString(_T("default"), _T("set_language"), _T(""), buffer, sizeof(buffer), m_file);
	if(length > 0)
	{
		buffer[length] = NULL;
		m_country = buffer;

		if(m_country == _T("kor"))
		{
			m_language = LN_KOR;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL ioLanguages::LoadWords(const TCHAR* language)
{
	int index = m_languages.size();

	WORDS* words = new WORDS;
	words->push_back(NULL);
	m_languages[index] = words;

	TCHAR buffer[512];
	CString key;
	for(int i = 1 ; i <= MAX_WORD ; i++)
	{
		key.Format(_T("%d"), i);
		
		int length = GetPrivateProfileString(language, key, _T(""), buffer, sizeof(buffer), m_file);
		if(length > 0)
		{
			buffer[length] = NULL;
			
			TCHAR* node = new TCHAR[length+1];
			CopyMemory(node, buffer, length+1);

			words->push_back(node);
		}
		else
		{
			words->push_back(NULL);
		}
	}
	return TRUE;
}

BOOL ioLanguages::Generate(const TCHAR* file)
{
	TCHAR temp[MAX_PATH+1];
	GetModuleFileName(NULL, temp, MAX_PATH);

	TCHAR* token = _tcsrchr(temp, _T('\\'));
	*(token+1) = _T('\0');

	_stprintf_s(m_file, sizeof(m_file), _T("%s%s"), temp, file);
	return TRUE;
}
