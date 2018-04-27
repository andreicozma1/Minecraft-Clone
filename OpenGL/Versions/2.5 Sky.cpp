#include <iostream>
#include <algorithm>
#include <iterator>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <cstdlib>
#include <math.h>
#include <stdlib.h>
#include <vector> 
#include <iostream>
#include <fstream>
#include <thread>
#include <FastNoise.h>
#include <algorithm>
#include <chrono>
#include <time.h>  

using namespace std;

const int elementsInBox = 6 * 6 * 6;

float positions[elementsInBox] = {
	-0.5f, -0.5f, -0.5f, 0.341f, 0.231f, 0.047f,
	0.5f, -0.5f, -0.5f,0.341f, 0.231f, 0.047f,
	0.5f,  0.5f, -0.5f,  0.341f, 0.231f, 0.047f, // BACK
	0.5f,  0.5f, -0.5f,0.341f, 0.231f, 0.047f,
	-0.5f,  0.5f, -0.5f,0.341f, 0.231f, 0.047f,
	-0.5f, -0.5f, -0.5f,0.341f, 0.231f, 0.047f,

	-0.5f, -0.5f,  0.5f,  0.341f,0.231f, 0.047f,
	0.5f, -0.5f,  0.5f, 0.341f,0.231f, 0.047f,
	0.5f,  0.5f,  0.5f,   0.341f,0.231f, 0.047f, // FRONT
	0.5f,  0.5f,  0.5f, 0.341f,0.231f, 0.047f,
	-0.5f,  0.5f,  0.5f, 0.341f,0.231f, 0.047f,
	-0.5f, -0.5f,  0.5f, 0.341f,0.231f, 0.047f,

	-0.5f,  0.5f,  0.5f, 0.341f, 0.231f,0.047f,
	-0.5f,  0.5f, -0.5f,0.341f, 0.231f,0.047f,
	-0.5f, -0.5f, -0.5f,0.341f, 0.231f,0.047f, //LEFT
	-0.5f, -0.5f, -0.5f,0.341f, 0.231f,0.047f,
	-0.5f, -0.5f,  0.5f,0.341f, 0.231f,0.047f,
	-0.5f,  0.5f,  0.5f,0.341f, 0.231f,0.047f,

	0.5f,  0.5f,  0.5f,0.341f, 0.231f,0.047f,
	0.5f,  0.5f, -0.5f,0.341f, 0.231f,0.047f,
	0.5f, -0.5f, -0.5f,0.341f, 0.231f,0.047f,  // RIGHT
	0.5f, -0.5f, -0.5f,0.341f, 0.231f,0.047f,
	0.5f, -0.5f,  0.5f,0.341f, 0.231f,0.047f,
	0.5f,  0.5f,  0.5f,0.341f, 0.231f,0.047f,

	-0.5f, -0.5f, -0.5f,  0.341f,0.231f, 0.047f,
	0.5f, -0.5f, -0.5f,	  0.341f,0.231f, 0.047f,
	0.5f, -0.5f,  0.5f,	  0.341f,0.231f, 0.047f,	// BOTTOM
	0.5f, -0.5f,  0.5f,	  0.341f,0.231f, 0.047f,
	-0.5f, -0.5f,  0.5f,  0.341f,0.231f, 0.047f,
	-0.5f, -0.5f, -0.5f,  0.341f,0.231f, 0.047f,

	-0.5f,  0.5f, -0.5f,0.192f, 0.388f, 0.0f,
	0.5f,  0.5f, -0.5f,	0.192f, 0.388f, 0.0f,
	0.5f,  0.5f,  0.5f,	0.192f, 0.388f, 0.0f,	// TOP
	0.5f,  0.5f,  0.5f,	0.192f, 0.388f, 0.0f,
	-0.5f,  0.5f,  0.5f,0.192f, 0.388f, 0.0f,
	-0.5f,  0.5f, -0.5f,0.192f, 0.388f, 0.0f
};

float skybox[elementsInBox];
float sun[elementsInBox];
float moon[elementsInBox];

float *elements = new float[1000];

glm::mat4 projectionTransform(1.0f);

FastNoise noiseGenerator;

int vertexCt = 0;
int verticesInCube = 6 * 6;

unsigned int shader;

bool shouldGenerateChunks = true;
bool regenerateChunks = true;

