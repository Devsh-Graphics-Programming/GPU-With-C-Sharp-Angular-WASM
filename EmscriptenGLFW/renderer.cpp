/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#include "renderer.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"

const char* fragmentShader = R"(#version 300 es
uniform int       iEnv;
uniform vec3      iResolution;           
uniform float     iTime;                 
uniform vec4      iMouse;                
uniform sampler2D iChannel0;             
uniform samplerCube iChannel1;           
const int NUM_SAMPLES = 1;const int NUM_ITERS = 7;
float saturate(float a) { return clamp(a, 0.0, 1.0); }
const int BIG_INT = 2000000000;
const float PI=3.14159265;
const float farPlane = 1024.0;
vec3 RotateX(vec3 v, float rad)
{
float cos = cos(rad);
float sin = sin(rad);
return vec3(v.x, cos * v.y + sin * v.z, -sin * v.y + cos * v.z);
}
vec3 RotateY(vec3 v, float rad)
{
float cos = cos(rad);
float sin = sin(rad);
return vec3(cos * v.x - sin * v.z, v.y, sin * v.x + cos * v.z);
}
vec3 RotateZ(vec3 v, float rad)
{
float cos = cos(rad);
float sin = sin(rad);
return vec3(cos * v.x + sin * v.y, -sin * v.x + cos * v.y, v.z);
}mat3 MakeBasis(vec3 normal) {
mat3 result;
result[0] = normal;
if (abs(normal.y) > 0.5) {
result[1] = normalize(cross(normal, vec3(1.0, 0.0, 0.0)));
} else {
result[1] = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
}
result[2] = normalize(cross(normal, result[1]));
return result;
}int randomState = int(123456789);float Hashf1i1(int seed) {
int s2 = seed * seed;
int s3 = s2 * seed;
int a = mod((s2 * int(60449) + int(30011)) , 101159);
int b = mod(s3 * int(4111) , 104729);
int z = a ^ b;
return float(z & int(0xffff)) / float(0xffff); 
}
vec2 Hashf2i1(int n) {
float a = Hashf1i1(n+int(3));
float b = Hashf1i1(n+int(11));
return vec2(a, b);
}
vec3 Hashf3i1(int n) {
float a = Hashf1i1(n+int(3));
float b = Hashf1i1(n+int(11));
float c = Hashf1i1(n+int(17));
return vec3(a, b, c);
}
int GetRand() {
randomState ^= randomState << 13;
randomState ^= randomState >> 17;
randomState ^= randomState << 5;
return randomState;
}
float Randf1i1(int seed) {
randomState ^= randomState << 13;
randomState ^= randomState >> 17;
randomState ^= randomState << 5;
int s2 = seed * seed;
int s3 = s2 * seed;
int a = mod((s2 * int(60449) + int(30011)) , 101159);
int b = mod(s3 * int(4111) , 104729);
int z = a ^ b ^ randomState;
return float(z & int(0xffff)) / float(0xffff);
}
vec3 Randf3i1(int n) {
float a = Randf1i1(n);
float b = Randf1i1(n + int(7));
float c = Randf1i1(n + int(13));
return vec3(a, b, c);
}
vec2 Randf2i1(int n) {
float a = Randf1i1(n + int(3));
float b = Randf1i1(n + int(11));
return vec2(a, b);
}
vec2 RandGaussian(int n) {
vec2 u = Randf2i1(n);
u.x = max(u.x, 0.00000003); 
float a = sqrt(-2.0 * log(u.x));
return vec2(a * cos(2.0*PI*u.y), a * sin(2.0 * PI * u.y));
}float RayLifetime(float p, int seed) {if (p < 0.00000003) return farPlane;  
float unif = Randf1i1(seed);  unif = max(0.00000000000001, unif);p = min(p, .999999);return g;
}float Hash1d(float u)
{
return fract(sin(u)*143.9);
}
float Hash2d(vec2 uv)
{
float f = uv.x + uv.y * 37.0;
return fract(sin(f)*104003.9);
}
float Hash3d(vec3 uv)
{
float f = uv.x + uv.y * 37.0 + uv.z * 521.0;
return fract(sin(f)*110003.9);
}
float mixP(float f0, float f1, float a)
{
return mix(f0, f1, a*a*(3.0-2.0*a));
}
const vec2 zeroOne = vec2(0.0, 1.0);
float noise2d(vec2 uv)
{
vec2 fr = fract(uv.xy);
vec2 fl = floor(uv.xy);
float h00 = Hash2d(fl);
float h10 = Hash2d(fl + zeroOne.yx);
float h01 = Hash2d(fl + zeroOne);
float h11 = Hash2d(fl + zeroOne.yy);
return mixP(mixP(h00, h10, fr.x), mixP(h01, h11, fr.x), fr.y);
}
float noise3d(vec3 uv)
{
vec3 fr = fract(uv.xyz);
vec3 fl = floor(uv.xyz);
float h000 = Hash3d(fl);
float h100 = Hash3d(fl + zeroOne.yxx);
float h010 = Hash3d(fl + zeroOne.xyx);
float h110 = Hash3d(fl + zeroOne.yyx);
float h001 = Hash3d(fl + zeroOne.xxy);
float h101 = Hash3d(fl + zeroOne.yxy);
float h011 = Hash3d(fl + zeroOne.xyy);
float h111 = Hash3d(fl + zeroOne.yyy);
return mixP(
mixP(mixP(h000, h100, fr.x),
mixP(h010, h110, fr.x), fr.y),
mixP(mixP(h001, h101, fr.x),
mixP(h011, h111, fr.x), fr.y)
, fr.z);
}
vec3 RandPointOnSphere(int seed) {
vec2 uv = Randf2i1(seed);
float theta = 2.0 * PI * uv.x;
float psi = acos(2.0 * uv.y - 1.0);
float x = cos(theta) * sin(psi);
float y = sin(theta) * sin(psi);
float z = cos(psi);
return vec3(x, y, z);
}
vec3 RandPointInSphere(int seed) {
return RandPointOnSphere(seed);
vec3 p = Randf3i1(seed) * 2.0 - 1.0;
if (length(p) <= 1.0) return p;
p = Randf3i1(seed) * 2.0 - 1.0;
if (length(p) <= 1.0) return p;
p = Randf3i1(seed) * 2.0 - 1.0;
if (length(p) <= 1.0) return p;
p = Randf3i1(seed) * 2.0 - 1.0;
if (length(p) <= 1.0) return p;
p = Randf3i1(seed) * 2.0 - 1.0;
if (length(p) <= 1.0) return p;
return p;
}vec3 GetEnvMap2(vec3 rayDir)
{
vec3 final = vec3(1.0) * dot(rayDir, vec3(0.0, 1.0, 0.0)) * 0.5 + 0.5;
final *= 0.125;if ((rayDir.y > abs(rayDir.x)*1.0) && (rayDir.y > abs(rayDir.z*0.25))) final = vec3(2.0)*rayDir.y;float roundBox = length(max(abs(rayDir.xz/max(0.0,rayDir.y))-vec2(0.9, 4.0),0.0))-0.1;
final += vec3(0.8)* pow(saturate(1.0 - roundBox*0.5), 6.0);final += vec3(8.0,6.0,7.0) * saturate(0.001/(1.0 - abs(rayDir.x)));final += vec3(8.0,7.0,6.0) * saturate(0.001/(1.0 - abs(rayDir.z)));
return vec3(final);
}vec3 GetEnvMap(vec3 rayDir) {
vec3 tex = texture(iChannel1, rayDir).xyz;
tex = tex * tex;  
vec3 light = vec3(0.0);if ((rayDir.y > abs(rayDir.x+0.6)*0.29) && (rayDir.y > abs(rayDir.z*2.5))) light = vec3(2.0)*rayDir.y;
vec3 texp = pow(tex, vec3(14.0));
light *= texp;  
return (tex + light*3.0)*1.0;
}vec3 sunDir = normalize(vec3(0.93, 1.0, 1.0));
const vec3 sunCol = vec3(250.0, 220.0, 200.0) / 155.0;
const vec3 horizonCol = vec3(0.95, 0.9, 0.85)*1.0;
const vec3 skyCol = vec3(0.03,0.46,1.0)*0.9;
const vec3 groundCol = vec3(0.8,0.7,0.55)*1.1;vec3 GetSunColorSmall(vec3 rayDir)
{
vec3 localRay = normalize(rayDir);
float dist = 1.0 - (dot(localRay, sunDir) * 0.5 + 0.5);
float sunIntensity = 0.05 / dist;
sunIntensity += exp(-dist*150.0)*7000.0;
sunIntensity = min(sunIntensity, 40000.0);
return sunCol * sunIntensity*0.002;
}
vec3 GetEnvMap3(vec3 rayDir)
{vec3 finalColor = mix(horizonCol, skyCol, pow(saturate(rayDir.y), 0.25))*0.95;float n = noise2d(rayDir.xz/rayDir.y*1.0);
n += noise2d(rayDir.xz/rayDir.y*2.0)*0.5;
n += noise2d(rayDir.xz/rayDir.y*4.0)*0.25;
n += noise2d(rayDir.xz/rayDir.y*8.0)*0.125;
float nc = pow(abs(n), 3.0);
nc = nc * saturate(abs(rayDir.y * 4.0));  
float dist = 1.0 - (dot(rayDir, sunDir) * 0.5 + 0.5);
vec3 cloudCol = (vec3(1.0,0.6,0.2)/dist*1.05+sunCol*1.0)*0.75*saturate((rayDir.y+0.2)*5.0);
finalColor = mix(finalColor, cloudCol, saturate(nc*0.0625));
float n2 = noise2d(rayDir.xz * 8.0)*1.7;
n2 += noise2d(rayDir.xz * 16.0) * 0.5;
n2 += noise2d(rayDir.xz * 32.0) * 0.25;
n2 += noise2d(rayDir.xz * 64.0) * 0.125;
n2 += 1.5;
vec3 mountainCol = (horizonCol * 0.75 + skyCol * 0.25) * 0.6 - rayDir.y*2.0;
finalColor = mix(finalColor, mountainCol, saturate((n2*0.025-rayDir.y)*256.0));
n2 = noise2d(-rayDir.xz * 8.0)*1.7;
n2 += noise2d(-rayDir.xz * 16.0) * 0.5;
n2 += noise2d(-rayDir.xz * 32.0) * 0.25;
n2 += noise2d(-rayDir.xz * 64.0) * 0.125;
n2 += 1.0;
mountainCol = (horizonCol * 0.95 + skyCol * 0.05) * 0.4;
finalColor = mix(finalColor, mountainCol, saturate((n2*0.0125-rayDir.y)*256.0));
n += noise2d(rayDir.xz/rayDir.y*16.0)*0.35;
vec3 groundTex = groundCol - (n-1.25)*0.15;
finalColor = mix(finalColor, (groundTex)*0.9, saturate(-rayDir.y*164.0));
finalColor += GetSunColorSmall(rayDir);
return finalColor;
}
vec3 GetEnvMap4(vec3 rayDir) {   return vec3(1.0,1.0,1.0)*(rayDir.z*0.5+0.5); }struct RayHit
{
vec3 hit;
vec3 norm;
float t;
};
RayHit NewRayHit()
{
RayHit rh;
rh.t = farPlane;
rh.hit = vec3(0.0);
rh.norm = vec3(0.0);
return rh;
}
struct Ray
{
vec3 p0, dirNormalized;
int hash;
int outside;  
};
struct SceneHit
{
vec3 hitPos;
vec3 hitNormal;
float t;
int objIndex;
int materialIndex;
};
struct ColorHit {
vec3 diffuse;
vec3 emission;
};
ColorHit colorHits[NUM_ITERS];
int colorHitIndex;
void ResetColorHitList() {
colorHitIndex = 0;
for (int i = 0; i < NUM_ITERS; i++) {
colorHits[i].emission.x = -1.0;
}
}
void SaveHit(in vec3 diffuse, in vec3 emission) {
colorHits[colorHitIndex] = ColorHit(diffuse, emission);
colorHitIndex++;
}
float SphereIntersect(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 spherePos, float rad)
{
vec3 radialVec = pos - spherePos;
float b = dot(radialVec, dirVecPLZNormalizeMeFirst);
float c = dot(radialVec, radialVec) - rad * rad;
float h = b * b - c;
if (h < 0.0) return -1.0;
return -b - sqrt(h);
}
RayHit SphereIntersect3(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 spherePos, float rad, int outside)
{
RayHit rh = NewRayHit();
vec3 delta = spherePos - pos;
float projdist = dot(delta, dirVecPLZNormalizeMeFirst);
vec3 proj = dirVecPLZNormalizeMeFirst * projdist;
vec3 bv = proj - delta;
float b2 = dot(bv, bv);
if (b2 > rad*rad) return rh;  
float x = sqrt(rad*rad - b2);
rh.t = projdist - (x * float(outside));
rh.hit = pos + dirVecPLZNormalizeMeFirst * rh.t;
rh.norm = normalize(rh.hit - spherePos);  
return rh;
}vec3 ElementwiseEqual(in vec3 a, in float b) {
return abs(sign(a - b));
}
RayHit BoxIntersect(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 boxPos, vec3 rad, int outside)
{
)";

