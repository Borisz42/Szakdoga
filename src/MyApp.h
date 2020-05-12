#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "gCamera.h"

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	float GetDist(glm::vec3 pos);
	float RayMarch(glm::vec3 ro, glm::vec3 rd);
	float MultiBallDist(glm::vec3 pos);
	glm::vec3 GetNormal(glm::vec3 p);
	glm::vec3 iter_fold(glm::vec3 pt);
	glm::vec3 multi_fold(glm::vec3 pt, float xx, float yy, float zz);

	void Update();
	void Render(int WindowX, int WindowY);

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

protected:

	gCamera	m_camera;


	// shaderekhez szükséges változók
	GLuint m_programID; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object erõforrás azonosító
	GLuint m_vboID; // vertex buffer object erõforrás azonosító

	float getDist;
	float energyRemaining = 0.7;
	float gravity = 20;
	bool playerCall = false;
	float shoot_time;
	GLuint	m_loc_ballPos;
	GLuint	m_loc_multiBallPos;
	static const int Max_ballCount = 100;  //  a shaderben a #define Max_ballCount értékénél nem lehet nagyobb
	float multiBallPos[Max_ballCount * 4];
	float multiBallVel[Max_ballCount * 3];
	int ballCount;
	bool shoot = true;
	glm::vec3 roll;
	float roll_lenght = 0.0;
	float camera_speed = 10.0;

	//Fraktál paraméterek
	float shift_x = -0.187;
	float shift_y = 0.169;
	float shift_z = 0.0;
	float fold_x = 0.007*2.0;
	float fold_y = 0.562*2.0;
	float fold_z = 0.000*2.0;
	float rot_x = 0.002;
	float rot_y = 0.262;
	float rot_z = 0.946;
	float new_shift_x = shift_x;
	float new_shift_y = shift_y;
	float new_shift_z = shift_z;
	float new_fold_x = fold_x;
	float new_fold_y = fold_y;
	float new_fold_z = fold_z;
	float new_rot_x = rot_x;
	float new_rot_y = rot_y;
	float new_rot_z = rot_z;
	float animation = 0.0;
	int iterations = 16;


	bool TESTING = false;
	float rtime;
	bool update_time = true;
	Uint32 last_time = 1;
	double delta_time = 0.01;
	int delta_time_counter = 0;
	static int const avg = 100;
	double delta_time_arr[avg] = { 0.0 };
	double loopindex = 2.0;
	int Framerate = 60;
	int Simulationsrate = 2; 
	int Last_Framerate = 60;
	int Last_Simulationsrate = 2;
	float zoom = 3.0;
	bool ctrl = false;


	// mátrixok helye a shaderekben
	GLuint	m_loc_world;
	GLuint	m_loc_view;
	GLuint	m_loc_proj;
	GLuint	m_loc_window_x;
	GLuint	m_loc_window_y;
	GLuint	m_loc_eye;
	GLuint	m_loc_at;
	GLuint	m_loc_up;
	GLuint	m_loc_time;
	GLuint	m_loc_zoom;

	GLuint  m_loc_shift_x;
	GLuint  m_loc_shift_y;
	GLuint  m_loc_shift_z;
	GLuint  m_loc_fold_x;
	GLuint  m_loc_fold_y;
	GLuint  m_loc_fold_z;
	GLuint  m_loc_rot_x;
	GLuint  m_loc_rot_y;
	GLuint  m_loc_rot_z;
	GLuint  m_loc_iterations;
	GLuint  m_loc_ballCount;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};
};

