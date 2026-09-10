
#include "stdafx.h"

#include "ioJumpController.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

ioJumpController::ioJumpController()
{
	m_dwCurrentTime = 0;
}

ioJumpController::~ioJumpController()
{
	JumpMap::iterator iCreate;
	for( iCreate = m_JumpMap.begin() ; iCreate != m_JumpMap.end() ; ++iCreate )
	{
		JumpList &kList = iCreate->second;
		kList.m_Jump.clear();
	}
	m_CurJump.m_Jump.clear();
}

void ioJumpController::LoadController( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "JumpController" );
	int iMaxController = rkLoader.LoadInt_e( "MaxController", 0 );
	for(int i = 0;i < iMaxController;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "jump%d", i + 1 );
		rkLoader.SetTitle( szKey );

		JumpList kJumpList;
		kJumpList.m_fJumpRange = rkLoader.LoadFloat_e( "TargetRange", 99999999.0f );

		int iMaxJump = rkLoader.LoadInt_e( "MaxKey", 0 );
		for(int j = 0;j < iMaxJump;j++)
		{
			JumpData kData;

			sprintf_e( szKey, "Key%d", j + 1 );
			kData.m_wKeyData  = rkLoader.LoadInt( szKey, 0 );
			sprintf_e( szKey, "Key%d_Delay", j + 1 );
			kData.m_dwDelayTime = rkLoader.LoadInt( szKey, 0 );

			kJumpList.m_Jump.push_back( kData );
		}
		m_JumpMap.insert( JumpMap::value_type( i + 1, kJumpList ) );
	}
}

void ioJumpController::ClearCurrentData()
{
	m_dwCurrentTime = 0;
	m_CurJump.m_Jump.clear();
}

void ioJumpController::AttachJumpData( JumpList &rkList )
{
	// Jump Copy
	m_CurJump.m_fJumpRange = rkList.m_fJumpRange;
	m_CurJump.m_Jump.clear();
	for(int i = 0;i < (int)rkList.m_Jump.size();i++)
	{
		JumpData &rkData = rkList.m_Jump[i];
		m_CurJump.m_Jump.push_back( rkData );
	}
	m_dwCurrentTime = FRAMEGETTIME();
}

bool ioJumpController::Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fDistSq )
{
	if( pOwner == NULL ) return false;

	float fMinRange = 0.0f;
	DWORDVec vJumpID;
	JumpMap::iterator iCreate;
	for( iCreate = m_JumpMap.begin() ; iCreate != m_JumpMap.end() ; ++iCreate )
	{
		JumpList &rkList = iCreate->second;
		
		float fJumpRange = rkList.m_fJumpRange * rkList.m_fJumpRange;
		if( fDistSq > fMinRange && fDistSq < fJumpRange )
		{
			vJumpID.push_back( iCreate->first );						
		}
		fMinRange += rkList.m_fJumpRange;
	}

	if( !vJumpID.empty() )
	{
		int r = rand()%vJumpID.size();
		JumpMap::iterator iter = m_JumpMap.find( vJumpID[r] );
		if( iter != m_JumpMap.end() )
		{
			AttachJumpData( iter->second );
			return true;
		}
	}
	return false;
}

bool ioJumpController::OnJumpAction( ioNpcChar *pOwner, ioBaseChar *pTarget )
{
	if( m_CurJump.m_Jump.empty() ) return false;

	JumpData &rkData = m_CurJump.m_Jump[0];
	{
		pOwner->SetActionKey( KEY_NONE, rkData.m_wKeyData, KEY_NONE );

		if( FRAMEGETTIME() - m_dwCurrentTime > rkData.m_dwDelayTime )
		{
			m_dwCurrentTime = FRAMEGETTIME();
			m_CurJump.m_Jump.erase( m_CurJump.m_Jump.begin() );
		}
	}			
	return true;
}

bool ioJumpController::IsJump()
{
	if( m_CurJump.m_Jump.empty() )
		return false;
	return true;
}