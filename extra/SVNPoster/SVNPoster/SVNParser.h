#pragma once

class CSVNParser
{
public:
	CSVNParser(void);
	~CSVNParser(void);

	void Init();
	void Destroy();

public:
	BOOL Load(CString& file);
	void Parse(CString& dist, int& revision, CString& output);

protected:
	void Reset();

	void LoadTitle(CString& text);
	void LoadFile(CString& text);
	void LoadExplain(CString& text);

	void GetDistZone(CString& dist, CString& output);
	void GetRivision(int& revision, CString& output);
	void GetExplain(CString& output);
	void GetFiles(CString& output);
	void GetSpecific(CString& output);

protected:
	CString m_file;
	CStringArray m_books;

	CString m_title;
	CString m_revision, m_author, m_date, m_effected;
	CString m_files, m_explains;
};

