#include "GameObject.h"

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ComponentFabric.h"

using namespace std;

GameObject::GameObject(const string& name) : name(name)
{
	id = App->scene->GetNewGameObjectId();
	AddComponent(ComponentType::Transform);
}

GameObject::~GameObject()
{ }

GameObject* GameObject::GetParent() const
{
	return parent;
}

size_t GameObject::GetChildCount() const
{
	return children.size();
}

GameObject* GameObject::GetChild(size_t index) const
{
	assert(children.size() > index);

	return children[index];
}

const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}

GameObject* GameObject::GetChild(unsigned long long int gameObjectId) const
{
	// Check 1st level children first

	for(GameObject* child : children)
	{
		if(child->GetId() == gameObjectId)
		{
			return child;
		}
	}

	// Recursion

	for(GameObject* child : children)
	{
		GameObject* childRecursive = child->GetChild(gameObjectId);
		
		if(childRecursive != nullptr)
		{
			return childRecursive;
		}
	}

	return nullptr;
}

GameObject* GameObject::GetSelfOrChild(unsigned long long int gameObjectId) const
{
	if(id == gameObjectId)
	{
		return (GameObject*)this;
	}

	return GetChild(gameObjectId);
}

void GameObject::DeleteChild(GameObject & childToRemove)
{
	if(this->children.size() > 0)
	{
		for(uint i = 0; i < this->children.size(); i++)
		{
			if(this->children[i] == &childToRemove)
			{
				this->children.erase(this->children.begin() + (i));
			}
		}
	}
}

const string& GameObject::GetName() const
{
	return name;
}

unsigned long long int GameObject::GetId() const
{
	return id;
}

const bool GameObject::IsEnabled() const
{
	return enable;
}

/* void GameObject::AddChild(GameObject& newChild)
{
	newChild.SetParent(*this);
} */

void GameObject::SetParent(GameObject& newParent)
{
	if(parent != nullptr)
	{
		vector<GameObject*>::iterator childParentIterator = find(parent->children.begin(), parent->children.end(), this);
		assert(childParentIterator != parent->children.end());
		parent->children.erase(childParentIterator);
	}
	
	parent = &newParent;
	newParent.children.push_back(this);
}

void GameObject::Update(float deltaTimeS, float realDeltaTimeS)
{
	CheckToStart();
	CheckToDestroy();

	if(enable)
	{
		for(Component* component : components)
		{
			component->Update(deltaTimeS, realDeltaTimeS);
		}
	}
}

void GameObject::OnDestroy()
{
	for(Component* component : components)
	{
		component->Destroy();
	}

	for(Component* component : components)
	{
		RELEASE(component);
	}

	components.clear();
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = ComponentFabric::CreateComponent(type);
	assert(component);

	AddInternalComponent(component);

	if(type == ComponentType::Mesh)
	{
		Component* mat = ComponentFabric::CreateComponent(ComponentType::Material);
		assert(mat);
		AddInternalComponent(mat);
	}

	return component;
}

void GameObject::RemoveComponent(Component * component)
{
	if(component->type == ComponentType::Mesh)
	{
		Component* mat = GetComponent(ComponentType::Material);
		assert(mat);
		vector<Component*>::iterator it = find(components.begin(), components.end(), mat);
		if(it != components.end())
		{
			toDestroy.push_back(mat);
		}
	}

	vector<Component*>::iterator it = find(components.begin(), components.end(), component);
	if (it != components.end()) 
	{
		toDestroy.push_back(component);
	}
}

Component* GameObject::GetComponent(ComponentType type)
{
	for(Component* component : components)
	{
		if(component->type == type)
		{
			return component;
		}
	}

	return nullptr;
}

const std::vector<Component*>& GameObject::GetComponents() const
{
	return components;
}

std::vector<Component*> GameObject::GetComponents(ComponentType type)
{
	std::vector<Component*> ret;

	for (Component* component : components)
	{
		if (component->type == type)
		{
			ret.push_back(component);
		}
	}

	return ret;
}

void GameObject::GetComponents(ComponentType type, std::vector<Component*>& ret)
{
	for (Component* component : components)
	{
		if (component->type == type)
		{
			ret.push_back(component);
		}
	}
}

void GameObject::DrawUI()
{
	ImGui::Checkbox("", &enable); ImGui::SameLine();

	static char buffer[180] = {};
	strcpy(buffer, name.c_str());
	if(ImGui::InputText("##label", buffer, sizeof(buffer)))
	{
		name = buffer;
	}
	static int selectedComponent = 0;
	ImGui::Combo("Comp.", &selectedComponent, "Mesh");
	ImGui::SameLine();
	if(ImGui::Button("Add"))
	{
		switch(selectedComponent)
		{
			case 0:
				AddComponent(ComponentType::Mesh);
				break;
		}
	}

	ImGui::NewLine();

	for(Component* c : components)
	{
		c->DrawUI();
	}

	ImGui::NewLine();
}

void GameObject::CheckToStart()
{
	for(Component* component : toStart)
	{
		component->Start();

		components.push_back(component);
	}

	toStart.clear();
}

void GameObject::CheckToDestroy()
{
	for(Component* component : toDestroy)
	{
		vector<Component*>::iterator it = find(components.begin(), components.end(), component);
		assert(it != components.end());
		components.erase(it);

		component->Destroy();
	}

	for(Component* component : toDestroy)
	{
		RELEASE(component);
	}

	/* for(Component* component : toDestroy)
	{
		components.erase(find(components.begin(), components.end(), component));

		DestroyAndRelease(component);
	} */

	toDestroy.clear();
}

void GameObject::AddInternalComponent(Component * component)
{
	component->gameObject = this;
	component->Awake();

	toStart.push_back(component);
}

/* void GameObject::DestroyAndRelease(Component* component) const
{
	// component->Destroy();

	RELEASE(component);
} */