const char* vertexShader = R"(#version 300 es
attribute vec2 texCoord;
void main()
{
    gl_Position = vec4(texCoord, 0.0, 1.0 );
})";

/// <summary>
/// helper function to load 6 images and arrange them into a cubemap
/// </summary>
static constexpr int CUBE_SIDES_NUM = 6;
GLuint load_cubemap_from_file(std::string faces[], std::string& env_path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int window_width, window_height, nrChannels;
	for (unsigned int i = 0; i < CUBE_SIDES_NUM; i++)
	{
		unsigned char* data = stbi_load((env_path + faces[i]).c_str(), &window_width, &window_height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
			std::cout << "Cubemap tex successfuly loaded: " << faces[i] << std::endl;

		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}

//create cubemaps
void Renderer::init_cubemaps() {
	std::cout << "Loading cubemaps" << std::endl;

	current_cubemap = 0;
	std::string storage_folder_dir = "http://localhost:8000/data/env/";
	std::string files[2][6]{
		{"back.jpg",   "front.jpg",   "top.jpg",   "bottom.jpg",   "right.jpg",   "left.jpg"},
		{"back1.jpg",  "front1.jpg",  "top1.jpg",  "bottom1.jpg",  "right1.jpg",  "left1.jpg"}
	};
	for (size_t i = 0; i < CUBEMAP_COUNT; i++)
	{
		cubemaps[i] = load_cubemap_from_file(files[i], storage_folder_dir);
	}
}

//toggle between cubemap backgrounds
void Renderer::change_cubemap() {
	current_cubemap = (current_cubemap + 1) % CUBEMAP_COUNT;
}

//check if shaders compiled successfuly
void checkShaderStatus(GLuint obj)
{
	#define DEBUG
	#ifdef DEBUG
	GLint status = GL_FALSE;
	if (glIsShader(obj)) glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (glIsProgram(obj)) glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_TRUE) return;
	GLchar log[1 << 16] = { 0 };
	if (glIsShader(obj)) glGetShaderInfoLog(obj, sizeof(log), NULL, log);
	if (glIsProgram(obj)) glGetProgramInfoLog(obj, sizeof(log), NULL, log);
	std::cerr << log << std::endl;
	//exit(-1);
	#endif
}

//create a shader from string
void load_shader(const char* shaderPath, GLenum shaderType, GLuint program)
{
	FILE *file = fopen(shaderPath, "r");
	if (!file) {
    	std::cerr <<"cannot open file" << std::endl;
    	return;
  	}else{
		std::cout <<"can open file" << std::endl;
	}
	// std::ifstream file(shaderPath);
	// std::stringstream buffer;
	// buffer << file.rdbuf();
	// std::string frag = buffer.str();
	// //std::cout << "Shader code: " << shaderCode <<std::endl;
	// GLuint shader = glCreateShader(shaderType);
	// glShaderSource(shader, 1, &shaderCode, NULL);
	// glCompileShader(shader);
	// checkShaderStatus(shader);
	// glAttachShader(program, shader);
	// glDeleteShader(shader);
	// std::cout << "Loaded shader " << shaderType << std::endl;
}

//clear accumulated image
void Renderer::clear_render() {
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glClear(GL_COLOR_BUFFER_BIT);

	}
}

