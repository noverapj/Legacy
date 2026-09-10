
#include "stdafx.h"



AniCollisionType GetAniCollisionType( const ioHashString &rkName )
{
	if( !rkName.IsEmpty() )
	{
		if( rkName == "l_hand" )
			return ACT_LEFT_HAND;
		if( rkName == "r_hand" )
			return ACT_RIGHT_HAND;
		if( rkName == "l_foot" )
			return ACT_LEFT_FOOT;
		if( rkName == "r_foot" )
			return ACT_RIGHT_FOOT;
		if( rkName == "r_weapon" )
			return ACT_RIGHT_WEAPON;
		if( rkName == "l_weapon" )
			return ACT_LEFT_WEAPON;
		if( rkName == "special" )
			return ACT_SPECIAL;
	}

	return ACT_NONE;
}
//////////////////////////////////////////////////////////////////////////
void FishingResultInfo::AttachSoldierExp()
{
	if( m_iClassType > 0 && m_iAddSoldierExp > 0 )
	{
		if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) == false )
			g_MyInfo.AddClassExp( m_iClassType, m_iAddSoldierExp );
		g_MyInfo.AddGradeExp( m_iAddSoldierExp );
		m_iAddSoldierExp = 0;
	}
}
