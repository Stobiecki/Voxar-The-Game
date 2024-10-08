#version 150 core

// Kolor ostateczny
out vec4 outColor;

struct PointLightParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position;
	int IsTurnedOn;
};

// Struktura oswietlenia punktowego
struct GlobalLightParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Direction; // Direction dla kierunkowego
	int IsTurnedOn;
};

// Struktura materialu obiektu
struct MaterialParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

// Dane wejsciowe z shadera wierzcholkow
in VS_OUT
{
	in vec3 fragPos;
	in vec2 fragUV;
	in vec3 fragNormal;
	in vec3 outlightCoef;
	in vec4 matMP;
	in vec4 matVMP;
	in vec3 outPosition;
	in vec3 outNormal;
};

vec3 lightCoef = outlightCoef;

// Pozycja kamery przekazana z aplikacji
uniform vec3 cameraPos;
// Tekstura z aplikacji
uniform sampler2D tex;


uniform vec3 staticColor;


uniform int lightMode;
uniform bool shaderMode;
uniform int lightCount;
uniform PointLightParam myLight[100];
uniform GlobalLightParam globalLightParam;
uniform PointLightParam relativeLightParam;
uniform MaterialParam myMaterial;

vec3 inPosition = outPosition;
vec3 inNormal = outNormal;

vec3 calculateDirectionalLight(vec4 pos, GlobalLightParam light, MaterialParam material)
{
	vec3 lightCoef;
	vec3 AmbientPart;
	vec3 DiffusePart;
	vec3 SpecularPart;
	vec3 L = normalize(-light.Direction);
	vec3 E = normalize(cameraPos - pos.xyz);
	vec3 R = reflect(-E, inNormal);	
	float diff = dot(L, inNormal);
	float spec = pow(max(dot(R,L), 0), material.Shininess);
	float LV = 0.f;
	float Latt = 1.0f/(light.Attenuation.x + (light.Attenuation.y * LV) + (light.Attenuation.z * LV * LV));

	AmbientPart = light.Ambient * material.Ambient;
	DiffusePart = diff * light.Diffuse * material.Diffuse;
	SpecularPart = spec * light.Specular * material.Specular;

	lightCoef = AmbientPart + Latt * (DiffusePart + SpecularPart) * .05f;

	return lightCoef;
}

vec3 calculatePointLight(vec4 pos, PointLightParam light, MaterialParam material)
{
	vec3 lightCoef;

	vec3 AmbientPart;
	vec3 DiffusePart;
	vec3 SpecularPart;
	vec3 L = normalize(light.Position - pos.xyz);
	vec3 E = normalize(cameraPos - pos.xyz);
	vec3 R = reflect(-E, inNormal);	
	float diff = dot(L, inNormal);
	float spec = pow(max(dot(R,L), 0), material.Shininess);
	float LV = distance(pos.xyz, light.Position);
	float Latt = 1.0f/(light.Attenuation.x + (light.Attenuation.y * LV) + (light.Attenuation.z * LV * LV));

	AmbientPart = light.Ambient * material.Ambient;
	DiffusePart = diff * light.Diffuse * material.Diffuse;
	SpecularPart = spec * light.Specular * material.Specular;

	lightCoef = AmbientPart + Latt * (DiffusePart + SpecularPart) * 1.f;

	return lightCoef;
}

vec3 calculateRelativeLight(vec4 pos, PointLightParam light, MaterialParam material)
{
	vec3 lightCoef;

	vec3 AmbientPart;
	vec3 DiffusePart;
	vec3 SpecularPart;
	vec3 L = normalize(light.Position - pos.xyz);
	vec3 E = normalize(cameraPos - pos.xyz);
	vec3 R = reflect(-E, inNormal);	
	float diff = dot(L, inNormal);
	float spec = pow(max(dot(R,L), 0), material.Shininess);
	float LV = distance(pos.xyz, light.Position);
	float Latt = 1.0f/(light.Attenuation.x + (light.Attenuation.y * LV) + (light.Attenuation.z * LV * LV));

	AmbientPart = light.Ambient * material.Ambient;
	DiffusePart = diff * light.Diffuse * material.Diffuse;
	SpecularPart = spec * light.Specular * material.Specular;

	lightCoef = AmbientPart + Latt * (DiffusePart + SpecularPart) * 1.5f;

	return lightCoef;
}

vec3 calculateBlinnPhongLight(vec4 pos, PointLightParam light, MaterialParam material)
{
	vec3 lightCoef;

	vec3 AmbientPart;
	vec3 DiffusePart;
	vec3 SpecularPart;

    vec3 E = normalize(cameraPos - pos.xyz); // viewDir
    vec3 normal = normalize(inNormal);
    vec3 L = normalize(light.Position - pos.xyz); // LightDir
    vec3 H = normalize(L + E);

	float diff = max(dot(L, normal), 0);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    float LV = distance(pos.xyz, light.Position);
    float latt = 1.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);

    AmbientPart = light.Ambient * material.Ambient;
	DiffusePart = diff * light.Diffuse * material.Diffuse;
	SpecularPart = spec * light.Specular * material.Specular;

    lightCoef = AmbientPart + latt * (DiffusePart + SpecularPart);

    return lightCoef;
}

void main()
{
	//vec4 texColor = texture(tex, fragUV);

	// vec3 fogColor = vec3 (0.2 , 0.2 , 0.2);
	// float lightDist = distance ( myLight[1].Position, fragPos );
	// float fogCoeff = clamp ( exp(- lightDist /6.0) , 0.0 , 1.0);

	// vec3 finalColor1 = lightCoef * texColor.xyz;
	// vec3 finalColor2 = mix (fogColor, finalColor1, fogCoeff);
	
	outColor = vec4(staticColor, 1.0);
}
