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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

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

Texture farolaTexture;
Model Farola_M;

Texture charmanderTexture;
Model Charmander_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLightsDos[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
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

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
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
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");


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

	farolaTexture = Texture("Textures/texture_lamp.tga");
	farolaTexture.LoadTextureA();

	Farola_M = Model();
	Farola_M.LoadModel("Models/Farola.fbx");


	charmanderTexture = Texture("Textures/charmander.tga");
	charmanderTexture.LoadTextureA();

	Charmander_M = Model();
	Charmander_M.LoadModel("Models/charmander.dae");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,  // Naranja
		0.5f, 0.9f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.25f, 0.15);
	pointLightCount++;
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,  // Blanco
		0.3f, 0.9f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.3f, 0.2f, 0.1f,
		25.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.05f, 0.1f, 0.01f,
		25.0f);
	spotLightCount++;

	spotLightsDos[0] = spotLights[0];
	spotLightsDos[1] = spotLights[1];
	spotLightsDos[2] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.02f, 0.1f, 0.025f,
		30.0f);
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	// array temporal para reorganizar las luces activas
	PointLight activeLightsArray[MAX_POINT_LIGHTS];
	unsigned int activeLightCount = 0;
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
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		spotLightsDos[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelaux2(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(6.0f, -1.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		farolaTexture.UseTexture();
		Farola_M.RenderModel();
		pointLights[1].SetPos(glm::vec3(model[3].x, model[3].y + 3.2f, model[3].z));

		//Charmander
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -1.0f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		charmanderTexture.UseTexture();
		Charmander_M.RenderModel();
		pointLights[0].SetPos(glm::vec3(model[3].x, model[3].y + 0.4, model[3].z + 1.3f));

		// Lógica para encender/apagar las luces de forma independiente
		activeLightCount = 0;

		// Agregamos las luces que están encendidas al array temporal
		if (mainWindow.getcharmanderencendido()) {
			activeLightsArray[activeLightCount] = pointLights[0];  // Charmander
			activeLightCount++;
		}
		if (mainWindow.getfarolaencendida()) {
			activeLightsArray[activeLightCount] = pointLights[1];  // Farola
			activeLightCount++;
		}

		// Enviamos solo las luces activas al shader
		shaderList[0].SetPointLights(activeLightsArray, activeLightCount);
		//Base Tsuru
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -1.0f, -4.5f + (mainWindow.getarticulacion3() / 50)));
		model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseTexture.UseTexture();
		Tsuru_M.RenderModel();

		if (mainWindow.getavanza()) {
			spotLights[2].SetFlash(
				glm::vec3(model[3].x - 1.5f, model[3].y + 1.55f, model[3].z - 4.1f),
				glm::vec3(0.0f, 0.0f, -1.0f)
			);

			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else if (mainWindow.getretrocede()) {
			spotLightsDos[2].SetFlash(
				glm::vec3(model[3].x - 1.5f, model[3].y + 1.8f, model[3].z + 4.7f),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);
			shaderList[0].SetSpotLights(spotLightsDos, spotLightCount);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}


		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 88.0f, -96.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofreTexture.UseTexture();
		Cofre_M.RenderModel();

		spotLightsDos[1].SetFlash(
			glm::vec3(model[3].x, model[3].y, model[3].z),
			glm::normalize(glm::mat3(model) * glm::vec3(0.0f, 0.0f, -1.0f))
		);
		spotLights[1].SetFlash(
			glm::vec3(model[3].x, model[3].y, model[3].z),
			glm::normalize(glm::mat3(model) * glm::vec3(0.0f, 0.0f, -1.0f))
		);

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