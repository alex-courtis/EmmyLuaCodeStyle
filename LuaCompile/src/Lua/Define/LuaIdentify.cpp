﻿#include "LuaCompile/Lua/Define/LuaIdentify.h"
#include "LuaCompile/Lua/Define/LuaDefine.h"

std::array<unsigned char, UCHAR_MAX + 2> LuaIdentify::LuaCharType = {
	0x00, /* EOZ */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0. */
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 1. */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, /* 2. */
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, /* 3. */
	0x16, 0x16, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x05, /* 4. */
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, /* 5. */
	0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x05,
	0x04, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x05, /* 6. */
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, /* 7. */
	0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x00,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* 8. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* 9. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* a. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* b. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	0x00, 0x00, NONA, NONA, NONA, NONA, NONA, NONA, /* c. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* d. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA, /* e. */
	NONA, NONA, NONA, NONA, NONA, NONA, NONA, NONA,
	NONA, NONA, NONA, NONA, NONA, 0x00, 0x00, 0x00, /* f. */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void LuaIdentify::AddIdentifyChar(char ch)
{
	if (ch < 0)
	{
		return;
	}
	LuaCharType[ch + 1] |= MASK(ALPHABIT);
}
