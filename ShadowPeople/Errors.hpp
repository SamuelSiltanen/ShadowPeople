#pragma once

// The purpose of this file is to unify error handling accross the project,
// remove mundane details off the main code path, and collect error strings
// in one place. This should clarify the main code and help localization if
// necessary.

#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

#ifndef NDEBUG
#define SP_ASSERT(cond, msg) assert((cond) && msg);
#else
#define SP_ASSERT(cond, msg)
#endif

#define ERROR_CODE_NO_ERROR							__COUNTER__
#define ERROR_CODE_WINDOW_CREATION_FAILED			__COUNTER__
#define ERROR_CODE_GET_CLIENT_RECT_FAILED			__COUNTER__
#define ERROR_CODE_WIN_CLASS_REG_FAILED				__COUNTER__
#define ERROR_CODE_GET_BACK_BUFFER_FAILED			__COUNTER__
#define ERROR_CODE_GET_SWAP_CHAIN_DESC_FAILED		__COUNTER__
#define ERROR_CODE_COMPUTE_SHADER_NOT_CREATED		__COUNTER__
#define ERROR_CODE_VERTEX_SHADER_NOT_CREATED		__COUNTER__
#define ERROR_CODE_PIXEL_SHADER_NOT_CREATED			__COUNTER__
#define ERROR_CODE_DEPTH_STENCIL_STATE_NOT_CREATED	__COUNTER__
#define ERROR_CODE_BLEND_STATE_NOT_CREATED			__COUNTER__
#define ERROR_CODE_RASTERIZER_STATE_NOT_CREATED		__COUNTER__
#define ERROR_CODE_MAP_DISCARD_FAILED				__COUNTER__
#define ERROR_CODE_INPUT_LAYOUT_NOT_CREATED			__COUNTER__
#define ERROR_CODE_PRESENT_FAILED					__COUNTER__

#define ERROR_CODE_TEXTURE_NULL						__COUNTER__
#define ERROR_CODE_TEXTURE_VIEW_NULL				__COUNTER__
#define ERROR_CODE_BUFFER_NULL						__COUNTER__
#define ERROR_CODE_BUFFER_VIEW_NULL					__COUNTER__
#define ERROR_CODE_SAMPLER_NULL						__COUNTER__
#define ERROR_CODE_COMMAND_BUFFER_NULL				__COUNTER__
#define ERROR_CODE_GRAPHICS_PIPELINE_NULL			__COUNTER__
#define ERROR_CODE_COMPUTE_PIPELINE_NULL			__COUNTER__
#define ERROR_CODE_DEVICE_NULL						__COUNTER__
#define ERROR_CODE_SHADER_MANAGER_NULL				__COUNTER__

static const LPSTR SP_error_messages[] = 
{
	_T("Success."),
	_T("Failed to create window!"),
	_T("Failed to get window client rectangle!"),
	_T("Window class registration failed!"),
	_T("Failed to get the back buffer!"),
	_T("Failed to get swap chain descriptor!"),
	_T("Failed to create compute shader!"),
	_T("Failed to create vertex shader!"),
	_T("Failed to create pixel shader!"),
	_T("Failed to create depth stencil state!"),
	_T("Failed to create blend state!"),
	_T("Failed to create rasterizer state!"),
	_T("Map discard failed!"),
	_T("Failed to create input layout!"),
	_T("Present failed!"),

	_T("Texture used, but not created with Device::createTexture()!"),
	_T("TextureView used, but not created with Device::createTextureView()!"),
	_T("Buffer used, but not created with Device::createBuffer()!"),
	_T("BufferView used, but not created with Device::createBufferView()!"),
	_T("Sampler used, but not created with Device::createSampler()!"),
	_T("CommandBuffer used, but created with Device::createCommandBuffer()!"),
	_T("GraphicsPipeline used, but not created with Device::createGraphicsPipeline()!"),
	_T("ComputePipeline used, but not created with Device::createComputePipeline()!"),
	_T("Device used after a failed Device creation!"),
	_T("ShaderManager used after a failed Device creation!")
};

#define SP_EXPECT_NOT_NULL_RET(var, code, ret) \
	if (!(var)) \
	{ \
		MessageBox(NULL, SP_error_messages[code], _T("Error"), NULL); \
		return ret; \
	}
#define SP_EXPECT_NOT_NULL(var, code) \
	if (var == nullptr) \
	{ \
		MessageBox(NULL, SP_error_messages[code], _T("Error"), NULL); \
		assert(false); \
	}

#define SP_ASSERT_HR(hr, code) \
	if (FAILED(hr)) \
	{ \
		MessageBox(NULL, SP_error_messages[code], _T("Error"), NULL); \
	}

#define SP_DEBUG_OUTPUT(str) \
	OutputDebugString(str);