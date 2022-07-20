#version 300 es
precision highp float; precision highp int;
/*--------------------------------------------------------------------------------------
License CC0 - http://creativecommons.org/publicdomain/zero/1.0/
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
----------------------------------------------------------------------------------------
^This means do anything you want with this code. Because we are programmers, not lawyers.

I tried to make a physically correct renderer as much as possible. For materials, it supports:
- Proper reflection / refraction.
- Filtered light rays like in the red glass.
- Gold has rgb reflectance.
- Emission.
- Diffuse / specular.
- Sub surface scattering, like in water.
- Scattering in air or whereever for fog.

- Support for ray traced and ray marched geometry in the same scene.
- Antialiasing.

-Otavio Good
*/

uniform sampler2D iChannel0;             // input channel. XX = 2D/Cube
uniform samplerCube iChannel1;           // input channel. XX = 2D/Cube
uniform ivec2 iResolution;     // viewport resolution (in pixels)
uniform vec2 iMouse;                // mouse pixel coords. xy: current (if MLB down)
uniform float iTime;                 // shader playback time (in seconds)
uniform int iEnv;
uniform float iFramesRcp; // 1/(current frame number)


// Number of samples per pixel - bigger takes more compute
#define NUM_SAMPLES 4
// Number of times the ray bounces off things before terminating
#define NUM_ITERS 7

// ---- general helper functions / constants ----
#define saturate(a) clamp(a, 0.0, 1.0)
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
}
// Find 2 perpendicular vectors to the input vector.
mat3 MakeBasis(vec3 normal) {
	mat3 result;
    result[0] = normal;
    if (abs(normal.y) > 0.5) {
        result[1] = normalize(cross(normal, vec3(1.0, 0.0, 0.0)));
    } else {
        result[1] = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    }
    result[2] = normalize(cross(normal, result[1]));
    return result;
}


// ---- Hash functions ----
// This is the single state variable for the random number generator.
uint randomState = uint(123456789);

// ---- Hash functions - deterministic ----
float Hashf1i1(uint seed) {
    // polynomial hashing thing
    uint s2 = seed * seed;
    uint s3 = s2 * seed;

    uint a = (s2 * uint(60449) + uint(30011)) % uint(101159);
    uint b = s3 * uint(4111) % uint(104729);
    uint z = a ^ b;
    return float(z & uint(0xffff)) / float(0xffff);  // Why this constant?
}
vec2 Hashf2i1(uint n) {
    float a = Hashf1i1(n+uint(3));
    float b = Hashf1i1(n+uint(11));
    return vec2(a, b);
}
vec3 Hashf3i1(uint n) {
    float a = Hashf1i1(n+uint(3));
    float b = Hashf1i1(n+uint(11));
    float c = Hashf1i1(n+uint(17));
    return vec3(a, b, c);
}
// ---- Random functions use one state var to change things up ----
uint GetRand() {
    // simple xor shift random number generator https://en.wikipedia.org/wiki/Xorshift
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;
    return randomState;
}
// Combine random state with integer polynomial hash function to get a random [0..1]
float Randf1i1(uint seed) {
    // simple xor shift random number generator https://en.wikipedia.org/wiki/Xorshift
    randomState ^= randomState << 13;
    randomState ^= randomState >> 17;
    randomState ^= randomState << 5;

    // polynomial hashing thing
    uint s2 = seed * seed;
    uint s3 = s2 * seed;

    uint a = (s2 * uint(60449) + uint(30011)) % uint(101159);
    uint b = s3 * uint(4111) % uint(104729);
    uint z = a ^ b ^ randomState;
    return float(z & uint(0xffff)) / float(0xffff);
}
vec3 Randf3i1(uint n) {
    float a = Randf1i1(n);
    float b = Randf1i1(n + uint(7));
    float c = Randf1i1(n + uint(13));
    return vec3(a, b, c);
}
vec2 Randf2i1(uint n) {
    float a = Randf1i1(n + uint(3));
    float b = Randf1i1(n + uint(11));
    return vec2(a, b);
}
// Returns random number sampled from a circular gaussian distribution
// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
vec2 RandGaussian(uint n) {
    vec2 u = Randf2i1(n);
    u.x = max(u.x, 0.00000003); // We don't want log() to fail because it's 0.
    float a = sqrt(-2.0 * log(u.x));
    return vec2(a * cos(2.0*PI*u.y), a * sin(2.0 * PI * u.y));
}
// Randomly sample the lifetime of a ray moving through particles
float RayLifetime(float p, uint seed) {
    // p = prob. of collision with particle per unit distance traveled
    if (p < 0.00000003) return farPlane;  // Lower than this and the math seems to fail.
    float unif = Randf1i1(seed);  // uniform(0, 1)
    // This random shouldn't be allowed to hit 0 because log() is undefined.
    unif = max(0.00000000000001, unif);
    // p can't be 0 because log(1) == 0 and it divides by 0. Can't be 1 because log(0) is undefined.
    p = min(p, .999999);
    //float g = math.floor(math.log(unif) / math.log(1 - p))  # natural logs
    // g = number of successes before failure
    float g = log(unif) / log(1.0 - p);
    return g;
}


