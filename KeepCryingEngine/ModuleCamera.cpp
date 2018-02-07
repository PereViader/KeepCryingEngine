#include "ModuleCamera.h"

#include "Camera.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"

const float ModuleCamera::SHIFT_MULTIPLIER = 10.0f;
const float ModuleCamera::WHEEL_FORCE = 10.0f;
const float ModuleCamera::ORBIT_FORCE_REDUCTION = 10.0f;

ModuleCamera::ModuleCamera()
{ }

ModuleCamera::~ModuleCamera()
{ }

bool ModuleCamera::Init()
{
	camera = new Camera();

	return true;
}

update_status ModuleCamera::Update(float deltaTimeS, float realDeltaTimeS)
{
	if(App->input->GetKey(SDL_SCANCODE_I) == KeyState::KEY_DOWN)
	{
		camera->SetUpFrustum();
	}

	float shiftDeltaMultiplier = deltaTimeS;

	if(App->input->GetKeyPressed(SDL_SCANCODE_LSHIFT) || App->input->GetKeyPressed(SDL_SCANCODE_RSHIFT))
	{
		shiftDeltaMultiplier *= SHIFT_MULTIPLIER;
	}

	Rotation(deltaTimeS);

	Movement(shiftDeltaMultiplier);

	return update_status::UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	RELEASE(camera);

	return true;
}

float ModuleCamera::GetMoveSpeed() const
{
	return movementSpeed;
}

float ModuleCamera::GetRotationSpeed() const
{
	return rotationSpeed;
}

float ModuleCamera::GetDragSpeed() const
{
	return movementDragSpeed;
}

float ModuleCamera::GetOrbitSpeed() const
{
	return movementOrbitSpeed;
}

float ModuleCamera::GetZoomSpeed() const
{
	return movementZoomSpeed;
}

void ModuleCamera::SetMoveSpeed(float speed)
{
	movementSpeed = speed;
}

void ModuleCamera::SetRotationSpeed(float speed)
{
	rotationSpeed = speed;
}

void ModuleCamera::SetDragSpeed(float speed)
{
	movementDragSpeed = speed;
}

void ModuleCamera::SetOrbitSpeed(float speed)
{
	movementOrbitSpeed = speed;
}

void ModuleCamera::SetZoomSpeed(float speed)
{
	movementZoomSpeed = speed;
}

void ModuleCamera::Rotation(float deltaTimeS)
{
	RotateKeyboard(deltaTimeS);
	RotateMouse(deltaTimeS);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) && !(App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT)))
	{
		RotateMouseRotation(deltaTimeS);
	}
	
}

void ModuleCamera::Movement(float shiftDeltaMultiplier)
{
	MovementMouse(shiftDeltaMultiplier);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		MovementKeyBoard(shiftDeltaMultiplier);
	}
}

void ModuleCamera::RotateMouse(float deltaTimeS)
{
	bool leftPressed = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);
	bool altPressed = App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT);

	if(altPressed)
	{
		if(leftPressed)
		{
			RotateMouseOrbit(deltaTimeS);
		}
	}
}

void ModuleCamera::MovementMouse(float shiftDeltaMultiplier)
{
	bool leftPressed = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);
	bool rightPressed = App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT);
	bool altPressed = App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT);
	float wheelMotion = App->input->GetWheelMotion();

	if(wheelMotion == 1)
	{
		MovementWheelZoom(shiftDeltaMultiplier*WHEEL_FORCE);
	}
	if(wheelMotion == -1)
	{
		MovementWheelZoom(-shiftDeltaMultiplier*WHEEL_FORCE);
	}

	if(altPressed)
	{
		if(rightPressed && wheelMotion == 0)
		{
			MovementMouseZoom(shiftDeltaMultiplier);
		}
	}
	else if(leftPressed)
	{
		MovementMouseDrag(shiftDeltaMultiplier);
	}
}

void ModuleCamera::MovementMouseDrag(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector *= movementDragSpeed * shiftDeltaMultiplier;

	translateVector.x *= -1.0f;

	float3 upMovement = translateVector.y * camera->GetUpVector(); // frustum.up;
	float3 rightMovement = translateVector.x * camera->GetSideVector(); // frustum.WorldRight();

	camera->Translate(upMovement + rightMovement);

	// frustum.Translate(upMovement + rightMovement);
}

