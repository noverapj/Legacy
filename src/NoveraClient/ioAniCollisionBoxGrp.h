#pragma once


#include "../io3DEngine/ioOrientBox.h"


struct AniCollisionBox
{
	AniCollisionType m_Type;
	ioOrientBox m_Box;
	ioHashString m_LinkBone;
};

class ioAniCollisionBoxGrp
{
private:
	typedef std::vector< AniCollisionBox* > AniCollisionBoxList;
	AniCollisionBoxList m_AniCollisionBoxList;

public:
	bool InitBoxList( const char *szFileName, bool bSpecialType );

private:
	bool IsAniCollisionFile( const char *szToken, int iVersion );
	AniCollisionBox* CreateNewBox();
	void ClearBoxList();

public:
	AniCollisionBox* GetCollisionBox( AniCollisionType eType );
	bool HasCollisionBox( AniCollisionType eType );
	int GetCollisionBoxCnt();

public:
	ioAniCollisionBoxGrp* Clone();

public:
	ioAniCollisionBoxGrp();
	ioAniCollisionBoxGrp( const ioAniCollisionBoxGrp &rhs );
	~ioAniCollisionBoxGrp();
};

