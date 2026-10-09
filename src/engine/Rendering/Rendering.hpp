#pragma once

#include "Shader.hpp"
#include "Buff.hpp"
#include "Image.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"
#include "Window.hpp"

#include <unordered_map>

namespace Engine::Rendering
{
	struct InternalParams
	{
		enum Fields : uint32_t
		{
			HasInstanceCount	= 1 << 0,
			HasSettings			= 1 << 1,
			HasUseArray     	= 1 << 2,
			HasArrayData    	= 1 << 3,
		};

		static const uint32_t ENABLE_ALL = ~ uint32_t(0); // all 1s

		uint32_t fieldMask = ENABLE_ALL;

		enum Settings : uint32_t {
			DisableScreen		= 1 << 0,
			EnableFBO			= 1 << 1,
			EnableClear			= 1 << 2,
			DisableCulling		= 1 << 3,
			DisableDepth		= 1 << 4,
		};

		uint32_t settings = 0;

		enum class MeshTypes
		{
			Points,
			Lines,
			Triangles
		};

		MeshTypes meshType = MeshTypes::Triangles;



		size_t instanceCount = 0;


		FrameBuffer* fbo = nullptr;
		Shader* shader = nullptr;

		bool useArray = false;
		int arrayFirst = 0;
		int arrayCount = 6;

		std::unordered_map<Uint32, Image*> images;
		std::unordered_map<Uint32, Buff*> ubo;
		std::unordered_map<Uint32, Buff*> ssbo;
	};

	typedef InternalParams DrawParams;
	typedef InternalParams Material;
}