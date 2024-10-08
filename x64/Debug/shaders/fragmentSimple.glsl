#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;

const float offset = 1.0 / 300.0;  
const float offset2 = (1.0 / 300.0)*(1.0 / 300.0);

uniform vec3 palette[64];
uniform int paletteSize;

const int indexMatrix4x4[16] = int[](0,  8,  2,  10,
                                     12, 4,  14, 6,
                                     3,  11, 1,  9,
                                     15, 7,  13, 5);



vec3 HUEtoRGB(in float H)
{
	float R = abs(H * 6 - 3) - 1;
	float G = 2 - abs(H * 6 - 2);
	float B = 2 - abs(H * 6 - 4);
	return clamp(vec3(R,G,B), 0.0, 1.0);
}

float Epsilon = 1e-10;
 
vec3 RGBtoHCV(in vec3 RGB)
{
	// Based on work by Sam Hocevar and Emil Persson
	vec4 P = (RGB.g < RGB.b) ? vec4(RGB.bg, -1.0, 2.0/3.0) : vec4(RGB.gb, 0.0, -1.0/3.0);
	vec4 Q = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
	float C = Q.x - min(Q.w, Q.y);
	float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
	return vec3(H, C, Q.x);
}

vec3 HSLtoRGB(in vec3 HSL)
{
	vec3 RGB = HUEtoRGB(HSL.x);
	float C = (1 - abs(2 * HSL.z - 1)) * HSL.y;
	return (RGB - 0.5) * C + HSL.z;
}

vec3 RGBtoHSL(in vec3 RGB)
{
	vec3 HCV = RGBtoHCV(RGB);
	float L = HCV.z - HCV.y * 0.5;
	float S = HCV.y / (1 - abs(L * 2 - 1) + Epsilon);
	return vec3(HCV.x, S, L);
}

float indexValue() 
{
    int x = int(mod(gl_FragCoord.x, 4));
    int y = int(mod(gl_FragCoord.y, 4));
    return indexMatrix4x4[(x + y * 4)] / 16.0;
}

float hueDistance(float h1, float h2) 
{
    float diff = abs((h1 - h2));
    return min(abs((1.0 - diff)), diff);
}

vec3[2] closestColors(float hue) 
{
    vec3 ret[2];
    vec3 closest = vec3(-2, 0, 0);
    vec3 secondClosest = vec3(-2, 0, 0);
    vec3 temp;
    for (int i = 0; i < paletteSize; ++i) {
        temp = palette[i];
        float tempDistance = hueDistance(temp.x, hue);
        if (tempDistance < hueDistance(closest.x, hue)) {
            secondClosest = closest;
            closest = temp;
        } else {
            if (tempDistance < hueDistance(secondClosest.x, hue)) {
                secondClosest = temp;
            }
        }
    }
    ret[0] = closest;
    ret[1] = secondClosest;
    return ret;
}

const float lightnessSteps = 4.0;

float lightnessStep(float l) 
{

    return floor((0.5 + l * lightnessSteps)) / lightnessSteps;
}

vec3 dither(vec3 color) 
{
    vec3 hsl = RGBtoHSL(color);

    vec3 cs[2] = closestColors(hsl.x);
    vec3 c1 = cs[0];
    vec3 c2 = cs[1];
    float d = indexValue();
    float hueDiff = hueDistance(hsl.x, c1.x) / hueDistance(c2.x, c1.x);

    float l1 = lightnessStep(max((hsl.z - 0.125), 0.0));
    float l2 = lightnessStep(min((hsl.z + 0.124), 1.0));
    float lightnessDiff = (hsl.z - l1) / (l2 - l1);

    vec3 resultColor = (hueDiff < d) ? c1 : c2;
    resultColor.z = (lightnessDiff < d) ? l1 : l2;
    return HSLtoRGB(resultColor);
}



