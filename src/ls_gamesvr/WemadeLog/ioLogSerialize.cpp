#include "stdafx.h"
#include "ioWemadeLogger.h"
#include "ioLogSerialize.h"

ioWemadeLogger g_WemadeLogger;

ioLogSerialize::ioLogSerialize()
{
	Init();
}

ioLogSerialize::~ioLogSerialize()
{
	Destroy();
}

void ioLogSerialize::Init()
{
}

void ioLogSerialize::Destroy()
{
	End();
}

void ioLogSerialize::Begin( const int logType )
{
	g_WemadeLogger.Begin( logType );
}

void ioLogSerialize::End()
{
	g_WemadeLogger.End();
}

void ioLogSerialize::Append( uint8 value )
{
	g_WemadeLogger.Write( (uint32)value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( uint16 value )
{
	g_WemadeLogger.Write( (uint32)value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( uint32 value )
{
	g_WemadeLogger.Write( value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( uint64 value )
{
	g_WemadeLogger.Write( value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( short value )
{
	g_WemadeLogger.Write( (int32)value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( float value )
{
	g_WemadeLogger.Write( value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( double value )
{
	g_WemadeLogger.Write( value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( int value )
{
	g_WemadeLogger.Write( (int32)value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( int64 value )
{
	g_WemadeLogger.Write( (int64)value );
	cSerialize::Write( value );
}

void ioLogSerialize::Append( const char* value, uint32 length, BOOL saveLength )
{
	g_WemadeLogger.Write( value );
	cSerialize::Write( value, length, saveLength );
}
