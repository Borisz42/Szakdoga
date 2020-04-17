#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

#define MAXMANDELBROTDIST 1.5
#define MANDELBROTSTEPS 100

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_camera.SetView(glm::vec3(4, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// t�rl�si sz�n legyen k�kes
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // m�lys�gi teszt bekapcsol�sa (takar�s)
	glCullFace(GL_BACK); // GL_BACK: a kamer�t�l "elfel�" n�z� lapok, GL_FRONT: a kamera fel� n�z� lapok

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{ 
		// 1. h�romsz�g
		//          x,  y, z             R, G, B
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},

		// 2. h�romsz�g
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1)},
	};

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen gener�lt VAO beallitasa akt�vnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk l�tre egy �j VBO er�forr�s nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegy�k "akt�vv�" a l�trehozott VBO-t
	// t�lts�k fel adatokkal az akt�v VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az akt�v VBO-ba t�lts�nk adatokat
				  sizeof(vert),		// ennyi b�jt nagys�gban
				  vert,	// err�l a rendszermem�riabeli c�mr�l olvasva
				  GL_STATIC_DRAW);	// �gy, hogy a VBO-nkba nem tervez�nk ezut�n �rni �s minden kirajzol�skor felhasnz�ljuk a benne l�v� adatokat
	

	// VAO-ban jegyezz�k fel, hogy a VBO-ban az els� 3 float sizeof(Vertex)-enk�nt lesz az els� attrib�tum (poz�ci�)
	glEnableVertexAttribArray(0); // ez lesz majd a poz�ci�
	glVertexAttribPointer(
		0,				// a VB-ben tal�lhat� adatok k�z�l a 0. "index�" attrib�tumait �ll�tjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. index� attrib�tum hol kezd�dik a sizeof(Vertex)-nyi ter�leten bel�l
	); 

	// a m�sodik attrib�tumhoz pedig a VBO-ban sizeof(Vertex) ugr�s ut�n sizeof(glm::vec3)-nyit menve �jabb 3 float adatot tal�lunk (sz�n)
	glEnableVertexAttribArray(1); // ez lesz majd a sz�n
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	glBindVertexArray(0); // felt�lt�tt�k a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // felt�lt�tt�k a VBO-t is, ezt is vegy�k le

	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket t�rol� program l�trehoz�sa
	m_programID = glCreateProgram();

	// adjuk hozz� a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attrib�tumok hozz�rendel�se a shader v�ltoz�khoz
	// FONTOS: linkel�s el�tt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonos�t�ja, amib�l egy v�ltoz�hoz szeretn�nk hozz�rendel�st csin�lni
							0,				// a VAO-beli azonos�t� index
							"vs_in_pos");	// a shader-beli v�ltoz�n�v
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illessz�k �ssze a shadereket (kimen�-bemen� v�ltoz�k �sszerendel�se stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);


	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	// vet�t�si m�trix l�trehoz�sa

	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	m_loc_window_x = glGetUniformLocation(m_programID, "windowX");
	m_loc_window_y = glGetUniformLocation(m_programID, "windowY");
	m_loc_eye = glGetUniformLocation(m_programID, "eye");
	m_loc_at = glGetUniformLocation(m_programID, "at");
	m_loc_up = glGetUniformLocation(m_programID, "up");
	m_loc_time = glGetUniformLocation(m_programID, "time");
	m_loc_ballPos = glGetUniformLocation(m_programID, "ballPos");
	

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}


float sdBox(glm::vec3 p, glm::vec3 b)
{
	glm::vec3 q = abs(p) - b;
	float ret1 = glm::length(max(q, glm::vec3(0.0)));
	float max1 = glm::max(q.y, q.z);
	float max2 = glm::max(q.x, max1);
	return ret1 + glm::min(max2, (float)0.0);
}


glm::vec2 fold(glm::vec2 p, float ang) {
	glm::vec2 n = glm::vec2(cos(-ang), sin(-ang));
	p -= glm::vec2(2.) * glm::min(glm::vec2(0.), glm::dot(p, n)) * n;
	return p;
}
#define PI 3.14159

glm::vec3 tri_fold(glm::vec3 pt) {
	pt.x = fold(glm::vec2(pt.x, pt.y), (float)(PI / 3. + .5)).x;
	pt.y = fold(glm::vec2(pt.x, pt.y), (float)(PI / 3. + .5)).y;
	pt.x = fold(glm::vec2(pt.x, pt.y), (float)(-PI / 3.)).x;
	pt.y = fold(glm::vec2(pt.x, pt.y), (float)(-PI / 3.)).y;
	pt.z = fold(glm::vec2(pt.z, pt.y), (float)(PI / 6. + .7)).x;
	pt.y = fold(glm::vec2(pt.z, pt.y), (float)(PI / 6. + .7)).y;
	pt.z = fold(glm::vec2(pt.z, pt.y), (float)(-PI / 6.)).x;
	pt.y = fold(glm::vec2(pt.z, pt.y), (float)(-PI / 6.)).y;
	return pt;
}
glm::vec3 tri_curve(glm::vec3 pt) {
	for (int i = 0; i < 20; i++) {
		pt *= 1.5;
		pt.x -= 1.6;
		pt.z += 1.9;
		pt = tri_fold(pt);
	}
	pt /= pow(1.5, 20);
	return pt;
}

