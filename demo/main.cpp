#include <memory>
#include <iostream>
#include <vector>

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


bool handleKeydown(SDL_KeyboardEvent const& event)
{
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			return true;

		case SDLK_s:
			std::cout << "Step" << std::endl;
			break;
	}

	return false;
}


template <typename BodiesContainer>
void logBodies(BodiesContainer const& bodies)
{
	for (auto* const pBody: bodies)
	{
		auto const pos = pBody->GetPosition();
		float32 const angle = pBody->GetAngle();
		std::cout << "Body (" << pos.x << ", " << pos.y << ") @ " << angle
			<< std::endl;
	}
}


void run(int argc, char const* const argv[])
{
	// Initialise SDL with video and events.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		throw std::runtime_error{"SDL_Init failed"};
	}
	atexit(SDL_Quit);


	// Initialise the window.
	auto const pWindow = std::unique_ptr<SDL_Window, demo::WindowDeleter>{
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
		throw std::runtime_error{"SDL_CreateWindow failed"};
	}


	// Set OpenGL version to 3.1, and use Core profile.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE
	);


	// Initialise the OpenGL context.
	auto const pGLContext = std::unique_ptr<void, demo::GLContextDeleter>(
		SDL_GL_CreateContext(pWindow.get())
	);
	if (not pGLContext)
	{
		throw std::runtime_error{"SDL_GL_CreateContext failed"};
	}


	// Initialise GLEW.
	glewExperimental = GL_TRUE;
	{
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			std::cout << "GLEW error: " << glewGetErrorString(glewError)
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


	// Set up scene for rendering.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	b2draw::DebugDraw debugDraw{
		programID,
		pPositionAttribName,
		pColourAttribName
	};
	debugDraw.SetFlags(0xff);

	b2Vec2 const gravity{0.0f, -9.8f};
	b2World world{gravity};
	world.SetDebugDraw(&debugDraw);

	std::vector<b2Body*> bodies;
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(0.0f, -4.0f);
		auto const pGroundBody = world.CreateBody(&bodyDef);
		bodies.push_back(pGroundBody);

		b2PolygonShape box;
		box.SetAsBox(-30.0f, 1.0f);
		pGroundBody->CreateFixture(&box, 0.0f /* Density. */);
	}

	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0.0f, 4.0f);
		auto const pDynamicBody = world.CreateBody(&bodyDef);
		bodies.push_back(pDynamicBody);

		b2PolygonShape box;
		box.SetAsBox(1.0f, 1.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &box;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		pDynamicBody->CreateFixture(&fixtureDef);
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
		debugDraw.Clear();
		world.DrawDebugData();
		debugDraw.BufferData();
		// mvpMat = projMat * viewMat * modelMat;
	};

	auto const render = [
		&debugDraw,
		programID,
		pSDLWindow = pWindow.get(),
		mvpAttribLoc,
		pMvpMatStart
	] {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	update();
	render();

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

						case SDLK_s:
							std::cout << "Step" << std::endl;
							update();
							render();
							logBodies(bodies);
							break;

						default:
							break;
					}
					break;

				default:
					break;
			}
		}
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
