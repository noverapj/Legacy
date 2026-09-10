// VoiceChatDefine.h: interface for the VoiceChatDefine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOICECHATDEFINE_H__64C6FDE3_6893_4FB0_8BD2_300B2F701552__INCLUDED_)
#define AFX_VOICECHATDEFINE_H__64C6FDE3_6893_4FB0_8BD2_300B2F701552__INCLUDED_


#define NUM_REC_NOTIFICATIONS     5
#define SIZE_NOTIFY               4160

#define WAVE_FORMAT_CH            1  
#define WAVE_FORMAT_SAMPLESPERSEC 8000
#define WAVE_FORMAT_BITPERSAMPLE  16

#define MAX_DS_BUFFER_SIZE        (SIZE_NOTIFY*40)
#define VOICE_DATA_SIZE           390


typedef struct tagVOICEDATA
{
	char DATA[VOICE_DATA_SIZE];

}VOICEDATA;

#endif // !defined(AFX_VOICECHATDEFINE_H__64C6FDE3_6893_4FB0_8BD2_300B2F701552__INCLUDED_)
