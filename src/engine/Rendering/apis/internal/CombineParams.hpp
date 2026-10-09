#pragma once

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