// ---- noise functions ----
float Hash1d(float u)
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
// Random point *ON* sphere
// Not sure which is faster - this function, or normalizing RandPointInSphere().
vec3 RandPointOnSphere(uint seed) {
    vec2 uv = Randf2i1(seed);
    float theta = 2.0 * PI * uv.x;
    float psi = acos(2.0 * uv.y - 1.0);
    float x = cos(theta) * sin(psi);
    float y = sin(theta) * sin(psi);
    float z = cos(psi);
    return vec3(x, y, z);
}
// Random point *IN* sphere
// This is biased!!! About 1/32 of the time, it will return a point in box instead of a sphere.
vec3 RandPointInSphere(uint seed) {
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
}

// ---- Environment maps ----
// Make a procedural environment map with a giant softbox light and 4 lights around the sides.
vec3 GetEnvMap2(vec3 rayDir)
{
    // fade bottom to top so it looks like the softbox is casting light on a floor
    // and it's bouncing back
    vec3 final = vec3(1.0) * dot(rayDir, vec3(0.0, 1.0, 0.0)) * 0.5 + 0.5;
    final *= 0.125;
    // overhead softbox, stretched to a rectangle
    if ((rayDir.y > abs(rayDir.x)*1.0) && (rayDir.y > abs(rayDir.z*0.25))) final = vec3(2.0)*rayDir.y;
    // fade the softbox at the edges with a rounded rectangle.
    float roundBox = length(max(abs(rayDir.xz/max(0.001,rayDir.y))-vec2(0.9, 4.0),0.0))-0.1;
    final += vec3(0.8)* pow(saturate(1.0 - roundBox*0.5), 6.0);
    // purple lights from side
    final += vec3(8.0,6.0,7.0) * saturate(0.001/(1.01 - abs(rayDir.x)));
    // yellow lights from side
    final += vec3(8.0,7.0,6.0) * saturate(0.001/(1.01 - abs(rayDir.z)));
    return final;
}

// Courtyard environment map texture with extra sky brightness for HDR look.
// Commented out due to no cubemaps for this example
vec3 GetEnvMap(vec3 rayDir) {
    vec3 tex = texture(iChannel1, rayDir).xyz;
    tex = tex * tex;  // gamma correct
    vec3 light = vec3(0.0);
    // overhead softbox, stretched to a rectangle
    if ((rayDir.y > abs(rayDir.x+0.6)*0.29) && (rayDir.y > abs(rayDir.z*2.5))) light = vec3(2.0)*rayDir.y;
    vec3 texp = pow(tex, vec3(14.0));
    light *= texp;  // Masked into the existing texture's sky
    return (tex + light*3.0)*1.0;
}

vec3 sunDir = normalize(vec3(0.93, 1.0, 1.0));
const vec3 sunCol = vec3(250.0, 220.0, 200.0) / 155.0;
const vec3 horizonCol = vec3(0.95, 0.9, 0.85)*1.0;
const vec3 skyCol = vec3(0.03,0.46,1.0)*0.9;
const vec3 groundCol = vec3(0.8,0.7,0.55)*1.1;
// This function basically is a procedural environment map that makes the sun
vec3 GetSunColorSmall(vec3 rayDir)
{
	vec3 localRay = normalize(rayDir);
	float dist = 1.0 - (dot(localRay, sunDir) * 0.5 + 0.5);
	float sunIntensity = 0.05 / dist;
    sunIntensity += exp(-dist*150.0)*7000.0;
	sunIntensity = min(sunIntensity, 40000.0);
	return sunCol * sunIntensity*0.002;
}

// Procedural desert environment map
vec3 GetEnvMap3(vec3 rayDir)
{
    // fade the sky color, multiply sunset dimming
    vec3 finalColor = mix(horizonCol, skyCol, pow(saturate(rayDir.y), 0.25))*0.95;
    // make clouds - just a horizontal plane with noise
    float n = noise2d(rayDir.xz/rayDir.y*1.0);
    n += noise2d(rayDir.xz/rayDir.y*2.0)*0.5;
    n += noise2d(rayDir.xz/rayDir.y*4.0)*0.25;
    n += noise2d(rayDir.xz/rayDir.y*8.0)*0.125;
    float nc = pow(abs(n), 3.0);
    nc = nc * saturate(abs(rayDir.y * 4.0));  // fade clouds in distance
	float dist = 1.0 - (dot(rayDir, sunDir) * 0.5 + 0.5);
    vec3 cloudCol = (vec3(1.0,0.6,0.2)/dist*1.05+sunCol*1.0)*0.75*saturate((rayDir.y+0.2)*5.0);
    finalColor = mix(finalColor, cloudCol, saturate(nc*0.0625));

    // Background mountains
    float n2 = noise2d(rayDir.xz * 8.0)*1.7;
    n2 += noise2d(rayDir.xz * 16.0) * 0.5;
    n2 += noise2d(rayDir.xz * 32.0) * 0.25;
    n2 += noise2d(rayDir.xz * 64.0) * 0.125;
    n2 += 1.5;
    vec3 mountainCol = (horizonCol * 0.75 + skyCol * 0.25) * 0.6 - rayDir.y*2.0;
    finalColor = mix(finalColor, mountainCol, saturate((n2*0.025-rayDir.y)*256.0));

    // Foreground mountains
    n2 = noise2d(-rayDir.xz * 8.0)*1.7;
    n2 += noise2d(-rayDir.xz * 16.0) * 0.5;
    n2 += noise2d(-rayDir.xz * 32.0) * 0.25;
    n2 += noise2d(-rayDir.xz * 64.0) * 0.125;
    n2 += 1.0;
    mountainCol = (horizonCol * 0.95 + skyCol * 0.05) * 0.4;
    finalColor = mix(finalColor, mountainCol, saturate((n2*0.0125-rayDir.y)*256.0));

    // Ground color fade
    n += noise2d(rayDir.xz/rayDir.y*16.0)*0.35;
    vec3 groundTex = groundCol - (n-1.25)*0.15;
    finalColor = mix(finalColor, (groundTex)*0.9, saturate(-rayDir.y*164.0));

    // add the sun
    finalColor += GetSunColorSmall(rayDir);
    return finalColor;
}
vec3 GetEnvMap4(vec3 rayDir)
{   
    return vec3(1.0,1.0,1.0)*(rayDir.z*0.5+0.5);
}
// ---- Ray intersection functions and data structures ----
struct RayHit
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
    int outside;  // 1 ray is outside, -1 ray is inside, 0 terminate ray tracing iteration
};
struct SceneHit
{
    vec3 hitPos;
    vec3 hitNormal;
    float t;
    int objIndex;
    int materialIndex;
};

