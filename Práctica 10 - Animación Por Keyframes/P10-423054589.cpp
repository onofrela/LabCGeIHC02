/*
Animaci�n por keyframes
La textura del skybox fue conseguida desde la p�gina https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edit� en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
	*/
	//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <fstream>

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

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animaci�n
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;


Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

Model Globo_M;
Texture globoTexture;

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

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//funci�n para teclado de keyframes 
void inputKeyframes(bool* keys);

//c�lculo del promedio de las normales para sombreado de Phong
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


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;


//NEW// Keyframes
float posXglobo = -20.0, posYglobo = 80.0, posZglobo = -60.0;
float	movGlobo_x = 0.0f, movGlobo_y = 0.0f, movGlobo_z = 0.0f;
float giroGlobo = 0;

#define MAX_FRAMES 100 //N�mero de cuadros m�ximos
int i_max_steps = 10; //N�mero de pasos entre cuadros para interpolaci�n, a mayor n�mero , m�s lento ser� el movimiento
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movGlobo_x;		//Variable para PosicionX
	float movGlobo_y;		//Variable para PosicionY
	float movGlobo_z;		//Variable para PosicionZ
	float movGlobo_xInc;		//Variable para IncrementoX
	float movGlobo_yInc;		//Variable para IncrementoY
	float movGlobo_zInc;		//Variable para IncrementoY
	float giroGlobo;		//Variable para GiroGlobo
	float giroGloboInc;		//Variable para IncrementoGiroGlobo
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 1;			//El n�mero de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movGlobo_x = movGlobo_x;
	KeyFrame[FrameIndex].movGlobo_y = movGlobo_y;
	KeyFrame[FrameIndex].movGlobo_z = movGlobo_z;
	KeyFrame[FrameIndex].giroGlobo = giroGlobo;
	//Se agregan nuevas l�neas para guardar m�s variables si es necesario

	//no volatil,se requiere agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0
{

	movGlobo_x = KeyFrame[0].movGlobo_x;
	movGlobo_y = KeyFrame[0].movGlobo_y;
	movGlobo_y = KeyFrame[0].movGlobo_z;
	giroGlobo = KeyFrame[0].giroGlobo;
}

void interpolation(void)
{
	KeyFrame[playIndex].movGlobo_xInc = (KeyFrame[playIndex + 1].movGlobo_x - KeyFrame[playIndex].movGlobo_x) / i_max_steps;
	KeyFrame[playIndex].movGlobo_yInc = (KeyFrame[playIndex + 1].movGlobo_y - KeyFrame[playIndex].movGlobo_y) / i_max_steps;
	KeyFrame[playIndex].movGlobo_zInc = (KeyFrame[playIndex + 1].movGlobo_z - KeyFrame[playIndex].movGlobo_z) / i_max_steps;
	KeyFrame[playIndex].giroGloboInc = (KeyFrame[playIndex + 1].giroGlobo - KeyFrame[playIndex].giroGlobo) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animaci�n entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animaci�n con �ltimo frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolaci�n del pr�ximo cuadro
			{

				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animaci�n
			movGlobo_x += KeyFrame[playIndex].movGlobo_xInc;
			movGlobo_y += KeyFrame[playIndex].movGlobo_yInc;
			movGlobo_z += KeyFrame[playIndex].movGlobo_zInc;
			giroGlobo += KeyFrame[playIndex].giroGloboInc;
			i_curr_steps++;
		}

	}
}

void saveKeyframesToFile(void)
{
	std::ofstream file("keyframes.txt");
	if (file.is_open())
	{
		file << FrameIndex << std::endl;
		for (int i = 0; i < FrameIndex; i++)
		{
			file << KeyFrame[i].movGlobo_x << " "
				<< KeyFrame[i].movGlobo_y << " "
				<< KeyFrame[i].movGlobo_z << " "
				<< KeyFrame[i].giroGlobo << std::endl;
		}
		file.close();
		printf("Keyframes guardados en keyframes.txt\n");
	}
	else
	{
		printf("Error: No se pudo guardar el archivo keyframes.txt\n");
	}
}

