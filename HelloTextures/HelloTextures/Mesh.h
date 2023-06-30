#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Bezier.h"


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, int nVertices, Shader* shader, int textureID, glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0), float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();
	void draw();
	void togglePosition();
	void setMaxMinX(float maxX, float minX);
	// void generateRandomCurve();

	GLuint VAO; //Identificador do Vertex Array Object - Vértices e seus atributos

protected:
	int nVertices;

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 originalPosition;
	float angle;
	glm::vec3 axis;

	int textureID;

	bool isMoving;
	int curvePointsCount;
	int movementPointIndex;
	vector <glm::vec3> curvePoints;
	// bool isWalkingForward;
	// bool isWalkingBackwards;
	
	float maxX, minX;

	//Referência (endereço) do shader
	Shader* shader;

	Bezier* bezier;

	std::vector<glm::vec3> generateUnisinosPointsSet();
	GLuint generateControlPointsBuffer(vector <glm::vec3> controlPoints);

};

