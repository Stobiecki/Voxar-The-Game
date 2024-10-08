// Fragment shader
#version 330 core


// Predefiniowane dane wejsciowe
/*
in  vec4 gl_FragCoord;
in  int  gl_PrimitiveID;
*/

// Dane wyjsciowe
out vec4 outColor;


void main()
{

	if (gl_PrimitiveID%2 == 0)
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		outColor = vec4(0.0, 1.0, 0.0, 1.0);

}
