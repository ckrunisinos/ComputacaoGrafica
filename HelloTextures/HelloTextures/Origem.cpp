/* Hello Triangle - c digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr fico - Jogos Digitais - Unisinos
 * Vers o inicial: 7/4/2017
 *  ltima atualiza  o em 12/05/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stb_image
#include "stb_image.h"

#include "Shader.h"

struct Lighting {
	float q, kd;
	glm::vec3 ka, ks;
};

struct Vertex {
    GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat u, v;
	GLfloat nx, ny, nz;
};

struct TextureCoordinate {
	GLfloat u, v;
};

struct NormalCoordinate {
	GLfloat nx, ny, nz;
};

struct Triangle {
	int vertex1Index, vertex2Index, vertex3Index;
	int texture1Index, texture2Index, texture3Index;
	int normal1Index, normal2Index, normal3Index;
};

struct tuple_hash {
    template <class T1, class T2, class T3>
    std::size_t operator () (const std::tuple<T1,T2,T3> &tuple) const {
        std::size_t seed = 0;
        std::size_t h1 = std::hash<T1>()(std::get<0>(tuple));
        std::size_t h2 = std::hash<T2>()(std::get<1>(tuple));
        std::size_t h3 = std::hash<T3>()(std::get<2>(tuple));
        return h1 ^ h2 ^ h3 + h1 + h2 + h3;
    }
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Prot tipos das fun  es
int setupSprite(Lighting* lighting);
int loadTexture(string path);

void addTriangleInformation(
	Triangle triangle,
	std::vector<Vertex>* vertices,
	std::vector<TextureCoordinate>* textureCoordinates,
	std::vector<NormalCoordinate>* normalCoordinates,
	std::vector<GLfloat>* verticesToDraw,
	std::vector<int>* triangleIndices,
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash>* hashMap
);

void addVertexInformation(
	int vertexIndex,
	int textureIndex,
	int normalIndex,
	std::vector<Vertex>* vertices,
	std::vector<TextureCoordinate>* textureCoordinates,
	std::vector<NormalCoordinate>* normalCoordinates,
	std::vector<GLfloat>* verticesToDraw,
	std::vector<int>* triangleIndices,
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash>* hashMap
);

// Dimens es da janela (pode ser alterado em tempo de execu  o)
const GLuint WIDTH = 1000, HEIGHT = 1000;


bool rotateX = false, rotateY = false, rotateZ = false;
float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
float scale = 1.0f;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05;
float pitch = 0.0, yaw = -90.0;

int verticesToDrawCount = 0;

// Fun  o MAIN
int main()
{
	// Inicializa  o da GLFW
	glfwInit();

	//Muita aten  o aqui: alguns ambientes n o aceitam essas configura  es
	//Voc  deve adaptar para a vers o do OpenGL suportada por sua placa
	//Sugest o: comente essas linhas de c digo para desobrir a vers o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria  o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Caio Rodrigues!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun  o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetCursorPos(window,WIDTH / 2, HEIGHT / 2);

	// GLAD: carrega todos os ponteiros d fun  es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa  es de vers o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	std::cout << "Renderer: " << renderer << endl;
	std::cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens es da viewport com as mesmas dimens es da janela da aplica  o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");
	glUseProgram(shader.ID);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view",value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Compilando e buildando o programa de shader
	// GLuint texID = loadTexture("../textures/mario.png");
	// GLuint texID = loadTexture("../textures/wolf.png");
	// GLuint texID = loadTexture("../../models/Renegade.jpg");
	GLuint texID = loadTexture("../../models/Suzanne.png");

	// Gerando um buffer simples, com a geometria de um tri ngulo
	Lighting lighting;
	GLuint VAO = setupSprite(&lighting);

	shader.setFloat("kd", 0.5);
	shader.setVec3("ka", 0.4, 0.4, 0.4);
	shader.setVec3("ks", 0.5, 0.5, 0.5);
	shader.setFloat("q", 7.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", -2.0, 10.0, 2.0);
	shader.setVec3("lightColor", 1.0, 1.0, 1.0);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		//Atualizando a posição e orientação da câmera
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", glm::value_ptr(view));
		
		//Atualizando o shader com a posição da câmera
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
		// model = glm::rotate(model, glm::radians(0.0), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		shader.setMat4("model", glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesToDrawCount);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun  o de callback de teclado - s  pode ter uma inst ncia (deve ser est tica se
// estiver dentro de uma classe) -   chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// if (key == GLFW_KEY_X && action == GLFW_PRESS)
	// {
	// 	rotateX = true;
	// 	rotateY = false;
	// 	rotateZ = false;
	// }

	// if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	// {
	// 	rotateX = false;
	// 	rotateY = true;
	// 	rotateZ = false;
	// }

	// if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	// {
	// 	rotateX = false;
	// 	rotateY = false;
	// 	rotateZ = true;
	// }

	float cameraSpeed = 0.05;

	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraFront * cameraSpeed;
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (key == GLFW_KEY_D) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	offsetx *= sensitivity;
	offsety *= sensitivity;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}

void buildObjectVertices(string basePath, string fileName, std::vector<GLfloat> *_verticesToDraw, std::vector<int> *_triangleIndices, Lighting *_lighting)
{
	string materialsFileName;
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<NormalCoordinate> normalCoordinates;
	std::vector<TextureCoordinate> textureCoordinates;

	std::ifstream file(basePath + fileName);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::vector<std::string> lineElements = split(line, ' ');
			if (lineElements.size() > 0) {
				if (lineElements[0].compare("mtllib") == 0) {
					materialsFileName = lineElements[1];
				}
				else if (lineElements[0].compare("vn") == 0) {
					if (lineElements.size() > 3) {
						NormalCoordinate normalCoordinate;

						GLfloat nx = std::stof(lineElements[1]);
						GLfloat ny = std::stof(lineElements[2]);
						GLfloat nz = std::stof(lineElements[3]);

						normalCoordinate.nx = nx;
						normalCoordinate.ny = ny;
						normalCoordinate.nz = nz;

						normalCoordinates.push_back(normalCoordinate);
					}
				}
				else if (lineElements[0].compare("vt") == 0) {
					if (lineElements.size() > 2) {
						TextureCoordinate textureCoordinate;

						GLfloat u = std::stof(lineElements[1]);
						GLfloat v = std::stof(lineElements[2]);

						textureCoordinate.u = u;
						textureCoordinate.v = v;

						if (textureCoordinate.u < 0) {
							cout << "Texture coordinate is negative: " << textureCoordinate.u << endl;
							cout << lineElements[1] << endl;
						}

						textureCoordinates.push_back(textureCoordinate);
					}
					else {
						cout << "lineElements size < 2" << endl;
					}
				}
				else if (lineElements[0].compare("v") == 0) {
					Vertex vertice;

					GLfloat x = std::stof(lineElements[1]);
					GLfloat y = std::stof(lineElements[2]);
					GLfloat z = std::stof(lineElements[3]);

					vertice.x = x;
					vertice.y = y;
					vertice.z = z;

					vertice.r = (float) rand()/RAND_MAX;
					vertice.g = (float) rand()/RAND_MAX;
					vertice.b = (float) rand()/RAND_MAX;

					vertices.push_back(vertice);
				}
				else if (lineElements[0].compare("f") == 0) {
					Triangle triangle;

					if (split(lineElements[1], '/').size() > 1) {
						std::vector<std::string> xWithTexture = split(lineElements[1], '/');
						std::vector<std::string> yWithTexture = split(lineElements[2], '/');
						std::vector<std::string> zWithTexture = split(lineElements[3], '/');

						int vertice1 = std::stoi(xWithTexture[0]) - 1;
						int vertice2 = std::stoi(yWithTexture[0]) - 1;
						int vertice3 = std::stoi(zWithTexture[0]) - 1;

						int texture1 = std::stoi(xWithTexture[1]) - 1;
						int texture2 = std::stoi(yWithTexture[1]) - 1;
						int texture3 = std::stoi(zWithTexture[1]) - 1;

						int normal1 = std::stoi(xWithTexture[2]) - 1;
						int normal2 = std::stoi(yWithTexture[2]) - 1;
						int normal3 = std::stoi(zWithTexture[2]) - 1;

						triangle.vertex1Index = vertice1;
						triangle.vertex2Index = vertice2;
						triangle.vertex3Index = vertice3;

						triangle.texture1Index = texture1;
						triangle.texture2Index = texture2;
						triangle.texture3Index = texture3;

						triangle.normal1Index = normal1;
						triangle.normal2Index = normal2;
						triangle.normal3Index = normal3;
					}
					else {
						cout << "lineElements size < 2" << endl;
						int vertice1 = std::stoi(lineElements[1]) - 1;
						int vertice2 = std::stoi(lineElements[2]) - 1;
						int vertice3 = std::stoi(lineElements[3]) - 1;

						triangle.vertex1Index = vertice1;
						triangle.vertex2Index = vertice2;
						triangle.vertex3Index = vertice3;

						triangle.texture1Index = -1;
						triangle.texture2Index = -1;
						triangle.texture3Index = -1;
					}

					triangles.push_back(triangle);
				}
			}
		}
		file.close();
	}
	else {
		std::cerr << "Unable to open file";
		exit(0);
	}

	cout << "materialsFileName: " << materialsFileName << endl;
	if (materialsFileName.length() > 0) {
		Lighting lighting;

		string materialsFilePath = basePath + materialsFileName;
		cout << "materialsFilePath: " << materialsFilePath << endl;
		std::ifstream file(basePath + materialsFileName);
		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				std::vector<std::string> lineElements = split(line, ' ');
				if (lineElements.size() > 0) {
					if (lineElements[0].compare("Ns") == 0) {
						lighting.q = std::stof(lineElements[1]);
					}
					else if (lineElements[0].compare("Ka") == 0) {
						lighting.ka = glm::vec3(std::stof(lineElements[1]), std::stof(lineElements[2]), std::stof(lineElements[3]));
					}
					else if (lineElements[0].compare("Ks") == 0) {
						lighting.ks = glm::vec3(std::stof(lineElements[1]), std::stof(lineElements[2]), std::stof(lineElements[3]));
					}
				}
			}
			file.close();
			*_lighting = lighting;
		}
		else {
			std::cerr << "Unable to open materials file";
			exit(0);
		}
	}

	std::cout << "Arquivo OBJ lido com sucesso" << endl;

	std::vector<int> triangleIndices;
	std::vector<GLfloat> verticesToDraw;
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash> hashMap;
	for (int i = 0; i < int(triangles.size()); i++) {
		// Triangles
		Triangle triangle = triangles[i];

		addTriangleInformation(
			triangle,
			&vertices,
			&textureCoordinates,
			&normalCoordinates,
			&verticesToDraw,
			&triangleIndices,
			&hashMap
		);
	}

	*_verticesToDraw = verticesToDraw;
	*_triangleIndices = triangleIndices;
}

void addTriangleInformation(
	Triangle triangle,
	std::vector<Vertex>* vertices,
	std::vector<TextureCoordinate>* textureCoordinates,
	std::vector<NormalCoordinate>* normalCoordinates,
	std::vector<GLfloat>* verticesToDraw,
	std::vector<int>* triangleIndices,
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash>* hashMap
) {
	addVertexInformation(
		triangle.vertex1Index,
		triangle.texture1Index,
		triangle.normal1Index,
		vertices,
		textureCoordinates,
		normalCoordinates,
		verticesToDraw,
		triangleIndices,
		hashMap
	);

	addVertexInformation(
		triangle.vertex2Index,
		triangle.texture2Index,
		triangle.normal2Index,
		vertices,
		textureCoordinates,
		normalCoordinates,
		verticesToDraw,
		triangleIndices,
		hashMap
	);

	addVertexInformation(
		triangle.vertex3Index,
		triangle.texture3Index,
		triangle.normal3Index,
		vertices,
		textureCoordinates,
		normalCoordinates,
		verticesToDraw,
		triangleIndices,
		hashMap
	);
}

void addVertexInformation(
	int vertexIndex,
	int textureIndex,
	int normalIndex,
	std::vector<Vertex>* vertices,
	std::vector<TextureCoordinate>* textureCoordinates,
	std::vector<NormalCoordinate>* normalCoordinates,
	std::vector<GLfloat>* verticesToDraw,
	std::vector<int>* triangleIndices,
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash>* hashMapPtr
) {
	// std::unordered_map<std::tuple<int, int, int>, int, tuple_hash> hashMap;
	// hashMapPtr = &hashMap;

	// auto it = hashMap.find(std::make_tuple(vertexIndex+1, textureIndex+1, normalIndex+1));
	// if (it != hashMap.end()) {
	// 	cout << "Reusing vertex: " << it->second << endl;
	// 	triangleIndices->push_back(it->second);
	// }
	// else {
	Vertex vertex = vertices->at(vertexIndex);

	verticesToDraw->push_back(vertex.x);
	verticesToDraw->push_back(vertex.y);
	verticesToDraw->push_back(vertex.z);

	verticesToDraw->push_back(vertex.r);
	verticesToDraw->push_back(vertex.g);
	verticesToDraw->push_back(vertex.b);

	if (textureIndex < 0) {
		float u = (float) rand()/RAND_MAX;
		float v = (float) rand()/RAND_MAX;
		verticesToDraw->push_back(u);
		verticesToDraw->push_back(v);
	}
	else {
		TextureCoordinate textureCoordinate = textureCoordinates->at(textureIndex);

		verticesToDraw->push_back(textureCoordinate.u);
		verticesToDraw->push_back(textureCoordinate.v);
	}

	if (normalIndex < 0) {
		float nx = (float) rand()/RAND_MAX;
		float ny = (float) rand()/RAND_MAX;
		float nz = (float) rand()/RAND_MAX;
		verticesToDraw->push_back(nx);
		verticesToDraw->push_back(ny);
		verticesToDraw->push_back(nz);
	}
	else {
		NormalCoordinate normalCoordinate = normalCoordinates->at(normalIndex);

		verticesToDraw->push_back(normalCoordinate.nx);
		verticesToDraw->push_back(normalCoordinate.ny);
		verticesToDraw->push_back(normalCoordinate.nz);
	}

	int verticeToDrawIndex = (verticesToDraw->size() / 11) - 1;
	triangleIndices->push_back(verticeToDrawIndex);

	// hashMap[std::make_tuple(vertexIndex+1, textureIndex+1, normalIndex+1)] = verticeToDrawIndex;
	// }
}

int setupSprite(Lighting* lighting)
{
	std::vector<GLfloat> verticesToDraw;
	std::vector<int> triangleIndices;

// "../../models/wolf.obj"
	// buildObjectVertices("../../models/cow.obj", &verticesToDraw, &triangleIndices);
	// buildObjectVertices("../../models/wolf.obj", &verticesToDraw, &triangleIndices);
	// buildObjectVertices("../../models/", "Renegade.obj", &verticesToDraw, &triangleIndices, lighting);
	buildObjectVertices("../../3D_Models/Suzanne/", "SuzanneTriTextured.obj", &verticesToDraw, &triangleIndices, lighting);
	// buildObjectVertices("../../3D_Models/Suzanne/suzanneTriLowPoly.obj", &verticesToDraw, &triangleIndices);
	verticesToDrawCount = triangleIndices.size();

	cout << "CONSTRUIMOS O ARRAY" << endl;

	cout << "verticesToDraw.size(): " << verticesToDraw.size() << endl;
	cout << "triangleIndices.size(): " << triangleIndices.size() << endl;
	cout << "verticesToDrawCount: " << verticesToDrawCount << endl;

	std::cout << "Array de vértices montado com sucesso" << endl;

	GLuint VAO;
	GLuint VBO, EBO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, verticesToDraw.size() * sizeof(float), verticesToDraw.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo normal do vértice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);


	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

int loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na mem�ria 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os par�metros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

