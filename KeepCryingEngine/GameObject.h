#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <list>
#include <vector>

#include <AABB.h>

#include "Component.h"

class Transform;

class GameObject
{
public:
	GameObject(const std::string& name);
	virtual ~GameObject();

	GameObject* GetParent() const;

	size_t GetChildCount() const;

	GameObject* GetChild(size_t index) const;
	const std::vector<GameObject*>& GetChildren() const;

	GameObject* GetChild(unsigned long long int gameObjectId) const;
	GameObject* GetSelfOrChild(unsigned long long int gameObjectId) const;
	void DeleteChildFromList(GameObject& childToRemove);

	const std::string& GetName() const;

	unsigned long long int GetId() const;

	const bool IsEnabled() const;
	const bool IsStatic() const;

	void SetStatic(bool value);


	void SetParent(GameObject& newParent);

	Component* AddComponent(ComponentType type, bool forceAddition = false);
	void RemoveComponent(Component* component);
	Component* GetComponent(ComponentType type) const;
	const std::vector<Component*>& GetComponents() const;
	std::vector<Component*> GetComponents(ComponentType type);
	std::vector<Component*> GetComponentsInChildren(ComponentType type);
	void GetComponents(ComponentType type, std::vector<Component*>& components);
	Transform* GetTransform() const;

	void DrawUI();

	// void Awake() {}
	// void OnEnable() {}
	// void Start() {}
	// void PreUpdate() {}
	void Update(float deltaTimeS, float realDeltaTimeS);
	// void LateUpdate() {}
	// void OnDisable() {}
	void OnDestroy();

	void SetAABB(const AABB& newAABB);
	AABB &GetAABB();

	bool GetVisible() const;
	void SetVisible(bool visible);

private:
	void CheckToStart();
	void CheckToDestroy();

	void AddInternalComponent(Component* component);

	bool CanAttach(const Component& component) const;

private:
	GameObject* parent = nullptr;

	std::list<Component*> toStart;
	std::list<Component*> toDestroy;

	std::vector<GameObject*> children;
	std::vector<Component*> components;
	Transform* transform = nullptr;
	std::string name;

	bool enable = true;
	bool isStatic = false;
	unsigned long long int id;

	AABB aabb;

	bool visible = false;
};

#endif