// As the ray bounces, it records hits in these vars.
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

// dirVec MUST BE NORMALIZED FIRST!!!!
float SphereIntersect(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 spherePos, float rad)
{
    vec3 radialVec = pos - spherePos;
    float b = dot(radialVec, dirVecPLZNormalizeMeFirst);
    float c = dot(radialVec, radialVec) - rad * rad;
    float h = b * b - c;
    if (h < 0.0) return -1.0;
    return -b - sqrt(h);
}

// outside is 1 to intersect from the outside of the sphere, -1 to intersect from inside of sphere.
RayHit SphereIntersect3(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 spherePos, float rad, int outside)
{
    RayHit rh = NewRayHit();
    vec3 delta = spherePos - pos;
    float projdist = dot(delta, dirVecPLZNormalizeMeFirst);
    vec3 proj = dirVecPLZNormalizeMeFirst * projdist;
    vec3 bv = proj - delta;
    float b2 = dot(bv, bv);
    if (b2 > rad*rad) return rh;  // Ray missed the sphere
    float x = sqrt(rad*rad - b2);
    rh.t = projdist - (x * float(outside));
    rh.hit = pos + dirVecPLZNormalizeMeFirst * rh.t;
    rh.norm = normalize(rh.hit - spherePos);  // Normal still points outwards if collision from inside.
    return rh;
}

vec3 ElementwiseEqual(in vec3 a, in float b) {
	return abs(sign(a - b));
}

// https://tavianator.com/fast-branchless-raybounding-box-intersections/
RayHit BoxIntersect(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 boxPos, vec3 rad, int outside)
{
    vec3 bmin = boxPos - rad;
    vec3 bmax = boxPos + rad;
    vec3 rayInv = 1.0 / dirVecPLZNormalizeMeFirst;

    vec3 t1 = (bmin - pos) * rayInv;
    vec3 t2 = (bmax - pos) * rayInv;

    vec3 vmin = min(t1, t2);
    vec3 vmax = max(t1, t2);

    float tmin = max(vmin.z, max(vmin.x, vmin.y));
    float tmax = min(vmax.z, min(vmax.x, vmax.y));
    if (outside == -1) {
        float temp = tmin;
        tmin = tmax;
        tmax = temp;
    }

    RayHit rh = NewRayHit();
    //rh.tMin = bignum;
    //rh.tMax = bignum;
    if ((tmax <= tmin) || (tmin <= 0.0)) return rh;
    rh.t = tmin;
    //rh.tMax = tmax;
    rh.hit = pos + dirVecPLZNormalizeMeFirst * rh.t;
    //rh.hitMax = pos + dirVecPLZNormalizeMeFirst * rh.tMax;
    // optimize me!
    if (t1.x == tmin) rh.norm = vec3(-1.0, 0.0, 0.0);
    if (t2.x == tmin) rh.norm = vec3(1.0, 0.0, 0.0);
    if (t1.y == tmin) rh.norm = vec3(0.0, -1.0, 0.0);
    if (t2.y == tmin) rh.norm = vec3(0.0, 1.0, 0.0);
    if (t1.z == tmin) rh.norm = vec3(0.0, 0.0, -1.0);
    if (t2.z == tmin) rh.norm = vec3(0.0, 0.0, 1.0);
    //rh.norm = ElementwiseEqual(t1, tmin) - ElementwiseEqual(t2, tmin);
    // optimize me!
    //if (t1.x == tmax) rh.normMax = -vec3(-1.0, 0.0, 0.0);
    //if (t2.x == tmax) rh.normMax = -vec3(1.0, 0.0, 0.0);
    //if (t1.y == tmax) rh.normMax = -vec3(0.0, -1.0, 0.0);
    //if (t2.y == tmax) rh.normMax = -vec3(0.0, 1.0, 0.0);
    //if (t1.z == tmax) rh.normMax = -vec3(0.0, 0.0, -1.0);
    //if (t2.z == tmax) rh.normMax = -vec3(0.0, 0.0, 1.0);
    //rh.normMax = ElementwiseEqual(t1, tmax) - ElementwiseEqual(t2, tmax);
    //rh.norm *= float(outside);
    //rh.norm = normalize(rh.norm);
    //rh.normMax = ElementwiseEqual(t1, tmax) - ElementwiseEqual(t2, tmax);
    //rh.material = material;
    return rh;
}

