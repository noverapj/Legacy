#pragma once
enum ConsoleTextColor
{
	CTC_Black = 0,
	CTC_DarkBlue,
	CTC_DarkGreen,
	CTC_DarkSkyBlue,
	CTC_DarkRed,
	CTC_DarkPurple,
	CTC_DarkYellow,
	CTC_Gray,
	CTC_DarkGray,
	CTC_LightBlue,
	CTC_LightGreen,
	CTC_LightSkyBlue,
	CTC_LightRed,
	CTC_LightPurple,
	CTC_LightYellow,
	CTC_White,
	CTC_Default = (CTC_Black<<4 | CTC_Gray)
};

#define CTC_Custom(TextColor, BGColor) (ConsoleTextColor)(BGColor<<4 | TextColor)