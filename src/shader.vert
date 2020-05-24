#version 130

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec3 vs_in_col;

// a pipeline-ban tovább adandó értékek
out vec2 vs_out_pos;



uniform float windowX;
uniform float windowY;




void main()
{
	gl_Position = vec4( vs_in_pos, 1 );
	vec2 temp = vs_in_pos.xy;
	temp.x *= windowX/windowY;
	vs_out_pos = temp;

}