#version 130

in vec3 vs_out_col;
in vec2 vs_out_pos;
out vec4 fs_out_col;

uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;
uniform float time;
uniform vec4 ballPos;

uniform float shift_x;
uniform float shift_z;
uniform float fold_z;
uniform float fold_x;
uniform float rot_x;
uniform float rot_y;
uniform int iterations;

#define MAX_STEPS 100
#define MAX_DIST 50.
#define SURF_DIST .001

#define PI 3.14159

void rotX(inout vec3 z, float a) {
    float s = sin(a);
    float c = cos(a);
    z.yz = vec2(c*z.y + s*z.z, c*z.z - s*z.y);
}
void rotY(inout vec3 z, float a) {
    float s = sin(a);
    float c = cos(a);
	z.xz = vec2(c*z.x - s*z.z, c*z.z + s*z.x);
}
void rotZ(inout vec3 z, float a) {
    float s = sin(a);
    float c = cos(a);
	z.xy = vec2(c*z.x + s*z.y, c*z.y - s*z.x);
}

mat3 rotationMatrix(vec3 axis, float angle){
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c         );
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

vec2 fold(vec2 p, float ang){
    vec2 n=vec2(cos(-ang),sin(-ang));
    p-=2.*min(0.,dot(p,n))*n;
    return p;
}

vec3 multi_fold(vec3 pt) {
    pt.xy = fold(pt.xy,PI/3. + fold_z );
    pt.xy = fold(pt.xy,-PI/3.);
    pt.yz = fold(pt.yz,PI/6. + fold_x );
    pt.yz = fold(pt.yz,-PI/6.);
    return pt;
}

vec3 iter_fold(vec3 pt) {
    for(int i = 1; i < iterations+1; ++i){
        pt.x += shift_x;
        pt.z += shift_z;
        rotX(pt, 1/i + rot_x);
        rotY(pt, 1/i + rot_y);
        pt=multi_fold(pt);
    }
    return pt;
}



float GetDist(vec3 pos, out float col) {
    float boxDist = sdBox(iter_fold(pos), vec3(1., 1., 2.));
    float planeDist = pos.y+4;
    float ballDist = length(pos - ballPos.xyz) - ballPos.w;

    float minDist = min(boxDist, planeDist);
    minDist = min(minDist, ballDist);

    col = 0.1;
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
    vec3 lightPos = vec3(0, 5, 0);                // a f�ny kiindul� helyzete
    lightPos.xz += vec2(sin(time), cos(time))*12.; // a f�ny k�rp�ly�n mozog
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

    vec3 canvas = ray_origin + forward*2.0;
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
            //col = GetNormal(p); 
            col = min(light * 1.5 * normalize(vec3(0.1, 0.2, 0.3)) + 0.1, 1.0);
        }else if (getColor < 3.0){
            col = min(light * normalize(vec3(0.1, 0.8, 0.2)) + 0.1, 1.0);     
        }else if (getColor < 4.0){    
            col = min(vec3(light)+0.15, 1.0);
        }
    }

	fs_out_col = vec4(col,1.0);;
}

