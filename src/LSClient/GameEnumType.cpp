

#include "stdafx.h"

#include "GameEnumType.h"

bool IsWinTeam( WinTeamType eWinTeam, TeamType eTeam )
{
	switch( eWinTeam )
	{
	case WTT_RED_TEAM:
	case WTT_VICTORY_RED_TEAM:
		if( eTeam == TEAM_RED )
			return true;
		break;
	case WTT_BLUE_TEAM:
	case WTT_VICTORY_BLUE_TEAM:
		if( eTeam == TEAM_BLUE )
			return true;
		break;
	}

	return false;
}

bool IsRedWin( WinTeamType eWinTeam )
{
	if( eWinTeam == WTT_RED_TEAM || eWinTeam == WTT_VICTORY_RED_TEAM )
		return true;

	return false;
}

bool IsBlueWin( WinTeamType eWinTeam )
{
	if( eWinTeam == WTT_BLUE_TEAM || eWinTeam == WTT_VICTORY_BLUE_TEAM )
		return true;

	return false;
}

bool IsVictoryWin( WinTeamType eWinTeam )
{
	if( eWinTeam == WTT_VICTORY_RED_TEAM ||
		eWinTeam == WTT_VICTORY_BLUE_TEAM )
		return true;

	return false;
}

RaceType ConvertStringToRaceType( const char *szType )
{
	if( !strcmp( szType, "HUMAN" ) )
		return RT_HUMAN;
	if( !strcmp( szType, "ELF" ) )
		return RT_ELF;
	if( !strcmp( szType, "DWARF" ) )
		return RT_DWARF;

	LOG.PrintTimeAndLog( 0, "ConvertStringToRaceType - %s UnknownType", szType );

	return RT_HUMAN;
}

TeamType ConvertStringToTeamType( const char *szType )
{
	if( !strcmp( szType, "RED" ) )
		return TEAM_RED;
	else if( !strcmp( szType, "BLUE" ) )
		return TEAM_BLUE;

	return TEAM_NONE;
}

RaceDetailType GetRaceDetailType( const CHARACTER &rkInfo )
{
	switch( rkInfo.m_kindred )
	{
	case 1:
		if( rkInfo.m_sex == 1 )
			return RDT_HUMAN_MAN;
		else
			return RDT_HUMAN_WOMAN;
		break;
	case 2:
		if( rkInfo.m_sex == 1 )
			return RDT_ELF_MAN;
		else
			return RDT_ELF_WOMAN;
		break;
	case 3:
		if( rkInfo.m_sex == 1 )
			return RDT_DWARF_MAN;
		else
			return RDT_DWARF_WOMAN;
		break;
	}

	return RDT_HUMAN_MAN;
}

RaceDetailType GetRaceDetailType( RaceType eRace, bool  bMale )
{
	switch( eRace )
	{
	case RT_HUMAN:
		if( bMale )
			return RDT_HUMAN_MAN;
		else
			return RDT_HUMAN_WOMAN;
		break;
	case RT_ELF:
		if( bMale )
			return RDT_ELF_MAN;
		else
			return RDT_ELF_WOMAN;
		break;
	case RT_DWARF:
		if( bMale )
			return RDT_DWARF_MAN;
		else
			return RDT_DWARF_WOMAN;
		break;
	}

	return RDT_HUMAN_MAN;
}

bool SetCharLoader( ioINILoader &rkLoader, RaceDetailType eType )
{
	switch( eType )
	{
	case RDT_HUMAN_MAN:
		rkLoader.SetFileName_e( "config/sp2_race_human_man.ini" );
		return true;
	case RDT_HUMAN_WOMAN:
		rkLoader.SetFileName_e( "config/sp2_race_human_woman.ini" );
		return true;
	case RDT_ELF_MAN:
		rkLoader.SetFileName_e( "config/sp2_race_elf_man.ini" );
		return true;
	case RDT_ELF_WOMAN:
		rkLoader.SetFileName_e( "config/sp2_race_elf_woman.ini" );
		return true;
	case RDT_DWARF_MAN:
		rkLoader.SetFileName_e( "config/sp2_race_dwarf_man.ini" );
		return true;
	case RDT_DWARF_WOMAN:
		rkLoader.SetFileName_e( "config/sp2_race_dwarf_woman.ini" );
		return true;
	}

	return false;
}

