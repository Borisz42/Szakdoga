#version 130

in vec3 vs_out_col;
in vec2 vs_out_pos;
out vec4 fs_out_col;

uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;
uniform float time;
uniform vec4 multiBallPos[20];

uniform float shift_x;
uniform float shift_y;
uniform float shift_z;
uniform float fold_x;
uniform float fold_y;
uniform float fold_z;
uniform float rot_x;
uniform float rot_y;
uniform float rot_z;
uniform int iterations;
uniform int ballCount;

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

void mengerFold(inout vec3 z) {
	float a = min(z.x - z.y, 0.0);
	z.x -= a;
	z.y += a;
	a = min(z.x - z.z, 0.0);
	z.x -= a;
	z.z += a;
	a = min(z.y - z.z, 0.0);
	z.y -= a;
	z.z += a;
}

void sierpinskiFold(inout vec3 z) {
	z.xy -= min(z.x + z.y, 0.0);
	z.xz -= min(z.x + z.z, 0.0);
	z.yz -= min(z.y + z.z, 0.0);
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

vec3 multi_fold(vec3 pt, float xx, float yy, float zz) {
    pt.yz = fold(pt.yz, 2.0 * xx );
    pt.zx = fold(pt.zx, 2.0 * yy );
    pt.xy = fold(pt.xy, 2.0 * zz );

    pt.yz = fold(pt.yz, -1.0 * xx );
    pt.zx = fold(pt.zx, -1.0 * yy );
    pt.xy = fold(pt.xy, -1.0 * zz );
    return pt;
}

vec3 iter_fold(vec3 pt) {
    vec3 before = pt;
    for(int i = 1; i < iterations+1; ++i){
        pt.x -= shift_x;
        pt.y -= shift_y;
        pt.z -= shift_z;
        rotX(pt, rot_x);
        rotY(pt, rot_y);
        rotZ(pt, rot_z);
        pt=multi_fold(pt, fold_x, fold_y, fold_z);
 //       mengerFold(pt);
    }
    return pt;
}

float MultiBallDist(vec3 pos)
{
    float ballDist;
    float minDist = 100.0;
    for (int i = 0; i<ballCount; ++i)
    {
         ballDist = length(pos - multiBallPos[i].xyz) - multiBallPos[i].w;
         minDist = min(minDist, ballDist);
    }
    return minDist;
}


float GetDist(vec3 pos, out float col) {
    float boxDist = sdBox(iter_fold(pos), vec3(1., 1., 2.));
    float planeDist = pos.y+4;
    float mod_ballDist = length(vec3(mod(abs(pos.x), 15), pos.y, mod(abs(pos.z), 15)) - vec3(4.0, -3.0, 8.0)) - 1.0; 
    float multiBallDist = MultiBallDist(pos);

    float minDist = min(boxDist, planeDist);
    minDist = min(minDist, mod_ballDist);
    minDist = min(minDist, multiBallDist);

    if (minDist == planeDist) {col = 2.1;}
    if (minDist == mod_ballDist || minDist == multiBallDist) {col = 3.1;}

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

// http://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float calcSoftshadow( in vec3 ro, in vec3 rd, in float mint, in float tmax )
{
    float res = 1.0;
    float t = mint;
    for( int i=0; i<30; i++ )
    {
		float h = GetDist( ro + rd*t );
        float s = clamp(8.0*h/t,0.0,1.0);
        res = min( res, s/1.5 );
        t += h;
        if( res<0.005 || t>tmax ) break;
    }
    return clamp( res, 0.0, 1.0 );
}

float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = GetDist( aopos );
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) * (0.5+0.5*nor.y);
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
    vec3 lightPos = vec3(0, 5, 0);                // a fény kiinduló helyzete
    lightPos.xz += vec2(sin(time/time), cos(time/time))*12.; // a fény körpályán mozog
    vec3 l = normalize(lightPos-p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l)+0.1, 0., 1.)*0.8;
    float d = RayMarch(p+n*SURF_DIST*2., l);
    if(d<length(lightPos-p)) dif *= 0.5;
    
    return dif;
}

