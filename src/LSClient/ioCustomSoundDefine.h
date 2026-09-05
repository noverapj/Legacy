#pragma once


#define CUSTOM_SOUND_PACKET_SIZE 400

typedef struct tagCustomSoundPacket
{
	char  m_Data[CUSTOM_SOUND_PACKET_SIZE];
	DWORD m_dwWrittenSize;

	tagCustomSoundPacket()
	{
		ZeroMemory( m_Data, sizeof(m_Data) );
		m_dwWrittenSize = 0;
	}
}CustomSoundPacket;


