

#ifndef _ioParticleIterator_h_
#define _ioParticleIterator_h_

class ioParticle;
class ioParticleSystem;

class __EX ioParticleIterator
{
	friend class ioParticleSystem;

private:
	std::list<ioParticle*>::iterator m_Pos;
	std::list<ioParticle*>::iterator m_Start;
	std::list<ioParticle*>::iterator m_End;

	ioParticleIterator( std::list<ioParticle*>::iterator iStart,
						std::list<ioParticle*>::iterator iEnd );

public:
	inline bool end()
	{
		return ( m_Pos == m_End );
	}

	inline ioParticle* GetNext()
	{
		return *m_Pos++;
	}
};

#endif
