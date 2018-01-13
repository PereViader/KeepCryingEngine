#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleTime.h"

using namespace std;

Application::Application()
{
	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(time = new ModuleTime());
}

Application::~Application()
{
	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend(); it++)
		RELEASE(*it);
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; it++)
		ret = (*it)->Init(); // we init everything, even if not anabled

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; it++)
	{
		if((*it)->IsEnabled() == true)
			ret = (*it)->Start();
	}

	return ret;
}

update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled() == true) 
			ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled() == true) 
			ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled() == true) 
			ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		if((*it)->IsEnabled() == true) 
			ret = (*it)->CleanUp();

	return ret;
}
