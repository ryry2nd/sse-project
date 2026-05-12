#include "SDL3/SDL_surface.h"
#include <string>
struct TTF_Font;

namespace Rendering {
	class Font
	{
	public:
		static void init();
		static void deinit();
		Font(const std::string &fontPath, int size);
		~Font();
		SDL_Surface *renderText(const std::string &message, const glm::vec4 &color);

	private:
		TTF_Font *font;
	};
}