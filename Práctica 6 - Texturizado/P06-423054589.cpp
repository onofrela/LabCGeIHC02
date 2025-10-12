/*
Práctica 6: Texturizado
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;

Texture baseTexture;
Texture cofreTexture;
Texture llantaDDTexture;
Texture llantaDITexture;
Texture llantaTDTexture;
Texture llantaTITexture;

Model Tsuru_M;
Model Cofre_M;
Model Llanta_DD_M;
Model Llanta_DI_M;
Model Llanta_TD_M;
Model Llanta_TI_M;
Model Dado_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}



void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);


	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
{
	// Coordenadas de textura y vértices del dado (octaedro modificado)
	GLfloat octaedro_vertices[] = {
		// Cara 1 (Frente Superior)
		 0.0f,  0.707f,  0.0f,    2 / 4.0f - 0.01f, 3 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f, // F
		 0.5f,  0.0f,  0.5f,      3 / 4.0f, 2 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // D
		-0.5f,  0.0f,  0.5f,      1 / 4.0f + 0.01f, 2 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // I

		// Cara 2 (Izquierda Inferior)
		0.0f, -0.707f, 0.0f,     0 / 4.0f + 0.02f, 1 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // F
		-0.5f,  0.0f,  0.5f,      2 / 4.0f - 0.01f, 1 / 4.0f + 0.01f ,   0.0f, 0.0f, 0.0f, // D
		-0.5f,  0.0f,  -0.5f,      1 / 4.0f, 2 / 4.0f,   0.0f, 0.0f, 0.0f, // I

		 // Cara 3 (Izquierda Superior)
		 -0.5f,  0.0f, -0.5f,      1 / 4.0f, 2 / 4.0f,   0.0f, 0.0f, 0.0f, // D
		 0.0f,  0.707f, 0.0f,     0 / 4.0f, 3 / 4.0f,   0.0f, 0.0f, 0.0f, // F
		 -0.5f,  0.0f, 0.5f,      2 / 4.0f, 3 / 4.0f,   0.0f, 0.0f, 0.0f, // I

		  // Cara 4 (Frente Inferior)
		  0.0f, -0.707f, 0.0f,     2 / 4.0f, 1 / 4.0f,   0.0f, 0.0f, 0.0f, // F
		  0.5f,  0.0f,  0.5f,      3 / 4.0f, 2 / 4.0f,   0.0f, 0.0f, 0.0f, // D
		  -0.5f,  0.0f,  0.5f,      1 / 4.0f, 2 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // I

		  // Cara 5 (Atrás Superior)
		  0.0f,  0.707f, 0.0f,     3 / 4.0f, 4 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f, // F
		  0.5f,  0.0f, -0.5f,      2 / 4.0f, 3 / 4.0f ,   0.0f, 0.0f, 0.0f, // D
		  -0.5f,  0.0f, -0.5f,      4 / 4.0f, 3 / 4.0f,   0.0f, 0.0f, 0.0f, // I

		  // Cara 6 (Derecha Inferior)
		  0.0f, -0.707f, 0.0f,     4 / 4.0f - 0.01f, 1 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // F
		  0.5f,  0.0f,  0.5f,      2 / 4.0f + 0.01f, 1 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // I
		  0.5f,  0.0f, -0.5f,      3 / 4.0f, 2 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f, // D

		  // Cara 7 (Derecha Superior)
			0.0f,  0.707f, 0.0f,     4 / 4.0f - 0.01f, 3 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f, // F
			0.5f,  0.0f, -0.5f,      3 / 4.0f, 2 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // D
			0.5f,  0.0f,  0.5f,      2 / 4.0f + 0.01f, 3 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f, // I

			// Cara 8 (Atrás Inferior)
			 0.0f, -0.707f, 0.0f,     3 / 4.0f, 0 / 4.0f + 0.01f,   0.0f, 0.0f, 0.0f, // F
			 0.5f,  0.0f, -0.5f,      2 / 4.0f + 0.02f, 1 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f, // D
			-0.5f,  0.0f, -0.5f,      4 / 4.0f - 0.02f, 1 / 4.0f - 0.01f,   0.0f, 0.0f, 0.0f  // I
	};


	// --- Índices (cada cara usa los 3 vértices consecutivos) ---
	unsigned int indices[] = {
		0, 1, 2,     // Cara 1 - Frente Superior
		3, 4, 5,     // Cara 2 - Izquierda Inferior
		6, 7, 8,     // Cara 3 - Izquierda Superior
		9, 10, 11,   // Cara 4 - Frente Inferior
		12, 13, 14,  // Cara 5 - Atrás Superior
		15, 16, 17,  // Cara 6 - Derecha Inferior
		18, 19, 20,  // Cara 7 - Derecha Superior
		21, 22, 23   // Cara 8 - Atrás Inferior
	};


	calcAverageNormals(indices, 24, octaedro_vertices, 192, 8, 5);

	Mesh* dado = new Mesh();
	dado->CreateMesh(octaedro_vertices, indices, 192, 24);
	meshList.push_back(dado);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/octaedro.tga");
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();

	baseTexture = Texture("Textures/carro.tga");
	baseTexture.LoadTextureA();
	cofreTexture = Texture("Textures/carro.tga");
	cofreTexture.LoadTextureA();
	llantaDDTexture = Texture("Textures/carro.tga");
	llantaDDTexture.LoadTextureA();
	llantaDITexture = Texture("Textures/carro.tga");
	llantaDITexture.LoadTextureA();
	llantaTDTexture = Texture("Textures/carro.tga");
	llantaTDTexture.LoadTextureA();
	llantaTITexture = Texture("Textures/carro.tga");
	llantaTITexture.LoadTextureA();
	
	Tsuru_M = Model();
	Tsuru_M.LoadModel("Models/Tsuru.dae");

	Cofre_M = Model();
	Cofre_M.LoadModel("Models/Cofre.dae");

	Llanta_DD_M = Model();
	Llanta_DD_M.LoadModel("Models/Llanta_DD.dae");

	Llanta_DI_M = Model();
	Llanta_DI_M.LoadModel("Models/Llanta_DI.dae");

	Llanta_TD_M = Model();
	Llanta_TD_M.LoadModel("Models/Llanta_TD.dae");

	Llanta_TI_M = Model();
	Llanta_TI_M.LoadModel("Models/Llanta_TI.dae");
	
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();


		//Dado de Opengl
		//Ejercicio 1: Texturizar su cubo con la imagen dado_animales ya optimizada por ustedes
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();
		
		//Ejercicio 2:Importar el cubo texturizado en el programa de modelado con 
		//la imagen dado_animales ya optimizada por ustedes
		/*
		//Dado importado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 3.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado_M.RenderModel();
		*/

		
		
		/*Reporte de práctica :
		Ejercicio 1: Crear un dado de 8 caras y texturizarlo por medio de código
		Ejercicio 2: Importar el modelo de su coche con sus 4 llantas acomodadas
		y tener texturizadas las 4 llantas (diferenciar caucho y rin)  y 
		texturizar el logo de la Facultad de ingeniería en el cofre de su propio modelo de coche
	
		*/
		//Instancia del coche 

		//Base Tsuru
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -2.0f, -4.5f + (mainWindow.getarticulacion3() / 50)));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseTexture.UseTexture();
		Tsuru_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 88.0f, -96.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofreTexture.UseTexture();
		Cofre_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-80.0f, 32.0f, -128.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1() + mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDITexture.UseTexture();
		Llanta_DI_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(80.0f, 32.0f, -128.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1() + mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDDTexture.UseTexture();
		Llanta_DD_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-80.0f, 32.0f, 112.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1() + mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTITexture.UseTexture();
		Llanta_TI_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(80.0f, 32.0f, 112.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1() + mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTDTexture.UseTexture();
		Llanta_TD_M.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/