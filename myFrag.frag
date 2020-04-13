#version 130

in vec3 vs_out_col;
in vec2 vs_out_pos;
out vec4 fs_out_col;



uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;
uniform float time;
uniform vec4 ballPos;

#define MAX_STEPS 1000
#define MAX_DIST 1000.
#define SURF_DIST .0005

#define MAXMANDELBROTDIST 1.5
#define MANDELBROTSTEPS 100

// distance estimator from: https://www.shadertoy.com/view/wdjGWR
float GetDist(vec3 pos, out float col) {
	float Power = 3.0+4.0*(sin(1/10.0)+1.0);
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < MANDELBROTSTEPS ; i++) {
		r = length(z);
        col = min(i/4.5, 1.0);
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
    float ballDist = length(pos - ballPos.xyz) - ballPos.w;
    float boxDist = length(max(abs(pos-vec3(-3.5, -0.75, 6))-vec3(1,.75,1), 0.));

    float minDist = min(fractalDist, planeDist);
    minDist = min(minDist, ballDist);
    minDist = min(minDist, boxDist);

    if (minDist == planeDist) {col = 2.1;}
    if (minDist == ballDist) {col = 3.1;}

	return minDist;
}

float GetDist(vec3 pos) { 
    float temp;
    return GetDist(pos, temp);
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
    vec2 e = vec2(.00001, 0);
    
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
    
    float dif = clamp(dot(n, l)+0.1, 0., 1.)*0.8;
    float d = RayMarch(p+n*SURF_DIST*2., l);
    if(d<length(lightPos-p)) dif *= 0.5;
    
    return dif;
}

void main()
{

    vec2 uv = vs_out_pos;

    vec3 col = vec3(135.0, 206.0, 235.0)/255;
    
    vec3 ray_origin = eye;
    vec3 forward = normalize(at - ray_origin); 
    vec3 right = normalize(cross(up, forward));
    vec3 upward = normalize(cross(forward, right));

    vec3 canvas = ray_origin + forward*1.0;
    vec3 canvas_point = canvas + uv.x*right + uv.y*upward;
    vec3 ray_direction = normalize(canvas_point - ray_origin);

    float distance = RayMarch(ray_origin, ray_direction);
    float light = 0.0;
    float getColor = 0.0;

    if (distance < MAX_DIST-2.0) 
    {
        vec3 p = ray_origin + ray_direction * distance; 
        light = GetLight(p);
        GetDist(p, getColor);
        if (getColor < 2.0) {
            col = min(light * 1.5 * normalize(vec3(0.1, 0.2, 0.3)) + 0.1, 1.0);
        }else if (getColor < 3.0){
            col = min(light * normalize(vec3(0.1, 0.8, 0.2)) + 0.1, 1.0);     
        }else if (getColor < 4.0){    
            col = min(vec3(light)+0.15, 1.0);
        }
    }

	fs_out_col = vec4(col,1.0);;
}

