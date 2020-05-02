#include "MyApp.h"
#include "GLUtils.hpp"
#include <imgui/imgui.h>
#include <math.h>

#define MAX_STEPS 1000
#define MAX_DIST 1000.
#define SURF_DIST .001
#define PHYSICS_UNIT_TIME 0.003
#define PI 3.14159265359

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
	m_camera.SetView(glm::vec3(8, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// törlési szín 
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
	m_loc_multiBallPos = glGetUniformLocation(m_programID, "multiBallPos");

	m_loc_shift_x = glGetUniformLocation(m_programID, "shift_x");
	m_loc_shift_y = glGetUniformLocation(m_programID, "shift_y");
	m_loc_shift_z = glGetUniformLocation(m_programID, "shift_z");
	m_loc_fold_x = glGetUniformLocation(m_programID, "fold_x");
	m_loc_fold_y = glGetUniformLocation(m_programID, "fold_y");
	m_loc_fold_z = glGetUniformLocation(m_programID, "fold_z");
	m_loc_rot_x = glGetUniformLocation(m_programID, "rot_x");
	m_loc_rot_y = glGetUniformLocation(m_programID, "rot_y");
	m_loc_rot_z = glGetUniformLocation(m_programID, "rot_z");
	m_loc_iterations = glGetUniformLocation(m_programID, "iterations");
	m_loc_ballCount = glGetUniformLocation(m_programID, "ballCount");

	ballCount = 1;
	for (int i = 0; i < 20; ++i)
	{
		multiBallPos[i * 4 + 0] = 0.0;
		multiBallPos[i * 4 + 1] = -3;
		multiBallPos[i * 4 + 2] = -2.0 + i/5.0;
		multiBallPos[i * 4 + 3] = 0.07;

		multiBallVel[i * 3 + 0] = 0.0;
		multiBallVel[i * 3 + 1] = 0.0;
		multiBallVel[i * 3 + 2] = 0.0;

	}

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
float sdSphere( glm::vec3 p, float r)
{
	return glm::length(p) - r;
}

float onion(float d, float h)
{
	return abs(d) - h;
}

glm::vec3 rotX(glm::vec3 z, float a) {
	float s = sin(a);
	float c = cos(a);
	return glm::vec3(z.x, c * z.y + s * z.z, c * z.z - s * z.y);
}
glm::vec3 rotY(glm::vec3 z, float a) {
	float s = sin(a);
	float c = cos(a);
	return glm::vec3(c * z.x - s * z.z, z.y, c * z.z + s * z.x);
}
glm::vec3 rotZ(glm::vec3 z, float a) {
	float s = sin(a);
	float c = cos(a);
	return glm::vec3(c * z.x + s * z.y, c * z.y - s * z.x, z.z);
}

glm::vec2 fold(glm::vec2 p, float ang) {
	glm::vec2 n = glm::vec2(cos(-ang), sin(-ang));
	n *= 2. * glm::min((float)0.0, glm::dot(p, n));
	p -= n;
	return p;
}

glm::vec3 CMyApp::multi_fold(glm::vec3 pt, float xx, float yy, float zz) {
	pt = glm::vec3(pt.x, fold(glm::vec2(pt.y, pt.z), (float)(2.0 * xx)));
	glm::vec2 temp = fold(glm::vec2(pt.z, pt.x), (float)(2.0 * yy));
	pt = glm::vec3(temp.y, pt.y, temp.x);
	pt = glm::vec3( fold(glm::vec2(pt.x, pt.y), (float)(2.0 * zz)), pt.z);

	pt = glm::vec3(pt.x, fold(glm::vec2(pt.y, pt.z), (float)(-1.0 * xx)));
	temp = fold(glm::vec2(pt.z, pt.x), (float)(-1.0 * yy));
	pt = glm::vec3(temp.y, pt.y, temp.x);
	pt = glm::vec3(fold(glm::vec2(pt.x, pt.y), (float)(-1.0 * zz)), pt.z);

	return pt;				 
}
glm::vec3 CMyApp::iter_fold(glm::vec3 pt) {
	for (int i = 1; i < iterations+1; ++i) {
		pt.x -= shift_x;
		pt.y -= shift_y;
		pt.z -= shift_z;
		pt = rotX(pt, rot_x);
		pt = rotY(pt, rot_y);
		pt = rotZ(pt, rot_z);
		pt = multi_fold(pt, fold_x, fold_y, fold_z);
	}
	return pt;
}

float CMyApp::MultiBallDist(glm::vec3 pos)
{
	float ballDist;
	float minDist = 10.0;
	float same;
	for (int i = 0; i < ballCount; ++i)
	{
		same = glm::length(pos - glm::vec3(multiBallPos[i * 4 + 0], multiBallPos[i * 4 + 1], multiBallPos[i * 4 + 2]));
		if (same > 0.00011) 
		{ 
			ballDist = same - multiBallPos[i * 4 + 3];
			//ballDist = sdBox(pos - glm::vec3(multiBallPos[i * 4 + 0], multiBallPos[i * 4 + 1], multiBallPos[i * 4 + 2]), glm::vec3(multiBallPos[i * 4 + 3]));
			minDist = glm::min(minDist, ballDist);
		}		
	}
	return minDist;
}

float CMyApp::GetDist(glm::vec3 pos) {
	glm::vec3 q = pos - glm::vec3(8.0, -3.0, 0.0);
	glm::vec3 plane = pos;
	plane = rotX(plane, -0.3);
	float onionDist = glm::max(plane.y + 3.0f, onion(sdSphere(glm::vec3(q.y, q.z, q.x), 1.0), 0.05));
	float boxDist = sdBox(iter_fold(pos), glm::vec3(1., 1., 2.));
	float planeDist = pos.y + 4;
	float mod_ballDist = glm::length(glm::vec3(fmod( abs(pos.x), (float)15.0), pos.y, fmod( abs(pos.z), (float)15.0)) - glm::vec3(4.0, -3.0, 8.0)) - 1.0;
	float multiBallDist = MultiBallDist(pos);

	float minDist = glm::min(boxDist, planeDist);
	minDist = glm::min(minDist, mod_ballDist);
	minDist = glm::min(minDist, multiBallDist);
	minDist = glm::min(minDist, onionDist);

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

float  CMyApp::RayMarch(glm::vec3 ro, glm::vec3 rd) {
	float dist = 0.;

	for (int i = 0; i < MAX_STEPS; i++) {
		glm::vec3 p = ro + rd * dist;
		float dS = GetDist(p);
		dist += dS;
		if (dist > MAX_DIST || dS < SURF_DIST) break;
	}

	return dist;
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
	if (update_time)
	{
		last_time = SDL_GetTicks();
		update_time = false;
	}
	else
	{
		delta_time = (SDL_GetTicks() - last_time) / 1000.0;
		update_time = true;
	}
	m_camera.Update(delta_time * 0.1);
	time = SDL_GetTicks() / 1000.0f;

	glm::vec3 eye = m_camera.GetEye();
	glm::vec3 at = m_camera.GetAt();
	glm::vec3 up = m_camera.GetUp();
	glm::vec3 forward = glm::normalize(at - eye);
	forward *= 0.5;
	glm::vec3 ballHome[20];

	for (int i = 0; i < ballCount; ++i)
	{
		glm::vec3 right = glm::normalize(glm::cross(up, forward)); 
		glm::vec3 upward = glm::normalize(glm::cross(forward, right));
		glm::vec3 temp = eye + forward*(float)2.6;
		ballHome[i] = temp + upward * rotationMatrix(forward, 2.0 * PI / ballCount * i + time/2) * (float)(0.021 + ballCount / 40.0);
	}

	glm::vec3 norm = glm::vec3(0.0, 1.0, 0.0);
	
	loopindex = delta_time / PHYSICS_UNIT_TIME;

	for (int j = 0; j < loopindex; ++j)
	{
		for (int i = 0; i < ballCount; ++i)
		{
			float Collision = GetDist(glm::vec3(multiBallPos[i * 4 + 0], multiBallPos[i * 4 + 1], multiBallPos[i * 4 + 2])) - multiBallPos[i * 4 + 3];
			getDist = Collision;

			if (Collision < 0.0) //Ütközés bármivel
			{
				norm = GetNormal(glm::vec3(multiBallPos[i * 4 + 0], multiBallPos[i * 4 + 1], multiBallPos[i * 4 + 2]));
				if (glm::dot(norm, glm::normalize(glm::vec3(multiBallVel[i * 3 + 0], multiBallVel[i * 3 + 1], multiBallVel[i * 3 + 2]))) < 0.0)
				{
					glm::vec3 temp = rotationMatrix(norm, PI) * glm::vec3(multiBallVel[i * 3 + 0], multiBallVel[i * 3 + 1], multiBallVel[i * 3 + 2]);
					multiBallVel[i * 3 + 0] = temp.x * -(0.98 - PHYSICS_UNIT_TIME);
					multiBallVel[i * 3 + 1] = temp.y * -(0.98 - PHYSICS_UNIT_TIME);
					multiBallVel[i * 3 + 2] = temp.z * -(0.98 - PHYSICS_UNIT_TIME);
					multiBallVel[i * 3 + 0] *= 1.0 - norm.x * (1.0 - energyRemaining);
					multiBallVel[i * 3 + 1] *= 1.0 - norm.y * (1.0 - energyRemaining);
					multiBallVel[i * 3 + 2] *= 1.0 - norm.z * (1.0 - energyRemaining);
				}
				if (Collision < -0.0005)
				{
					norm *= 0.001f + Collision * 0.00000f;
					multiBallPos[i * 4 + 0] += norm.x;
					multiBallPos[i * 4 + 1] += norm.y;
					multiBallPos[i * 4 + 2] += norm.z;
					multiBallVel[i * 3 + 0] *= 0.99 - PHYSICS_UNIT_TIME;
					multiBallVel[i * 3 + 1] *= 0.99 - PHYSICS_UNIT_TIME;
					multiBallVel[i * 3 + 2] *= 0.99 - PHYSICS_UNIT_TIME;
				}

			}

			if (playerCall)
			{
				multiBallVel[i * 3 + 0] = ballHome[i].x - multiBallPos[i * 4 + 0];
				multiBallVel[i * 3 + 1] = ballHome[i].y - multiBallPos[i * 4 + 1];
				multiBallVel[i * 3 + 2] = ballHome[i].z - multiBallPos[i * 4 + 2];
				multiBallVel[i * 3 + 0] *= 10.0;
				multiBallVel[i * 3 + 1] *= 10.0;
				multiBallVel[i * 3 + 2] *= 10.0;
				shoot_time = time + delta_time * 3.0;
			}

			if (time < shoot_time && !playerCall && shoot)
			{
				multiBallVel[i * 3 + 0] += forward.x * 1.0;
				multiBallVel[i * 3 + 1] += forward.y * 1.0;
				multiBallVel[i * 3 + 2] += forward.z * 1.0;
			}
			else 
			{
				multiBallVel[i * 3 + 1] -= gravity * PHYSICS_UNIT_TIME; //Gravitáció
			}
		}
		for (int i = 0; i < ballCount; ++i)
		{
			multiBallPos[i * 4 + 0] += PHYSICS_UNIT_TIME * multiBallVel[i * 3 + 0];
			multiBallPos[i * 4 + 1] += PHYSICS_UNIT_TIME * multiBallVel[i * 3 + 1];
			multiBallPos[i * 4 + 2] += PHYSICS_UNIT_TIME * multiBallVel[i * 3 + 2];
		}
	}


	Framerate = (int)(round(1.0 / delta_time));
	Simulationsrate = loopindex;
}


void CMyApp::Render(int WindowX, int WindowY)
{
	if (ImGui::Begin("MyWindow")) {
		ImGui::Text("Frame rate: %i FPS", Framerate);
		static float values[90] = { 60 };
		static int values_offset = 0;
		static float refresh_time = 0.0f;
		if (refresh_time == 0.0f)
			refresh_time = ImGui::GetTime();
		while (refresh_time < ImGui::GetTime())
		{
			static float phase = 0.0f;
			values[values_offset] = Framerate;
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			phase += 0.10f * values_offset;
			refresh_time += 1.0f / 60.0f;
		}
		ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, "", 0.0f, 65.0f, ImVec2(0, 50));
		ImGui::Text("Physics: %i Simulations/Frame", Simulationsrate);
		ImGui::Text("Physics: %i Simulations/Second", (int)(round(Simulationsrate * (1.0 / delta_time))));
		ImGui::Text("Distance from anything: %f", getDist);
		ImGui::DragFloat("shift_x", &shift_x, 0.001f);
		ImGui::DragFloat("shift_y", &shift_y, 0.001f);
		ImGui::DragFloat("shift_z", &shift_z, 0.001f);
		ImGui::DragFloat("fold_x", &fold_x, 0.001f);
		ImGui::DragFloat("fold_y", &fold_y, 0.001f);
		ImGui::DragFloat("fold_z", &fold_z, 0.001f);
		ImGui::DragFloat("rot_x", &rot_x, 0.001f);
		ImGui::DragFloat("rot_y", &rot_y, 0.001f);
		ImGui::DragFloat("rot_z", &rot_z, 0.001f);
		ImGui::SliderInt("iterations", &iterations, 0, 36);
		ImGui::SliderInt("ball count", &ballCount, 1, 20);
		if (ImGui::Button("Reset values")) {
			shift_x = 0.0;
			shift_y = 0.0;
			shift_z = 0.0;
			fold_x = 0.0;
			fold_y = 0.0;
			fold_z = 0.0;
			rot_x = 0.0;
			rot_y = 0.0;
			rot_z = 0.0;
			iterations = 1;
		}ImGui::SameLine();
		ImGui::Checkbox("Shoot", &shoot);
		ImGui::Text("---------------------------------------------");
		ImGui::Text("--------------Hogyan mukodik?----------------");
		ImGui::Text("A fraktal beallitasahoz hasznald a csuszkakat!");
		ImGui::Text("A terben mozgashoz hasznald a WASD billentyuket!");
		ImGui::Text("A gyorsabb mozgashoz nyomd le a SHIFT billentyut!");
		ImGui::Text("A sebesseg beallitasahoz hasznald a gorgot, vagy ezt:");
		ImGui::SliderFloat("moving speed", &camera_speed, 1.0f, 100.0f, "%.1f");
		ImGui::Text("A kamera mozgatasahoz nyomd le a jobbegergombot!");
		ImGui::Text("A labda hivasahoz nyomde le a space billenytut!");
		ImGui::Text("A labda kilovesehez engedd fel a space billenytut!");
		
	}
	ImGui::End();


	//ImGui::ShowTestWindow();

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
	glProgramUniform4fv(m_programID, m_loc_multiBallPos, 20, multiBallPos);
	glUniform1f(m_loc_shift_x, shift_x);
	glUniform1f(m_loc_shift_y, shift_y);
	glUniform1f(m_loc_shift_z, shift_z);
	glUniform1f(m_loc_fold_x, fold_x);
	glUniform1f(m_loc_fold_y, fold_y);
	glUniform1f(m_loc_fold_z, fold_z);
	glUniform1f(m_loc_rot_x, rot_x);
	glUniform1f(m_loc_rot_y, rot_y);
	glUniform1f(m_loc_rot_z, rot_z);
	glUniform1i(m_loc_iterations, iterations);
	glUniform1i(m_loc_ballCount, ballCount);


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
	if (wheel.y > 0 && camera_speed < 100.0) // scroll up
	{
		camera_speed *= 1.1;
		m_camera.SetSpeed(camera_speed);
	}
	else if (wheel.y < 0 && camera_speed > 1.0) // scroll down
	{
		camera_speed *= 0.9;
		m_camera.SetSpeed(camera_speed);
	}
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

}