// ---- Scattering vars ----
// Scattering needs a stack because it's all about what you're moving through.
// So if you move through fog and then through glass, when you come out of the glass,
// you're back into the fog.

// This is the scatter var for the outer-most place. RGB diffuse, density.
vec4 globalScatter = vec4(0.8, 0.8, 0.8, 0.0);
// Scatter stack
vec4 scatterStack[NUM_ITERS*2];  // Size correct?
int scatterStackIndex;
void PushScatter(in vec4 s) {
    scatterStack[scatterStackIndex] = s;
    scatterStackIndex++;
}
vec4 PopScatter() {
    scatterStackIndex--;
    return scatterStack[scatterStackIndex];
}
vec4 PeekScatter() {
    return scatterStack[scatterStackIndex-1];
}
void InitScatterStack(in vec4 s) {
    scatterStackIndex = 0;
    PushScatter(s);
}

// ---- Materials ----
// Linear reflectance values from http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
// In w, refrective index - https://en.wikipedia.org/wiki/List_of_refractive_indices
// These can be calculated from each other: https://en.wikipedia.org/wiki/Schlick%27s_approximation
const vec4 refDebug = vec4(1.0,1.0,1.0, 1.005);
const vec4 refNone = vec4(0.0,0.0,0.0, 1.0);
const vec4 refWater = vec4(0.02,0.02,0.02, 1.330);
const vec4 refPlasticGlassLow = vec4(0.03,0.03,0.03, 1.42);
const vec4 refGlassWindow = vec4(0.043,0.043,0.043,1.52);
const vec4 refPlasticHigh = vec4(0.05,0.05,0.05, 1.58);
const vec4 refGlassHigh = vec4(0.08,0.08,0.08,1.79);  // Ruby
const vec4 refDiamond = vec4(0.172,0.172,0.172,2.417);
// Metals - refractive index is placeholder and shouldn't be used I guess
const vec4 refIron = vec4(0.56,0.57,0.58,1.0);
const vec4 refCopper = vec4(0.95,0.64,0.54,1.0);
const vec4 refGold = vec4(1.0, 0.71, 0.29,1.0);
const vec4 refAluminum = vec4(0.91,0.92,0.92,1.0);
const vec4 refSilver = vec4(0.95,0.93,0.88,1.0);

struct Material {
	vec4 reflectRefract;
    float refMult;
    bool doRefraction;
    vec3 diffuse;
    vec3 emission;
    vec3 filterDiffuseDensity;  // This is like a cheap scatter. Not sure I like it.
    vec4 scatter;  // Diffuse in xyz, scatter probability in w.
};
const int defaultMaterialIndex = 3;
const int NUM_MATERIALS = 7;
Material materials[NUM_MATERIALS] = Material[NUM_MATERIALS](
  Material(refWater, 1.0, true, vec3(1.0), vec3(0.0), vec3(1.0), vec4(0.3, 0.6, 0.6, 0.4)),
  Material(refGlassWindow, 1.0, true, vec3(0.0), vec3(0.0), vec3(0.99, 0.2, 0.05), vec4(0.0)),
  Material(refGold, 1.0, false, vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0)),
  Material(refPlasticGlassLow, 1.0, false, vec3(0.1, 0.4, 0.6), vec3(0.0), vec3(0.0), vec4(0.0)),
  Material(refPlasticGlassLow, 0.0, false, vec3(1.0), vec3(0.0), vec3(0.0), vec4(0.0)),
  //Material(refPlasticGlassLow, 1.0, false, vec3(1.0), vec3(0.0), vec3(0.0), vec4(0.0)),
  Material(refNone, 1.0, false, vec3(0.0), vec3(14.0, 8.0, 3.0)*2.4, vec3(0.0), vec4(0.0)),
  Material(refAluminum, 1.0, false, vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0))
);

