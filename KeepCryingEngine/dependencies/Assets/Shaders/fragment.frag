in vec4 ourColor;
in vec2 TexCoord;
in vec3 Normal;

#ifdef LIGHTNING

in vec3 vertexPosition;
in vec3 vertexNormal;

#endif

out vec4 color;

uniform sampler2D ourTexture;

#ifdef LIGHTNING

uniform mat4 transform;
uniform mat4 rotation;
uniform vec3 lightSourcePosition;
uniform vec3 cameraPosition;

#endif

#ifdef CARTOON

uniform vec3 lightDir;

#endif

void main()
{
	color = texture2D(ourTexture,TexCoord);

#ifdef LIGHTNING

		//Diffuse Lightning ---------
	vec4 worldPosition = transform * vec4(vertexPosition, 1.0f);
	vec3 lightDir = lightSourcePosition - worldPosition.xyz;
	float intensity = dot((rotation * vec4(vertexNormal, 0.0f)).xyz, normalize(lightDir));
	
	float diffuse = max(intensity, 0);
	// --------------------------
	
	//Specular Lightning --------
	float shininess = 48.0f;
	vec3 cameraDir = cameraPosition - worldPosition.xyz;
	vec3 halfVector = normalize(normalize(lightDir) + normalize(cameraDir));
	
	float specularIntensity = dot((rotation * vec4(vertexNormal, 0.0f)).xyz, normalize(halfVector));
	specularIntensity = max(specularIntensity, 0);		
	float specular = pow(specularIntensity, shininess);	
	//---------------------------
	
	//Ambient Lightning ---------
	vec4 ambientLight = vec4(0.0, 0.0, 0.0, 1.0); 
	// --------------------------
	
	color = ((diffuse * texture2D(ourTexture, TexCoord)) + specular) + ambientLight;
#endif

#ifdef CARTOON

	float intensity;
	intensity = dot(Normal,lightDir);

	if (intensity > 0.90)
		color = vec4(1.0,1.0,1.0,1.0);
	else if (intensity > 0.75)
		color = vec4(0.8,0.8,0.8,1.0);
	else if (intensity > 0.55)
		color = vec4(0.6,0.6,0.6,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.4,0.4,1.0);
	else
		color = vec4(0.2,0.2,0.2,1.0);

	color = color * texture2D(ourTexture,TexCoord);
#endif

}