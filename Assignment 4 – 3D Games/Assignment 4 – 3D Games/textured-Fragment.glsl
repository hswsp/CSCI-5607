#version 150 core

#define MAX_LIGHTS 10

in vec3 Color;
in vec3 vertNormal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;
in mat4 curview;

uniform float materialShininess;
uniform vec3 materialSpecularColor;

uniform vec3 cameraPosition;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform int texID;

const float ambient = .3;

uniform int numLights;
uniform struct Light {
   vec4 position;
   vec3 intensities; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
   vec3 coneDirection;
};

uniform vec4 position0;
uniform vec3 intensities0;
uniform float attenuation0;
uniform float ambientCoefficient0;
uniform float coneAngle0;
uniform vec3 coneDirection0;


uniform vec4 position1;
uniform vec3 intensities1;
uniform float attenuation1;
uniform float ambientCoefficient1;
uniform float coneAngle1;
uniform vec3 coneDirection1;

Light allLights0;
Light allLights1;



vec3 ApplyLight(Light light, vec3 surfaceColor, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) 
{
    vec3 surfaceToLight;
    float attenuation = 1.0;
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	float diffuseCoefficient; 
    if(light.position.w == 0.0) 
	{
        //directional light
        surfaceToLight = -normalize(light.position.xyz);
        attenuation = 1.0; //no attenuation for directional lights
    } else 
	{
        //point light
        surfaceToLight =  normalize(light.position.xyz - surfacePos);
        float distanceToLight = length(light.position.xyz - surfacePos);
        attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.coneDirection))));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }
	//ambient
    ambient = light.ambientCoefficient * surfaceColor.rgb * light.intensities;

    //diffuse
    diffuseCoefficient  = max(0.0, dot(normal, surfaceToLight));
	diffuse= diffuseCoefficient * surfaceColor.rgb * light.intensities;//
    
    //specular
    float specularCoefficient= 0.0;// 
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(-surfaceToLight, reflect(surfaceToCamera, normal))), materialShininess);
    specular = specularCoefficient * materialSpecularColor * light.intensities;

    //linear color (color before gamma correction)
    return  ambient+ attenuation*(specular +diffuse);// 
}

void main() 
{
  vec3 color;
  vec3 oColor=vec3(0);
  if (texID == -1)
    color = Color;
  else if (texID == 0)
    color = texture(tex0, texcoord).rgb;
  else if (texID == 1)
    color = texture(tex1, texcoord).rgb;
  else if (texID == 2)
	color = texture(tex2, texcoord).rgb;  
  else if (texID == 3)
	color = texture(tex3, texcoord).rgb;  	
  else
  {
    outColor = vec4(1,0,0,1);
    return; //This was an error, stop lighting!
  }
  vec3 normal = normalize(vertNormal);
  //sports
  allLights0.position=position0;
  allLights0.intensities=intensities0;
  allLights0.attenuation=attenuation0;
  allLights0.ambientCoefficient=ambientCoefficient0;
  allLights0.coneAngle=coneAngle0;
  allLights0.coneDirection=(curview *vec4(normalize(coneDirection0),0.0)).xyz;
  
  //Direction
  allLights1.position=normalize(vec4((curview*normalize(position1)).xyz,0.0));
  allLights1.intensities=intensities1;
  allLights1.attenuation=attenuation1;
  allLights1.ambientCoefficient=ambientCoefficient1;
  allLights1.coneAngle=coneAngle1;
  allLights1.coneDirection=(curview *vec4(normalize(coneDirection1),0.0)).xyz;
  
  
  
  vec3 lightDir1 = allLights1.position.xyz;/**/

  vec3 diffuseC = color*max(dot(-lightDir,normal),0.0);//lightDir
  vec3 ambC = color*ambient;//allLights1.ambientCoefficient   
  vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)! (Do you know why?)
  vec3 reflectDir = reflect(viewDir,normal);
  float spec = max(dot(reflectDir,lightDir),0.0);//lightDir
  if (dot(-lightDir,normal) <= 0.0) spec = 0; //No highlight if we are not facing the light lightDir
  vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4);//
  oColor = ambC+diffuseC+specC;//
  
  
  //combine color from all the lights
  vec3 linearColor = vec3(0);
  //for(int i = 0; i < numLights; ++i)
  //{
	linearColor += ApplyLight(allLights0, color, normal, pos, viewDir);//surfaceToCamera
	linearColor += ApplyLight(allLights1, color, normal, pos, viewDir);
  //}
  oColor += linearColor;/**/
  
 
  //final color (after gamma correction)
  vec3 gamma = vec3(1.0/2.2);
  outColor = vec4(pow(oColor, gamma), 1);//surfaceColor.a
  //outColor = vec4(oColor,1);
}