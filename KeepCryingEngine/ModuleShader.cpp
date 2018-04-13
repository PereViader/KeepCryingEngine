#include "ModuleShader.h"

#include <fstream>
#include <assert.h>

using namespace std;

ModuleShader::ModuleShader()
{}

ModuleShader::~ModuleShader()
{}

bool ModuleShader::Init()
{
	/* SetUpColorProgram();
	SetUpDepthShader();
	SetUpDefaultShader();
	SetUpCartoonShader();
	SetUpDiffuseShader(); */

	SetUpUberShader();

	return true;
}

uint ModuleShader::AddShaderPath(const char * path, GLenum shaderType, const char* defines)
{
	uint id = 0;
	ifstream t(path);
	if(t.good())
	{
		string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		id = AddShader(str.c_str(), shaderType, defines);
	}

	t.close();
	return id;
}

uint ModuleShader::AddShader(const char* source, GLenum shaderType, const char* defines)
{
	string definesAndSource = "#version 330 core\n";
	definesAndSource += defines;
	definesAndSource += source;

	const char* charDefinesAndSource = definesAndSource.c_str();

	GLuint shaderId = glCreateShader(shaderType);

	glShaderSource(shaderId, 1, &charDefinesAndSource, nullptr);
	glCompileShader(shaderId);

	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
		LOG_DEBUG("Shader compilation error: %s", infoLog);

		return 0;
	}

	return shaderId;
}

uint ModuleShader::AddProgram(initializer_list<uint> shaders)
{
	GLuint shaderProgramId = glCreateProgram();
	
	for(uint shader : shaders)
	{
		glAttachShader(shaderProgramId, shader);
	}

	glLinkProgram(shaderProgramId);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(shaderProgramId, 512, nullptr, infoLog);
		LOG_DEBUG("Shader link error: %s", infoLog);

		return 0;
	}

	return shaderProgramId;
}

/* GLuint ModuleShader::GetShaderId(ShaderType shaderType) const
{
	GLuint shaderId = 0;
	switch (shaderType) 
	{
	case ShaderType::Default:
	{
		shaderId = defaultShaderId;
	}
		break;

	case ShaderType::Cartoon:
	{
		shaderId = cartoonShaderId;
	}
		break;
	case ShaderType::Color:
	{
		shaderId = colorShaderId;
	}
		break;
	case ShaderType::Depth:
	{
		shaderId = depthShaderId;
	}
		break;
	case ShaderType::Diffuse:
	{
		shaderId = diffuseShaderId;
	}
		break;
	default:
		assert(false);
	}

	return shaderId;
} */

GLuint ModuleShader::GetProgramId(int flags, const std::string& name) const
{
	map<pair<int, string>, GLuint>::const_iterator it = shaders.find(make_pair(flags, name));

	assert(it != shaders.end());

	return it->second;
}

/* void ModuleShader::SetUpColorProgram()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShaderPath("Assets/Shaders/colorFragment.frag", GL_FRAGMENT_SHADER);
	colorShaderId = AddProgram({ vertexId, fragmentId });
} */

uint ModuleShader::AddProgram(const list<uint>& shaders)
{
	GLuint shaderProgramId = glCreateProgram();

	for(uint shader : shaders)
	{
		glAttachShader(shaderProgramId, shader);
	}

	glLinkProgram(shaderProgramId);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(shaderProgramId, 512, nullptr, infoLog);
		LOG_DEBUG("Shader link error: %s", infoLog);

		return 0;
	}

	return shaderProgramId;
}

/* void ModuleShader::SetUpDefaultShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShaderPath("Assets/Shaders/textureFragment.frag", GL_FRAGMENT_SHADER);
	defaultShaderId = AddProgram({ vertexId, fragmentId });
} */

/* void ModuleShader::SetUpCartoonShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShaderPath("Assets/Shaders/cartoon.frag", GL_FRAGMENT_SHADER);
	cartoonShaderId = AddProgram({ vertexId, fragmentId });
} */

/* void ModuleShader::SetUpDepthShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/cameraShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShaderPath("Assets/Shaders/depthShader.frag", GL_FRAGMENT_SHADER);
	depthShaderId = AddProgram({ vertexId, fragmentId });
} */

/* void ModuleShader::SetUpDiffuseShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/lightningVertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShaderPath("Assets/Shaders/lightningFragmentShader.frag", GL_FRAGMENT_SHADER);
	diffuseShaderId = AddProgram({ vertexId, fragmentId });
} */

void ModuleShader::SetUpDefaultShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertex.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShaderPath("Assets/Shaders/fragment.frag", GL_FRAGMENT_SHADER);

	int program = AddProgram({ vertexId, fragmentId });

	shaders[make_pair(DEFAULT, "UberShader")] = program;
}

void ModuleShader::SetUpLightningShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertex.vert", GL_VERTEX_SHADER, "#define LIGHTNING\n");
	uint fragmentId = AddShaderPath("Assets/Shaders/fragment.frag", GL_FRAGMENT_SHADER, "#define LIGHTNING\n");

	int program = AddProgram({ vertexId, fragmentId });

	shaders[make_pair(LIGHTNING, "UberShader")] = program;
}

void ModuleShader::SetUpCartoonShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertex.vert", GL_VERTEX_SHADER, "#define CARTOON\n");
	uint fragmentId = AddShaderPath("Assets/Shaders/fragment.frag", GL_FRAGMENT_SHADER, "#define CARTOON\n");
	
	int program = AddProgram({ vertexId, fragmentId });

	shaders[make_pair(CARTOON, "UberShader")] = program;
}

void ModuleShader::SetUpDepthShader()
{
	uint vertexId = AddShaderPath("Assets/Shaders/vertex.vert", GL_VERTEX_SHADER, "#define DEPTH\n");
	uint fragmentId = AddShaderPath("Assets/Shaders/fragment.frag", GL_FRAGMENT_SHADER, "#define DEPTH\n");
	
	int program = AddProgram({ vertexId, fragmentId });

	shaders[make_pair(DEPTH, "UberShader")] = program;
}

void ModuleShader::SetUpUberShader()
{
	SetUpDefaultShader();
	SetUpLightningShader();
	SetUpCartoonShader();
	SetUpDepthShader();
}