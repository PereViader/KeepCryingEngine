#include "Material.h"

#include <string>

#include <imgui.h>

#include "Application.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"
#include "Texture.h"

using namespace std;

Material::Material()
{
	texture = App->texture->GetCheckerTexture();
	programId = App->shader->GetShaderId(ShaderType::Default);
}

Material::Material(const Material & mat) :
	shaderType(mat.shaderType),
	programId(mat.programId),
	texture(mat.texture),
	path(mat.path)
{
	App->texture->Subscribe(texture);
}

Material::~Material()
{
	App->texture->Release(texture);
}

void Material::DrawUI()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		static char materialBuffer[252] = {};
		ImGui::InputText("##setTexture", materialBuffer, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			string s = "Assets/";
			s += materialBuffer;
		
			SetTextureByPath(s.c_str());
		}
		
		ImGui::NewLine();
		
		int tmpShaderMode = (int)shaderType;
		if(ImGui::Combo("Shader", &tmpShaderMode, "Default\0Cartoon\0Color\0Depth"))
		{
			shaderType = (ShaderType)tmpShaderMode;
			programId = App->shader->GetShaderId(shaderType);
		}
	}
}

GLuint Material::GetProgramId() const
{
	return programId;
}

Texture* Material::GetTexture() const
{
	return texture;
}

void Material::SetTexture(Texture * texture)
{
	assert(texture != nullptr);
	App->texture->Release(this->texture);
	this->texture = texture;
	path = "undefinedPath";
}

void Material::SetTextureByPath(const std::experimental::filesystem::path& path)
{
	this->path = path;
	Texture * texture = App->texture->GetAsset(path);
	if(texture != nullptr)
	{
		App->texture->Release(this->texture);
		this->texture = texture;
	}
}

void Material::SetPath(const std::experimental::filesystem::path& path)
{
	this->path = path;
}

const std::experimental::filesystem::path & Material::GetPath() const
{
	return path;
}

ShaderType Material::GetShaderType() const
{
	return shaderType;
}

void Material::SetShaderType(ShaderType shaderType)
{
	this->shaderType = shaderType;
}