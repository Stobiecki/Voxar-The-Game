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

	float farPlane;
	vec3  cameraPos;
	vec3  lightPos;
	// samplerCube texShadowCubeMap;
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
	vec3 fragPos;
	vec2 fragUV;
	vec3 fragNormal;
	vec4 fragPosLight;
	vec3 outlightCoef;
	vec4 matMP;
	vec4 matVMP;
	vec3 outPosition;
	vec3 outNormal;
};

vec3 lightCoef = outlightCoef;

// Pozycja kamery przekazana z aplikacji
uniform vec3 cameraPos;
// Tekstura z aplikacji
uniform sampler2D tex;

uniform int lightMode;
uniform bool shaderMode;
uniform int lightCount;
uniform PointLightParam myLight[16];
uniform GlobalLightParam globalLightParam;
uniform PointLightParam relativeLightParam;
uniform MaterialParam myMaterial;

// point shadow samplers
//uniform samplerCube texShadowCubeMap[10];
uniform int samplerCubeCount = 6;

uniform int lightShadowId1 = 0;
uniform int lightShadowId2 = 0;
uniform int lightShadowId3 = 0;
uniform int lightShadowId4 = 0;
uniform int lightShadowId5 = 0;
uniform int lightShadowId6 = 0;

uniform samplerCube texShadowCubeMap1;
uniform samplerCube texShadowCubeMap2;
uniform samplerCube texShadowCubeMap3;
uniform samplerCube texShadowCubeMap4;
uniform samplerCube texShadowCubeMap5;
uniform samplerCube texShadowCubeMap6;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

uniform float BIAS = 0.004f;

// mgła
uniform vec3 fogColor = vec3 (0.2 , 0.2 , 0.2);
uniform float fogDensity = 0.02f;

// cienie
uniform sampler2D tex_shadowMap;
uniform vec3 lightDirection;

vec3 inPosition = outPosition;
vec3 inNormal = outNormal;

