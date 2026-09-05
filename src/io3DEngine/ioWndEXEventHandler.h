//----------------------------------------------------------------------------------
/*
Filename	: ioWndEXEventHandler.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOWNDEXEVENTTYPEHANDLER_H_
#define _IOWNDEXEVENTTYPEHANDLER_H_

//----------------------------------------------------------------------------------
/*
ioWndEXEvent
*/
class __EX ioWndEXEvent
{
public:

	ioWndEXEvent() {}
	virtual ~ioWndEXEvent() {}
};

//----------------------------------------------------------------------------------
/*
ioWndEXEventHandlerBase
*/
class ioWndEX;
class __EX ioWndEXEventHandlerBase
{
public:

	ioWndEXEventHandlerBase( ioWndEX* ownerbase ) : m_ownerbase( ownerbase ) {}
	virtual ~ioWndEXEventHandlerBase() {}

	ioWndEX*		GetOwnerBase() { return m_ownerbase; }

	virtual void	Call( ioWndEX* wnd, ioWndEXEvent& event ) = 0;

protected:

	ioWndEX*	m_ownerbase;
};

//----------------------------------------------------------------------------------
/*
ioWndEXEventHandler
*/
template < typename T, typename EOT >
class __EX ioWndEXEventHandler : public ioWndEXEventHandlerBase
{
public:

	typedef void (T::*callback)( ioWndEX*, EOT& );

public:

	ioWndEXEventHandler( T* owner, callback cb )
		: ioWndEXEventHandlerBase( owner )
		, m_owner( owner )
		, m_callback( cb )
	{}

	virtual ~ioWndEXEventHandler()
	{}

	virtual void	Call( ioWndEX* wnd, ioWndEXEvent& event )
	{
		if ( m_owner && m_callback ) {
			(m_owner->*m_callback)( wnd, dynamic_cast<EOT&>( event ) );
		}
	}

protected:

	T*				m_owner;
	callback		m_callback;
};

//----------------------------------------------------------------------------------
/*
ioWndEXMouseEvent
*/
class __EX ioWndEXMouseEvent : public ioWndEXEvent
{
public:

	ioWndEXMouseEvent() : ioWndEXEvent(), m_x( 0 ), m_y( 0 ), m_wheeldelta( 0 ) {}
	ioWndEXMouseEvent( int x, int y ) : ioWndEXEvent(), m_x( x ), m_y( y ), m_wheeldelta( 0 ) {}
	ioWndEXMouseEvent( int wheel ) : ioWndEXEvent(), m_x( 0 ), m_y( 0 ), m_wheeldelta( wheel ) {}

	virtual ~ioWndEXMouseEvent() {}

public:

	int				m_x;
	int				m_y;
	int				m_wheeldelta;
};

//----------------------------------------------------------------------------------
/*
ioWndEXMouseEventHandler
*/
template < typename T >
class __EX ioWndEXMouseEventHandler : public ioWndEXEventHandler< T, ioWndEXMouseEvent >
{
public:

	ioWndEXMouseEventHandler( T* owner, callback cb )
		: ioWndEXEventHandler< T, ioWndEXMouseEvent >( owner, cb )
	{}

	virtual ~ioWndEXMouseEventHandler() {}
};


//----------------------------------------------------------------------------------
/*
ioWndEXRadioEvent
*/
class __EX ioWndEXRadioEvent : public ioWndEXEvent
{
public:

	ioWndEXRadioEvent() : ioWndEXEvent(), m_button( 0 ), m_index( 0 ) {}
	ioWndEXRadioEvent( ioWndEX* btn, int idx ) : ioWndEXEvent(), m_button( btn ), m_index( idx ) {}

	virtual ~ioWndEXRadioEvent() {}

public:

	ioWndEX*	m_button;
	int			m_index;
};

//----------------------------------------------------------------------------------
/*
ioWndEXRadioEventHandler
*/
template < typename T >
class __EX ioWndEXRadioEventHandler : public ioWndEXEventHandler< T, ioWndEXRadioEvent >
{
public:

	ioWndEXRadioEventHandler( T* owner, callback cb )
		: ioWndEXEventHandler< T, ioWndEXRadioEvent >( owner, cb )
	{}

	virtual ~ioWndEXRadioEventHandler() {}
};

//----------------------------------------------------------------------------------
/*
ioWndEXScrollBarEvent
*/
class __EX ioWndEXScrollBarEvent : public ioWndEXEvent
{
public:

	ioWndEXScrollBarEvent() : ioWndEXEvent(), m_pos( 0 ) {}
	ioWndEXScrollBarEvent( int pos ) : ioWndEXEvent(), m_pos( pos ) {}

	virtual ~ioWndEXScrollBarEvent() {}

public:

	int		m_pos;
};

//----------------------------------------------------------------------------------
/*
ioWndEXScrollBarEventHandler
*/
template < typename T >
class __EX ioWndEXScrollBarEventHandler : public ioWndEXEventHandler< T, ioWndEXScrollBarEvent >
{
public:

	ioWndEXScrollBarEventHandler( T* owner, callback cb )
		: ioWndEXEventHandler< T, ioWndEXScrollBarEvent >( owner, cb )
	{}

	virtual ~ioWndEXScrollBarEventHandler() {}
};


#endif // _IOWNDEXEVENTTYPEHANDLER_H_