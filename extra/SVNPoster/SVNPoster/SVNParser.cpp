#include "StdAfx.h"
#include "SVNParser.h"


CSVNParser::CSVNParser(void)
{
	Init();
}

CSVNParser::~CSVNParser(void)
{
	Destroy();
}

void CSVNParser::Init()
{
}

void CSVNParser::Destroy()
{
}

BOOL CSVNParser::Load(CString& file)
{
	m_file = file;

	CStdioFile sf;
	if(sf.Open(file, CFile::modeRead))
	{
		Reset();

		if(sf.GetLength() == 0)
		{
			sf.Close();
			return FALSE;
		}

		CString temp;
		while(sf.ReadString(temp))
		{
			temp.TrimLeft();
			temp.TrimRight();

			m_books.Add(temp);
			TRACE(_T("%s\n"), temp);
		}

		sf.Close();
		return TRUE;
	}
	return FALSE;
}

void CSVNParser::Parse(CString& dist, int& revision, CString& output)
{
	BOOL afterFile = FALSE;

	int count = m_books.GetCount();
	for(int i = 0 ; i < count ; i++)
	{
		CString temp = m_books[i];

		CString symbol = temp.Mid(0, 1);
		if(symbol == _T("r"))
		{
			LoadTitle( temp );
		}
		else if(symbol == _T("M"))
		{
			afterFile = TRUE;
			LoadFile( temp );
		}
		else if(symbol == _T("A"))
		{
			afterFile = TRUE;
			LoadFile( temp );
		}
		else if(symbol == _T("D"))
		{
			afterFile = TRUE;
			LoadFile( temp );
		}
		else
		{
			if(afterFile && (temp != _T("------------------------------------------------------------------------")))
			{
				LoadExplain( temp );
			}
		}
	}

	GetDistZone(dist, output);
	GetRivision(revision, output);
	GetExplain(output);
	GetFiles(output);
	GetSpecific(output);
}

void CSVNParser::Reset()
{
	m_books.RemoveAll();
	m_files.Empty();
	m_explains.Empty();
}

void CSVNParser::LoadTitle(CString& text)
{
	// r4709 | HwangJunGi | 2013-03-19 12:00:03 +0900 (2013-03-19, 화) | 5 개의 행
	TRACE(_T("Title : %s\n"), text);
	m_title = text;

	int pos = 0;
	m_revision = m_title.Tokenize(_T("|"), pos);
	m_author = m_title.Tokenize(_T("|"), pos);
	m_date = m_title.Tokenize(_T("|"), pos);
	m_effected = m_title.Tokenize(_T("|"), pos);
}

void CSVNParser::LoadFile(CString& text)
{
	if(text.IsEmpty()) return;

	CString temp;
	temp.Format(_T("%s\r\n"), text);
	m_files = m_files + temp;
}

void CSVNParser::LoadExplain(CString& text)
{
	if(text.IsEmpty()) return;

	CString temp;
	temp.Format(_T("%s\r\n"), text);
	m_explains = m_explains + temp;
}

void CSVNParser::GetDistZone(CString& dist, CString& output)
{
	CString temp;
	temp.Format(_T("<B>[적용존]</B>\r\n%s\r\n\r\n\r\n"), dist);
	output = output + temp;
}

void CSVNParser::GetRivision(int& revision, CString& output)
{
	CString temp;
	temp.Format(_T("<B>[Revision]</B>\r\n%d\r\n\r\n"), revision);
	output = output + temp;
}

void CSVNParser::GetExplain(CString& output)
{
	CString temp;
	temp.Format(_T("<B>[변경내용]</B>\r\n%s\r\n\r\n"), m_explains);
	output = output + temp;
}

void CSVNParser::GetFiles(CString& output)
{
	CString temp;
	temp.Format(_T("<B>[파일목록]</B>\r\n%s\r\n\r\n"), m_files);
	output = output + temp;
}

void CSVNParser::GetSpecific(CString& output)
{
	CString temp;
	temp.Format(_T("<B>[세부사항]</B>\r\n"));
	output = output + temp;
}