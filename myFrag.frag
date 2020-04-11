#version 130

in vec3 vs_out_col;
in vec2 vs_out_pos;
out vec4 fs_out_col;



uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;
uniform float time;

#define MAX_STEPS 1000
#define MAX_DIST 1000.
#define SURF_DIST .001

#define MAXMANDELBROTDIST 2.5
#define MANDELBROTSTEPS 200


// distance estimator from: https://www.shadertoy.com/view/wdjGWR
float GetDist(vec3 pos) {
	float Power = 3.0+4.0*(sin(time/10.0)+1.0);
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < MANDELBROTSTEPS ; i++) {
		r = length(z);
		if (r>MAXMANDELBROTDIST) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=pos;
	}
    float fractalDist = 0.5*log(r)*r/dr;
    float planeDist = pos.y+1.5;
	return min(fractalDist, planeDist);
}

float RayMarch(vec3 ro, vec3 rd) {
	float dist=0.;
    
    for(int i=0; i<MAX_STEPS; i++) {
    	vec3 p = ro + rd*dist;
        float dS = GetDist(p);
        dist += dS;
        if(dist>MAX_DIST || dS<SURF_DIST) break;
    }
    
    return dist;
}

vec3 GetNormal(vec3 p) {
	float d = GetDist(p);
    vec2 e = vec2(.001, 0);
    
    vec3 n = d - vec3(
        GetDist(p-e.xyy),
        GetDist(p-e.yxy),
        GetDist(p-e.yyx));
    
    return normalize(n);
}

float GetLight(vec3 p) {
    vec3 lightPos = vec3(0, 4, 0);                // a fény kiinduló helyzete
    lightPos.xz += vec2(sin(time), cos(time))*6.; // a fény körpályán mozog
    vec3 l = normalize(lightPos-p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l)*0.3+0.4, 0., 1.);
    float d = RayMarch(p+n*SURF_DIST*2., l);
    if(d<length(lightPos-p)) dif *= 0.3;
    
    return dif;
}

void main()
{

    vec2 uv = vs_out_pos;

    vec3 col = vec3(0);
    
    vec3 ray_origin = eye;
    vec3 forward = normalize(ray_origin - at); 
    vec3 right = normalize(cross(forward, up));
    vec3 upward = normalize(cross(forward, right));

    vec3 canvas = ray_origin + forward*1.0;
    vec3 canvas_point = canvas + uv.x*right + uv.y*upward;
    vec3 ray_direction = normalize(ray_origin-canvas_point);

    float distance = RayMarch(ray_origin, ray_direction);
    float light = 0.0;

    if (distance < MAX_DIST-2.0) 
    {
        vec3 p = ray_origin + ray_direction * distance; 
        light = GetLight(p);
    }

    col = vec3(light);

	fs_out_col = vec4(col,1.0);;
}