struct {
	float width = 1200;
	float height = 800;
	float fov = 45.0;
	bool hasFocus = false;
	GLFWwindow* instance;
} window;

struct {
	struct {
		double x;
		double y;
	} mouse;
	struct {
		bool isKeyPressed[GLFW_KEY_MENU];
	} keyboard;

} input;


struct {
	struct {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} position;
	struct {
		float x = 0;
		float y = 0;
		float z = 0;
	} looking;
	struct {
		int health = 5;
		int hunger = 0;
	} vitals;
	float height = 1.2f;
	float forwardSpeed = 0.15f;
	float runSpeedMultiplier = 1.5;
	float strafeSpeed = 0.1f;
	float flySpeed = .5f;
	bool isJumping = false;
	float fallSpeed = -0.25f;
} player;

struct {
	struct {
		struct {
			int x;
			int z;
		} current;
		int size = 17;
		int totalCount = 0;
		static const int toGenerate = 5;
		static const int toRender = 3;
		int rendered = 0;
	} chunks;
	struct {
		int time = 0;
	} stats;
} world;

bool sunOrMoon = true;

bool flying = true;
float jumpCounter = 0;
float jumpHeight = player.height * 2;
float jumpSpeed = 6;
int jumpBlockX = 0;
int jumpBlockZ = 0;

const int chunksToRenderAcross = world.chunks.toRender * 2 + 1;
const int chunksToGenerateAcross = world.chunks.toGenerate * 2 + 1;

int ***chunks = new int**[1000000];
int **skyData = new int*[1000000];
float **chunkMeshes = new float*[1000000];

__int64 timeCurrent;
__int64 timeWhenStarted;
float frameTime;
float frameTimeMultiplier;

float cameraRotX = 0, cameraRotY = 0;
float cameraSpeed = 0.5f;

int *chunkX = new int[10000];
int *chunkZ = new int[10000];
intptr_t *chunkAddresses = new intptr_t[10000];

ofstream myFile;

int dirX;
int dirZ;

#define BIOME_PLAINS 0
#define BIOME_HILLS 1
#define BIOME_MOUNTAINS 2
#define BIOME_CELLULAR 3
#define CLOUD -1

struct {
	int current = BIOME_PLAINS;
	int maxHeight = 0;
} biomes;

float getHeight(int biome, int x, int z) {
	int result;

	switch (biome) {
	case BIOME_PLAINS:
		noiseGenerator.SetNoiseType(FastNoise::PerlinFractal);
		noiseGenerator.SetFrequency(0.01);
		biomes.maxHeight = 15;
		result = noiseGenerator.GetNoise(x, z) * biomes.maxHeight;
		break;
	case BIOME_HILLS:
		noiseGenerator.SetNoiseType(FastNoise::PerlinFractal);
		noiseGenerator.SetFrequency(0.01);
		biomes.maxHeight = 25;
		result = noiseGenerator.GetNoise(x, z) * biomes.maxHeight;
		break;
	case BIOME_MOUNTAINS:
		noiseGenerator.SetNoiseType(FastNoise::SimplexFractal);
		noiseGenerator.SetFrequency(0.005);
		biomes.maxHeight = 75;
		result = noiseGenerator.GetNoise(x, z) * biomes.maxHeight;
		break;
	case BIOME_CELLULAR:
		noiseGenerator.SetNoiseType(FastNoise::Cellular);
		noiseGenerator.SetFrequency(0.05);
		biomes.maxHeight = 15;
		result = noiseGenerator.GetNoise(x, z) * biomes.maxHeight;
		break;
	case CLOUD:
		noiseGenerator.SetNoiseType(FastNoise::Cubic);
		noiseGenerator.SetFrequency(0.015);
		biomes.maxHeight = 3;
		result = noiseGenerator.GetNoise(x, z) * biomes.maxHeight;
		break;
	}

	result = result;
	return result;
}

