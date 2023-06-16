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

struct Vertex {
    GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat u, v;
};

struct TextureCoordinate {
	GLfloat u, v;
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

// Prot tipo da fun  o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot tipos das fun  es
int setupSprite();
int loadTexture(string path);

void addTriangleInformation(
	Triangle triangle,
	std::vector<Vertex>* vertices,
	std::vector<TextureCoordinate>* textureCoordinates,
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
	std::vector<GLfloat>* verticesToDraw,
	std::vector<int>* triangleIndices,
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash>* hashMap
);

// Dimens es da janela (pode ser alterado em tempo de execu  o)
const GLuint WIDTH = 1000, HEIGHT = 1000;


bool rotateX = false, rotateY = false, rotateZ = false;
float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
float scale = 1.0f;

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

	Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

	// Definindo as dimens es da viewport com as mesmas dimens es da janela da aplica  o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	// GLuint texID = loadTexture("../textures/mario.png");
	// GLuint texID = loadTexture("../textures/wolf.png");
	// GLuint texID = loadTexture("../../models/Renegade.jpg");
	GLuint texID = loadTexture("../../models/Suzanne.png");

	// Gerando um buffer simples, com a geometria de um tri ngulo
	GLuint VAO = setupSprite();

	// Ativando o shader
	glUseProgram(shader.ID);

	// Associando com o shader o buffer de textura que conectaremos
	// antes de desenhar com o bindTexture
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	glDisable(GL_CULL_FACE);

	//Criando a matriz de projeção usando a GLM
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(000.0, 1000.0, 000.0, 1000.0, 100.0, 1000.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Criando a matriz de modelo usando a GLM
		glm::mat4 model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(400.0, 400.0, -500.0));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(200.0, 200.0, 200.0));
		// model = glm::scale(model, glm::vec3(70.0, 70.0, 70.0));
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		//Ativando o primeiro buffer de textura (0) e conectando ao identificador gerado
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		// Observe que como  usamos EBO, agora usamos a glDrawElements!
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, verticesToDrawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); //unbind - desconecta
		glBindTexture(GL_TEXTURE_2D, 0); //unbind da textura
		// Troca os buffers da tela
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

void buildObjectVertices(string path, std::vector<GLfloat> *_verticesToDraw, std::vector<int> *_triangleIndices)
{
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<TextureCoordinate> textureCoordinates;

	std::ifstream file(path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::vector<std::string> lineElements = split(line, ' ');
			if (lineElements.size() > 0) {
				if (lineElements[0].compare("vt") == 0) {
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

						triangle.vertex1Index = vertice1;
						triangle.vertex2Index = vertice2;
						triangle.vertex3Index = vertice3;

						triangle.texture1Index = texture1;
						triangle.texture2Index = texture2;
						triangle.texture3Index = texture3;
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
	std::vector<GLfloat>* verticesToDraw,
	std::vector<int>* triangleIndices,
	std::unordered_map<std::tuple<int, int, int>, int, tuple_hash>* hashMap
) {
	addVertexInformation(
		triangle.vertex1Index,
		triangle.texture1Index,
		// triangle.normal1Index,
		10,
		vertices,
		textureCoordinates,
		verticesToDraw,
		triangleIndices,
		hashMap
	);

	addVertexInformation(
		triangle.vertex2Index,
		triangle.texture2Index,
		// triangle.normal2Index,
		10,
		vertices,
		textureCoordinates,
		verticesToDraw,
		triangleIndices,
		hashMap
	);

	addVertexInformation(
		triangle.vertex3Index,
		triangle.texture3Index,
		// triangle->normal3Index,
		10,
		vertices,
		textureCoordinates,
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

		// if (textureCoordinate.u < 0) {
		// 	cout << "Texture coordinate is negative: " << textureCoordinate.u << endl;
		// }

		verticesToDraw->push_back(textureCoordinate.u);
		verticesToDraw->push_back(textureCoordinate.v);
	}

	int verticeToDrawIndex = (verticesToDraw->size() / 8) - 1;
	triangleIndices->push_back(verticeToDrawIndex);

	// hashMap[std::make_tuple(vertexIndex+1, textureIndex+1, normalIndex+1)] = verticeToDrawIndex;
	// }
}

int setupSprite()
{
	std::vector<GLfloat> verticesToDraw;
	std::vector<int> triangleIndices;

// "../../models/wolf.obj"
	// buildObjectVertices("../../models/cow.obj", &verticesToDraw, &triangleIndices);
	// buildObjectVertices("../../models/wolf.obj", &verticesToDraw, &triangleIndices);
	// buildObjectVertices("../../models/Renegade.obj", &verticesToDraw, &triangleIndices);
	buildObjectVertices("../../3D_Models/Suzanne/SuzanneTriTextured.obj", &verticesToDraw, &triangleIndices);
	verticesToDrawCount = triangleIndices.size();

	cout << "CONSTRUIMOS O ARRAY" << endl;

	cout << "verticesToDraw.size(): " << verticesToDraw.size() << endl;
	cout << "triangleIndices.size(): " << triangleIndices.size() << endl;
	cout << "verticesToDrawCount: " << verticesToDrawCount << endl;

	std::cout << "Array de vértices montado com sucesso" << endl;

	GLuint VAO;
	GLuint VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesToDraw.size() * sizeof(float), verticesToDraw.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleIndices.size() * sizeof(int), triangleIndices.data(), GL_STATIC_DRAW);

	// Primeiro atributo - Layout 0 - posição - 3 valores - x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Segundo atributo - Layout 1 - cor - 3 valores - r, g, b
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Terceiro atributo - Layout 2 - coordenadas de textura - 2 valores - s, t (ou u, v)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); //desvincula

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

