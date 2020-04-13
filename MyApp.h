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
	glm::vec3 GetNormal(glm::vec3 p);

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

	// Labda
	glm::vec4 ballPos = glm::vec4(3, 0, 0, 0.07);
	glm::vec3 ballVel = glm::vec3(0, 0, 0);
	float energyRemaining = 0.8;
	float gravity = 10;
	bool playerCall = false;
	float shoot_time;
	GLuint	m_loc_ballPos;

	float time;


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

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};
};

