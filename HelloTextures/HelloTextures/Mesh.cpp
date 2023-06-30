#include "Mesh.h"

#include "glm/ext.hpp"
#include <algorithm>

void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, int textureID, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis)
{
	cout << "scale: " << glm::to_string(scale) << endl;
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->position = position;
	this->scale = scale;
	this->angle = angle;
	this->axis = axis;
    this->textureID = textureID;
	this->originalPosition = glm::vec3(position.x, position.y, position.z);

	this->isMoving = false;
}

void Mesh::update()
{
	if (isMoving) {
		glm::vec3 pointOnCurve = curvePoints[movementPointIndex];
		position = glm::vec3(originalPosition.x + pointOnCurve.x, originalPosition.y, originalPosition.z + pointOnCurve.y);
		movementPointIndex = (movementPointIndex + 1) % curvePointsCount;
	}

	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Mesh::draw()
{
    glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
}

void Mesh::setMaxMinX(float maxX, float minX) {
	this->maxX = maxX;
	this->minX = minX;
}

void Mesh::togglePosition() {
	std::vector<glm::vec3> uniPoints = generateUnisinosPointsSet();
	Bezier myBezier;
	myBezier.setControlPoints(uniPoints);
	myBezier.setShader(this->shader);
	myBezier.generateCurve(10);

	this->bezier = &myBezier;

	this->movementPointIndex = 0;
	this->curvePointsCount = myBezier.getNbCurvePoints();
	this->isMoving = true;

	for (int i = 0; i < curvePointsCount; i++) {
		glm::vec3 pointOnCurve = myBezier.getPointOnCurve(i);
		curvePoints.push_back(pointOnCurve);
	}
}

GLuint Mesh::generateControlPointsBuffer(vector <glm::vec3> controlPoints)
{
	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(GLfloat)* 3, controlPoints.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

std::vector<glm::vec3> Mesh::generateUnisinosPointsSet()
{
	float vertices[] = {
		-0.262530, 0.376992, -3.0,
		-0.262530, 0.377406, -3.0,
		-0.262530, 0.334639, -3.0,
		-0.262530, 0.223162, -3.0,
		-0.262530, 0.091495, -3.0,
		-0.262371, -0.006710, -3.0,
		-0.261258, -0.071544, -3.0,
		-0.258238, -0.115777, -3.0,
		-0.252355, -0.149133, -3.0,
		-0.242529, -0.179247, -3.0,
		-0.227170, -0.208406, -3.0,
		-0.205134, -0.237216, -3.0,
		-0.177564, -0.264881, -3.0,
		-0.146433, -0.289891, -3.0,
		-0.114730, -0.309272, -3.0,
		-0.084934, -0.320990, -3.0,
		-0.056475, -0.328224, -3.0,
		-0.028237, -0.334170, -3.0,
		0.000000, -0.336873, -3.0,
		0.028237, -0.334170, -3.0,
		0.056475, -0.328224, -3.0,
		0.084934, -0.320990, -3.0,
		0.114730, -0.309272, -3.0,
		0.146433, -0.289891, -3.0,
		0.177564, -0.264881, -3.0,
		0.205134, -0.237216, -3.0,
		0.227170, -0.208406, -3.0,
		0.242529, -0.179247, -3.0,
		0.252355, -0.149133, -3.0,
		0.258238, -0.115777, -3.0,
		0.261258, -0.071544, -3.0,
		0.262371, -0.009704, -3.0,
		0.262530, 0.067542, -3.0,
		0.262769, 0.153238, -3.0,
		0.264438, 0.230348, -3.0,
		0.268678, 0.284286, -3.0,
		0.275462, 0.320338, -3.0,
		0.284631, 0.347804, -3.0,
		0.296661, 0.372170, -3.0,
		0.311832, 0.396628, -3.0,
		0.328990, 0.419020, -3.0,
		0.347274, 0.436734, -3.0,
		0.368420, 0.450713, -3.0,
		0.393395, 0.462743, -3.0,
		0.417496, 0.474456, -3.0,
		0.436138, 0.487056, -3.0,
		0.450885, 0.500213, -3.0,
		0.464572, 0.513277, -3.0,
		0.478974, 0.525864, -3.0,
		0.494860, 0.538133, -3.0,
		0.510031, 0.552151, -3.0,
		0.522127, 0.570143, -3.0,
		0.531124, 0.593065, -3.0,
		0.537629, 0.620809, -3.0,
		0.542465, 0.650303, -3.0,
		0.546798, 0.678259, -3.0,
		0.552959, 0.703513, -3.0,
		0.563121, 0.725745, -3.0,
		0.577656, 0.745911, -3.0,
		0.596563, 0.764858, -3.0,
		0.620160, 0.781738, -3.0,
		0.648302, 0.795385, -3.0,
		0.678670, 0.805057, -3.0,
		0.710336, 0.810741, -3.0,
		0.750111, 0.814914, -3.0,
		0.802994, 0.819945, -3.0,
		0.860771, 0.825435, -3.0,
		0.860771, 0.825435, -3.0,
		0.802994, 0.819945, -3.0,
		0.750111, 0.814914, -3.0,
		0.710336, 0.810741, -3.0,
		0.67867, 0.805057, -3.0,
		0.648302, 0.795385, -3.0,
		0.62016, 0.781738, -3.0,
		0.596563, 0.764858, -3.0,
		0.577656, 0.745911, -3.0,
		0.563121, 0.725745, -3.0,
		0.552959, 0.703513, -3.0,
		0.546798, 0.678259, -3.0,
		0.542465, 0.650303, -3.0,
		0.537629, 0.620809, -3.0,
		0.531124, 0.593065, -3.0,
		0.522127, 0.570143, -3.0,
		0.510031, 0.552151, -3.0,
		0.49486, 0.538133, -3.0,
		0.478974, 0.525864, -3.0,
		0.464572, 0.513277, -3.0,
		0.450885, 0.500213, -3.0,
		0.436138, 0.487056, -3.0,
		0.417496, 0.474456, -3.0,
		0.393395, 0.462743, -3.0,
		0.36842, 0.450713, -3.0,
		0.347274, 0.436734, -3.0,
		0.32899, 0.41902, -3.0,
		0.311832, 0.396628, -3.0,
		0.296661, 0.37217, -3.0,
		0.284631, 0.347804, -3.0,
		0.275462, 0.320338, -3.0,
		0.268678, 0.284286, -3.0,
		0.264438, 0.230348, -3.0,
		0.262769, 0.153238, -3.0,
		0.26253, 0.067542, -3.0,
		0.262371, -0.009704, -3.0,
		0.261258, -0.071544, -3.0,
		0.258238, -0.115777, -3.0,
		0.252355, -0.149133, -3.0,
		0.242529, -0.179247, -3.0,
		0.22717, -0.208406, -3.0,
		0.205134, -0.237216, -3.0,
		0.177564, -0.264881, -3.0,
		0.146433, -0.289891, -3.0,
		0.11473, -0.309272, -3.0,
		0.084934, -0.32099, -3.0,
		0.056475, -0.328224, -3.0,
		0.028237, -0.33417, -3.0,
		0.0, -0.336873, -3.0,
		-0.028237, -0.33417, -3.0,
		-0.056475, -0.328224, -3.0,
		-0.084934, -0.32099, -3.0,
		-0.11473, -0.309272, -3.0,
		-0.146433, -0.289891, -3.0,
		-0.177564, -0.264881, -3.0,
		-0.205134, -0.237216, -3.0,
		-0.22717, -0.208406, -3.0,
		-0.242529, -0.179247, -3.0,
		-0.252355, -0.149133, -3.0,
		-0.258238, -0.115777, -3.0,
		-0.261258, -0.071544, -3.0,
		-0.262371, -0.00671, -3.0,
		-0.26253, 0.091495, -3.0,
		-0.26253, 0.223162, -3.0,
		-0.26253, 0.334639, -3.0,
		-0.26253, 0.377406, -3.0,
		-0.26253, 0.376992, -3.0,
	};

	vector <glm::vec3> uniPoints;

	for (int i = 0; i < 134*3; i+=3)
	{
		glm::vec3 point;
		point.x = vertices[i];
		point.y = vertices[i + 1];
		point.z = 0.0;

		uniPoints.push_back(point);
	}

	return uniPoints;
}