//main render loop 
void Renderer::render() {

	int i = frame_num & 1;
	int ii = i ^ 1;
	glClearColor(0, 0, 0, 1);
	//first, draw to a framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);

	//use previous frame render result as accumulation buffer
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, render_textures[ii]);
	glActiveTexture(GL_TEXTURE0);

	//bind current cubemap
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps[current_cubemap]);
	glUniform1i(iChannel1_location, 2);

	//bind shaders and render a full screen triangle
	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//draw contents of a framebuffer to glfw window buffer (buffer 0)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBOs[0]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//wait for end of frame
	glFinish();
	frame_num++;
}


void Renderer::Set_iMouse(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(iMouse_location, x, y, z, w);
	clear_render();
}


void Renderer::Set_iEnv(GLint env) {
	glUniform1i(iEnv_location, env);
	clear_render();
}


void Renderer::Set_iTime(GLfloat t) {
	glUniform1f(iTime_location, t);
}

Renderer::Renderer(int w, int h) {
	window_width = w;
	window_height = h;
	program = glCreateProgram();

	//TODO RESOLVE BUGS WITH glEnable with __EMSCRIPTEN__
	//enable necessary features
	//glEnable(GL_TEXTURE_CUBE_MAP);
	//glEnable(GL_TEXTURE_2D);

	//create FBOs
	glGenFramebuffers(FBOCOUNT, FBOs);
	glGenTextures(FBOCOUNT, render_textures);
	for (size_t i = 0; i < FBOCOUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);
		glBindTexture(GL_TEXTURE_2D, render_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_textures[i], 0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//load a simple vertex shader
	load_shader(vertexShader, GL_VERTEX_SHADER, program);

	//load text file containing fragment shader code
	//source https://www.shadertoy.com/view/4ddcRn
	load_shader(fragmentShader, GL_FRAGMENT_SHADER, program);
	glLinkProgram(program);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint vertex_buffer = 0;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	float data[] = //full screen triangle, FS tri > FS quad for complex fragment shaders
	{
		-1.0f,-1.0f,
		3.0f, -1.0f,
		-1.0f,3.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	GLuint index_buffer = 0;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	unsigned int indexes[] =
	{
		0,1,2
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	iResolution_location = glGetUniformLocation(program, "iResolution");
	iTime_location = glGetUniformLocation(program, "iTime");
	iMouse_location = glGetUniformLocation(program, "iMouse");
	iChannel0_location = glGetUniformLocation(program, "iChannel0");
	iChannel1_location = glGetUniformLocation(program, "iChannel1");
	iEnv_location = glGetUniformLocation(program, "iEnv");

	init_cubemaps();

	glUniform3f(iResolution_location, window_width, window_height, 0);
	glUniform4f(iMouse_location, 0, 0, 0, 0);
	glUniform1i(iChannel0_location, 1);

	glViewport(0, 0, window_width, window_height);
	frame_num = 0;
	std::cout << "Renderer created\n";
}