float* drawChunk(int **chunk) {

	float *chunkMesh = new float[*chunk[2] * 36];

	//cout << *chunk[2] << endl;

	int vertexCt = 0;
	for (int x = 0; x < *chunk[2]; x++) {
		//myFile << "type: " << *chunk[3][x] << " ... " << *chunk[4][x] << " " << *chunk[5][x] << " " << *chunk[6][x] << endl;
		if ((chunk[3][x] >= 0) && chunk[3][x] <= 150) {
			bool drawFront = false;
			bool drawBack = false;
			bool drawTop = false;
			bool drawBottom = false;
			bool drawLeft = false;
			bool drawRight = false;

			if (chunk[5][x] + 1 > getHeight(biomes.current, *chunk[0] * world.chunks.size + chunk[4][x], *chunk[1] * world.chunks.size + chunk[6][x] - 1)) {
				drawBack = true;
				// BACK
			}
			if (chunk[5][x] + 1 > getHeight(biomes.current, *chunk[0] * world.chunks.size + chunk[4][x], *chunk[1] * world.chunks.size + chunk[6][x] + 1)) {
				// FRONT
				drawFront = true;
			}

			if (chunk[3][x - (world.chunks.size + 2)] < 0 && player.position.y < chunk[5][x]) { // 2 is offset to account for "air" *chunk[3] surrounding *chunk
				drawBottom = true;
				// BOTTOM
			}

			if (chunk[3][x + (world.chunks.size + 2)] < 0) { //&& player.position.y > chunk[5][x]) { // 2 is offset to account for "air" *chunk[3] surrounding *chunk
				drawTop = true;
				// TOP
			}

			if (chunk[5][x] + 1 > getHeight(biomes.current, *chunk[0] * world.chunks.size + chunk[4][x] - 1, *chunk[1] * world.chunks.size + chunk[6][x])) { // 2 is offset to account for "air" *chunk[3] surrounding *chunk. 1 accounts for 0
				drawFront = true;
				// LEFT
			}

			if (chunk[5][x] + 1 > getHeight(biomes.current, *chunk[0] * world.chunks.size + chunk[4][x] + 1, *chunk[1] * world.chunks.size + chunk[6][x])) { // 2 is offset to account for "air" *chunk[3] surrounding *chunk. 1 accounts for 0
				drawRight = true;
				// RIGHT
			}

			// BACK FRONT LEFT RIGHT BOTTOM TOP

			for (int i = 0; i < 6; i++) {
				if (i == 0 && !drawBack ||
					i == 1 && !drawFront ||
					i == 2 && !drawFront ||
					i == 3 && !drawRight ||
					i == 4 && !drawBottom ||
					i == 5 && !drawTop) {
					continue;
				}
				for (int j = 0; j < 36; j++) {
					float value;
					if (j % 6 == 0) {
						value = positions[36 * i + j] + chunk[4][x];
					}
					if (j % 6 == 1) {
						value = positions[36 * i + j] + chunk[5][x];
					}
					if (j % 6 == 2) {
						value = positions[36 * i + j] + chunk[6][x];
					}
					if (j % 6 == 3) {
						value = positions[36 * i + j];
					}
					if (j % 6 == 4) {
						value = positions[36 * i + j];
					}
					if (j % 6 == 5) {
						value = positions[36 * i + j];
					}
					chunkMesh[vertexCt] = value;
					vertexCt++;
				}
			}
		}
	}

	return chunkMesh;
}

int** addChunk(float chunkX, float chunkZ) {

	//vector<float> chunkMesh(10000000);
	int maxBlocks;

	int chunkLim;
	int chunkLimY;

		maxBlocks = (world.chunks.size + 2) * (255 * 2 + 2 + 1) * (world.chunks.size + 2);
		chunkLim = (world.chunks.size - 1) / 2;
		chunkLimY = biomes.maxHeight;


	int *blocks = new int[maxBlocks];
	int *xPos = new int[maxBlocks];
	int *yPos = new int[maxBlocks];
	int *zPos = new int[maxBlocks];
	int blocksIter = 0;

	for (int x = -chunkLim - 1; x <= chunkLim + 1; x++) {
		for (int y = -chunkLimY - 1; y <= chunkLimY + 1; y++) {
			for (int z = -chunkLim - 1; z <= chunkLim + 1; z++) {
				if (x >= -chunkLim &&
					x <= chunkLim &&
					y >= -chunkLimY &&
					y <= chunkLimY &&
					z >= -chunkLim &&
					z <= chunkLim) {
					int heightVal = getHeight(biomes.current, world.chunks.size*chunkX + x, world.chunks.size*chunkZ + z);
					if (y < heightVal) {
						blocks[blocksIter] = 9;
						xPos[blocksIter] = x;
						yPos[blocksIter] = y;
						zPos[blocksIter] = z;
					}	
				}
				blocksIter++;
			}
		}
	}

	int **chunkData = new int*[8];
	int *chunX = new int;
	*chunX = chunkX;
	int *chunZ = new int;
	*chunZ = chunkZ;
	int *blocCt = new int;
	*blocCt = blocksIter;
	chunkData[0] = chunX;
	chunkData[1] = chunZ;
	chunkData[2] = blocCt;
	chunkData[3] = blocks;
	chunkData[4] = xPos;
	chunkData[5] = yPos;
	chunkData[6] = zPos;

	
	return chunkData;
}


