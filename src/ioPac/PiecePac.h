// PiecePac.h: interface for the CPiecePac class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIECEPAC_H__5F0E5747_0E8B_456B_BC22_D850FB8DB2BC__INCLUDED_)
#define AFX_PIECEPAC_H__5F0E5747_0E8B_456B_BC22_D850FB8DB2BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PacUtil.h"

class CZipMemFile;
class PAC_API CPiecePac : public CPacUtil
{
protected:
	CPiecePac();
	virtual ~CPiecePac();
	
public:

#ifdef PATCH_PAC_API // 패치매니저가 사용하는 함수들
	bool  PacPieceFile( IN const char *szFileName, IN const char* szPassword ="", IN const char* szComment = "" );
	int   GetPieceFileSize(IN const char *szFileName, IN const char* szPassword = "");
	DWORD GetPieceFileCRC( IN const char *szFileName, IN const char *szPassword = "" );
	CPacUtil::ReturnValue UnPacPieceFileWithPassword( IN const char* szFileName, IN const char* szPassword = "" );
#endif
	CPacUtil::ReturnValue UnPacPieceFile(IN const char *szFileName);
	void GetPieceFileComment( OUT char *szComment, IN int iCommentSize, IN const char *szFileName );

#ifndef STATIC_PAC_API
protected:
	CZipMemFile *m_pMemFile;
public:
	// 함수 사용전 NewMemFile, 완료후 DeleteMemFile 할것
	void  NewMemFile();                                                  // start
	bool  PacPieceFileInMem( IN const char *szFileName );                // action
	DWORD GetMemFileSize();                                              // action
	bool  GetPacPieceFileData( OUT BYTE *pPacData, IN DWORD dwPacSize ); // action
	void  DeleteMemFile();                                               // end
	//

	bool UnPacPieceFileByMem( IN const char *szFileName, IN BYTE *pPacData , IN DWORD dwDataSize );
#endif
};

#endif // !defined(AFX_PIECEPAC_H__5F0E5747_0E8B_456B_BC22_D850FB8DB2BC__INCLUDED_)
