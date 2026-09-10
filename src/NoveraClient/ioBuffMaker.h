#pragma once

#include "ioBuff.h"

class ioBuffMaker : public Singleton< ioBuffMaker >
{
protected:
	typedef std::map< ioHashString, ioBuff* > BuffMap;
	BuffMap m_BuffMap;

public:
	void ClearBuffList();
	void LoadBuffList( const char *szFileName );

#ifndef SHIPPING //UJ
	#define LoadBuffList_e	LoadBuffList
#else
	//UJ 130812, 스트링 상수 암호화 및 숨기기 위한 코드
	template <int N>
	__forceinline void LoadBuffList_e( const char (&szFileName)[N] )
	{
		_ENCSTR(szFileName, szEncFileName);
		LoadBuffList(szEncFileName);
	}
#endif

public:
	ioBuff* CreateBuff( const ioHashString &rkName );

protected:
	ioBuff* CreateBuffTemplete( int iTypeID, int iBuffCategory );
	ioBuff* CreateNormalBuffTemplete( int iTypeID, int iBuffCategory );
	ioBuff* CreateAccessoryBuffTemplete( int iTypeID, int iBuffCategory );

public:
	const ioBuff* GetBuffConst( const ioHashString &rkName ) const;
	int GetBuffType( const ioHashString &rkName ) const;

public:
	static ioBuffMaker& GetSingleton();

public:
	ioBuffMaker();
	virtual ~ioBuffMaker();
};

#define g_BuffMaker ioBuffMaker::GetSingleton()

