#pragma once

#include <memory>
#include "../../Window.hpp"
#include "../../Shader.hpp"
#include "../../Mesh.hpp"
#include "../../Image.hpp"
#include "../../Buff.hpp"
#include "../../FrameBuffer.hpp"

namespace NApiFunctions {
	using namespace Engine::Rendering;

	using CreateShaderFn =
		std::unique_ptr<Shader>(*)(...);

	using CreateMeshFn =
		std::unique_ptr<Mesh>(*)(...);

	using CreateImageFromSurfaceFn =
		std::unique_ptr<Image>(*)(...);

	using CreateImageFromSizeFn =
		std::unique_ptr<Image>(*)(...);

	using CreateWindowFn =
		std::unique_ptr<Window>(*)(...);

	using CreateBuffFn =
		std::unique_ptr<Buff>(*)(...);

	using CreateDrawFn =
		void(*)(...);

	using CreateDrawCompFn =
		void(*)(...);

	using CreateFBOFn =
		std::unique_ptr<FrameBuffer>(*)(...);

	typedef const char *(*GetNameFn)();

	struct APIFunctions {
		CreateShaderFn createShaderFunc = nullptr;
		CreateMeshFn createMeshFunc = nullptr;
		CreateImageFromSurfaceFn createImageFromSurfaceFunc = nullptr;
		CreateImageFromSizeFn createImageFromSizeFunc = nullptr;
		CreateWindowFn createWindowFunc = nullptr;
		CreateBuffFn createBuffFunc = nullptr;
		CreateDrawFn createDrawFunc = nullptr;
		CreateDrawCompFn createDrawCompFunc = nullptr;
		CreateFBOFn createFboFunc = nullptr;
	};
	APIFunctions *getApiFunctions();
}