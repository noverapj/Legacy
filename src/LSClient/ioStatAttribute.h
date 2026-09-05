#pragma once


#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"



class ioINILoader;
class ioBaseChar;

class ioStatAttribute : public Singleton< ioStatAttribute >
{
protected:
	float m_MaxStatAttribute[RT_MAX_RACE_TYPE][4];
	StatAttributeList	m_StatAttributeList;

public:
	void InitINIValue();

public:
	float GetCalcStatAttribute( float fTarget, int iStatAttributeID, ioBaseChar *pChar );
	float GetAverageMoveSpeed( float fDefaultSpeed );

public:
	void GetStatAttributeTitle( int iStatAttributeID, ioHashString &szTitle );
	void GetStatAttributeDesc( int iStatAttributeID, ioHashString &szDesc );
	int  GenerateAttributeID( const ioHashString &szID );

public:
	StatAttribute* FindAttribute( int iAttributeID );
	bool CheckStatType( int iType, int iOffSet );
	int GetAttributeCount();

protected:
	float GetStatAttributeLev( int iStatAttributeID, ioBaseChar *pChar );
	void  ClearAttributeList();

public:
	static ioStatAttribute& GetSingleton();

public:
	ioStatAttribute();
	virtual ~ioStatAttribute();
};

#define g_StatAttribute ioStatAttribute::GetSingleton()

