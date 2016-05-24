#include <SDL2/SDL.h>


namespace demo {


struct WindowDeleter
{
	inline void operator()(SDL_Window* pWindow)
	{
		if (pWindow)
		{
			SDL_DestroyWindow(pWindow);
		}
	}
};


struct GLContextDeleter
{
	inline void operator()(void* pContext)
	{
		if (pContext)
		{
			SDL_GL_DeleteContext(pContext);
		}
	}
};


} // namespace demo