void ModuleCamera::RotateMouseOrbit(float deltaTimeS)
{
	//It rotate around a point "x" at distance "y" in front direction, "y" value increases with zoom out, and decreases with zoom in
	if(zoomAmount > 1.0f)
	{
		float3 orbitCenter = camera->GetPosition() + (camera->GetFrontVector() * zoomAmount);
		App->renderer->DrawCross(orbitCenter, zoomAmount);
		MovementMouseDrag(deltaTimeS * zoomAmount / ORBIT_FORCE_REDUCTION);
		camera->LookAt(orbitCenter);
	}
	else
	{
		RotateMouseRotation(deltaTimeS);
	}
}

void ModuleCamera::MovementMouseZoom(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector.x *= -1.0f;

	float movementZ = translateVector.y - translateVector.x;

	float movementTemp = movementZoomSpeed * movementZ * shiftDeltaMultiplier;
	zoomAmount -= movementTemp;
	if(zoomAmount >= 0.0f)
	{
		camera->Translate(camera->GetFrontVector() * movementTemp);
		// frustum.Translate(frustum.front * movementTemp);
	}
	else
	{
		zoomAmount = 0.0f;
	}
}

void ModuleCamera::MovementWheelZoom(float shiftDeltaMultiplier)
{
	float movementTemp = movementZoomSpeed * shiftDeltaMultiplier;
	zoomAmount -= movementTemp;

	if(zoomAmount >= 0.0f)
	{
		camera->Translate(camera->GetFrontVector() * movementTemp);
		// frustum.Translate(frustum.front * movementTemp);
	}
	else
	{
		zoomAmount = 0.0f;
	}
}

void ModuleCamera::RotateMouseRotation(float deltaTimeS)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector *= movementDragSpeed * deltaTimeS;

	translateVector.x *= -1.0f;

	Quat rotation = Quat::identity;
	float movementDeltaOrbitSpeed = movementOrbitSpeed * deltaTimeS;

	if(translateVector.y > 0.0f)
	{
		if(camera->GetFrontVector().y < 1.0f - movementDeltaOrbitSpeed)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(translateVector.y * camera->GetSideVector(), movementDeltaOrbitSpeed));
		}
	}
	else if(translateVector.y < 0.0f)
	{
		if(camera->GetFrontVector().y > movementDeltaOrbitSpeed - 1.0f)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(translateVector.y * camera->GetSideVector(), movementDeltaOrbitSpeed));
		}
	}

	rotation = rotation.Mul(Quat::RotateAxisAngle(translateVector.x * float3::unitY, movementDeltaOrbitSpeed));

	camera->SetUpVector(rotation.Mul(camera->GetUpVector()).Normalized());
	camera->SetFrontVector(rotation.Mul(camera->GetFrontVector()).Normalized());

	// frustum.up = rotation.Mul(frustum.up);
	// frustum.front = rotation.Mul(frustum.front);
}

void ModuleCamera::MovementKeyBoard(float shiftDeltaMultiplier)
{
	float3 translateVector = float3::zero;

	if(App->input->GetKeyPressed(SDL_SCANCODE_Q))
	{
		translateVector += camera->GetUpVector();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_E))
	{
		translateVector -= camera->GetUpVector();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_W))
	{
		translateVector += camera->GetFrontVector();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_S))
	{
		translateVector -= camera->GetFrontVector();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_D))
	{
		translateVector += camera->GetSideVector();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_A))
	{
		translateVector -= camera->GetSideVector();
	}

	camera->Translate(translateVector * movementSpeed * shiftDeltaMultiplier);
	// frustum.Translate(translateVector * movementSpeed * shiftDeltaMultiplier);
}

void ModuleCamera::RotateKeyboard(float deltaTimeS)
{
	Quat rotation = Quat::identity;
	float rotationDeltaSpeed = rotationSpeed * deltaTimeS;

	if(App->input->GetKeyPressed(SDL_SCANCODE_UP))
	{
		if(camera->GetFrontVector().y < 1.0f - rotationDeltaSpeed)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(camera->GetSideVector(), rotationDeltaSpeed));
		}
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_DOWN))
	{
		if(camera->GetFrontVector().y > rotationDeltaSpeed - 1.0f)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(camera->GetSideVector(), -rotationDeltaSpeed));
		}
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_LEFT))
	{
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, rotationDeltaSpeed));
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_RIGHT))
	{
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, -rotationDeltaSpeed));
	}

	camera->SetUpVector(rotation.Mul(camera->GetUpVector()).Normalized());
	camera->SetFrontVector(rotation.Mul(camera->GetFrontVector()).Normalized());

	// frustum.up = rotation.Mul(frustum.up);
	// frustum.front = rotation.Mul(frustum.front);
}