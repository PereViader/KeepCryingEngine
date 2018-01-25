#include "ModuleInput.h"

#include <SDL.h>

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module(), mouse({ 0, 0 }), mouse_motion({ 0,0 })
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

bool ModuleInput::Init()
{
	LOG_DEBUG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG_DEBUG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleInput::Start()
{
	return true;
}

update_status ModuleInput::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	static SDL_Event event;

	mouse_motion = { 0, 0 };
	memset(windowEvents, false, WE_COUNT * sizeof(bool));

	const Uint8* keys = SDL_GetKeyboardState(nullptr);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
				break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_RESIZED:
						SDL_GetWindowSize(App->window->window, &App->configuration.screenWidth, &App->configuration.screenHeight);
						App->camera->SetAspectRatio();
						break;

					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
						windowEvents[WE_HIDE] = true;
						break;

						//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						windowEvents[WE_SHOW] = true;
						break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = (float)event.motion.xrel;
				mouse_motion.y = (float)event.motion.yrel;
				mouse.x = (float)event.motion.x;
				mouse.y = (float)event.motion.y;
				break;
		}
	}

	if(GetWindowEvent(EventWindow::WE_QUIT) || GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return update_status::UPDATE_STOP;

	return update_status::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG_DEBUG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

float ModuleInput::GetAxis(Axis axis) const
{
	float ret = 0.0f;
	if(axis == Axis::Horizontal)
	{
		ret -= GetKey(SDL_SCANCODE_A) == KEY_REPEAT ? 1 : 0;
		ret += GetKey(SDL_SCANCODE_D) == KEY_REPEAT ? 1 : 0;
	}
	else
	{
		ret -= GetKey(SDL_SCANCODE_W) == KEY_REPEAT ? 1 : 0;
		ret += GetKey(SDL_SCANCODE_S) == KEY_REPEAT ? 1 : 0;
	}

	return ret;
}

bool ModuleInput::GetWindowEvent(EventWindow ev) const
{
	return windowEvents[ev];
}

const float2& ModuleInput::GetMousePosition() const
{
	return mouse;
}

const float2& ModuleInput::GetMouseMotion() const
{
	return mouse_motion;
}