void loadKeyframesFromFile(void)
{
	std::ifstream file("keyframes.txt");
	if (file.is_open())
	{
		file >> FrameIndex;
		for (int i = 0; i < FrameIndex; i++)
		{
			file >> KeyFrame[i].movGlobo_x
				>> KeyFrame[i].movGlobo_y
				>> KeyFrame[i].movGlobo_z
				>> KeyFrame[i].giroGlobo;
		}
		file.close();
		printf("Keyframes cargados desde keyframes.txt (%d frames)\n", FrameIndex);
	}
	else
	{
		// Si no existe el archivo, inicializar con un frame en posicion inicial
		printf("Archivo keyframes.txt no encontrado. Iniciando con frame por defecto.\n");
		FrameIndex = 1;
		KeyFrame[0].movGlobo_x = 0.0f;
		KeyFrame[0].movGlobo_y = 0.0f;
		KeyFrame[0].movGlobo_z = 0.0f;
		KeyFrame[0].giroGlobo = 0.0f;
	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");


	globoTexture = Texture("Textures/globo.png");
	globoTexture.LoadTextureA();
	Globo_M = Model();
	Globo_M.LoadModel("Models/globo.dae");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sp2_rt.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_lf.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_dn.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_up.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_bk.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_ft.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
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
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posglobo = glm::vec3(2.0f, 0.0f, 0.0f);

	// Cargar keyframes desde archivo
	loadKeyframesFromFile();

	printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presiona L para guardar frame\n3.-Presiona P para habilitar guardar nuevo frame\n");
	printf("4.-Presiona 1 para disminuir X\n5.-Presiona 2 para habilitar tecla 1\n6.-Presiona 3 para X\n7.-Presiona 4 para habilitar tecla 3\n");
	printf("8.-Presiona 5 para aumentar Y\n9.-Presiona 6 para habilitar tecla 5\n10.-Presiona 7 para disminuir Y\n11.-Presiona 8 para habilitar tecla 7\n");
	printf("12.-Presiona 9 para girar\n13.-Presiona 0 para habilitar tecla 9\n");
	printf("14.-Presiona V para aumentar Z\n15.-Presiona B para habilitar tecla V\n16.-Presiona N para disminuir Z\n17.-Presiona M para habilitar tecla N\n");


	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();


		model = glm::mat4(1.0);
		posglobo = glm::vec3(posXglobo + movGlobo_x, posYglobo + movGlobo_y, posZglobo + movGlobo_z);
		model = glm::translate(model, posglobo);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, giroGlobo * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		globoTexture.UseTexture();
		Globo_M.RenderModel();





		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	// Guardar keyframes antes de salir
	saveKeyframesToFile();

	return 0;
}


void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animaci�n'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animaci�n con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;

		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movGlobo_x es: %f\n", movGlobo_x);
			printf("movGlobo_y es: %f\n", movGlobo_y);
			printf("giroGlobo es: %f\n", giroGlobo);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
			reinicioFrame++;
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_x -= 8.0f;
			printf("\n movGlobo_x es: %f\n", movGlobo_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}

	if (keys[GLFW_KEY_3])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_x += 8.0f;
			printf("\n movGlobo_x es: %f\n", movGlobo_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 4 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_4])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 3\n");
		}
	}

	if (keys[GLFW_KEY_5])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_y += 4.0f;
			printf("\n movGlobo_y es: %f\n", movGlobo_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_6])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 5\n");
		}
	}

	if (keys[GLFW_KEY_7])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_y -= 4.0f;
			printf("\n movGlobo_y es: %f\n", movGlobo_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_8])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 7\n");
		}
	}

	if (keys[GLFW_KEY_9])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			giroGlobo += 45.0f;
			if (giroGlobo > 360.0f) {
				giroGlobo -= 360.0f;
			}
			printf("\n giroGlobo es: %f\n", giroGlobo);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 0 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_0])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 9\n");
		}
	}

	if (keys[GLFW_KEY_V])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_z += 8.0f;
			printf("\n movGlobo_z es: %f\n", movGlobo_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla V para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_B])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla B\n");
		}
	}
	if (keys[GLFW_KEY_N])
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_z -= 8.0f;
			printf("\n movGlobo_z es: %f\n", movGlobo_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla M para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_M])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla N\n");
		}
	}

}