EquipSlot GetEquipSlotByCode( DWORD dwItemCode )
{
	if( COMPARE( dwItemCode, 1, 100000 ) )
		return ES_WEAPON;
	if( COMPARE( dwItemCode, 100001, 200000 ) )
		return ES_ARMOR;
	if( COMPARE( dwItemCode, 200001, 300000 ) )
		return ES_HELMET;
	if( COMPARE( dwItemCode, 300001, 400000 ) )
		return ES_CLOAK;
	if( COMPARE( dwItemCode, 400001, 500000 ) )
		return ES_OBJECT;
	if( COMPARE( dwItemCode, 500001, 600000 ) )
		return ES_WEAR;
	
	//Accessory
	if( COMPARE( dwItemCode, 1000001, 2000000 ) )
		return ES_RING;
	if( COMPARE( dwItemCode, 2000001, 3000000 ) )
		return ES_NECKLACE;
	if( COMPARE( dwItemCode, 3000001, 4000000 ) )
		return ES_BRACELET;

	return ES_UNKNOWN;
}

// attack_face.dds -> attack_face.dds or attack_face_hw.dds or attack_face2_hw.dds 
// ioCharTool.exe의 face_change의 all/sex/each 타입은 미사용
bool GetFaceChangeName( OUT ioHashString &rszNewFaceName,
					    IN  const char *szPostFix,
					    IN  bool bMale,
						IN  int iFaceType )
{
	// type이 없는 경우거나 1인 경우는 숫자가 붙지 않는다.
	if( iFaceType <= 1 )
	{
		rszNewFaceName = Help::ConvertTailName( ioHashString(szPostFix), bMale );
		return true;
	}

	// 나중에 2,3,4가 추가되어서 첫번째 디폴트 표정은 파일명에 숫자가 안들어가 있다;
	StringVector vParam = ioStringConverter::Split( std::string(szPostFix), ".", 1 );
	if( vParam.size() == 2 )
	{
		char szFullFaceName[MAX_PATH]="";
		wsprintf( szFullFaceName, "%s%d.%s", vParam[0].c_str(), iFaceType, vParam[1].c_str() );
		rszNewFaceName = Help::ConvertTailName( ioHashString(szFullFaceName), bMale );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Face Texture Name Error", rszNewFaceName.c_str() );
		return false;
	}

	// error 체크
	for (int i=0; i<rszNewFaceName.Length(); i++)
	{
		if ((!COMPARE(rszNewFaceName.At(i), 'A', 'Z'+1)) &&
			(!COMPARE(rszNewFaceName.At(i), 'a', 'z'+1)) &&
			(!COMPARE(rszNewFaceName.At(i), '0', '9'+1)) &&
			( rszNewFaceName.At(i) != '-' )              &&
			( rszNewFaceName.At(i) != '_' )              &&
			( rszNewFaceName.At(i) != '.' )              )
		{
			LOG.PrintTimeAndLog( 0, "%s Face Texture Name Error (2)", rszNewFaceName.c_str() );
			return false;
		}
	}
	
	return true;
}

int ConvertPowerUpItemToChar( int eItemGradeType )
{
	switch( eItemGradeType )
	{
	case PUGT_ITEM_GRADE1: return PUGT_CHAR_GRADE1;
	case PUGT_ITEM_GRADE2: return PUGT_CHAR_GRADE2;
	case PUGT_ITEM_GRADE3: return PUGT_CHAR_GRADE3;
	case PUGT_ITEM_GRADE4: return PUGT_CHAR_GRADE4;
	case PUGT_ITEM_GRADE5: return PUGT_CHAR_GRADE5;
	default: return PUGT_NONE;
	}
}

int ConvertPowerUpCharToItem( int eCharGradeType )
{
	switch( eCharGradeType )
	{
	case PUGT_CHAR_GRADE1: return PUGT_ITEM_GRADE1;
	case PUGT_CHAR_GRADE2: return PUGT_ITEM_GRADE2;
	case PUGT_CHAR_GRADE3: return PUGT_ITEM_GRADE3;
	case PUGT_CHAR_GRADE4: return PUGT_ITEM_GRADE4;
	case PUGT_CHAR_GRADE5: return PUGT_ITEM_GRADE5;
	default: return PUGT_NONE;
	}
}

