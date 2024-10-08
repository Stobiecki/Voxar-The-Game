#version 330 core

// Atrybuty wierzcholkow z VAO
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;

// Macierz rzutowania i transformacji
uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform vec3 cameraPos;

// Struktura oswietlenia punktowego
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

uniform int lightMode;
uniform bool shaderMode;
uniform int lightCount;
uniform PointLightParam myLight[100];
uniform GlobalLightParam globalLightParam;
uniform PointLightParam relativeLightParam;
uniform MaterialParam myMaterial;


// Dane wyjsciowe do shadera fragmentow
out VS_OUT
{
	out vec3 fragPos;
	out vec2 fragUV;
	out vec3 fragNormal;
	out vec3 outlightCoef;
	out vec4 matMP;
	out vec4 matVMP;
	out vec4 outPosition;
	out vec3 outNormal;
};

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
	// Przekazanie danych do shadera fragmentow
	fragPos = inPosition.xyz;
	fragUV = inUV;
	fragNormal = inNormal;

	vec3 lightCoef = vec3(0.0f);

	// Ostateczna pozycja wierzcholka
	vec4 newPos = matModel * inPosition;
	vec4 forRelativeLight = matView * newPos;

	if(shaderMode == false) // per_vertex
	{
		if(globalLightParam.IsTurnedOn != 0)
			lightCoef = calculateDirectionalLight(newPos, globalLightParam, myMaterial);

		for(int i=0; i<lightCount; i++)
		{
			if(myLight[i].IsTurnedOn != 0)
			{
				if(lightMode == 0)
					lightCoef = max(lightCoef, calculatePointLight(newPos, myLight[i], myMaterial));
				else if(lightMode == 1)
					lightCoef = max(lightCoef, calculateBlinnPhongLight(newPos, myLight[i], myMaterial));
			}
		}
		if(relativeLightParam.IsTurnedOn != 0)
			lightCoef = max(lightCoef, calculateRelativeLight(forRelativeLight, relativeLightParam, myMaterial));
	}
	else // (shaderMode == true) // per_fragment
	{
		matMP = newPos;
		matVMP = forRelativeLight;
		outPosition = inPosition;
		outNormal = inNormal;
	}
	outlightCoef = lightCoef;
	gl_Position = matProj * forRelativeLight;
}
