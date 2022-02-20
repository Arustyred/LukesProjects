#version 430

#define MAX_HITS 3
#define MAX_DISTANCE 140
#define MAX_MARCHES 200
#define EPSILON 0.002
#define NUM_RAYS 1

struct Light {
	vec3 position;
	vec3 direction;
	vec4 ambient, diffuse, specular;
	float range;
};

struct Material {
	vec4 ambient, diffuse, specular;
	float reflectiveness;
	float shininess;
};

struct Ray {
	vec3 pos;
	vec3 direction;
	int hits;
	int marches;
	float distance;
};

struct RayHit {
	bool hit;
	vec3 pos;
	vec3 normal;
	float distance;
};

struct Sphere {
    vec3 position;
    float radius;
	mat4 tmat;
	Material material;
};

struct Box {
    vec3 bounds;
	mat4 tmat;
	Material material;
};

struct Cylinder {
    float height;
    float radius;
	mat4 tmat;
	Material material;
};

struct Plane {
    vec3 normal;
    float height;
	mat4 tmat;
	Material material;
};

//Methods
Ray GetRay(vec2 pixelCoords);
vec2 GetScreenSpaceCoordinates(vec2 pixelCoords);
vec2 GetCameraSpaceCoordinates(vec2 pixelSS);
vec4 GetWorldSpaceImagePlaneCoordinates(vec2 imagePlaneCoords);
vec4 GetPixelColor(Ray ray);
RayHit ShootRay(Ray ray);
void CreateScene();

float DistanceFunc(vec3 pos);
float SphereDis(vec3 point, Sphere s);
float BoxDis(vec3 point, Box b);
float CylinderDis(vec3 p, Cylinder c);
float PlaneDis(vec3 p, Plane plane);

vec3 EstimateNormal(vec3 point);
vec4 CalculateLighting(RayHit hit);

mat4 RotateX(float theta);
mat4 RotateY(float theta);
mat4 RotateZ(float theta);
mat4 Translate(float x, float y, float z);
mat4 Translate(vec3 t);

/*
###################################################################################################################################################

MAIN

###################################################################################################################################################
*/

layout(local_size_x = 1, local_size_y = 1) in;   //local work group size for this shader (set to 1x1 pixel for now)
layout(rgba32f, binding = 0) uniform image2D imageOutput;  //the image output of the shader

uniform float aspectRatio;
uniform float fovFactor;

uniform mat4 camWorldMatrix;
uniform mat4 camViewMatrix;
uniform vec4 camPos;

vec4 pixel;
vec4 camWorldPos;

//#########
//Scene Objects
//########

Light l;
Sphere s1, s2;
Plane p1, p2, p3, p4, p5, p6;
Material wall, ball;
Material closestMat;

void main() {
	//get global work group index
	ivec2 iPixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 pixelCoords = vec2(iPixelCoords.x, iPixelCoords.y);
	vec3 sum = vec3(0.0, 0.0, 0.0);

	CreateScene();

	Ray ray = GetRay(pixelCoords);
	sum += GetPixelColor(ray).xyz;
	for(int i = 0; i < NUM_RAYS - 1; i++) {
		float angle = radians(i * 360.0 / (NUM_RAYS - 1));
		ray = GetRay(pixelCoords + 0.5 * vec2(sin(angle), cos(angle)));
		sum += GetPixelColor(ray).xyz;
	}
	pixel = vec4(sum / NUM_RAYS, 1.0);

	imageStore(imageOutput, iPixelCoords, pixel);
}

/*
###################################################################################################################################################

Camera Rays

###################################################################################################################################################
*/

Ray GetRay(vec2 pixelCoords) {
	vec2 imagePlaneCoords = GetScreenSpaceCoordinates(pixelCoords);
	imagePlaneCoords = GetCameraSpaceCoordinates(imagePlaneCoords);
	vec4 imagePlaneCoordsWorld = GetWorldSpaceImagePlaneCoordinates(imagePlaneCoords);
	//camWorldPos = camWorldMatrix * camPos;
	camWorldPos = camWorldMatrix * vec4(0, 0, 0, 1);

	return Ray(camWorldPos.xyz, normalize(imagePlaneCoordsWorld.xyz - camWorldPos.xyz), 0, 0, 0.0);
}

