#include "StdAfx.h"
#include ".\fsmstatemgr.h"

FSMState::FSMState( int a_iState )
{
	m_iState = a_iState;

	m_vInput.reserve(10);
	m_vOutput.reserve(10);
	m_vDelay.reserve(10);
}

FSMState::~FSMState()
{
	m_vInput.clear();
	m_vOutput.clear();
	m_vDelay.clear();
}

int FSMState::GetOutput( int a_iInput , DWORD a_dwCurTime )
{
	int iSize = m_vInput.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( a_iInput == m_vInput[i] )
		{
			if( m_vDelay[i] == NONE_VALUE )
				return m_vOutput[i];

			if( (int)m_vDelay[i] < (int)a_dwCurTime )
				return m_vOutput[i];
		}
	}

	return m_iState;
}

void FSMState::Add( int a_iInput, int a_iOutput, int a_iDelay /*= NONE_VALUE*/  )
{
	m_vInput.push_back( a_iInput );
	m_vOutput.push_back( a_iOutput );
	m_vDelay.push_back( a_iDelay );
}

void FSMState::Delete( int a_iInput )
{
	int iSize = m_vInput.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( a_iInput == m_vInput[i] )
		{
			m_vInput.erase(m_vInput.begin() + i);
			m_vOutput.erase(m_vOutput.begin() + i);
			m_vDelay.erase(m_vDelay.begin() + i);
			break;
		}
	}
}

int FSMState::GetDelay( int a_iInput )
{
	int iSize = m_vInput.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( a_iInput == m_vInput[i] )
		{
			return m_vDelay[i];
		}
	}
	return NONE_VALUE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
FSMStateMgr::FSMStateMgr( )
{
	m_iCurState   = FSMState::NONE_VALUE;
	m_dwStartTime = 0;
	m_iCurState   = 0;
	m_iCurDelay   = FSMState::NONE_VALUE;
	m_iCurArray   = FSMState::NONE_VALUE;
	m_vState.reserve(10);
}

FSMStateMgr::~FSMStateMgr()
{
	for(vState::iterator it = m_vState.begin(); it != m_vState.end(); ++it)
	{
		delete (*it);
	}
	m_vState.clear();
}

int FSMStateMgr::ChangeState( int a_iInput )
{
	bool bChange = false;
	int iCurState = FSMState::NONE_VALUE;
	for(vState::iterator it = m_vState.begin(); it != m_vState.end(); ++it)
	{
		FSMState *pState = (*it);
		if( !pState ) continue;
		if( m_iCurState != pState->GetState() ) continue;

		iCurState = pState->GetOutput( a_iInput,  NONE_CHECK_TIME );

		if( iCurState != m_iCurState )
		{
			bChange = true;
			break;
		}
	}

	if( bChange )
	{
		m_dwStartTime = FRAMEGETTIME();
		m_dwCurTime   = 0;
		m_iCurState   = iCurState;
		m_iCurDelay   = GetDelay( m_iCurState );
		m_iCurArray   = GetArray();
	}

	return m_iCurState;
}

void FSMStateMgr::Add( FSMState *a_pState )
{
	m_vState.push_back( a_pState );
}

void FSMStateMgr::Delete( int a_iState )
{
	for(vState::iterator it = m_vState.begin(); it != m_vState.end(); ++it)
	{
		FSMState *pState = (*it);
		if( !pState ) continue;

		if( a_iState == pState->GetState() )
		{
			m_vState.erase( it );
			break;
		}
	}
}

int FSMStateMgr::GetDelay( int a_iInput )
{
	for(vState::iterator it = m_vState.begin(); it != m_vState.end(); ++it)
	{
		FSMState *pState = (*it);
		if( !pState ) continue;
		if( m_iCurState != pState->GetState() ) continue;

		return pState->GetDelay( a_iInput );
	}

	return FSMState::NONE_VALUE;
}

void FSMStateMgr::UpdateTime()
{
	if( !COMPARE( m_iCurArray, 0, (int) m_vState.size() ) ) return;
	
	m_dwCurTime = FRAMEGETTIME() - m_dwStartTime;
	int iCurState = m_vState[m_iCurArray]->GetOutput( m_iCurState,  m_dwCurTime );

	if( iCurState != m_iCurState )
	{
		m_dwStartTime = FRAMEGETTIME();
		m_dwCurTime   = 0;
		m_iCurState   = iCurState;
		m_iCurDelay   = GetDelay( m_iCurState );
		m_iCurArray   = GetArray();
	}
}

int FSMStateMgr::GetArray()
{
	int iSize = m_vState.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( !m_vState[i] ) continue;
		if( m_iCurState != m_vState[i]->GetState() ) continue;
		
		return i;
	}

	return FSMState::NONE_VALUE;
}

void FSMStateMgr::SetCurState( int iCurState )
{
	m_dwStartTime = FRAMEGETTIME();
	m_dwCurTime   = 0;
	m_iCurState   = iCurState;
	m_iCurDelay   = GetDelay( m_iCurState );
	m_iCurArray   = GetArray();
}

void FSMStateMgr::ClearState()
{
	for(vState::iterator it = m_vState.begin(); it != m_vState.end(); ++it)
	{
		delete (*it);
	}
	m_vState.clear();

	m_dwStartTime = 0;
	m_iCurState   = 0;
	m_iCurDelay   = FSMState::NONE_VALUE;
	m_iCurArray   = FSMState::NONE_VALUE;
}