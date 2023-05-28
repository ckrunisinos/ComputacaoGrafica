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

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Prot tipo da fun  o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot tipos das fun  es
int setupShader();
int setupGeometry();

// Dimens es da janela (pode ser alterado em tempo de execu  o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// C digo fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de c digo aqui!
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

//C difo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

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
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens es da viewport com as mesmas dimens es da janela da aplica  o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando um buffer simples, com a geometria de um tri ngulo
	GLuint VAO = setupGeometry();


	glUseProgram(shaderID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	//
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glEnable(GL_DEPTH_TEST);


	// Loop da aplica  o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun  es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1);
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		if (translateX)
		{
			model = glm::translate(model, glm::vec3(translateX, 0.0f, 0.0f));
		}
		if (translateY)
		{
			model = glm::translate(model, glm::vec3(0.0f, translateY, 0.0f));
		}
		if (translateZ)
		{
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, translateZ));
		}

		if (scale)
		{
			model = glm::scale(model, glm::vec3(scale, scale, scale));
		}

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesToDrawCount);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		// glDrawArrays(GL_POINTS, 0, verticesToDrawCount);
		// glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu  o da GLFW, limpando os recursos alocados por ela
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

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		// rotateX = false;
		rotateX = !rotateX;
		// rotateY = false;
		// rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		// rotateX = false;
		rotateY = !rotateY;
		// rotateY = false;
		// rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		// rotateX = false;
		// rotateY = false;
		// rotateZ = true;
		rotateZ = !rotateZ;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		translateY += 0.1f;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		translateY -= 0.1f;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		translateX += 0.1f;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		translateX -= 0.1f;
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		translateZ -= 0.1f;
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		translateZ += 0.1f;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{

		rotateX = false;
		rotateY = false;
		rotateZ = false;

		translateX = 0.0f;
		translateY = 0.0f;
		translateZ = 0.0f;
	}

	if (key == GLFW_KEY_RIGHT_BRACKET)
	{
		scale += 0.1f;
	}

	if (key == GLFW_KEY_LEFT_BRACKET)
	{
		scale -= 0.1f;
	}

}

//Esta fun  o est  basntante hardcoded - objetivo   compilar e "buildar" um programa de
// shader simples e  nico neste exemplo de c digo
// O c digo fonte do vertex e fragment shader est  nos arrays vertexShaderSource e
// fragmentShader source no ini io deste arquivo
// A fun  o retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compila  o (exibi  o via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compila  o (exibi  o via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Esta fun  o est  bastante harcoded - objetivo   criar os buffers que armazenam a 
// geometria de um tri ngulo
// Apenas atributo coordenada nos v rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun  o retorna o identificador do VAO
int setupGeometry()
{
	int verticesCount = 0;
	GLfloat vertices[10000][3] = { };

	int trianglesCount = 0;
	int triangles[10000][3] = { };

	std::ifstream file("cow.obj");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
			std::vector<std::string> lineElements = split(line, ' ');

			if (lineElements.size() > 0) {
				if (lineElements[0].compare("v") == 0) {
					// vertice
					GLfloat x = std::stof(lineElements[1]);
					GLfloat y = std::stof(lineElements[2]);
					GLfloat z = std::stof(lineElements[3]);

					vertices[verticesCount][0] = x;
					vertices[verticesCount][1] = y;
					vertices[verticesCount][2] = z;
					verticesCount++;
				}
				else if (lineElements[0].compare("f") == 0) {
					int vertice1 = std::stoi(lineElements[1]);
					int vertice2 = std::stoi(lineElements[2]);
					int vertice3 = std::stoi(lineElements[3]);

					triangles[trianglesCount][0] = vertice1;
					triangles[trianglesCount][1] = vertice2;
					triangles[trianglesCount][2] = vertice3;
					trianglesCount++;
				}
			}
        }
        file.close();
    } else {
        std::cerr << "Unable to open file";
    }

	GLfloat verticesToDraw[150000] = { };

	int auxCount = 0;
	for (int i = 0; i < trianglesCount; i++)  {
		for (int j = 0; j < 3; j++) {
			int verticeIndex = triangles[i][j] - 1;
			verticesToDraw[auxCount] = vertices[verticeIndex][0];
			auxCount++;
			verticesToDraw[auxCount] = vertices[verticeIndex][1];
			auxCount++;
			verticesToDraw[auxCount] = vertices[verticeIndex][2];
			auxCount++;

			verticesToDraw[auxCount] = 1.0;
			auxCount++;
			verticesToDraw[auxCount] = 0.20;
			auxCount++;
			verticesToDraw[auxCount] = 0.8;
			auxCount++;
		}
	}

	verticesToDrawCount = auxCount;

	GLuint VBO, VAO;

	//Gera  o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesToDraw), verticesToDraw, GL_STATIC_DRAW);

	//Gera  o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza  o no shader * (a localiza  o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est  normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posi  o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);



	// Observe que isso   permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (  uma boa pr tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

