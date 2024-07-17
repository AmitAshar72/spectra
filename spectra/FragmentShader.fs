#version 330 core

struct Material {    
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionLight 
{
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular; 
};

#define NR_POINT_LIGHTS 1  

in vec3 FragPos;
in vec3 Normal;
in vec3 ourColor;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

//Light
uniform vec3 viewPos;
uniform DirectionLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

//Tex
uniform sampler2D diffuse0; 
uniform sampler2D specular0; 

//Total Point Lights
uniform int num_pointLights;

uniform sampler2D shadowMap;
uniform samplerCube depthMap[NR_POINT_LIGHTS];
uniform float far_plane;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

vec3 CalculateDirectionLights(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLights(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float PointLightShadowCalculation(int pointLightIndex, vec3 fragPos);

/* //COMMENTED CODE START - 1
//To visualize depth buffer
float near = 0.1; 
float far  = 100.0; 
 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
*/ //COMMENTED CODE END - 1


void main()
{
    //Get Normal
    vec3 norm = normalize(Normal);
    //Get View Direction
    vec3 viewDir =  normalize(viewPos - FragPos);

    float shadow = 0.0f;

    //Direction Light
    vec3 result = CalculateDirectionLights(dirLight, norm, viewDir);
    
    //Point Lights 
    for(int i = 0; i < num_pointLights; ++i)
    {
        result += CalculatePointLights(pointLights[i],norm, FragPos, viewDir);
        shadow = PointLightShadowCalculation(i, FragPos);
    }

    //Spot Light
    //result += CalculateSpotLights(spotLight,norm, FragPos, viewDir);    
        
    // Shadow calculation
    //float shadow = ShadowCalculation(FragPosLightSpace, norm, -dirLight.direction);

    // Combine results
    vec3 lighting = (1.0 - shadow) * result;
    
    FragColor = vec4(lighting, 1.0);

    //FragColor = vec4(result, 1.0);

    /* //COMMENTED CODE START - 2
    //Visualize Depth Buffer, Fog Effect
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    vec4 depthVec4 = vec4(vec3(pow(depth, 1.4)), 1.0);
    FragColor = vec4(vec3(depth) * result, 1.0) * (1 - depthVec4) + depthVec4;
    */ //COMMENTED CODE END - 2
}

vec3 CalculateDirectionLights(DirectionLight light, vec3 normal, vec3 viewDir)
{
    vec3 tex = vec3(texture(diffuse0, TexCoord)); //tex ambient and tex diffuse have same values
    
    vec3 texSpecular = vec3(texture(specular0, TexCoord));

    //Set light direction. 
    //Remember to reverse the direction. Users set the direction from the light source. OpenGl reads it the other way
  	vec3 lightDir = normalize(-light.direction); 
    
    //Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);    
    
    //Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);    
        
    //Ambient, diffuse, specular
    vec3 ambient = light.ambient  * tex;
    vec3 diffuse = light.diffuse * (diff * tex);
    vec3 specular = light.specular * (spec * texSpecular); 

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 tex = vec3(texture(diffuse0, TexCoord)); //tex ambient and tex diffuse have same values
    
    vec3 texSpecular = vec3(texture(specular0, TexCoord));
    
    //Get light direction
    vec3 lightDir = normalize(light.position - fragPos);
    
    //Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
        
    //Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    //Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance)); 
    
    //Ambient, diffuse, specular
    vec3 ambient = light.ambient  * tex;
    vec3 diffuse = light.diffuse * (diff * tex);
    vec3 specular = light.specular * (spec * texSpecular); 

    //Combine attenuation
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;

    return result;
}

vec3 CalculateSpotLights(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 tex = vec3(texture(diffuse0, TexCoord)); //tex ambient and tex diffuse have same values
    
    vec3 texSpecular = vec3(texture(specular0, TexCoord));

    //Get light direction
    vec3 lightDir = normalize(light.position - fragPos);

    //Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    //Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    //Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance)); 

    //Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    //Ambient, diffuse, specular
    vec3 ambient = light.ambient  * tex;
    vec3 diffuse = light.diffuse * (diff * tex);
    vec3 specular = light.specular * (spec * texSpecular); 

    //Combine attenuation and intensity
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    vec3 result = ambient + diffuse + specular;
    return result;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
     // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
     //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float PointLightShadowCalculation(int pointLightIndex, vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - pointLights[pointLightIndex].position;
    // ise the fragment to light vector to sample from the depth map    
    //float closestDepth = texture(depthMap[pointLightIndex], fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    //closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    //float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    

    /*
    //Grid Sampling
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap[pointLightIndex], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    */

    // PCF
    float shadow = 0.0;
    float bias = 0.05; 
    float samples = 4.0;
    float offset = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(depthMap[pointLightIndex], fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                closestDepth *= far_plane;   // Undo mapping [0;1]
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
        }
     }
     shadow /= (samples * samples * samples);


        
    return shadow;
}
