#pragma once

#include "SDL3/SDL_stdinc.h"
#include "Shader.hpp"
#include "Buff.hpp"
#include "Image.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"
#include "Window.hpp"
#include "Font.hpp"
#include "Surface.hpp"
#include "Event.hpp"

#include <unordered_map>

namespace Engine::Rendering
{
	struct InternalParams
	{
		enum Fields : uint32_t
		{
			HasInstanceCount	= 1 << 0,
			HasSettings			= 1 << 1,
			HasIdxData    		= 1 << 2,
		};

		static const uint32_t ENABLE_ALL = ~ uint32_t(0); // all 1s

		uint32_t fieldMask = ENABLE_ALL;

		enum Settings : uint32_t {
			EnableClear			= 1 << 0,
			DisableCulling		= 1 << 1,
			DisableDepth		= 1 << 2,
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


		Shader* shader = nullptr;

		int idxFirst = 0;
		int idxCount = 0;

		std::unordered_map<Uint32, Image*> images;
		std::unordered_map<Uint32, Image*> rwimages;
		std::unordered_map<Uint32, Buff*> ubo;
		std::unordered_map<Uint32, Buff*> ssbo;
	};

	struct ComputeParams
	{
		Shader* shader = nullptr;

		glm::vec3 groups;

		std::unordered_map<Uint32, Image*> images;
		std::unordered_map<Uint32, Buff*> ubo;
		std::unordered_map<Uint32, Buff*> ssbo;

		enum Outputs : Uint8
		{
			SSBO	= 1 << 0,
			IMAGE	= 1 << 1,
		};

		Uint8 output = Outputs::SSBO;
	};

	typedef InternalParams DrawParams;
	typedef InternalParams Material;
}