#pragma once

#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

#ifndef NDEBUG
#define SP_ASSERT(cond, msg) assert((cond) && msg);
#else
#define SP_ASSERT(cond, msg)
#endif

#define ERROR_CODE_NO_ERROR					0x0000
#define ERROR_CODE_WINDOW_CREATION_FAILED	0x0001
#define ERROR_CODE_GET_CLIENT_RECT_FAILED	0x0002
#define ERROR_CODE_WIN_CLASS_REG_FAILED		0x0003
#define ERROR_CODE_GET_BACK_BUFFER_FAILED	0x0004

static const LPSTR SP_error_messages[] = 
{
	_T("Success."),
	_T("Failed to create window!"),
	_T("Failed to get window client rectangle!"),
	_T("Window class registration failed!"),
	_T("Failed to get the back buffer!")
};

#define SP_EXPECT_NOT_NULL_RET(var, code, ret) \
	if (!(var)) \
	{ \
		MessageBox(NULL, SP_error_messages[code], _T("Error"), NULL); \
		return ret; \
	}
#define SP_EXPECT_NOT_NULL(var, code) SP_EXPECT_NOT_NULL_RET(var, code, code)