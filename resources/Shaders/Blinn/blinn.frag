#version 330 core

layout(location = 0) out vec4 color;

in vec3 diffuseColor;
in vec3 specularColor;
in vec3 viewSpaceNormal;
in vec3 lightDir;
in vec3 ambientColor;
in vec3 viewDir;
in float lightIntensity;
in float ambientIntensity;
in float shininess;

void main()
{	
	vec3 normal = normalize(viewSpaceNormal);
	vec3 curColor = lightIntensity * diffuseColor * max(0,dot(normal,lightDir));
	vec3 halfVec = normalize(lightDir+viewDir);
	curColor += lightIntensity * specularColor * pow(max(0,dot(normal,halfVec)),shininess);	
	curColor += ambientIntensity * ambientColor;
	color = vec4(curColor,1.0);
}
