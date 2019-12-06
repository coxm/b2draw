#include <memory>
#include <iostream>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

#include "b2draw/DebugDraw.h"

#include "./util/deleters.h"
#include "./util/gl.h"


constexpr int screenWidth{640};
constexpr int screenHeight{480};


constexpr float worldTimeStep{1.0f / 60.0f};
constexpr unsigned velocityIterations{8};
constexpr unsigned positionIterations{3};


constexpr char const* const pPositionAttribName = "position";
constexpr char const* const pColourAttribName = "colour";


using sdl_window_ptr = std::unique_ptr<SDL_Window, demo::WindowDeleter>;

using gl_context_ptr = std::unique_ptr<void, demo::GLContextDeleter>;


void logBodies(b2World const* pWorld)
{
	for (
		b2Body const* pBody = pWorld->GetBodyList();
		pBody != nullptr;
		pBody = pBody->GetNext()
	)
	{
		auto const pos = pBody->GetPosition();
		float32 const angle = pBody->GetAngle();
		std::cout << "Body (" << pos.x << ", " << pos.y << ") @ " << angle
			<< std::endl;
	}
}


/**
 * Initialise SDL and the window.
 *
 * @returns a unique_ptr to the window.
 */
sdl_window_ptr initSDL()
{
	// Initialise SDL with video and events.
	atexit(SDL_Quit); // SDL_Quit is safe to call even if SDL_Init failed.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL_Init failed!" << std::endl;
		throw std::runtime_error{SDL_GetError()};
	}

	// Initialise the window.
	sdl_window_ptr pWindow{
		SDL_CreateWindow(
			"Debug draw demo",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			screenWidth,
			screenHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
		)
	};
	if (not pWindow)
	{
		std::cerr << "SDL_CreateWindow failed!" << std::endl;
		throw std::runtime_error{SDL_GetError()};
	}

	return pWindow;
}


/**
 * Initialise OpenGL and GLEW.
 *
 * @returns a unique_ptr to the GL context.
 */
gl_context_ptr initGL(SDL_Window* pWindow)
{
	// Set OpenGL version to 3.1, and use Core profile.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE
	);

	// Initialise the OpenGL context.
	gl_context_ptr pGLContext{SDL_GL_CreateContext(pWindow)};
	if (!pGLContext)
	{
		std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError()
			<< std::endl;
		throw std::runtime_error{"SDL_GL_CreateContext failed"};
	}

	// Initialise GLEW.
	glewExperimental = GL_TRUE;
	{
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			std::cerr << "GLEW error: " << glewGetErrorString(glewError)
				<< std::endl;
			throw std::runtime_error{"glewInit failed"};
		}
	}

	// Try to set VSync; failure is OK here.
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "[Warning] Failed to set VSync: " << SDL_GetError()
			<< std::endl;
	}

	return pGLContext;
}


/**
 * Create a GL program and add compiled shaders.
 *
 * @returns the ID of the created program.
 */