void jump(float height, float speed) {
	if (!player.isJumping) {
		jumpHeight = height;
		jumpSpeed = speed;
		player.isJumping = true;
		jumpBlockX = player.position.x;
		jumpBlockZ = player.position.z;
		jumpCounter = 0;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	input.keyboard.isKeyPressed[key] = action;
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		//cout << "here" << endl;
		flying = !flying;
		jumpCounter = 0;
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		player.forwardSpeed *= player.runSpeedMultiplier;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		player.forwardSpeed /= player.runSpeedMultiplier;
	} 
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
		if (!flying) {
			jump(player.height * 2, 6);
		}
	}
}
void handleInput() {

	if (input.keyboard.isKeyPressed[GLFW_KEY_W]) {
		player.position.z -= (flying ? player.flySpeed : player.forwardSpeed) * cos(glm::radians(cameraRotY)) * frameTimeMultiplier;
		player.position.x += (flying ? player.flySpeed : player.forwardSpeed) * sin(glm::radians(cameraRotY))* frameTimeMultiplier;
	}
	if (input.keyboard.isKeyPressed[GLFW_KEY_S]) {
		player.position.z += (flying ? player.flySpeed : player.forwardSpeed) * cos(glm::radians(cameraRotY))* frameTimeMultiplier;
		player.position.x -= (flying ? player.flySpeed : player.forwardSpeed) * sin(glm::radians(cameraRotY))* frameTimeMultiplier;
	}
	if (input.keyboard.isKeyPressed[GLFW_KEY_A]) {
		player.position.x -= (flying ? player.flySpeed : player.forwardSpeed) * cos(glm::radians(cameraRotY))* frameTimeMultiplier;
		player.position.z -= (flying ? player.flySpeed : player.forwardSpeed) * sin(glm::radians(cameraRotY))* frameTimeMultiplier;
	}
	if (input.keyboard.isKeyPressed[GLFW_KEY_D]) {
		player.position.x += (flying ? player.flySpeed : player.forwardSpeed) * cos(glm::radians(cameraRotY))* frameTimeMultiplier;
		player.position.z += (flying ? player.flySpeed : player.forwardSpeed) * sin(glm::radians(cameraRotY))* frameTimeMultiplier;
	}
	if (input.keyboard.isKeyPressed[GLFW_KEY_LEFT_SHIFT]) {
		if (flying) {
			player.position.y -= player.flySpeed;
		}
		else {
			
		}
	}
	if (input.keyboard.isKeyPressed[GLFW_KEY_SPACE]) {
		if (flying) {
			player.position.y += player.flySpeed;
		}
	}

	int newCurrChunkX = floor((player.position.x + world.chunks.size / 2) / world.chunks.size);
	int newCurrChunkZ = floor((player.position.z + world.chunks.size / 2) / world.chunks.size);
	if (!(world.chunks.current.x == newCurrChunkX) ||
		!(world.chunks.current.z == newCurrChunkZ)) {

		dirX = newCurrChunkX - world.chunks.current.x;
		dirZ = newCurrChunkZ - world.chunks.current.z;

		world.chunks.current.x = newCurrChunkX;
		world.chunks.current.z = newCurrChunkZ;
	}
}

static int CompileShader(unsigned int type, const string& source) {
	int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	return id;
}

static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) {

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int cube[36];
int blocks = 0;

unsigned int buffer;

void renderChunk(int **chunk, float *chunkMesh) {
	//cout << chunk << endl;
	glBufferData(GL_ARRAY_BUFFER, 1000000, chunkMesh, GL_DYNAMIC_DRAW);

	glm::mat4 modelTranslate(1.0f);
	glm::mat4 modelRotate(1.0f);
	glm::mat4 modelScale(1.0f);
	modelTranslate = glm::translate(modelTranslate, glm::vec3(world.chunks.size * *chunk[0] - player.position.x, 0.0f - player.position.y, world.chunks.size * *chunk[1] - player.position.z));
	glm::mat4 modelTransform = modelRotate * modelTranslate * modelScale;
	int modelUniform = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(modelTransform));

	glDrawArrays(GL_TRIANGLES, 0, 1000000);
}


