#pragma once

#include <memory>
#include "Window.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Image.hpp"
#include "Buff.hpp"
#include "FrameBuffer.hpp"

namespace NApiFunctions {
	using namespace Engine::Rendering;

	using CreateShaderFn =
		std::unique_ptr<Shader>(*)(...);

	using CreateMeshFn =
		std::unique_ptr<Mesh>(*)(...);

	using CreateImageFromFileFn =
		std::unique_ptr<Image>(*)(...);

	using CreateImageFromSurfaceFn =
		std::unique_ptr<Image>(*)(...);

	using CreateWindowFn =
		std::unique_ptr<Window>(*)(...);

	using CreateBuffFn =
		std::unique_ptr<Buff>(*)(...);

	using CreateDrawFn =
		void(*)(...);

	using CreateFBOFn =
		std::unique_ptr<FrameBuffer>(*)(...);

	typedef const char *(*GetNameFn)();

	struct APIFunctions {
		CreateShaderFn createShaderFunc = nullptr;
		CreateMeshFn createMeshFunc = nullptr;
		CreateImageFromFileFn createImageFromFileFunc = nullptr;
		CreateImageFromSurfaceFn createImageFromSurfaceFunc = nullptr;
		CreateWindowFn createWindowFunc = nullptr;
		CreateBuffFn createBuffFunc = nullptr;
		CreateDrawFn createDrawFunc = nullptr;
		CreateFBOFn createFboFunc = nullptr;
	};
	APIFunctions *getApiFunctions();
}