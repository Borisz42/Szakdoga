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
	// törlési szín legyen kékes
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	glCullFace(GL_BACK); // GL_BACK: a kamerától "elfelé" nézõ lapok, GL_FRONT: a kamera felé nézõ lapok

	//
	// geometria letrehozasa
	//

	Vertex vert[] =
	{ 
		// 1. háromszög
		//          x,  y, z             R, G, B
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},

		// 2. háromszög
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1)},
	};

	// 1 db VAO foglalasa
	glGenVertexArrays(1, &m_vaoID);
	// a frissen generált VAO beallitasa aktívnak
	glBindVertexArray(m_vaoID);
	
	// hozzunk létre egy új VBO erõforrás nevet
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); // tegyük "aktívvá" a létrehozott VBO-t
	// töltsük fel adatokkal az aktív VBO-t
	glBufferData( GL_ARRAY_BUFFER,	// az aktív VBO-ba töltsünk adatokat
				  sizeof(vert),		// ennyi bájt nagyságban
				  vert,	// errõl a rendszermemóriabeli címrõl olvasva
				  GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasnzáljuk a benne lévõ adatokat
	

	// VAO-ban jegyezzük fel, hogy a VBO-ban az elsõ 3 float sizeof(Vertex)-enként lesz az elsõ attribútum (pozíció)
	glEnableVertexAttribArray(0); // ez lesz majd a pozíció
	glVertexAttribPointer(
		0,				// a VB-ben található adatok közül a 0. "indexû" attribútumait állítjuk be
		3,				// komponens szam
		GL_FLOAT,		// adatok tipusa
		GL_FALSE,		// normalizalt legyen-e
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// a 0. indexû attribútum hol kezdõdik a sizeof(Vertex)-nyi területen belül
	); 

	// a második attribútumhoz pedig a VBO-ban sizeof(Vertex) ugrás után sizeof(glm::vec3)-nyit menve újabb 3 float adatot találunk (szín)
	glEnableVertexAttribArray(1); // ez lesz majd a szín
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	glBindVertexArray(0); // feltöltüttük a VAO-t, kapcsoljuk le
	glBindBuffer(GL_ARRAY_BUFFER, 0); // feltöltöttük a VBO-t is, ezt is vegyük le

	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	// a shadereket tároló program létrehozása
	m_programID = glCreateProgram();

	// adjuk hozzá a programhoz a shadereket
	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	// VAO-beli attribútumok hozzárendelése a shader változókhoz
	// FONTOS: linkelés elõtt kell ezt megtenni!
	glBindAttribLocation(	m_programID,	// shader azonosítója, amibõl egy változóhoz szeretnénk hozzárendelést csinálni
							0,				// a VAO-beli azonosító index
							"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	// illesszük össze a shadereket (kimenõ-bemenõ változók összerendelése stb.)
	glLinkProgram(m_programID);

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);


	// mar nincs ezekre szukseg
	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	// vetítési mátrix létrehozása

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

float CMyApp::GetDist(glm::vec3 pos) {
	float Power = 3.0 + 4.0 * (sin(1 / 10.0) + 1.0);
	glm::vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < MANDELBROTSTEPS; i++) {
		r = glm::length(z);
		if (r > MAXMANDELBROTDIST) break;

		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan(z.y / z.x);
		dr = pow(r, Power - 1.0) * Power * dr + 1.0;

		// scale and rotate the point
		float zr = pow(r, Power);
		theta = theta * Power;
		phi = phi * Power;

		// convert back to cartesian coordinates
		z = zr * glm::vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
		z += pos;
	}
	float fractalDist = 0.5 * log(r) * r / dr;
	float planeDist = pos.y + 1.5;
	glm::vec3 temp = abs(pos - glm::vec3(-3.5, -0.75, 6)) - glm::vec3(1, .75, 1);
	float boxDist = glm::length(max(temp, glm::vec3(0.0, 0.0, 0.0)));

	float minDist = glm::min(fractalDist, planeDist);
	minDist = glm::min(minDist, boxDist);


	return minDist;
}

glm::vec3 CMyApp::GetNormal(glm::vec3 p) {
	float d = GetDist(p);
	float e = 0.00001;

	glm::vec3 n = d - glm::vec3(
		GetDist(p - glm::vec3(e, 0, 0)),
		GetDist(p - glm::vec3(0, e, 0)),
		GetDist(p - glm::vec3(0, 0, e)));

	return glm::normalize(n);
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

	if (Collision < 0.0) //Ütközés bármivel
	{
		glm::vec3 norm = GetNormal(ballPos);
		if (glm::dot(norm, glm::normalize(ballVel)) < 0)
		{
			glm::vec4 ballVel_temp = glm::rotate<float>(3.14159265359, norm) * glm::vec4(ballVel, 0);
			ballVel.x = ballVel_temp.x * -1;
			ballVel.y = ballVel_temp.y * -1;
			ballVel.z = ballVel_temp.z * -1;
		}		
		if (Collision < -0.004)
		{
			norm *= 0.005;
			ballPos.x += norm.x;
			ballPos.y += norm.y;
			ballPos.z += norm.z;
		}		
			ballVel *= energyRemaining;
	}
	else if (Collision < 0.004)
	{
		ballVel.y -= gravity * delta_time * Collision; //Gravitáció
		ballVel *= energyRemaining;
	}
	else
	{
		ballVel.y -= gravity * delta_time; //Gravitáció
	}
	

	if (playerCall) 
	{
		ballVel.x = ballHome.x - ballPos.x;
		ballVel.y = ballHome.y - ballPos.y;
		ballVel.z = ballHome.z - ballPos.z;
		ballVel *= 5.0;
		//shoot_time = last_time + delta_time * 2000.0;
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
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
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


	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	glBindVertexArray(m_vaoID);

	// kirajzolás
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

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

}