vec3 calculateDirectionalLight(vec4 pos, GlobalLightParam light, MaterialParam material)
{
	vec3 lightCoef;
	vec3 AmbientPart = vec3(0.0f);
	vec3 DiffusePart;
	vec3 SpecularPart;
	vec3 L = normalize(-light.Direction);
	vec3 E = normalize(cameraPos - pos.xyz);
	vec3 R = reflect(-E, inNormal);	
	float diff = dot(L, inNormal);
	float spec = pow(max(dot(R,L), 0), material.Shininess);
	float LV = 0.f;
	float Latt = 1.0f/(light.Attenuation.x + (light.Attenuation.y * LV) + (light.Attenuation.z * LV * LV));

	//AmbientPart = light.Ambient * material.Ambient;
	DiffusePart = diff * light.Diffuse;
	SpecularPart = spec * light.Specular * material.Specular;

	//lightCoef = AmbientPart + Latt * (DiffusePart + SpecularPart);
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

////////////////////////////////////////////////////////////////////
// shadows
////////////////////////////////////////////////////////////////////

float calcDirectionalShadow(vec4 fragPosLight, vec3 fragNormal, vec3 lightDirection)
{

	// Brak cienia
	// return 0;

	// Korekcja perspektywiczna (dla oswietlenia kierunkowego niepotrzebna)
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // przeksztalcenie wartosci [-1,+1] na [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // pobranie z tekstury shadowMap odleglosci od zrodla swiatla fragmentu
    // do fragmentu oswietlonego na drodze do aktualnego fragmentu
    float closestDepth = texture(tex_shadowMap, projCoords.xy).r;

    // obliczenie aktualnej odleglosci od zrodla swiatla
    float currentDepth = projCoords.z;

    // Sprawdzenie czy fragment jest w cieniu po odleglosci

	// odkomentowanie powoduje "shadow achne"
	if(globalLightParam.IsTurnedOn == 0)
		return (currentDepth > closestDepth) ? 1.0 : 0.0;

	// Shadow acne
	// Parametry i stale ustawiamy eksperymentalnie
	 float bias = max(BIAS * (1.0 - dot(fragNormal, lightDirection)), 0.0001);
	// return ((currentDepth - bias > closestDepth) ? (1.0) : (0.0));

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize ( tex_shadowMap , 0);
	for( float x = -1; x <= 1; x +=1.0)
	for( float y = -1; y <= 1; y +=1.0) 
	{
		float pcfDepth = texture ( tex_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	}
	return ( shadow / 9.0);
}


float calcPointShadow(int id, vec3 lightPosition, vec3 fragPos, float farPlane, samplerCube texShadowCubeMap)
{
	// get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPosition;

	// now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(texShadowCubeMap, fragToLight).r * farPlane;


	float bias = 0.01;
	float shadow = 0.0;

	// simple test
	vec3 lightDirection = normalize(fragPos - lightPosition);

  	bias = max(0.04 * (1.0 - dot(fragNormal, lightDirection)), 0.01);

  	shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
	//return shadow;


	// filtering
    int samples = 20;
    float viewDistance = length(cameraPos - fragPos.xyz);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(texShadowCubeMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]

        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);

    return shadow/samplerCubeCount;

}

void main()
{
	vec3 lightCoefDS = vec3(0.0f);
	vec3 lightCoefA = vec3(0.0f);

	// 1. ustawienie bazowego koloru 
	vec4 texColor = texture(tex, fragUV);
	if(shaderMode == true) // per_fragment
	{
		// 2. ustawienie współ. światła kier. (składowadowa diff + spec)
		if(globalLightParam.IsTurnedOn != 0)
		{
			lightCoefDS += calculateDirectionalLight(matMP, globalLightParam, myMaterial);
			lightCoefA += globalLightParam.Ambient * myMaterial.Ambient;
		}

		//FIX_ME += lightcoef

		for(int i=0; i<lightCount; i++)
		{
			if(myLight[i].IsTurnedOn != 0)
			{
				if(lightMode == 0)
					lightCoef += calculatePointLight(matMP, myLight[i], myMaterial);
				else if(lightMode == 1)
					lightCoef += calculateBlinnPhongLight(matMP, myLight[i], myMaterial);
			}
		}
		if(relativeLightParam.IsTurnedOn != 0)
			lightCoef = max(lightCoef, calculateRelativeLight(matVMP, relativeLightParam, myMaterial));
	}

	vec3 fragColor = texColor.xyz;

	// 3. coef cienia kier.
	// Oswietlenie kierunkowe
	float shadowPart = 1.0f;
	if(globalLightParam.IsTurnedOn != 0)
		shadowPart = calcDirectionalShadow(fragPosLight, fragNormal, lightDirection);

	// 4. coef cienia punktowego
	float pointShadowPart = 0.0f;
	// for(int i=1; i<samplerCubeCount; i++)
	// {
	if(myLight[lightShadowId1].IsTurnedOn != 0)
	{
		pointShadowPart += calcPointShadow(lightShadowId1, myLight[lightShadowId1].Position, fragPos, myLight[lightShadowId1].farPlane, texShadowCubeMap1);
	}
	if(myLight[lightShadowId2].IsTurnedOn != 0)
	{
		pointShadowPart += calcPointShadow(lightShadowId2, myLight[lightShadowId2].Position, fragPos, myLight[lightShadowId2].farPlane, texShadowCubeMap2);
	}
	if(myLight[lightShadowId3].IsTurnedOn != 0)
	{
		pointShadowPart += calcPointShadow(lightShadowId3, myLight[lightShadowId3].Position, fragPos, myLight[lightShadowId3].farPlane, texShadowCubeMap3);
	}
	if(myLight[lightShadowId4].IsTurnedOn != 0)
	{
		pointShadowPart += calcPointShadow(lightShadowId4, myLight[lightShadowId4].Position, fragPos, myLight[lightShadowId4].farPlane, texShadowCubeMap4);
	}
	if(myLight[lightShadowId5].IsTurnedOn != 0)
	{
		pointShadowPart += calcPointShadow(lightShadowId5, myLight[lightShadowId5].Position, fragPos, myLight[lightShadowId5].farPlane, texShadowCubeMap5);
	}
	if(myLight[lightShadowId6].IsTurnedOn != 0)
	{
		pointShadowPart += calcPointShadow(lightShadowId6, myLight[lightShadowId6].Position, fragPos, myLight[lightShadowId6].farPlane, texShadowCubeMap6);
	}

	pointShadowPart = clamp(pointShadowPart, 0.0f, 0.8f);

	vec3 shadowedColor;
	// Kolor finalny
	// if(shadowPart >= 0.7f)
	// 	shadowedColor = ((1.428f - shadowPart) * lightCoef) * fragColor;
	// else
	shadowedColor = clamp(
		((lightCoefA + (1.0f - shadowPart) * lightCoefDS) 
		+((1.0f - pointShadowPart)) * (lightCoef*0.1f))
		* fragColor, 
		vec3(0.0f), vec3(1.0f));

	// Mgła
	float dist = abs( matVMP.z / matVMP.w);
	float fogCoeff = clamp(exp(- fogDensity * dist ), 0.0, 1.0);
	vec3 finalColor = mix (fogColor, shadowedColor, fogCoeff);

	outColor = vec4(finalColor, 1.0f);
}
