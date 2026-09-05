#pragma once


class ioINILoader;
class ioWorldEventReceiver;

class ioZoneAffector
{
public:
	enum ZoneAffectorType
	{
		ZAT_NONE,
		ZAT_FIRE_ZONE,
		ZAT_POISON_ZONE,
		ZAT_ICE_ZONE,
	};

public:
	virtual void LoadProperty( ioINILoader &rkLoader ) = 0;
	virtual ZoneAffectorType GetType() const = 0;

public:
	virtual void ApplyToReceiver( ioWorldEventReceiver *pReceiver ) = 0;

public:
	static ioZoneAffector* CreateAffector( ZoneAffectorType eType );

public:
	ioZoneAffector();
	ioZoneAffector( const ioZoneAffector &rhs );
	virtual ~ioZoneAffector();
};

//----------------------------------------------------------

class ioFireZoneAffector : public ioZoneAffector
{
protected:
	float m_fFireDamage;
	ioHashString m_FireBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ZoneAffectorType GetType() const;

public:
	virtual void ApplyToReceiver( ioWorldEventReceiver *pReceiver );

public:
	ioFireZoneAffector();
	ioFireZoneAffector( const ioFireZoneAffector &rhs );
	virtual ~ioFireZoneAffector();
};

//----------------------------------------------------------

class ioPoisonZoneAffector : public ioZoneAffector
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ZoneAffectorType GetType() const;

public:
	virtual void ApplyToReceiver( ioWorldEventReceiver *pReceiver );

public:
	ioPoisonZoneAffector();
	ioPoisonZoneAffector( const ioPoisonZoneAffector &rhs );
	virtual ~ioPoisonZoneAffector();
};