// ---- Intersect the ray with the scene, ray-trace style ----
SceneHit SceneIntersect(const in Ray ray) {
    SceneHit result;
    result.hitNormal = vec3(0.0);
    result.t = farPlane;
    result.objIndex = BIG_INT;
    result.materialIndex = defaultMaterialIndex;
    vec4 sA = vec4(0.0, 0.0, 0.75, 1.5);
    for (int i = 0; i < 8; i++) {
        sA.xyz = (Hashf3i1(uint(i+5)) - 0.5) * 12.0;
        sA.y *= 0.5;
        sA.y += 2.5;
        //sA.xyz += Randf1i1(uint(float(i)+iTime*60.0)) * sA.xyz * 0.93;
        sA.w = (Hashf1i1(uint(i+23)) + 0.75) * 0.8251;
        float tall = 0.25;
        //if ((i & 7) == 0) {
        //    sA.y += 2.0;
        //    sA.w *= 0.152;
        //    tall = 8.0*Hashf1i1(uint(i+23));
        //}
        RayHit rh = SphereIntersect3(ray.p0, ray.dirNormalized, sA.xyz, sA.w, ray.outside);
		//RayHit rh = BoxIntersect(ray.p0, ray.dirNormalized, sA.xyz, vec3(sA.w, sA.w*tall, sA.w), ray.outside);
        if ((rh.t > 0.0) && (rh.t < result.t)) {
            result.objIndex = i+27;//3;// i&3;
		    result.materialIndex = (i&1)*4+1;
            //if ((i & 7) == 0) result.objIndex =4 + ((i/8)&1);
            result.t = rh.t;
            result.hitPos = rh.hit;
            result.hitNormal = rh.norm;
        }
    }
    for (int i = 0; i < NUM_MATERIALS; i++) {
        float alpha = float(i) / float(NUM_MATERIALS);
        sA.xyz = vec3(sin(alpha * PI * 2.0), 0.0, cos(alpha * PI * 2.0)) * 5.05;
        sA.y = -3.5;
        sA.w = 1.0;
        RayHit rh = SphereIntersect3(ray.p0, ray.dirNormalized, sA.xyz, sA.w, ray.outside);
        if ((rh.t > 0.0) && (rh.t < result.t)) {
            result.objIndex = i;
		    result.materialIndex = i+7;
            result.t = rh.t;
            result.hitPos = rh.hit;
            result.hitNormal = rh.norm;
        }
    }
    /*RayHit rh = SphereIntersect3(ray.p0, ray.dirNormalized, vec3(0.0, -2.75, 0.0), 0.5, ray.outside);
    if ((rh.t > 0.0) && (rh.t < result.t)) {
        result.objIndex = 0;
        result.materialIndex = 0;
        result.t = rh.t;
        result.hitPos = rh.hit;
        result.hitNormal = rh.norm;
    }*/

    return result;
}

// ---- Also support ray marching, not just ray tracing. ----
// k should be negative. -4.0 works nicely.
// smooth blending function
float smin(float a, float b, float k)
{
	return log2(exp2(k*a)+exp2(k*b))/k;
}
// min function that supports materials in the y component
vec2 matmin(vec2 a, vec2 b)
{
    if (a.x < b.x) return a;
    else return b;
}
vec2 matmax(vec2 a, vec2 b)
{
    if (a.x > b.x) return a;
    else return b;
}
// signed box distance field
float sdBox(vec3 p, vec3 radius)
{
  vec3 dist = abs(p) - radius;
  return min(max(dist.x, max(dist.y, dist.z)), 0.0) + length(max(dist, 0.0));
}

// Noise generator from https://otaviogood.github.io/noisegen/
// Params: 2D, Seed 1, Waves 6, Octaves 3, Smooth 1.25
float NoiseGen(vec2 p) {
    // This is a bit faster if we use 2 accumulators instead of 1.
    // Timed on Linux/Chrome/TitanX Pascal
    float wave0 = 0.0;
    float wave1 = 0.0;
    wave0 += sin(dot(p, vec2(-1.699, -1.719))) * 0.2639797486;
    wave1 += sin(dot(p, vec2(3.196, -0.147))) * 0.1524620373;
    wave0 += sin(dot(p, vec2(3.654, 3.161))) * 0.0761552832;
    wave1 += sin(dot(p, vec2(1.047, 4.757))) * 0.0751807404;
    wave0 += sin(dot(p, vec2(-4.889, 0.548))) * 0.0740187224;
    wave1 += sin(dot(p, vec2(-5.403, -4.576))) * 0.0427534381;
    return wave0+wave1;
}

// This is the distance function that defines the ray marched scene's geometry.
// The input is a position in space.
// outside is 1 if the ray is intersecting with the outside of objects, -1 for insides (backface)
// The output is the distance to the nearest surface, and a material index
vec2 DistanceToObject(vec3 p, int outside)
{
    //float dist = p.y;
    //dist = length(p) - 1.4;
    //dist = smin(dist, length(p + vec3(2.25, -4.0, -4.0)) - 2.95, -0.95);
    float dist = 1000000.0;
    float noise = NoiseGen(p.xz*2.0)*0.1;

    vec2 water = vec2(dist, 5.0);
    //water = matmin(water, vec2(length(p.xz + vec2(7.0, -5.0))-0.5, 3.0));
    float cyl = length(vec2(p.y,abs(p.x)) + vec2(-5.5, -6.0)) - 0.5;
    cyl = max(cyl, abs(p.z)-18.0);
    //water = matmin(water, vec2(cyl, 6.0));
    float rad = length(p.xz)*4.0;
    // Make water radial waves (computationally expensive)
    noise = noise*0.4 -sin(rad)/rad;
    float waterBox = sdBox(p + vec3(0.0, 5.1+noise, 0.0), vec3(6.0, 0.5, 6.5));
    waterBox = smin(waterBox, length(p+vec3(0.0,2.95,0.0))-0.6, -3.95);
    water = matmin(water, vec2(waterBox, 0.0));
    float pool = sdBox(p + vec3(3.0, 5., 0.0), vec3(3.0, 0.5, 6.5));
    //pool = max(pool, -(length(p.xz) - 4.0));
    //water = matmin(water, vec2(pool, 5.0));
    //water = matmin(water, vec2(sdBox(p + vec3(1.0, -6.1, 0.0), vec3(0.6, 0.1, 8.04)), 6.0));
    //water = matmin(water, vec2(sdBox(p + vec3(1.0, -6.15, 0.0), vec3(0.8, 0.1, 8.4)), 3.0));
    
    float room = -sdBox(p + vec3(0.0, 0.0, 0.0), vec3(18.0, 18.5, 18.0));
    //water = matmin(water, vec2(room, 2.0));

    //water = matmin(water, vec2(length(p+vec3(0.0,2.5,0.0))-0.5, 1.0));
    //water = matmin(water, vec2(length(p+vec3(2.0,3.6,6.0))-1.0, 3.0));

    vec3 p2 = RotateX(p, -0.5) + vec3(0.0, -0.0, 0.0);
    float d = sdBox(p2, vec3(4.0,2.0,4.0));
    d = max(d, p2.y+(p2.x * p2.x + p2.z*p2.z) * 0.3);
    //water = matmin(water, vec2(d, 2.0));

    return water * vec2(float(outside), 1.0);
}

