#pragma once

#include <engine/Rendering/Rendering.hpp>
#include <spdlog/spdlog.h>

namespace Engine::Rendering {
	#define combineParams(out, params, size) \
	{ \
		out = *params; \
		for (size_t i = 0; i < size; i++) \
		{ \
			const auto& p = params[i]; \
			if (p.fieldMask & InternalParams::HasSettings) out.settings |= p.settings; \
			if (p.fieldMask & InternalParams::HasInstanceCount) out.instanceCount = p.instanceCount; \
			if (p.fbo) out.fbo = p.fbo; \
			if (p.shader) out.shader = p.shader; \
			if (p.fieldMask & InternalParams::HasUseArray) out.useArray = p.useArray; \
			if (p.fieldMask & InternalParams::HasArrayData) \
			{ \
				out.arrayFirst = p.arrayFirst; \
				out.arrayCount = p.arrayCount; \
			} \
			for (auto& [k, v] : p.images) out.images[k] = v; \
			for (auto& [k, v] : p.ubo)    out.ubo[k] = v; \
			for (auto& [k, v] : p.ssbo)   out.ssbo[k] = v; \
		} \
	}
	inline void drawinit(Window *win, DrawParams* pms, size_t size, InternalParams &params, FrameBuffer *& fbo, bool &renderScreen, bool &renderFBO) {
		if (!pms || !win) return;

		combineParams(params, pms, size);

		if (!params.shader) return;

		fbo	= params.fbo;
		renderScreen	= !(params.settings & InternalParams::DisableScreen);
		renderFBO		= (params.settings & InternalParams::EnableFBO) && (fbo != nullptr);

		if (renderScreen && renderFBO) {
			static bool isWarned = false;
			if (!isWarned) {
				spdlog::warn(
					"Rendering to an fbo and a screen at the same time is not supported.\n"
					"\t\tThis forces the engine to render the draw call twice.\n"
					"\t\tIf you want to do this then render to FBO only and then render it as a new draw call as a new image."
				);
				isWarned = true;
			}
		}
	}
}