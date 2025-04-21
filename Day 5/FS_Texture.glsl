#version 330

in vec2 frag_uv;
uniform sampler2D colormap;

void main()
{
	gl_FragColor = texture(colormap,frag_uv);
}