SceneHit SceneMarch(const in Ray ray) {
    SceneHit result;
    result.hitNormal = vec3(0.0);
    //result.t = farPlane;
    result.objIndex = BIG_INT;
    result.materialIndex = defaultMaterialIndex;
    vec2 distAndMat = vec2(0.0, -1.0);  // Distance and material
	// ----------------------------- Ray march the scene ------------------------------
	const float maxDepth = 48.0; // farthest distance rays will travel
    const float smallVal = 0.0625*0.125;
	result.t = 0.0;// smallVal*1.0 * float(ray.outside);
    const float safety = 0.975;
    // First, escape if we are touching a surface already.
    // Get the ray out of the negative part of the distance field. (rough hack)
    float jump = smallVal;
    for (int i = 0; i < 16; i++) {
        // Step along the ray.
        result.hitPos = (ray.p0 + ray.dirNormalized * result.t);
        distAndMat = DistanceToObject(result.hitPos, ray.outside);

        if (abs(distAndMat.x) >= smallVal) break;
        // move down the ray a safe amount
        result.t += jump;//safety;//* float(ray.outside);
        //result.t += distAndMat.x*2.0;//safety;//* float(ray.outside);
        jump *= 2.0;  // This is not super good. Fix me eventually.
        if (result.t > maxDepth) break;
    }
	// ray marching time
    for (int i = 110; i >= 0; i--)	// This is the count of the max times the ray actually marches.
    {
        // Step along the ray.
        result.hitPos = (ray.p0 + ray.dirNormalized * result.t);
        // This is _the_ function that defines the "distance field".
        // It's really what makes the scene geometry. The idea is that the
        // distance field returns the distance to the closest object, and then
        // we know we are safe to "march" along the ray by that much distance
        // without hitting anything. We repeat this until we get really close
        // and then break because we have effectively hit the object.
        distAndMat = DistanceToObject(result.hitPos, ray.outside);

        // If we are very close to the object, let's call it a hit and exit this loop.
        if (abs(distAndMat.x) < smallVal) break;
        // move down the ray a safe amount
        result.t += distAndMat.x*safety;
        if (i == 0) result.t = maxDepth+0.01;
        if (result.t > maxDepth) break;
    }

	// --------------------------------------------------------------------------------
	// If a ray hit an object, calculate the normal and save the hit info.
    if ((result.t <= maxDepth) && (result.t > 0.0))
	{
        float dist = distAndMat.x;
        // calculate the normal from the distance field. The distance field is a volume, so if you
        // sample the current point and neighboring points, you can use the difference to get
        // the normal.
        vec3 smallVec = vec3(smallVal, 0, 0);
        // Normals still point out even if we're on the inside.
        float mid = DistanceToObject(result.hitPos, 1).x;
        vec3 normalU = vec3(mid - DistanceToObject(result.hitPos - smallVec.xyy, 1).x,
                           mid - DistanceToObject(result.hitPos - smallVec.yxy, 1).x,
                           mid - DistanceToObject(result.hitPos - smallVec.yyx, 1).x);
        result.hitNormal = normalize(normalU);
        result.objIndex = int(distAndMat.y);
        result.materialIndex = int(distAndMat.y);
	}
    else
    {
        // Our ray trace hit nothing. Set object index to big int.
        result.hitNormal = vec3(0.0);
        result.t = farPlane;
        result.objIndex = BIG_INT;
        result.materialIndex = BIG_INT;
    }
    return result;
}

