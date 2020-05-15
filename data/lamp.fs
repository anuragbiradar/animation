#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;

uniform sampler2D myTextureSampler;

void main()
{
    vec4 texColor = texture(myTextureSampler, TexCoords);
    if(texColor.a < 0.1)
        discard;
    
    FragColor = texColor;
    //FragColor = vec4(1.0); // set alle 4 vector values to 1.0
}
