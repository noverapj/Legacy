#pragma once

#include <map>
#include <vector>
#include <string>

#define MAX_WORD 1000
enum Languages
{
	LN_KOR = 0,
};

typedef std::vector<TCHAR*> WORDS;
typedef std::map<int,WORDS*> LANGUAGES;

class ioLanguages
{
public:
	ioLanguages(void);
	~ioLanguages(void);

	void Init();
	void Destroy();

public:
	BOOL Load(const TCHAR* file);
	
	const TCHAR* GetWord(const UINT index);
	Languages GetLanguage()		{ return m_language; }

private:
	BOOL LoadCurrent();
	BOOL LoadWords(const TCHAR* language);
	BOOL Generate(const TCHAR* file);

private:
	Languages m_language;
	std::string m_country;

	LANGUAGES m_languages;

	TCHAR m_file[512];
};

