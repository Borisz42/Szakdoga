#version 130

// VBO-b�l �rkez� v�ltoz�k
in vec3 vs_in_pos;
in vec3 vs_in_col;

// a pipeline-ban tov�bb adand� �rt�kek
out vec3 vs_out_col;
out vec2 vs_out_pos;



// shader k�ls� param�terei - most a h�rom transzform�ci�s m�trixot k�l�n-k�l�n vessz�k �t
uniform float windowX;
uniform float windowY;



void main()
{
	gl_Position = vec4( vs_in_pos, 1 );
	vs_out_col = vs_in_pos;
	vec2 temp = vs_in_pos.xy;
	temp.x *= windowX/windowY;
	vs_out_pos = temp;

}