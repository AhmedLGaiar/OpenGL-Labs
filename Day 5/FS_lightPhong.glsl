#version 330

// main lights vectors
in vec3 v;
in vec3 n;
in vec3 l;
in vec3 h;

// fixed but can be uniform
// vectors
vec4 object_color = vec4(1,1,1,1);
vec4 light_color = vec4(0.6,0.1,1,1);

// fixed but can be uniform
// lights coefficient
float ka = 0.5;
float kd = 0.8;
float ks = 5;
float sh = 30;

void main()
{
	//float I = ka +
	//kd * max(dot(l,n),0) +
	//ks * max(pow(dot(h,n),sh),0);
	//vec4 light_final_color = I * light_color;

	vec4 light_final_color = ka * light_color +
	kd * max(dot(l,n),0) * light_color +
	ks * max(pow(dot(h,n),sh),0) * light_color;

	gl_FragColor = object_color * light_final_color;
}