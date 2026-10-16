#pragma once

#pragma once

#include "Core.hpp"

namespace Engine::Rendering
{
	class RenderTarget
	{
	public:
		RenderTarget& operator=(const RenderTarget&) = delete;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;
		virtual ~RenderTarget() = default;

		virtual void setBackgroundColor(glm::vec4 color) = 0;
		static const bool *getKeystates(int &numKeys);

		static Uint64 getTime();

		float getDeltaTime() {return deltaTime;}
		glm::vec2 getRes() {return res;}
		float getWidth() {return res.x;}
		float getHeight() {return res.y;}
		float getFPS() {return fps;}

		void update();
		virtual void clearBackground() = 0;
	protected:
		float fps = 0;
		glm::vec2 res;
		float deltaTime = 0;
		void startLastCounter();
	private:
		Uint64 lastCounter;
	};
}