vec2 GetScreenSpaceCoordinates(vec2 pixelCoords) {
	vec2 pixelSS;
	ivec2 size = imageSize(imageOutput);

	pixelSS.x = (pixelCoords.x + 0.5) / size.x;
	pixelSS.y = (pixelCoords.y + 0.5) / size.y;
	
	pixelSS.x = (pixelSS.x * 2.0) - 1.0;
	pixelSS.y = (pixelSS.y * 2.0) - 1.0;
	
	return pixelSS;
}

vec2 GetCameraSpaceCoordinates(vec2 pixelSS) {
	pixelSS.x *= aspectRatio * fovFactor;
	pixelSS.y *= fovFactor;
	return pixelSS;
}

vec4 GetWorldSpaceImagePlaneCoordinates(vec2 imagePlaneCoords) {
	vec4 imagePlaneCoordsWorld = vec4(imagePlaneCoords.x, imagePlaneCoords.y, 1.0, 1.0);
	imagePlaneCoordsWorld = camWorldMatrix * imagePlaneCoordsWorld;
	return imagePlaneCoordsWorld;
}

/*
###################################################################################################################################################

RayCasting

###################################################################################################################################################
*/

vec4 GetPixelColor(Ray ray) {
	vec4 color = vec4(0.0);
	vec4 hitColor = vec4(0.0);
	float frac = 1.0;
	RayHit hit;

	while(ray.hits < MAX_HITS && ray.distance < MAX_DISTANCE && ray.marches < MAX_MARCHES) {
		hit = ShootRay(ray);

		ray.pos += hit.distance * ray.direction;
		ray.distance += hit.distance;
		ray.marches++;

		if(hit.hit) {
			hit.distance = ray.distance;
			hitColor = CalculateLighting(hit) * (1 - closestMat.reflectiveness) * frac;
			frac *= closestMat.reflectiveness;
			if(frac < 0.05)
				break;
				
			color += hitColor;
			ray.hits++;
			ray.distance = 0;
			ray.direction = reflect(ray.direction, hit.normal);
			ray.pos = hit.pos + ray.direction * EPSILON;
		}
	}

	return color;
}

RayHit ShootRay(Ray ray) {
	float distance = DistanceFunc(ray.pos);
	RayHit hit = RayHit(false, vec3(0.0), vec3(0.0), distance);
	
	//Ray Hit
	if(distance < EPSILON) {
		hit.hit = true;
		hit.pos = ray.pos;
		hit.normal = EstimateNormal(hit.pos);
	}

	return hit;
}

/*
###################################################################################################################################################

Distance Function

###################################################################################################################################################
*/

float DistanceFunc(vec3 pos) {
	vec4 p = vec4(pos, 1.0);
	float m = MAX_DISTANCE;
	float d = MAX_DISTANCE;
	
	d = SphereDis((s1.tmat * p).xyz, s1);
	if(d < m) {
		m = d;
		closestMat = s1.material;
	}

	d = SphereDis((s2.tmat * p).xyz, s2);
	if(d < m) {
		m = d;
		closestMat = s2.material;
	}
		
	d = PlaneDis((p1.tmat * p).xyz, p1);
	if(d < m) {
		m = d;
		closestMat = p1.material;
	}
	d = PlaneDis((p2.tmat * p).xyz, p2);
	if(d < m) {
		m = d;
		closestMat = p2.material;
	}
	d = PlaneDis((p3.tmat * p).xyz, p3);
	if(d < m) {
		m = d;
		closestMat = p3.material;
	}
	d = PlaneDis((p4.tmat * p).xyz, p4);
	if(d < m) {
		m = d;
		closestMat = p4.material;
	}
	d = PlaneDis((p5.tmat * p).xyz, p5);
	if(d < m) {
		m = d;
		closestMat = p5.material;
	}
	d = PlaneDis((p6.tmat * p).xyz, p6);
	if(d < m) {
		m = d;
		closestMat = p6.material;
	}

	return m;
}

float SphereDis(vec3 point, Sphere s) {
    return length(point - s.position) - s.radius;
}

