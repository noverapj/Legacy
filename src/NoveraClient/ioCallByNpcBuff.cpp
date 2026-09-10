#include "StdAfx.h"

#include "ioCallByNpcBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioTowerDefMode.h"

ioCallByNpcBuff::ioCallByNpcBuff(void)
{
}

ioCallByNpcBuff::ioCallByNpcBuff( const ioCallByNpcBuff &rhs )
	: ioBuff( rhs ),
	m_vecBlueBuff(rhs.m_vecBlueBuff),
	m_vecRedBuff(rhs.m_vecRedBuff)
	
{
}


ioCallByNpcBuff::~ioCallByNpcBuff(void)
{
}

void ioCallByNpcBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_vecBlueBuff.clear();
	m_vecRedBuff.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int nBuff = rkLoader.LoadInt_e("bluebuffbynpc_cnt", 0);
	for(int i = 0; i < nBuff; i++)
	{
		BuffByNpc stBuff;
		stBuff.bLive = false;
		wsprintf_e( szKey, "bluenpc%d_code", i+1 );
		stBuff.dwNpcCode = rkLoader.LoadInt(szKey, 0);

		wsprintf_e( szKey, "bluecall_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		stBuff.szBuff = szBuf;

		m_vecBlueBuff.push_back(stBuff);
	}

	nBuff = rkLoader.LoadInt_e("redbuffbynpc_cnt", 0);
	for(int i = 0; i < nBuff; i++)
	{
		BuffByNpc stBuff;
		stBuff.bLive = false;
		wsprintf_e( szKey, "rednpc%d_code", i+1 );
		stBuff.dwNpcCode = rkLoader.LoadInt(szKey, 0);

		wsprintf_e( szKey, "redcall_buff%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		stBuff.szBuff = szBuf;

		m_vecRedBuff.push_back(stBuff);
	}
}

ioBuff* ioCallByNpcBuff::Clone()
{
	return new ioCallByNpcBuff( *this );
}

void ioCallByNpcBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if(pOwner->GetCreator())
	{
		ioPlayMode *pMode = pOwner->GetCreator()->GetPlayMode();

		if(!pMode) return;

		ioTowerDefMode* pTowerMode = ToTowerDefMode(pMode);

		if(!pTowerMode) return;
		if( pOwner->GetTeam() == TEAM_RED)
		{
			for(int i = 0; i < (int)m_vecRedBuff.size(); i++)
			{
				if(pTowerMode->IsAliveNpcbyCode(m_vecRedBuff[i].dwNpcCode))
				{
					if(!m_vecRedBuff[i].bLive)
					{
						m_vecRedBuff[i].bLive = true;
						if(m_pOwner)
							//m_pOwner->AddNewBuff(m_vecRedBuff[i].szBuff, "", "", NULL );
							m_pOwner->ReserveAddNewBuff( m_vecRedBuff[i].szBuff, "", "", NULL );
					}
				}
			}
		}
		else if( pOwner->GetTeam() == TEAM_BLUE)
		{
			for(int i = 0; i < (int)m_vecBlueBuff.size(); i++)
			{
				if(pTowerMode->IsAliveNpcbyCode(m_vecBlueBuff[i].dwNpcCode))
				{
					if(!m_vecBlueBuff[i].bLive)
					{
						m_vecBlueBuff[i].bLive = true;
						if(m_pOwner)
							//m_pOwner->AddNewBuff(m_vecBlueBuff[i].szBuff, "", "", NULL );
							m_pOwner->ReserveAddNewBuff( m_vecBlueBuff[i].szBuff, "", "", NULL );
					}
				}
			}
		}
	}
}


bool ioCallByNpcBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if(pOwner->GetCreator())
	{
		ioPlayMode *pMode = pOwner->GetCreator()->GetPlayMode();

		if(!pMode) return false;

		ioTowerDefMode* pTowerMode = ToTowerDefMode(pMode);
		if(!pTowerMode) return false;
		if( pOwner->GetTeam() == TEAM_RED)
		{
			for(int i = 0; i < (int)m_vecRedBuff.size(); i++)
			{
				if(pTowerMode->IsAliveNpcbyCode(m_vecRedBuff[i].dwNpcCode))
				{
					if(!m_vecRedBuff[i].bLive)
					{
						m_vecRedBuff[i].bLive = true;
						if(m_pOwner)
							//m_pOwner->AddNewBuff(m_vecRedBuff[i].szBuff, "", "", NULL );
							m_pOwner->ReserveAddNewBuff( m_vecRedBuff[i].szBuff, "", "", NULL );
					}
				}
			}
		}
		else if( pOwner->GetTeam() == TEAM_BLUE)
		{
			for(int i = 0; i < (int)m_vecBlueBuff.size(); i++)
			{
				if(pTowerMode->IsAliveNpcbyCode(m_vecBlueBuff[i].dwNpcCode))
				{
					if(!m_vecBlueBuff[i].bLive)
					{
						m_vecBlueBuff[i].bLive = true;
						if(m_pOwner)
							//m_pOwner->AddNewBuff(m_vecBlueBuff[i].szBuff, "", "", NULL );
							m_pOwner->ReserveAddNewBuff( m_vecBlueBuff[i].szBuff, "", "", NULL );
					}
				}
			}
		}
	}

	return true;
}

void ioCallByNpcBuff::SetBlueBuff(const DWORD dwNpcCode, bool bAlive)
{
	for(int i = 0; i < (int)m_vecBlueBuff.size(); i++)
	{
		if(dwNpcCode == m_vecBlueBuff[i].dwNpcCode)
		{
			if(bAlive && !m_vecBlueBuff[i].bLive)
			{
				m_vecBlueBuff[i].bLive = true;
				if(m_pOwner)
					//m_pOwner->AddNewBuff(m_vecBlueBuff[i].szBuff, "", "", NULL );
					m_pOwner->ReserveAddNewBuff( m_vecBlueBuff[i].szBuff, "", "", NULL );
			}
			else if(!bAlive && m_vecBlueBuff[i].bLive)
			{
				m_vecBlueBuff[i].bLive = false;
				if(m_pOwner)
					m_pOwner->RemoveBuff(m_vecBlueBuff[i].szBuff);
			}
		}
	}
}

void ioCallByNpcBuff::SetRedBuff(const DWORD dwNpcCode, bool bAlive)
{
	for(int i = 0; i < (int)m_vecRedBuff.size(); i++)
	{
		if(dwNpcCode == m_vecRedBuff[i].dwNpcCode)
		{
			if(bAlive && !m_vecRedBuff[i].bLive)
			{
				m_vecRedBuff[i].bLive = true;
				if(m_pOwner)
					//m_pOwner->AddNewBuff(m_vecRedBuff[i].szBuff, "", "", NULL );
					m_pOwner->ReserveAddNewBuff( m_vecRedBuff[i].szBuff, "", "", NULL );
			}
			else if(!bAlive && m_vecRedBuff[i].bLive)
			{
				m_vecRedBuff[i].bLive = false;
				if(m_pOwner)
					m_pOwner->RemoveBuff(m_vecRedBuff[i].szBuff);
			}
		}
	}
}



