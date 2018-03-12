#ifndef _INPUTTEXT_H_
#define _INPUTTEXT_H_

#include "Component.h"

class GameObject;

class InputText : public Component
{
public:
	static const Component::Type TYPE = Component::Type::InpuText;

	InputText();
	~InputText();

	void Awake() override;
	void Start() override;
	void Destroy() override;
	void SetEnable(bool value)override;

	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;

	void DrawUI() override;

	void OnFocus();

	std::vector<Component::Type> GetNeededComponents() const override;
	void SetPlaceHolderGameObject(GameObject& g);
	void SetTextGameObject(GameObject& g);
	void SetPasswordMode(bool value);

	bool GetPasswordMode()const;
	GameObject* GetPlaceHolderGameObject()const;
	GameObject* GetTextGameObject() const;

private:
	bool onFocus = false;
	bool passwordMode = false;
	std::string currentTextUnderPassword;
	GameObject * placeHolderGameObject;
	GameObject * textGameObject;
	bool dirtyText = true;
};

#endif // !_INPUTTEXT_H_