float BoxDis(vec3 point, Box b) {
  vec3 q = abs(point) - b.bounds;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float CylinderDis(vec3 p, Cylinder c)
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(c.height,c.radius);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float PlaneDis(vec3 p, Plane plane)
{
  // n must be normalized
  return dot(p, plane.normal) + plane.height;
}

/*
###################################################################################################################################################

Lighting / Surface Normals

###################################################################################################################################################
*/

vec3 EstimateNormal(vec3 point) {
    vec3 epsilonVec;  //very tiny amount to move the point

    epsilonVec = vec3(EPSILON, 0.0, 0.0);
    float x = DistanceFunc(point + epsilonVec) - DistanceFunc(point - epsilonVec);
    epsilonVec = vec3(0.0, EPSILON, 0.0);
    float y = DistanceFunc(point + epsilonVec) - DistanceFunc(point - epsilonVec);
    epsilonVec = vec3(0.0, 0.0, EPSILON);
    float z = DistanceFunc(point + epsilonVec) - DistanceFunc(point - epsilonVec);

    return normalize(vec3(x, y, z));
}

vec4 CalculateLighting(RayHit hit) {
    vec3 L = normalize(l.position - hit.pos);
    vec3 N = hit.normal;
	vec3 V = normalize(camWorldPos.xyz - hit.pos);

    vec3 R = normalize(reflect(-L, N));    // compute light reflection vector with respect to N
	float cosTheta = dot(L,N);             // get the angle between the light and surface normal
	float cosPhi = dot(V,R);               // angle between the view vector and reflected light

	float disFromLight = length(hit.pos - l.position);
    vec3 ambient = (l.ambient * closestMat.ambient).xyz;
	vec3 diffuse = l.diffuse.xyz * closestMat.diffuse.xyz * max(cosTheta,0.0) * max(l.range - disFromLight, 0.0)/l.range;
	vec3 specular = l.specular.xyz * closestMat.specular.xyz * pow(max(cosPhi,0.0), closestMat.shininess) * max(l.range - disFromLight, 0.0)/l.range;

	return vec4(ambient + diffuse + specular, 1.0);
	//return vec4(specular, 1.0);
}

/*
###################################################################################################################################################

Scene

###################################################################################################################################################
*/

void CreateScene() {
	l = Light(vec3(-3, 3, 3), vec3(0), vec4(1), vec4(1), vec4(0.625), 18);
	wall = Material(vec4(.1, .1, .1, 1.0), vec4(.11, .42, .85, 1.0), vec4(.85, .91, .96, 1.0), 0.2, 2);
	ball = Material(vec4(.1, .1, .1, 1.0), vec4(.89, .09, .67, 1.0), vec4(.98, .92, .90, 1.0), 0.375, 2);

	s1 = Sphere(vec3(0, 0, 5), 2.0, Translate(0, 2.5, 0), ball);
	s2 = Sphere(vec3(-3, 3, 3), 0.1, mat4(1.0), ball);
	p1 = Plane(vec3(0, 1, 0), 0, mat4(1.0), wall);
	p2 = Plane(vec3(0, -1, 0), 10, mat4(1.0), wall);
	p3 = Plane(vec3(1, 0, 0), 0, Translate(-10, 0, 0), wall);
	p4 = Plane(vec3(-1, 0, 0), 0, Translate(10, 0, 0), wall);
	p5 = Plane(vec3(0, 0, 1), 1, Translate(0, 0, -10), wall);
	p6 = Plane(vec3(0, 0, -1), 1, Translate(0, 0, 10), wall);
}

/*
###################################################################################################################################################

Matrices

###################################################################################################################################################
*/

//THETA AND POSITIONS ARE INVERTED
//this is because the point needs to be multiplied by the inverse of the translation matrix

mat4 RotateX(float theta) {
	return mat4(
		1, 0, 0, 0,
		0, cos(-theta), sin(-theta), 0,
		0, -sin(-theta), cos(-theta), 0,
		0, 0, 0, 1
	);
}

mat4 RotateY(float theta) {
	return mat4(
		cos(-theta), 0, -sin(-theta), 0,
		0, 1, 0, 0,
		sin(-theta), 0, cos(-theta), 0,
		0, 0, 0, 1
	);
}

mat4 RotateZ(float theta) {
	return mat4(
		cos(-theta), sin(-theta), 0, 0,
		-sin(-theta), cos(-theta), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

mat4 Translate(float x, float y, float z) {
	return mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-x, -y, -z, 1
	);
}

mat4 Translate(vec3 t) {
	return mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-t.x, -t.y, -t.z, 1
	);
}