uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec2 lightPositionOnScreen;
uniform sampler2D myTexture;
const int NUM_SAMPLES = 100 ;
void main()
{	
	vec2 deltaTextCoord = vec2( gl_TexCoord[0].xy - lightPositionOnScreen.xy );
	vec2 textCoord = gl_TexCoord[0].xy;
	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;
	
	vec3 outputColor = vec3(0,0,0);
	for(int i=0; i < NUM_SAMPLES ; i++)
	{
			textCoord -= deltaTextCoord;
			vec3 sample = texture2D(myTexture, textCoord ).xyz;
			
			sample *= illuminationDecay * weight;
			
			outputColor += sample;
			
			illuminationDecay *= decay;
	}
	
	
	outputColor *= exposure;
	
	gl_FragColor = vec4(outputColor,1);
}