int ConvertPowerUpRareItemToChar( int eItemGradeType )
{
	switch( eItemGradeType )
	{
	case PURIGT_RARE_ITEM_GRADE1: return PUGT_CHAR_GRADE1;
	case PURIGT_RARE_ITEM_GRADE2: return PUGT_CHAR_GRADE2;
	case PURIGT_RARE_ITEM_GRADE3: return PUGT_CHAR_GRADE3;
	case PURIGT_RARE_ITEM_GRADE4: return PUGT_CHAR_GRADE4;
	case PURIGT_RARE_ITEM_GRADE5: return PUGT_CHAR_GRADE5;
	default: return PURIGT_NONE;
	}
}

int ConvertPowerUpItemToItemGrade( int iItemCode )
{
	return iItemCode % DEFAULT_BASIC_ITEM_CODE / PUGT_ITEM_GRADE_BASE * PUGT_ITEM_GRADE_BASE;
}


int ConvertPowerUpRareItemToRareItemGrade( int iItemCode )
{
	return iItemCode % DEFAULT_BASIC_ITEM_CODE % PUGT_ITEM_GRADE_BASE / PURIGT_RARE_ITEM_GRADE_BASE * PURIGT_RARE_ITEM_GRADE_BASE;
}

int ConvertItemToClassType( int iItemCode )
{
	return ( iItemCode % DEFAULT_BASIC_ITEM_CODE ) % 10000 % 1000;
}

bool IsPowerUpItemGrade( int eItemGrade, bool bUseGradeUI )
{
	if(bUseGradeUI && eItemGrade == PUGT_ITEM_GRADE_BASE)
		return true;
	switch( eItemGrade )
	{
	case PUGT_ITEM_GRADE1:
	case PUGT_ITEM_GRADE2:
	case PUGT_ITEM_GRADE3:
	case PUGT_ITEM_GRADE4:
	case PUGT_ITEM_GRADE5:
		return true;
	}

	return false;
}

bool IsPowerUpRareItemGrade( int eItemGrade, bool bUsePowerUI )
{
	if(bUsePowerUI && eItemGrade == PURIGT_RARE_ITEM_GRADE_BASE)
		return true;
	switch( eItemGrade )
	{
	case PURIGT_RARE_ITEM_GRADE1:
	case PURIGT_RARE_ITEM_GRADE2:
	case PURIGT_RARE_ITEM_GRADE3:
	case PURIGT_RARE_ITEM_GRADE4:
	case PURIGT_RARE_ITEM_GRADE5:
		return true;
	}

	return false;
}

bool IsPowerUpItem( int iItemCode )
{
	return IsPowerUpItemGrade( ConvertPowerUpItemToItemGrade( iItemCode ) );
}

EquipItemExtendType GetEquipItemExtendType( int iItemCode )
{
	int iExtendType = ( iItemCode % DEFAULT_BASIC_ITEM_CODE ) / DEFAULT_EXTRA_ITEM_CODE;
	int iRare		= ( iItemCode % DEFAULT_BASIC_ITEM_CODE ) % DEFAULT_EXTRA_ITEM_CODE / DEFAULT_RARE_ITEM_CODE;

	//강화된 장비 등급
	if(  COMPARE( iExtendType, 5, 10 ) && iRare == 0 )
	{
		return EIET_BASIC_POWERUP;
	}
	else if(  COMPARE( iExtendType, 5, 10 ) && 0 < iRare )
	{
		return EIET_EXTRA_POWERUP;
	}
	else if( COMPARE( iExtendType, 2, 5 ) )
	{
		return EIET_SPECIAL_EXTREA;
	}
	else if( iExtendType == 1 && iRare == 1 )
	{
		return EIET_RARE;
	}
	else if( iExtendType == 1 && COMPARE( iRare, 5, 10 ) )
	{
		return EIET_RARE_POWERUP;
	}
	else if( iExtendType == 1 && iRare == 0 )
	{
		return EIET_BASIC_EXTREA;
	}
	else
	{
		return EIET_BASIC;
	}
}

