#version 330

uniform samplerCube cubemap;  

in vec3 frag_position;
in vec3 frag_normal;

//fixed but can be uniform
vec3 camera_position = vec3(0,0,3);

void main()
{
    vec3 refracted_uv = refract(normalize(frag_position - camera_position), normalize(frag_normal), 1/1.52);

    gl_FragColor = texture(cubemap, refracted_uv);
}