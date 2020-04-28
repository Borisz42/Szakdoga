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


	// shaderekhez sz�ks�ges v�ltoz�k
	GLuint m_programID; // shaderek programja

	// OpenGL-es dolgok
	GLuint m_vaoID; // vertex array object er�forr�s azonos�t�
	GLuint m_vboID; // vertex buffer object er�forr�s azonos�t�

	float getDist;
	float energyRemaining = 0.7;
	float gravity = 15;
	bool playerCall = false;
	float shoot_time;
	GLuint	m_loc_ballPos;
	GLuint	m_loc_multiBallPos;
	float multiBallPos[80];
	float multiBallVel[60];
	int ballCount = 1;

	//Frakt�l param�terek
	float shift_x = 0.0;
	float shift_y = 0.0;
	float shift_z = 0.0;
	float fold_x = 0.0;
	float fold_y = 0.0;
	float fold_z = 0.0;
	float rot_x = 0.0;
	float rot_y = 0.0;
	float rot_z = 0.0;
	int iterations = 1;


	float time;
	bool update_time = true;
	Uint32 last_time = 1;
	double delta_time = 0.01;
	double loopindex = 2.0;
	int Framerate = 60;
	int Simulationsrate = 2; 
	int Last_Framerate = 60;
	int Last_Simulationsrate = 2;


	// m�trixok helye a shaderekben
	GLuint	m_loc_world;
	GLuint	m_loc_view;
	GLuint	m_loc_proj;
	GLuint	m_loc_window_x;
	GLuint	m_loc_window_y;
	GLuint	m_loc_eye;
	GLuint	m_loc_at;
	GLuint	m_loc_up;
	GLuint	m_loc_time;

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