vec3 render(vec3 ro, vec3 rd)
{ 
    vec3 col = vec3(0.7, 0.7, 0.9) - max(rd.y,0.0);
    float dist = RayMarch(ro,rd);;
    if (dist > MAX_DIST-2.0)  { return vec3( clamp(col,0.0,1.0) ); } 
	float getColor = 0.0;

    vec3 pos = ro + dist*rd;
    GetDist(pos, getColor);
    vec3 nor = (getColor==2.1) ? vec3(0.0,1.0,0.0) : GetNormal(pos);
    vec3 ref = reflect( rd, nor );

    if (getColor < 2.0) {
          col = nor*0.6; 
      }else if (getColor < 3.0){
          col = vec3(0.01, 0.3, 0.01) * 0.6;     
      }else if (getColor < 4.0){    
          col = vec3(0.3);
      }
       
    // lighting
    float occ = calcAO( pos, nor );
	vec3  lig = normalize( vec3(0.0, 0.4, -0.6) );
    vec3  hal = normalize( lig-rd );
	float amb = sqrt(clamp( 0.5+0.5*nor.y, 0.0, 1.0 ));
    float dif = clamp( dot( nor, lig ), 0.0, 1.0 );
    float bac = clamp( dot( nor, normalize(vec3(-lig.x,0.0,-lig.z))), 0.0, 1.0 )*clamp( 1.0-pos.y,0.0,1.0);
    float dom = smoothstep( -0.2, 0.2, ref.y );
    float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 2.0 );
        
    dif *= calcSoftshadow( pos, lig, 0.01, MAX_DIST );
    dom *= calcSoftshadow( pos, ref, 0.01, 15.0 );

	float spe = pow( clamp( dot( nor, hal ), 0.0, 1.0 ),16.0) * dif * (0.04 + 0.96*pow( clamp(1.0+dot(hal,rd),0.0,1.0), 5.0 ));

	vec3 lin = vec3(0.0);
    lin += 2.0*dif*vec3(1.30,1.00,0.70);
    lin += 0.55*amb*vec3(0.40,0.60,1.15)*occ;
    lin += 0.55*dom*vec3(0.40,0.60,1.30)*occ;
    lin += 0.55*bac*vec3(0.25,0.25,0.25)*occ;
    lin += 0.55*fre*vec3(1.00,1.00,1.00)*occ;
	col = col*lin;

    col += 7.00*spe*vec3(1.10,0.90,0.70);

    // gamma
    col = pow( col, vec3(0.4545) );
    col = mix( col, vec3(0.7,0.7,0.9), 1.0-exp( -0.00004*dist*dist*dist ) );


	return vec3( clamp(col,0.0,1.0) );
}

vec3 render_old(vec3 ray_origin, vec3 ray_direction)
{
    vec3 col = vec3(135.0, 206.0, 235.0)/255; 

    float distance = RayMarch(ray_origin, ray_direction);
    float light = 0.0;
    float getColor = 0.0;

    if (distance < MAX_DIST-2.0) 
    {
        vec3 p = ray_origin + ray_direction * distance; 
        light = GetLight(p);
        GetDist(p, getColor);
        if (getColor < 2.0) {
            col = GetNormal(p); 
            //col = min(light * 1.5 * normalize(vec3(0.1, 0.2, 0.3)) + 0.1, 1.0);
        }else if (getColor < 3.0){
            col = min(light * normalize(vec3(0.1, 0.8, 0.2)) + 0.1, 1.0);     
        }else if (getColor < 4.0){    
            col = min(vec3(light)+0.15, 1.0);
        }
    }
    return vec3( clamp(col,0.0,1.0) );
}

void main()
{

    vec2 uv = vs_out_pos;
    
    vec3 ray_origin = eye;
    vec3 forward = normalize(at - ray_origin); 
    vec3 right = normalize(cross(up, forward));
    vec3 upward = normalize(cross(forward, right));

    vec3 canvas = ray_origin + forward*2.5;
    vec3 canvas_point = canvas + uv.x*right + uv.y*upward;
    vec3 ray_direction = normalize(canvas_point - ray_origin); 

    vec3 col = render(ray_origin, ray_direction);

	fs_out_col = vec4(col,1.0);;
}