void main()
{
	outColor = texture(tex0, inoutUV);
	//outColor = vec4(dither(outColor.rgb), 1);

	// szara tekstura
	// outColor = texture(tex0, inoutUV);
	// float v = 0.3* outColor.r + 0.59*outColor.g + 0.11*outColor.b;
	// outColor = vec4 ( vec3 (v), 1.0);

    // float kernel[4] = float[]
	// (
    //     0.0f/4,  2.0f/4,
    //     3.0f/4, 1.0f/4
    // );

	// // float kernel[4] = float[](
    // //     -2.0f/4,  0.0f/4,
    // //     1.0f/4, -1.0f/4
    // // );
    
	// vec2 texSize = vec2(textureSize (tex0 , 0));

	// // Przetwarzanie splotowe
	// vec3 result = vec3 (0.0);
	// for (int i=0; i<2; ++i) 
	// {
	// 	for (int j=0; j<2; ++j) 
	// 	{
	// 		vec2 offsets = vec2 (i-3.0/2.0, j-3.0/2.0)/texSize;
	// 		vec3 sampleColor = texture (tex0,inoutUV+offsets).rgb;
	// 		result += kernel[j+i*2]*sampleColor;
	// 	}
	// }
	// outColor = vec4(result, 0.0);



	// // dithering 4x4
    // vec2 offsets[16] = vec2[]
	// (
    //     vec2(-offset2,  offset2),	// [0][0] x
    //     vec2( -offset,    offset2), // [0][1] x
    //     vec2( offset,  offset2), 	// [0][2] x
    //     vec2(offset2,  offset2),	// [0][3] x

    //     vec2( -offset2,    offset),	// [1][0] x
    //     vec2(-offset,  offset),		// [1][1] x
    //     vec2(offset, offset), 		// [1][2] x
    //     vec2( offset2,   offset), 	// [1][3] x

	// 	vec2( -offset2,  -offset), 	// [2][0] x
    //     vec2(-offset,  -offset),	// [2][1] x
    //     vec2( offset, -offset),	// [2][2] x
    //     vec2( offset2,  -offset),  // [2][3] x

    //     vec2(-offset2, -offset2),	// [3][0] x
    //     vec2( -offset, -offset2),	// [3][1] x
    //     vec2( offset, -offset2),	// [3][2] x
	// 	vec2( offset2, -offset2) 	// [3][3] x
    // );

    // float kernel[16] = float[](
    //     0.0f/16,  8.0f/16,  2.0f/16,  10.0f/16,
    //     12.0f/16, 4.0f/16,  14.0f/16, 6.0f/16,
    //     3.0f/16,  11.0f/16, 1.0f/16,  9.0f/16,
    //     15.0f/16, 7.0f/16,  13.0f/16, 5.0f/16
    // );
    
    // vec3 sampleTex[16];
    // for(int i = 0; i < 16; i++)
    // {
    //     sampleTex[i] = vec3(texture(tex0, inoutUV.st + offsets[i]));
    // }
    // vec3 col = vec3(0.0);
    // for(int i = 0; i < 16; i++)
    //     col += sampleTex[i] * kernel[i];
    
    // outColor = vec4(col, 1.0);









    // vec2 offsets[9] = vec2[]
	// (
    //     vec2(-offset,  offset), // top-left
    //     vec2( 0.0f,    offset), // top-center
    //     vec2( offset,  offset), // top-right
    //     vec2(-offset,  0.0f),   // center-left
    //     vec2( 0.0f,    0.0f),   // center-center
    //     vec2( offset,  0.0f),   // center-right
    //     vec2(-offset, -offset), // bottom-left
    //     vec2( 0.0f,   -offset), // bottom-center
    //     vec2( offset, -offset)  // bottom-right    
    // );

    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    
    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++)
    // {
    //     sampleTex[i] = vec3(texture(tex0, inoutUV.st + offsets[i]));
    // }
    // vec3 col = vec3(0.0);
    // for(int i = 0; i < 9; i++)
    //     col += sampleTex[i] * kernel[i];
    
    // outColor = vec4(col, 1.0);




	// vec2 texSize = vec2(textureSize (tex0 , 0));

	// mat3 kernel;
	// kernel [0] = vec3 (-1, -1, 1);
	// kernel [1] = vec3 (-1, 0, 1);
	// kernel [2] = vec3 (-1, 1, 1);

	// // Przetwarzanie splotowe
	// vec3 result = vec3 (0.0);
	// for (int i=0; i<3; ++i) 
	// {
	// 	for (int j=0; j<3; ++j) 
	// 	{
    //         vec2 offsets = vec2 (i -3.0/2.0 ,j -3.0/2.0)/ texSize;
    //         vec3 sampleColor = texture (tex0 , inoutUV+offsets).rgb;
    //         result += kernel [i][j] * sampleColor ;
	// 	}
	// }
	// outColor = vec4(result, 0.0);

}
