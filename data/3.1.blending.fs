#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec3 fragVertex;

uniform sampler2D myTextureSampler;
uniform vec3 ligPos0;
uniform mat4 model;
uniform vec3 cameraPosition;


vec3 calculateLightPosition(vec3 objectColor)
{
	//objectColor vec3(0.10, 0.23, 0.45);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float diffuseStrength = 1.0;
	float specularStrength = 0.25;
	//vec3 ligPos01 = vec3(-100.0, 100.25, 100.0);
        vec3 normal = normalize(transpose(inverse(mat3(model))) * Normals);
        vec3 surfacePos = vec3(model * vec4(fragVertex, 1));
        vec3 surfaceToLight = normalize(ligPos0 - surfacePos);

        // Ambient
        vec3 ambient = 0.05 * objectColor;

        // diffuse
        float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
        vec3 diffuse = diffuseCoefficient * diffuseStrength * objectColor * lightColor;

        // Specular
        vec3 incidenceVector = -surfaceToLight; //a unit vector
        vec3 reflectionVector = reflect(incidenceVector, normal);
        vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);
        //vec3 surfaceToCamera = normalize(vec3(0) - surfacePos);
        float cosAngle = max(0.0, dot(surfaceToCamera, reflectionVector));
        float specularCoefficient = pow(cosAngle, 40.0);

        if (!(diffuseCoefficient > 0.0))
                specularCoefficient = 0.0;

        vec3 specular = specularCoefficient * specularStrength * lightColor;

        return (ambient + diffuse + specular);
        float distanceToLight = length(ligPos0 - surfacePos);
        float attenuation = 1.0 / (1.0 + 0.2f * pow(distanceToLight, 2));

        //linear color (color before gamma correction)
        vec3 linearColor = ambient + attenuation*(diffuse + specular);
        //final color (after gamma correction)
        vec3 gamma = vec3(1.0/2.2);
        vec3 result = vec3(pow(linearColor, gamma));
        return result;
}


void main()
{             
    vec4 texColor = texture(myTextureSampler, TexCoords);
    if(texColor.a < 0.1)
        discard;
    
    //FragColor = texColor;
    vec3 result = calculateLightPosition(texColor.xyz);
    FragColor = vec4(result, 1.0);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