// ---- Main ray trace function ----
// Trace a ray, hit, apply material, save hit, and return the bounced ray
Ray TraceOneRay(const in Ray ray) {
    vec3 diffuse = vec3(1.0);
    vec3 emission = vec3(0.0);

    // Send a ray out into the scene. Combine both ray marching and ray tracing.
    Ray newRay;
    newRay.hash = ray.hash + 389;
    newRay.outside = ray.outside;
    SceneHit resultA = SceneIntersect(ray);
    SceneHit resultB = SceneMarch(ray);
    SceneHit result;
    // Take closest hit.
    if (resultA.t < resultB.t) {
        result = resultA;
    } else {
        result = resultB;
    }

    vec4 currentScatter = PeekScatter();
    Material material = materials[result.materialIndex % materials.length()];
    // Calculate how far the ray goes before hitting a random scattering particle.
    float lifetime = RayLifetime(currentScatter.w, uint(ray.hash + 257));
    if (result.t < min(farPlane, lifetime)) {

        // Debug normal visualization
        //emission = result.hitNormal * 0.5 + 0.5;
        //newRay.outside = 0;
        //SaveHit(diffuse, emission);
        //return newRay;

        //mat3 basis = MakeBasis(result.hitNormal);
        newRay.p0 = result.hitPos;

        vec4 refMaterial = material.reflectRefract;
        float reflectance = material.refMult;
        float fresnel = refMaterial.z;
        // If reflectance is different for different wavelengths, then let's change from
        // rgb to r, g, or b stochastically while not losing energy. So we will end up
        // tracing a ray representing a single wavelength of light.
        // This can be done unconditionally because if xyz are the same, it doesn't matter anyway.
        float choice = Randf1i1(uint(ray.hash+257));
        if ((refMaterial.x != refMaterial.y) || (refMaterial.y != refMaterial.z)) {
            // If we have already split to single wavelength, don't do it again.
            if ( ((refMaterial.x == 0.0) && (refMaterial.y == 0.0)) ||
                ((refMaterial.y == 0.0) && (refMaterial.z == 0.0)) ||
                ((refMaterial.z == 0.0) && (refMaterial.x == 0.0)) ) {
                // Take the non-zero component as the fresnel value.
                fresnel = dot(refMaterial.xyz, vec3(1.0));
            } else {
                // .333 chance of switching to each single channel - r, g, or b.
                if (choice < 0.33333) {
                    fresnel = refMaterial.x;
                    diffuse *= vec3(1.0, 0.0, 0.0);
                } else if (choice < 0.66666) {
                    fresnel = refMaterial.y;
                    diffuse *= vec3(0.0, 1.0, 0.0);
                } else diffuse *= vec3(0.0, 0.0, 1.0);
                diffuse *= 3.0;  // To make up for stochastically dropping 2 out of 3 channels
            }
        }
        // Schlick's approximation
        float oneMinusCos = 1.0 - saturate(dot(ray.dirNormalized, -result.hitNormal* float(ray.outside)));
        float reflectProb = fresnel + (1.0-fresnel) * pow(oneMinusCos, 5.0);
        reflectProb *= reflectance;
        if (Randf1i1(uint(abs(ray.hash))) < reflectProb) {
            // reflect
	        vec3 reflection = reflect(ray.dirNormalized, result.hitNormal);// * float(ray.outside));
            newRay.dirNormalized = normalize(reflection);
            // Already did the probability of reflection before, so no need to multiply anything.
            //diffuse *= vec3(1.0);
        } else {
            if (material.doRefraction) {
                // refract
                // todo: check total internal reflection
                float refractionIndex = 1.0 / refMaterial.w;  // 1.33 is water, 1.5 is glass.
                if (ray.outside == -1) refractionIndex = 1.0 / refractionIndex;
                vec3 refraction = refract(ray.dirNormalized, result.hitNormal * float(ray.outside), refractionIndex);
                newRay.dirNormalized = normalize(refraction);
                if (ray.outside == 1) {
                    PushScatter(material.scatter);
                } else {
                    PopScatter();
                }
                newRay.outside = -ray.outside;
                //if (ray.outside == -1) diffuse.g *= 4.0;// DEBUGDEBUG******************
            } else {
                // Diffuse light
                // Get a random vector in the hemisphere pointing along the normal.
	            vec3 rand = RandPointOnSphere(uint(ray.hash));
                vec3 bounce = rand * sign(dot(result.hitNormal, rand));
                newRay.dirNormalized = bounce;
                // Lambert shading model
                float intensity = dot(bounce, result.hitNormal);
                diffuse *= material.diffuse;// * intensity;
                emission = material.emission;
                if (result.materialIndex == 5) {
                    // checkerboard because it's a ray tracer. :)
                    //diffuse *= float((int(newRay.p0.x+8.0) & 1) ^ (int(newRay.p0.y+8.0) & 1) ^ (int(newRay.p0.z+8.0) & 1) ) * 0.2 + 0.8;
                }
                if (result.materialIndex == 3) {
	            	diffuse = max(Hashf3i1(uint(result.objIndex*17)), vec3(0.2))*0.75;
                }
                //emission = pow(Hashf3i1(uint(result.objIndex*5)), vec3(55.0)) * 150.0;
                //emission += emission.yzx * 0.904;
                //emission += emission.zxy * 0.91;
            }

        }
    }
    else if (!any(isnan(ray.dirNormalized)))
    {
        if (lifetime < farPlane) {
            // Scattering
            newRay.p0 = ray.p0 + ray.dirNormalized * lifetime;
            newRay.dirNormalized = RandPointOnSphere(uint(ray.hash+211));
            diffuse *= currentScatter.xyz;
        } else {
            if(iEnv == 2)
            {
                emission = GetEnvMap2(ray.dirNormalized);
            }else if (iEnv == 3)
            {
                emission = GetEnvMap3(ray.dirNormalized);
            }
            else if (iEnv == 4)
            {
                emission = GetEnvMap4(ray.dirNormalized);
            }
            else if (iEnv == 5)
            {
                emission = GetEnvMap(ray.dirNormalized);
            }
            newRay.outside = 0;  // This terminates the ray.
        }
    }
    // Filtering
    //vec3 internal = Hashf3i1(uint(result.objIndex))*0.3+0.7;
    vec3 internal = material.filterDiffuseDensity.xyz;
    // Filter proportional to how long the ray moves inside the object
    // This can also be done with scattering, but this should converge quicker.
    if (ray.outside == -1) diffuse *= pow(internal, vec3(abs(result.t)));

    // Save the ray hit in a list so we can calculate color of the pixel later on.
    SaveHit(diffuse, emission);
    return newRay;
}

