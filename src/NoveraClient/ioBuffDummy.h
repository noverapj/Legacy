#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioBuffDummy : public ioDummyChar
{
private:
	enum
	{
		SYNC_ADD_TARGET = 1000,
		SYNC_DEL_TARGET = 1001,
	};
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetUpHeight;
	CEncrypt<float> m_fTargetUnderHeight;
	CEncrypt<int> m_iTargetCount;

	TargetWoundType m_TargetWoundType;

	ioHashStringVec m_vBuffName;

	ioHashString m_szGuideEffect;
	CEncrypt<int> m_iGuideMaxCount;

	struct TargetInfo
	{
		ioHashString target_name;
		std::vector<UniqueObjID> m_EffectList;
		TargetInfo()
		{
			Init();
		}
		TargetInfo( ioHashString _name )
		{
			target_name = _name;
			m_EffectList.clear();
		}
		void Init()
		{
			target_name = NULL;
			m_EffectList.clear();
		}
		void AddEffectID( UniqueObjID _id )
		{
			m_EffectList.push_back(_id);
		}
		void ClearEffectID()
		{
			m_EffectList.clear();
		}
		int GetEffectCount()
		{
			return (int)m_EffectList.size();
		}
		UniqueObjID GetEffectID( int iArray )
		{
			if( !COMPARE( iArray, 0, (int)m_EffectList.size() ) )
				return 0;

			return m_EffectList[iArray];
		}
	};
	typedef std::vector<TargetInfo> vTargetInfo;
	vTargetInfo m_vTarget;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );

	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

private:
	void CheckTarget();
	void FindTarget();
	bool EnableTarget( ioHashString szTargetName );
	bool IsAlreadyTarget( ioHashString szTargetName );

	void AddTarget( ioHashString szTargetName, bool bSend = true );
	void DelTarget( ioHashString szTargetName, bool bSend = true );
	void DelAllTarget();

// Guide Effect
private:
	void ProcessGuideEffect();
	void DestroyGuideEffect( TargetInfo &info );
	bool CheckGuideEffectCreated( TargetInfo &info );
	void ArrangeGuideEffect( TargetInfo &info );

public:
	ioBuffDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioBuffDummy();
};

inline ioBuffDummy* ToBuffDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_BUFF_DUMMY )
		return NULL;

	return dynamic_cast< ioBuffDummy* >( pDummy );
}