#pragma once

#define MAX_PATH 260
#define MAX_SIZE 64

class ioININode
{
public:
	ioININode(void);
	~ioININode(void);

public:
	bool SetData(const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const int32 value);
	bool SetData(const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const float value, const int floatType);
	bool SetData(const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const bool value);

public:	
	TCHAR* GetFileName()	{ return m_fileName; }
	TCHAR* GetSection()		{ return m_section; }
	TCHAR* Getkey()			{ return m_key; }
	BYTE GetSaveType()		{ return m_saveType; }	
	double GetValue()		{ return m_value; }

public:
	TCHAR	m_fileName[MAX_PATH];
	TCHAR	m_section[MAX_SIZE];
	TCHAR	m_key[MAX_SIZE];
	BYTE	m_saveType;	
	double	m_value;	
};