void renderElement(float *mesh, glm::vec3 transVec = glm::vec3(0.0f), glm::vec3 scaleVec = glm::vec3(1.0f), glm::vec3 rotVec = glm::vec3(1.0f) , float angle = 0.0f) {
	glBufferData(GL_ARRAY_BUFFER, elementsInBox * sizeof(float), mesh, GL_DYNAMIC_DRAW);

	glm::mat4 modelTranslate(1.0f);
	glm::mat4 modelRotate(1.0f);
	glm::mat4 modelScale(1.0f);
	modelTranslate = glm::translate(modelTranslate, transVec);
	modelScale = glm::scale(modelTranslate, scaleVec);
	modelRotate = glm::rotate(modelTranslate, angle, rotVec);


	glm::mat4 modelTransform = modelRotate * modelTranslate * modelScale;
	int modelUniform = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(modelTransform));

	glDrawArrays(GL_TRIANGLES, 0, elementsInBox * sizeof(float));
}

int refMiddleChunkX;
int refMiddleChunkZ;

void generateChunks() {

	if(window.hasFocus){
	refMiddleChunkX = world.chunks.current.x;
	refMiddleChunkZ = world.chunks.current.z;

	for (int a = 0; a <= world.chunks.toGenerate; a++) {
		for (int b = -1; b <= 1; b++) {
			if (b != 0) {
				int x = a * b;
				for (int z = 0; z < chunksToGenerateAcross; z++) {
					bool alreadyExists = false;
					for (int i = 0; i < world.chunks.totalCount; i++) {
						if (*chunks[i] != 0) {
							if (*chunks[i][0] == world.chunks.current.x + x &&
								*chunks[i][1] == world.chunks.current.z + z - world.chunks.toGenerate) {
								alreadyExists = true;
							}

							if (*chunks[i][0] < world.chunks.current.x - world.chunks.toGenerate ||
								*chunks[i][0] > world.chunks.current.x + world.chunks.toGenerate ||
								*chunks[i][1] < world.chunks.current.z - world.chunks.toGenerate ||
								*chunks[i][1] > world.chunks.current.z + world.chunks.toGenerate) {
								delete[] chunks[i][0];
								delete[] chunks[i][1];
								delete[] chunks[i][2];
								delete[] chunks[i][3];
								delete[] chunks[i][4];
								delete[] chunks[i][5];
								delete[] chunks[i][6];
								*chunks[i] = 0;
							}
						}
					}
					if (!alreadyExists) {
						//chunks[world.chunks.totalCount - chunksAcross * chunksAcross] = 0;
						chunks[world.chunks.totalCount] = addChunk(refMiddleChunkX + x, refMiddleChunkZ + z - world.chunks.toGenerate);
						chunkMeshes[world.chunks.totalCount] = drawChunk(chunks[world.chunks.totalCount]);
						//cout << "generated chunk " << world.chunks.totalCount << endl;
						world.chunks.totalCount++;
					}
					//cout << world.chunks.totalCount << " ... " << x << ", " << z << endl;
				}
			}
		}
	}
	}

	regenerateChunks = false;
	while (!regenerateChunks) {}
	generateChunks();
}

bool rerenderChunks = true;


__int64 getTime() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void addSky() {
	int maxBlocks = 512 * 512;

	int *blocks = new int[maxBlocks];
	int *xPos = new int[maxBlocks];
	int *zPos = new int[maxBlocks];


	int blocksIter = 0;
	for (int x = 0; x < 512; x++) {
		for (int z = 0; z < 512; z++) {
			//getHeight(CLOUD, x, z);
			blocks[blocksIter] = 10;
			xPos[blocksIter] = x;
			zPos[blocksIter] = z;
			blocksIter++;
		}
	}
}

