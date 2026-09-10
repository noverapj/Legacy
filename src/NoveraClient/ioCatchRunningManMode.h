#pragma once


#include "ioCatchMode.h"

class ioUIRenderImage;
class ioCatchRunningManMode : public ioCatchMode
{
protected:
	struct RunningManDeco
	{
		// 런닝맨 치장 남자
		int m_iManHair;
		int m_iManHairColor;
		int m_iManFace;
		int m_iManSkinColor;
		int m_iManUnderwearBlue;
		int m_iManUnderwearRed;
		ioHashString m_ManSkeletonName;
		
		// 런닝맨 치장 여자
		int m_iWomanHair;
		int m_iWomanHairColor;
		int m_iWomanFace;
		int m_iWomanSkinColor;
		int m_iWomanUnderwearBlue;
		int m_iWomanUnderwearRed;
		ioHashString m_WomanSkeletonName;

		// 포로 묶임 / 해제
		ioHashString m_PrisonerEffect;
		ioHashString m_EscapeEffect;

		RunningManDeco()
		{
			m_iManHair			= 0;
			m_iManHairColor		= 0;
			m_iManFace			= 0;
			m_iManSkinColor		= 0;
			m_iManUnderwearBlue	= 0;
			m_iManUnderwearRed 	= 0;

			m_iWomanHair		  = 0;
			m_iWomanHairColor	  = 0;
			m_iWomanFace		  = 0;	
			m_iWomanSkinColor	  = 0;
			m_iWomanUnderwearBlue = 0;
			m_iWomanUnderwearRed  = 0;
		}
	};
	typedef std::map< DWORD, RunningManDeco > RunningManDecoMap;
	RunningManDecoMap m_RunningManDecoList;

	ioHashString m_PrisonerRope;

	ioUIRenderImage *m_pRunningManTitleLeft;
	ioUIRenderImage *m_pRunningManTitleCenter;
	ioUIRenderImage *m_pRunningManTitleRight;

	bool m_bNameTitleUse;

public:
	virtual void InitMode( int iMapIndex );

protected:
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void LoadModeDeco( ioINILoader &rkLoader );

public:
	void GetModeFixedDeco( DWORD dwIndex, ioBaseChar *pChar, CHARACTER &rkCharInfo, TeamType eTeam );
	ioUIRenderImage *GetModeNameBarLeft() { return m_pRunningManTitleLeft; }
	ioUIRenderImage *GetModeNameBarCenter() { return m_pRunningManTitleCenter; }
	ioUIRenderImage *GetModeNameBarRight() { return m_pRunningManTitleRight; }

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

protected:
	void ClearRunningManDeco();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

public:
	const ioHashString &GetRunningManPrisonerEffect( DWORD dwIndex );
	const ioHashString &GetRunningManEscapeEffect( DWORD dwIndex );
	const ioHashString &GetRunningManPrisonerRope(){ return m_PrisonerRope; }
	const bool& IsRunningMainNameTitleUse(){ return m_bNameTitleUse; }

protected:
	void OnRunningManDecoSync( SP2Packet &rkPacket );
	void OnRunningManNameSync( SP2Packet &rkPacket );

public:
	virtual void OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar );

public:
	ioCatchRunningManMode( ioPlayStage *pCreator );
	virtual ~ioCatchRunningManMode();
};

inline ioCatchRunningManMode* ToCatchRunningManMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_CATCH_RUNNINGMAN )
		return NULL;

	return dynamic_cast< ioCatchRunningManMode* >( pMode );
}

