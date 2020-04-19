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
	glm::vec3 GetNormal(glm::vec3 p);
	glm::vec3 iter_fold(glm::vec3 pt);
	glm::vec3 multi_fold(glm::vec3 pt);

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

	// Labda
	glm::vec4 ballPos = glm::vec4(7, 0, 0, 0.07);
	glm::vec3 ballVel = glm::vec3(0, 0, 0);
	float getDist;
	float energyRemaining = 0.8;
	float gravity = 10;
	bool playerCall = false;
	float shoot_time;
	GLuint	m_loc_ballPos;

	//Fraktál paraméterek
	float shift_x = -0.1;
	float shift_z = 0.3;
	float fold_z = 0.5;
	float fold_x = 0.7;
	float rot_x = 0.0;
	float rot_y = 0.0;
	int iterations = 20;


	float time;


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
	GLuint  m_loc_shift_x;
	GLuint  m_loc_shift_z;
	GLuint  m_loc_fold_z;
	GLuint  m_loc_fold_x;
	GLuint  m_loc_rot_x;
	GLuint  m_loc_rot_y;
	GLuint  m_loc_iterations;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};
};

