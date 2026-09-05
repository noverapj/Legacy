// ioPac.h: interface for the ioPac class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOPAC_H__A1C7136A_84A0_4EB5_A12F_E689EBC09505__INCLUDED_)
#define AFX_IOPAC_H__A1C7136A_84A0_4EB5_A12F_E689EBC09505__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef STATIC_PAC_API
	#define PAC_API
#else
	#ifdef EXPORT_PAC_API
		#define PAC_API __declspec(dllexport)
	#else
		#define PAC_API __declspec(dllimport)
	#endif
#endif

#include "BigPac.h"
#include "PiecePac.h"
#include "PacErrorReport.h"

class PAC_API ioPac : public CPiecePac
{
private:
	ioPac();
	virtual ~ioPac();

private:
	static ioPac *sg_Instance;
	
public:
	static ioPac& GetInstance();
	static void ReleaseInstance();

private:
	struct BigPacInfo 
	{
		FileInfo m_FileInfo;
		CBigPac *m_pBigPac;
	};
	typedef std::vector<BigPacInfo> vBigPacInfo;
	vBigPacInfo m_vBigPacInfo;
	vDeleteFileList m_vDeleteFileList;
	int m_iMaxSize;
public:
	void Clear();
	bool Add( const char* szFilePath );
	bool Remove( const char* szFilePath );

	CBigPac *GetPac( const char* szFilePath ) const;
	CBigPac *GetPacArray( int iArray ) const;
	CBigPac *GetPacHasFile( const char *szFileName, int &iFileIndex ) const;

	void AddDeleteList( const char *szToken );
	void LoadDeleteList( const char *szFilePath );
	void RemoveDeleteList( const char *szToken );
	bool  PacPieceFile( IN const char *szFileName, IN const char* szPassword ="", IN const char* szComment = "" );
	DeleteFileList *GetDeleteList( const char *szToken );
	void RemoveAllDeleteList();
};

#define g_Pac ioPac::GetInstance()
#define g_PacChild(iArray) ioPac::GetInstance().GetPacArray(iArray)

#endif // !defined(AFX_IOPAC_H__A1C7136A_84A0_4EB5_A12F_E689EBC09505__INCLUDED_)