out vec4 fragColor;

void main()
{
    uint randomState = uint(123456789);
    vec2 position = vec2(gl_FragCoord.xy)/vec2(iResolution);
    // read original buffer so we can accumulate pixel values into back it.
	fragColor = texture(iChannel0, position);

	// ---------------- First, set up the camera rays for ray marching ----------------
	vec2 uv_orig = position * 2.0 - 1.0;
    float zoom = 1.7;
    vec2 uv = uv_orig / zoom;

	// Camera up vector.
	vec3 camUp=vec3(0,1,0);

	// Camera lookat.
	vec3 camLookat=vec3(0,-2.25,0);

    // camera orbit with mouse movement
    float mx=iMouse.x/float(iResolution.x)*PI*2.0-0.7;// + Randf1i1(uint(iTime*360.0))*0.51; // + iTime*3.1415 *0.666*0.0625*0.0625;
	float my=-iMouse.y/float(iResolution.y)*10.0;// - sin(iTime * 0.31)*0.5;//*PI/2.01;
	vec3 camPos = vec3(cos(my)*cos(mx),sin(my),cos(my)*sin(mx))*(12.2);
    // If mouse is in bottom left corner, then use pre-set camera angle.
    //if ((dot(iMouse.xy, vec2(1.0)) <= 8.0)) camPos = vec3(12.0, 0.0, 2.0);

	// Camera setup.
	vec3 camVec=normalize(camLookat - camPos);
	vec3 sideNorm=normalize(cross(camUp, camVec));
	vec3 upNorm=cross(camVec, sideNorm);
	vec3 worldFacing=(camPos + camVec);
	vec3 worldPix = worldFacing + uv.x * sideNorm * float(iResolution.x)/float(iResolution.y) + uv.y * upNorm;
	vec3 rayVec = normalize(worldPix - camPos);

	// --------------------------------------------------------------------------------
    vec3 colorSum = vec3(0.0);
    // Loop through for a few samples and average the pixel colors from ray tracing.
    for (int s = 0; s < NUM_SAMPLES; s++) {
        InitScatterStack(globalScatter);
        ResetColorHitList();
        Ray ray;
        ray.outside = 1;
        ray.p0 = camPos;
        // Anti-aliasing: Randomly jitter the ray direction by a gaussian distribution.
        vec2 gauss = RandGaussian(uint(iTime*60.0));
        float antialias = dFdx(uv.xy).x/1.5;
        ray.dirNormalized = normalize(rayVec +
                                      sideNorm*gauss.x*antialias +
                                      upNorm * gauss.y*antialias);

        // Make a hash value for the ray so we can get random numbers
        int width = iResolution.x;
        int height = iResolution.y;
        ray.hash = int(gl_FragCoord.x) + int(gl_FragCoord.y) * width + s * width*height +
            int(iTime*60.0)*width*height*NUM_SAMPLES;
        ray.hash ^= int(GetRand());
        // THIS MAKES IT NON-DETERMINISTIC, but seeds the random better.
        ray.hash += int(iTime*60.0);

        // Trace a ray from the camera outwards, bounce the ray off objects and keep
        // tracing until NUM_ITERS or until it hits the background.
        for (int i = 0; i < NUM_ITERS; i++) {
            if (i == (NUM_ITERS-1)) break;
            ray = TraceOneRay(ray);
            if (ray.outside == 0) break;
        }
        /*int i = 0;
        do {
            ray = TraceOneRay(ray);
            i++;
        } while ((ray.outside != 0) && (i < NUM_ITERS-1));*/

        // Once we're done iterating through rays from the camera outwards, we have a
        // list of hits. Now we can go from the light source toward the camera and apply
        // the color filters and emissions as we go.
	    vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);
        for (int i = NUM_ITERS-1; i >= 0; i--) {
            if (colorHits[i].emission.x != -1.0) {
                finalColor.xyz *= colorHits[i].diffuse;
                finalColor.xyz += colorHits[i].emission;
                //finalColor.xyzw = finalColor.yzwx;  // Debug ray depth
            }
        }
        colorSum += finalColor.xyz; 
    }
    colorSum /= float(NUM_SAMPLES);

    // accumulate the final color
	fragColor.rgb += (colorSum-fragColor.rgb)*iFramesRcp;
}
