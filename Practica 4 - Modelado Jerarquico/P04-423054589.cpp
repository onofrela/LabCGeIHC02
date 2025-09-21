/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(10, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 modelaux2(1.0);//Inicializar matriz de Modelo 4x4 segundo auxiliar para la jerarquía
	glm::mat4 modelaux3(1.0);//Inicializar matriz de Modelo 4x4 tercer auxiliar para la jerarquía

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//Sección Grua//

		/*

		//CREANDO LA CABINA
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, -4.0f));
		modelaux = model;
		modelaux2 = model;
		model = glm::scale(model, glm::vec3(5.0f, 3.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		//ARTICULACION1 CABINA-BRAZO
		//model = glm::mat4(1.0);//NO EXISTE, SUSTITUIRLA POR:
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		// Creando el brazo de una grúa
		//articulacion1 hasta articulación5 sólo son puntos de rotación o articulación, en este caso no dibujaremos esferas que los representen

		//primer brazo que conecta con la cabina
		//para reiniciar la matriz de modelo con valor de la matriz identidad
		//model = glm::mat4(1.0);
		//rotación alrededor de la articulación que une con la cabina
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		//Traslación inicial para posicionar en -Z a los objetos
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		//otras transformaciones para el objeto
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular
		//meshList[3]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro, cono, pirámide base cuadrangular
		//sp.render(); //dibuja esfera

		//SEGUNDA ARTICULACION 
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		//segundo brazo

		//para reiniciar la matriz de modelo con valor de la matriz identidad
		//Comentar y se modifica para agregar la jerarquia:
		//usar una matriz temporal o auxiliar

		//model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));

		//Traslación inicial para posicionar en -Z a los objetos
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		//otras transformaciones para el objeto
		//model = glm::translate(model, glm::vec3(-0.30f, 5.6f, 0.0f));
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		//TERCER ARTICULACION
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		//Tercer Brazo
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		//Cuarta ARTICULACION
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera
		//Canasta
		model = glm::translate(model, glm::vec3(0.0f, -1.25f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		//Base

		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		modelaux = model;
		modelaux2 = model;
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 2.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[4]->RenderMesh(); //dibuja cubo y pirámide triangular

		color = glm::vec3(0.05f, 0.05f, 0.05f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos

		//Articulación Llanta
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.5f, -1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionV()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry(); 


		//Articulación Llanta
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.5f, -1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionB()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();


		//Articulación Llanta
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-2.5f, -1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionN()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();


		//Articulación Llanta
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-2.5f, -1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionM()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();


		//Articulación Llanta
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.5f, -1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionV()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		sp.render(); //dibuja esfera

		model = modelaux;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMeshGeometry();
		
		*/

		// Zorro Robot //
		// ================================================================================================ //


		// Cuerpo (Base)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0000f, 0.0000f, -3.0000f));
		modelaux = model;
		modelaux2 = model;
		modelaux3 = model;
		model = glm::scale(model, glm::vec3(3.4512f, 1.4752f, 1.2281f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Cola

		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.7256f, 0.7376f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Cola
		model = modelaux;
		//model = glm::translate(model, glm::vec3(-2.2517f, 0.0459f, 0.0000f));
		model = glm::translate(model, glm::vec3(-0.7884f, -0.3899f, 0.0000f));
		model = glm::rotate(model, 0.5157f, glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.5768f, 0.7798f, 0.5142f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Punta de la Cola

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.7884f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Punta de la Cola
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.4953, -0.84035f, 0.0f));
		model = glm::rotate(model, -0.5822f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -0.4100f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 2.2397f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8906f, 1.6807f, 0.7202f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[4]->RenderMesh();

		// Articulación del cuello

		model = modelaux2;
		modelaux = modelaux2;
		model = glm::translate(model, glm::vec3(1.9889f, 0.2652f, 0.0000f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		modelaux2 = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Cuello
		model = modelaux;
		model = glm::rotate(model, -0.0000f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.0000f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.5467f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3981f, 0.8267f, 1.2281f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Cabeza
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0198f, 0.8076f, 0.0000f));
		model = glm::rotate(model, -0.0000f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.0000f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.5467f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(-1.2594f, 1.3321f, 1.5790f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Ojos
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.6858f, 1.0853f, 0.0000f));
		model = glm::rotate(model, -0.0000f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.0000f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.5467f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(-0.3946f, 0.5046f, 1.4269f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Nariz
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.8011f, 0.5822f, 0.0000f));
		model = glm::rotate(model, -0.0000f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.0000f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.5467f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2607f, 0.4458f, 0.3707f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Hocico superior
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.1817f, 0.5822f, 0.0000f));
		model = glm::rotate(model, -0.0000f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.0000f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.5467f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4192f, 1.3883f, 0.7175f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Oreja derecha

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1118f, 0.9942f, -0.50f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Oreja Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8171f, 2.2385f, 1.3188f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMesh();

		// Articulación de la Oreja Izquierda

		modelaux = modelaux2;
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1118f, 0.9942f, 0.50f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Oreja Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8171f, 2.2385f, 1.3188f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMesh();

		// Articulación de la mandíbula

		modelaux = modelaux2;
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.8366f, 0.150f, 0.0000f));
		model = glm::rotate(model, glm::radians(-mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Hocico inferior
		model = modelaux;
		model = glm::rotate(model, -0.0000f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 0.0000f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 1.5467f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4191f, 0.9678f, 0.4012f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna delantera superior derecha

		model = modelaux3;
		model = glm::translate(model, glm::vec3(1.0415f, -0.6536f, 0.3586f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionO()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna delantera superior derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.25f, -0.5f, 0.0f));
		model = glm::rotate(model, -0.3071f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna delantera inferior derecha

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.3425f, -1.1556f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionP()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna delantera inferior derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.25f, -0.5f, 0.0f));
		model = glm::rotate(model, 0.2425f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Pata delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.85f, -0.8755f, 0.0f));
		model = glm::rotate(model, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 0.6781f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna delantera superior izquierda

		model = modelaux3;
		model = glm::translate(model, glm::vec3(1.0415f, -0.6536f, -0.3586f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionU()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna delantera superior izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.25f, -0.5f, 0.0f));
		model = glm::rotate(model, -0.3071f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna delantera inferior izquierda

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.3425f, -1.1556f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionI()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna delantera inferior izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.25f, -0.5f, 0.0f));
		model = glm::rotate(model, 0.2425f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Pata delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.85f, -0.8755f, 0.0f));
		model = glm::rotate(model, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 0.6781f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna trasera superior derecha
		model = modelaux3;
		model = glm::translate(model, glm::vec3(-1.3225f, -0.6536f, 0.3586f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionN()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna trasera superior derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.25f, -0.5f, 0.0f));
		model = glm::rotate(model, -0.9649f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna trasera inferior derecha

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.4425f, -0.8556f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionM()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna trasera inferior derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, -0.5f, 0.0f));
		model = glm::rotate(model, 0.2425f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Bota talón derecho
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.3240f, -1.1839f, 0.0));
		model = glm::rotate(model, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4855f, 0.9612f, 0.6987f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Bota punta derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9777f, -1.2744f, 0.0));
		model = glm::rotate(model, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2516f, 0.9612f, 0.6987f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna trasera superior izquierda
		model = modelaux3;
		model = glm::translate(model, glm::vec3(-1.3225f, -0.6536f, -0.3586f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionV()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna trasera superior izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.25f, -0.5f, 0.0f));
		model = glm::rotate(model, -0.9649f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Articulación de la Pierna trasera inferior izquierda

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.4425f, -0.8556f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionB()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Pierna trasera inferior izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.05f, -0.5f, 0.0f));
		model = glm::rotate(model, 0.2425f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3425f, 1.1556f, 0.4929f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.9921f, 0.8117f, 0.2235f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Bota talón izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.3240f, -1.1839f, 0.0));
		model = glm::rotate(model, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4855f, 0.9612f, 0.6987f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// Bota punta izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.9777f, -1.2744f, 0.0));
		model = glm::rotate(model, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2516f, 0.9612f, 0.6987f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();





		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
