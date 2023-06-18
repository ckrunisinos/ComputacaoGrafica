//Código fonte do Fragment Shader (em GLSL): ainda hardcoded
#version 450

in vec3 finalColor;
in vec3 fragPos;
in vec3 scaledNormal;
in vec2 TexCoord; // the input texture coordinate

out vec4 color;

//Propriedades da superficie
// uniform float ka;
// uniform float kd;
// uniform float ks;
// uniform float q;
uniform vec3 ka;
uniform float kd;
uniform vec3 ks;
uniform float q;

//Propriedades da fonte de luz
uniform vec3 lightPos;
uniform vec3 lightColor;

//Posição da Camera
uniform vec3 cameraPos;

uniform sampler2D texture0; // uniform sampler for your texture

void main()
{
	vec4 texColor = texture(texture0, TexCoord); // get the color from the texture

	vec3 ambient = ka * lightColor;

	vec3 N = normalize(scaledNormal);
	vec3 L = normalize(lightPos - fragPos);
	float diff = max(dot(N,L),0.0);
	vec3 diffuse = kd * diff * lightColor;

	// vec3 V = normalize(cameraPos - fragPos);
	vec3 V = normalize(fragPos);
	vec3 R = normalize(reflect(-L,N));
	float spec = max(dot(R,V),0.0);
	spec = pow(spec, q);
	vec3 specular = ks * spec * lightColor;

	vec3 result = (ambient + diffuse) * finalColor * texColor.rgb + specular; // apply the texture color

	color = vec4(result,1.0);
}