#pragma once

class WordFilterManager
{

public:
	BOOL CheckSpecialLetters(const char *szWords, const int iLen);

public:
	void Init();
	void Destroy();

public:
	void InsertBlockWords();
	BOOL IsSpecialLetters(const char szWord);

public:
	WordFilterManager();
	virtual ~WordFilterManager();

private:
	typedef std::vector<char> BLOCK_WORDLIST;
	BLOCK_WORDLIST m_vBlockWords;
};