float CMyApp::GetDist(glm::vec3 pos) {
	float boxDist = sdBox(tri_curve(pos), glm::vec3(1., 1., 2.));
	float planeDist = pos.y + 1.5;
	float ballDist = glm::length(pos - glm::vec3(ballPos.x, ballPos.y, ballPos.z) - ballPos.w);

	float minDist = glm::min(boxDist, planeDist);
	minDist = glm::min(minDist, ballDist);

	return minDist;
}

glm::vec3 CMyApp::GetNormal(glm::vec3 p) {
	float d = GetDist(p);
	float e = 0.0001;

	glm::vec3 n = d - glm::vec3(
		GetDist(p - glm::vec3(e, 0, 0)),
		GetDist(p - glm::vec3(0, e, 0)),
		GetDist(p - glm::vec3(0, 0, e)));

	return glm::normalize(n);
}

glm::mat3 rotationMatrix(glm::vec3 axis, float angle) {
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return glm::mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
					 oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
					 oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}


void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;
	m_camera.Update(delta_time*0.1);
	time = SDL_GetTicks() / 1000.0f;

	glm::vec3 eye = m_camera.GetEye();
	glm::vec3 at = m_camera.GetAt();
	glm::vec3 forward = glm::normalize(at - eye);
	forward *= 0.5;
	glm::vec3 ballHome = eye + forward;

	float Collision = GetDist(ballPos) - ballPos.w;

	if (Collision < 0) //�tk�z�s b�rmivel
	{
		glm::vec3 norm = GetNormal(ballPos);
		if (glm::dot(norm, glm::normalize(ballVel)) < 0.0)
		{
			ballVel = rotationMatrix(norm, 3.14159) * ballVel;
			ballVel *= -1;
		}		
		if (Collision < -0.004)
		{
			norm *= 0.007;
			ballPos.x += norm.x;
			ballPos.y += norm.y;
			ballPos.z += norm.z;
		}		
			ballVel *= energyRemaining;
	}
	else if (Collision < 0.004)
	{
		ballVel.y -= gravity * delta_time * Collision; //Gyeng�tett gravit�ci�
		ballVel *= energyRemaining;
	}
	else if (!playerCall)
	{
		ballVel.y -= gravity * delta_time; //Gravit�ci�
	}
	

	if (playerCall) 
	{
		ballVel.x = ballHome.x - ballPos.x;
		ballVel.y = ballHome.y - ballPos.y;
		ballVel.z = ballHome.z - ballPos.z;
		ballVel *= 10.0;
		shoot_time = last_time + delta_time * 2000.0;
	}

	if (last_time < shoot_time && !playerCall)
	{
		ballVel = forward;
		ballVel *= 15;
	}

	ballPos.x += delta_time * ballVel.x;
	ballPos.y += delta_time * ballVel.y;
	ballPos.z += delta_time * ballVel.z;

	last_time = SDL_GetTicks();
}


void CMyApp::Render(int WindowX, int WindowY)
{
	// t�r�lj�k a frampuffert (GL_COLOR_BUFFER_BIT) �s a m�lys�gi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolasa
	glUseProgram( m_programID );
	glm::vec3 eye = m_camera.GetEye();
	glm::vec3 at = m_camera.GetAt();
	glm::vec3 up = m_camera.GetUp();

	time = SDL_GetTicks() / 1000.0f;

	glUniform1f(m_loc_window_x, GLfloat(WindowX));
	glUniform1f(m_loc_window_y, GLfloat(WindowY));
	glUniform3f(m_loc_eye, eye.x, eye.y, eye.z);
	glUniform3f(m_loc_at, at.x, at.y, at.z);
	glUniform3f(m_loc_up, up.x, up.y, up.z);
	glUniform1f(m_loc_time, time);
	glUniform4f(m_loc_ballPos, ballPos.x, ballPos.y, ballPos.z, ballPos.w);


	// kapcsoljuk be a VAO-t (a VBO j�n vele egy�tt)
	glBindVertexArray(m_vaoID);

	// kirajzol�s
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// VAO kikapcsolasa
	glBindVertexArray(0);

	// shader kikapcsolasa
	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
	if (key.keysym.sym == SDLK_SPACE) { playerCall = true; }
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
	if (key.keysym.sym == SDLK_SPACE) { playerCall = false; }
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a k�t param�terbe az �j ablakm�ret sz�less�ge (_w) �s magass�ga (_h) tal�lhat�
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

}