GLuint const createProgram()
{
	// Create OpenGL shaders and program.
	GLuint const vertShaderID{demo::compileShader(GL_VERTEX_SHADER, R"GLS(\
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 colour;

out vec4 fsColour;
uniform mat4 MVP;


void main() {
	gl_Position =  MVP * vec4(position.x, position.y, 0, 1);
	fsColour = colour;
}

	)GLS")};

	GLuint const fragShaderID{demo::compileShader(GL_FRAGMENT_SHADER, R"GLS(\
#version 330 core

in vec4 fsColour;
out vec4 fragColour;

void main() {
	fragColour = fsColour;
}
	)GLS")};

	GLuint const programID{glCreateProgram()};
	glAttachShader(programID, vertShaderID);
	glAttachShader(programID, fragShaderID);
	glLinkProgram(programID);
	{
		GLint success{GL_FALSE};
		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (success != GL_TRUE)
		{
			std::cout << "Failed to link program: "
				<< demo::getProgramLog(programID) << std::endl;
			throw std::runtime_error{"Program link failed"};
		}
	}

	return programID;
}


void run(int argc, char const* const argv[])
{
	auto pWindow{initSDL()};
	auto pGLContext{initGL(pWindow.get())};
	auto programID{createProgram()};

	// Set up scene for rendering.
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);

	b2draw::DebugDraw debugDraw{
		glGetAttribLocation(programID, pPositionAttribName),
		glGetAttribLocation(programID, pColourAttribName),
		16,
		0.01f,
		4.f
	};
	debugDraw.SetFlags(0xff);

	b2Vec2 const gravity{0.0f, -9.8f};
	b2World world{gravity};
	world.SetDebugDraw(&debugDraw);

	b2Filter collideEverything;
	collideEverything.categoryBits = 0x0001;
	collideEverything.maskBits = 0xffff;
	collideEverything.groupIndex = 1;

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.0f;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.filter = collideEverything;

	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(0.0f, -4.0f);
		auto const pGroundBody = world.CreateBody(&bodyDef);

		b2PolygonShape box;
		box.SetAsBox(-30.0f, 1.0f);

		fixtureDef.shape = &box;
		pGroundBody->CreateFixture(&fixtureDef);
	}

	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0.0f, 4.0f);
		auto const pDynamicBody = world.CreateBody(&bodyDef);

		b2PolygonShape box;
		box.SetAsBox(1.0f, 1.0f);

		fixtureDef.shape = &box;
		pDynamicBody->CreateFixture(&fixtureDef);
	}

	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(-7.0f, 8.0f);
		bodyDef.linearVelocity.Set(1.0f, 0.0f);
		bodyDef.angularVelocity = 1.5f;
		auto const pBody = world.CreateBody(&bodyDef);

		b2CircleShape circle;
		circle.m_p = {0.0f, 0.0f};
		circle.m_radius = 2.0f;

		fixtureDef.shape = &circle;
		pBody->CreateFixture(&fixtureDef);
	}


	constexpr float fieldOfView{45.0f};
	auto const projMat = glm::perspective(
		fieldOfView,
		4.0f / 3.0f,
		0.1f,
		100.0f
	);

	constexpr glm::vec3 eye{0.0f, 0.0f, 40.0f};
	constexpr glm::vec3 focus{0.0f, 0.0f, 0.0f};
	constexpr glm::vec3 up{0.0f, 1.0f, 0.0f};
	auto const viewMat = glm::lookAt(eye, focus, up);

	glm::mat4 const modelMat{1.0f};

	auto const mvpMat{projMat * viewMat * modelMat};
	auto const pMvpMatStart{&mvpMat[0][0]};
	auto const mvpAttribLoc{glGetUniformLocation(programID, "MVP")};
	if (mvpAttribLoc < 0)
	{
		throw std::runtime_error{"Unable to locate uniform 'MVP'"};
	}


	auto const update = [&debugDraw, &world] {
		world.Step(worldTimeStep, velocityIterations, positionIterations);
		world.ClearForces();
		debugDraw.Clear();
		world.DrawDebugData();
		debugDraw.BufferData();
		// logBodies(&world);
	};

	auto const render = [
		&debugDraw,
		programID,
		pSDLWindow = pWindow.get(),
		mvpAttribLoc,
		pMvpMatStart
	] {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
		glUniformMatrix4fv(mvpAttribLoc, 1, GL_FALSE, pMvpMatStart);
		debugDraw.Render();
		SDL_GL_SwapWindow(pSDLWindow);
	};

	// Ensure no GL errors.
	{
		bool foundErrors{false};
		decltype(glGetError()) error;
		while ((error = glGetError()))
		{
			std::cout << "GL error: " << error << std::endl;
			foundErrors = true;
		}
		if (foundErrors)
		{
			throw std::runtime_error{"GL errors detected"};
		}
	}

	SDL_Event event;
	bool userQuit{false};

	while (not userQuit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
				case SDL_QUIT:
					userQuit = true;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							userQuit = true;
							break;

						default:
							break;
					}
					break;

				default:
					break;
			}
		}

		update();
		render();
	}
}


int main(int argc, char const* const argv[])
{
	try
	{
		run(argc, argv);
	}
	catch (std::exception const& err)
	{
		std::cout << "[Fatal] " << err.what() << std::endl;
		return 1;
	}
	return 0;
}