int main(void)
{
	if (!glfwInit())
		return -1;

	window.instance = glfwCreateWindow(window.width, window.height, "Fake MineCrap", NULL, NULL);
	if (!window.instance)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window.instance);

	if (glewInit() != GLEW_OK) {
		//cout << "glew Error" << endl;
	}

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window.instance, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwSetKeyCallback(window.instance, key_callback);

	string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;"
		"layout(location = 1) in vec3 color;"
		"out vec4 fragColor;"
		"uniform mat4 view;"
		"uniform mat4 model;"
		"uniform mat4 projection;"
		"\n"
		"void main(){\n"
		"fragColor = vec4(color,1.0);"
		"gl_Position =  projection * view * model * position;"
		"}\n";

	string fragmentShader =
		"#version 330 core\n"
		"\n"
		"in vec4 fragColor;"
		"out vec4 outColor;"
		"uniform vec4 color;"
		"\n"
		"void main(){\n"
		"outColor = fragColor;"
		"}\n";

	shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	int projectionUniform = glGetUniformLocation(shader, "projection");

	int viewUniform = glGetUniformLocation(shader, "view");

	//myFile.open("blocks.txt", ios::trunc);

	player.position.y = getHeight(biomes.current, player.position.x, player.position.z) + player.height;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	copy(begin(positions), end(positions), begin(skybox));

	copy(begin(positions), end(positions), begin(sun));
	copy(begin(positions), end(positions), begin(moon));


	for (int i = 0; i < 6 * 6; i++) {
		sun[6 * i + 3] = 253 / 255.0f; //R
		sun[6 * i + 4] = 184 / 255.0f; //G
		sun[6 * i + 5] = 19 / 255.0f; //B
	}

	for (int i = 0; i < 6 * 6; i++) { //253, 184, 19
		moon[6 * i + 3] = 181 / 255.0f; //R
		moon[6 * i + 4] = 185 / 255.0f; //G
		moon[6 * i + 5] = 183 / 255.0f; //B
	}

	/*
	if (world.chunks.size % 2 == 0) {
	world.chunks.size++;
	}
	*/
	thread t1(generateChunks);

	/*
	refMiddleChunkX = world.chunks.current.x;
	refMiddleChunkZ = world.chunks.current.z;
	chunks[0] = addChunk(refMiddleChunkX, refMiddleChunkZ);
	world.chunks.totalCount++;
	chunks[0][7] = drawChunk(chunks[0]);
	*/

	srand(getTime());
	float seed = rand() % 1000000;
	noiseGenerator.SetSeed(seed);
	//cout << seed << endl;

	timeWhenStarted = getTime();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window.instance))
	{
		timeCurrent = getTime();

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projectionTransform = glm::perspective(glm::radians(window.fov), window.width / window.height, 0.1f, 255.0f);
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projectionTransform));

		glm::mat4 viewTransform(1.0f);
		glm::mat4 viewRotateX(1.0f);
		glm::mat4 viewRotateY(1.0f);
		viewRotateX = glm::rotate(viewRotateX, glm::radians(cameraRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		viewRotateY = glm::rotate(viewRotateY, glm::radians(cameraRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		viewTransform = viewRotateX * viewRotateY;// *viewRotate * viewScale;
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(viewTransform));

		window.hasFocus = glfwGetWindowAttrib(window.instance, GLFW_FOCUSED);
		//cout << "playerPosition: " << player.position.x << ", " <<player.position.y << ", " << player.position.z << endl;
		if (window.hasFocus){
			glfwGetCursorPos(window.instance, &input.mouse.x, &input.mouse.y);
			if (input.mouse.x > 0 && input.mouse.y > 0 && input.mouse.x < window.width && input.mouse.y < window.height) {

				handleInput();
				//cout << "mouse: " << input.mouse.x <<", " << input.mouse.y << endl;

				if (cameraRotX < 90 && cameraRotX > -90 ||
					(cameraRotX >= 90 && (input.mouse.y - window.height / 2) * cameraSpeed < 0) ||
					(cameraRotX <= -90 && (input.mouse.y - window.height / 2) * cameraSpeed > 0)) {
					cameraRotX += (input.mouse.y - window.height / 2) * cameraSpeed;
				}

				cameraRotY += (input.mouse.x - window.width / 2) * cameraSpeed;

				//cout << "cameraRot: " << cameraRotX << ", " << cameraRotY << endl;

				player.looking.x = sin(glm::radians(cameraRotY));
				player.looking.y = -sin(glm::radians(cameraRotX));
				player.looking.z = -cos(glm::radians(cameraRotY));
				//cout << "looking: " << player.looking.x << ", " << player.looking.y << ", " << player.looking.z << " & HEIGHT: " << player.position.y <<  " + " << 1.7 - (.7) * (player.position.y / 127) <<endl;
			}
			glfwSetCursorPos(window.instance, window.width / 2, window.height / 2);
		}

		regenerateChunks = true;
		
		
		if (world.chunks.totalCount > 0) {
			for (int i = 0; i < world.chunks.totalCount; i++) {
				if (*chunks[i] != 0) {
					
					//cout << world.chunks.current.x << ", " << world.chunks.current.z << endl;

					int absCurrX = abs(world.chunks.current.x);

					int absCurrZ = abs(world.chunks.current.x);

					int absCX = abs(*chunks[i][0]);
					int absCZ = abs(*chunks[i][1]);

					int disX = absCX - absCurrX;
					int disZ = absCZ - absCurrZ;


					if (player.looking.z < -.5 && 
						(world.chunks.current.z < *chunks[i][1] || 
						*chunks[i][1] < world.chunks.current.z - world.chunks.toRender || 
						*chunks[i][0] < world.chunks.current.x - world.chunks.toRender / ((player.looking.z < -.87) ? 1.7 -  (player.position.y/127) : 1) ||
						*chunks[i][0] > world.chunks.current.x + world.chunks.toRender / ((player.looking.z < -.87) ? 1.7  -  (player.position.y/127) : 1))) {
						continue;
					}
					if (player.looking.z > .5 && 
						(world.chunks.current.z > *chunks[i][1] ||
						*chunks[i][1] > world.chunks.current.z + world.chunks.toRender || 
						*chunks[i][0] < world.chunks.current.x - world.chunks.toRender / ((player.looking.z > .87) ? 1.7  -  (player.position.y/127) : 1) ||
						*chunks[i][0] > world.chunks.current.x + world.chunks.toRender / ((player.looking.z > .87) ? 1.7  -  (player.position.y/127) : 1))) {
						continue;
					}
					if (player.looking.x < -.5 && 
						(world.chunks.current.x < *chunks[i][0] ||
						*chunks[i][0] < world.chunks.current.x - world.chunks.toRender ||
						*chunks[i][1] < world.chunks.current.z - world.chunks.toRender/((player.looking.x < -.87) ? 1.7  -  (player.position.y/127) : 1) ||
						*chunks[i][1] > world.chunks.current.z + world.chunks.toRender/((player.looking.x < -.87) ? 1.7  -  (player.position.y/127) : 1))) {
						continue;
					}
					if (player.looking.x > .5 &&
						(world.chunks.current.x > *chunks[i][0] ||
						*chunks[i][0] > world.chunks.current.x + world.chunks.toRender ||
						*chunks[i][1] < world.chunks.current.z - world.chunks.toRender/ ((player.looking.x > .87) ? 1.7  -  (player.position.y/127) : 1) ||
						*chunks[i][1] > world.chunks.current.z + world.chunks.toRender/ ((player.looking.x > .87) ? 1.7  -  (player.position.y/127) : 1))) {
						continue;
					}



					renderChunk(chunks[i], chunkMeshes[i]);

				}
			}
		}

		
		float sunDivider = 1000.0;
		float sunSize = 10.0;
		float sunDistance = 45.0;
		float sunX = sunDistance * cos(world.stats.time / sunDivider);
		float sunY = sunDistance * sin(world.stats.time / sunDivider);
		float moonX = -sunDistance * cos(world.stats.time / sunDivider);
		float moonY = -sunDistance * sin(world.stats.time / sunDivider);


		for (int i = 0; i < 6 * 6; i++) {
			//if (skybox[6 * i + 1] == 0.5f) {
			int R = 96;
			int G = 181;
			int B = 255;


			if (sunY > -sunDistance / 3) {
				if (skybox[6 * i + 1] == .5) {
					skybox[6 * i + 3] = ((R - 30)/ 255.0f) * ((sunY / sunDistance)*.65 + .35);
					skybox[6 * i + 4] = ((G - 30)/ 255.0f) * ((sunY / sunDistance)*.65 + .35);
					skybox[6 * i + 5] = ((B - 30)/ 255.0f) * ((sunY / sunDistance)*.65 + .35);
				}
				else {
					skybox[6 * i + 3] = (R / 255.0f) * ((sunY / sunDistance)*.65 + .35);
					skybox[6 * i + 4] = (G / 255.0f) * ((sunY / sunDistance)*.65 + .35);
					skybox[6 * i + 5] = (B / 255.0f) * ((sunY / sunDistance)*.65 + .35);
				}
			}
		}
		renderElement(skybox, glm::vec3(0.0f), glm::vec3(280.0f, 280.0f, 280.0f));

		

		cout << "timeCurrent: " << world.stats.time/60/10 << "; sunX: " << sunX << "; sunY: " << sunY << ";" << endl;
		
		for (int i = 0; i < 6 * 6; i++) {
			if (sunY > -sunDistance/3 && sunX <0) {
				sun[6 * i + 3] = (253 / 255.0f)* ((sunY / sunDistance)*.45 + .65);
				sun[6 * i + 4] = (184 / 255.0f)* ((sunY / sunDistance)*.45 + .65);
				sun[6 * i + 5] = (19 / 255.0f)* ((sunY / sunDistance)*.45 + .65);
			}
			else {
				sun[6 * i + 3] = (253 / 255.0f);
				sun[6 * i + 4] = (184 / 255.0f);
				sun[6 * i + 5] = (19 / 255.0f);
			}
		}

		/*
		if (sunY < 0) {
			if (sunOrMoon) { // MOON 181, 185, 183

				for (int i = 0; i < 6 * 6; i++) { //253, 184, 19
					sun[6 * i + 3] = 181 / 255.0f; //R
					sun[6 * i + 4] = 185 / 255.0f; //G
					sun[6 * i + 5] = 183 / 255.0f; //B
				}
				sunOrMoon = !sunOrMoon;
			} else { // SUN 253, 184, 19
				for (int i = 0; i < 6 * 6; i++) {
					sun[6 * i + 3] = 253 / 255.0f; //R
					sun[6 * i + 4] = 184 / 255.0f; //G
					sun[6 * i + 5] = 19 / 255.0f; //B
				}
				sunOrMoon = !sunOrMoon;
			}
			
			timeWhenStarted = timeCurrent;
		}
		*/
		renderElement(sun, glm::vec3(sunX, sunY, 0.0), glm::vec3(sunSize, sunSize, sunSize));
		renderElement(moon, glm::vec3(moonX, moonY, 0.0), glm::vec3(sunSize, sunSize, sunSize));
		world.stats.time = timeCurrent-timeWhenStarted;
		//renderElement(sun);

		if (!flying) {
			if (!player.isJumping) {
				if (player.position.y > getHeight(biomes.current, player.position.x, player.position.z) + player.height) {
					player.position.y += player.fallSpeed;
					player.fallSpeed -= 0.02f * frameTimeMultiplier;
				}
				else if (player.position.y <= getHeight(biomes.current, player.position.x, player.position.z) + player.height - 0.25f) {
					jump(player.height * .7, 9);
				}
				else {
					player.position.y = getHeight(biomes.current, player.position.x, player.position.z) + player.height;
					player.fallSpeed = -0.25f;
				}
			}
			else {
				jumpCounter += frameTimeMultiplier;
				player.position.y = -pow(jumpCounter/80 * jumpSpeed - sqrt(jumpHeight),2) + jumpHeight + getHeight(biomes.current, jumpBlockX, jumpBlockZ) + player.height;
				if (player.position.y <= getHeight(biomes.current, player.position.x, player.position.z) + player.height) {
					player.position.y = getHeight(biomes.current, player.position.x, player.position.z) + player.height;
					player.isJumping = false;
					jumpCounter = 0;
				}
			}
		}

		glfwSwapBuffers(window.instance);
		glfwPollEvents();

		__int64 ms1 = getTime();
		frameTime = ms1 - timeCurrent;
		frameTimeMultiplier = frameTime /16;
		//cout << frameTimeMultiplier << endl;
	}

	t1.detach();

	for (int i = 0; i < world.chunks.totalCount; i++) {
		for (int z = 0; z < 7; z++) {
			delete[] chunks[i][z];
		}
		delete[] chunks[i];
	}
	delete[] chunks;

	glDeleteProgram(shader);

	glfwTerminate();

	return 0;
}

