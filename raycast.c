#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//create structs
typedef struct Sphere{
  int type;
  double position[3];
  double color[3];
  double radius;
}sphere;

typedef struct Plane{
  int type;
  double position[3];
  double color[3];
  double normal[3];
}plane;

typedef struct Camera{
  double width;
  double height;
}Camera;

typedef struct{
  unsigned char r, g, b;
}Pixel;

sphere sphereArray[128];
plane planeArray[128];


//create json reading functions

void read_json(char* filename);

void skip_to(FILE* file, int n);

char* next_string(FILE* file, int n);

double next_num(FILE* file);

void normalizeVector(double *v);

double scale(double val);

double sphereIntersect(double *o, double *d, double *Cam, double r);

double planeIntersect(double *o, double *d, double *c, double *n);

void construct_scene(Camera camera, sphere *Sphere, plane *Plane, Pixel *pixel, int width, int height);

int line = 1;

int sphereCount = 0;

int planeCount = 0;





void skip_to(FILE* file, int n){//skip to a specified character
  int x = fgetc(file);
  if(x == n){
    return;
  }
  else if(x == EOF){
    fprintf(stderr, "Error: Could not find Char %s\n", x);
  }
  else{
    x = fgetc(file);
    }
}


char* next_string(FILE *file, int n){//get next chars and build a string. Returns the string.
  char buffer[129];
  int i = 0;
  
  while(n != ',' && n != ':' && n != '[' && n != ']'){
    if(n == 10 || n == EOF){
      return (char *)n;
    }
    if(n < 32 || n > 126){
      fprintf(stderr, "Error: ASCII chars only! Given %d. \n", n);
      fclose(file);
      exit(1);
    }
    buffer[i] = n;
    i += 1;
    n = fgetc(file);
  }
  buffer[i] = 0;
  return (char *)strdup(buffer);
}

double next_num(FILE* file){//gets the next number value (double)
  double value;
  int count = fscanf(file, "%lf", &value);
  if(count != 1){
    fprintf(stderr, "Error: Cannot read number.\n");
    fclose(file);
    exit(1);
  }
  return value;
}

void read_json(char *file){//read the file in, and save all of the values into an array of spheres and planes.
  
  int n;
  double num;
  int temp_type;
  FILE* json = fopen(file, "r");
  char* key;
  char* val;
  double colors[3];
  double position[3];
  double normal[3];
  double radius;
  double width;
  double height;
  

  n = fgetc(json);
  while(n != EOF && (sphereCount + planeCount + 1) < 129){
    //fprintf(stdout, "Char found: %d\n", n);
    if(n == 10){
      if(temp_type == 1){
	Camera camera = {0, 0};
	camera.width = width;
	camera.height = height;
      }
      else if(temp_type == 2){
	sphere Sphere = {};
	Sphere.color[0] = colors[0];
	Sphere.color[1] = colors[1];
	Sphere.color[2] = colors[2];
	Sphere.position[0] = position[0];
	Sphere.position[1] = position[1];
	Sphere.position[2] = position[2];
	Sphere.radius = radius;
	sphereArray[sphereCount] = Sphere;
	sphereCount++;
	//fprintf(stdout, "sphere count: %d\n", sphereCount);
      }
      else if(temp_type == 3){
	plane Plane = {};
	Plane.color[0] = colors[0];
	Plane.color[1] = colors[1];
	Plane.color[2] = colors[2];
	Plane.position[0] = position[0];
	Plane.position[1] = position[1];
	Plane.position[2] = position[2];
	Plane.normal[0] = normal[0];
	Plane.normal[1] = normal[1];
	Plane.normal[2] = normal[2];
	planeArray[planeCount] = Plane;
	planeCount++;
      }
    }
    else if(n > 64 && n < 123){
      val = next_string(json, n);
      //fprintf(stdout, "String: %s\n", val);
      if(strcmp(val, "camera") == 0){
	temp_type = 1;//1 is a camera type
	//fprintf(stdout, "Found a camera.\n");
      }
      else if(strcmp(val, "sphere") == 0){
	temp_type = 2;//2 is a sphere type
	//fprintf(stdout, "Found a sphere.\n");
      }
      else if(strcmp(val, "plane") == 0){
	temp_type = 3;//3 is a plane type
	//fprintf(stdout, "Found a plane\n");
      }
      else if(strcmp(val, "width") == 0){//store width
	if(temp_type != 1){//checks for camera type
	  fprintf(stderr, "Error: width attribute on a non-camera object.\n");
	}
	else{
	  width = next_num(json);
	  //fprintf(stdout, "Found Width: %lf\n", width);
	}
      }
      else if(strcmp(val, "height") == 0){//store height
	if(temp_type != 1){
	  fprintf(stderr, "Error: height attribute on a non-camera object.\n");
	}
	else{
	  height = next_num(json);
	  //fprintf(stdout, "Found Height: %lf\n", height);
	}
      }
      else if(strcmp(val, "color") == 0){//store into color array
	fgetc(json);
	colors[0] = next_num(json);
	fgetc(json);
	colors[1] = next_num(json);
	fgetc(json);
	colors[2] = next_num(json);
	//fprintf(stdout, "Colors Array: %lf,%lf,%lf\n", colors[0], colors[1], colors[2]);
	
      }
      else if(strcmp(val, "position") == 0){//store into array for position
	fgetc(json);
	position[0] = next_num(json);
	fgetc(json);
	position[1] = next_num(json);
	fgetc(json);
	position[2] = next_num(json);
	//fprintf(stdout, "Position Array: %lf %lf %lf\n", position[0], position[1], position[3]);
      }
      else if(strcmp(val, "radius") == 0){//store radius
	radius = next_num(json);
	//fprintf(stdout, "Radius: %lf\n", radius);
      }
      else if(strcmp(val, "normal") == 0){//store normal info into array
	fgetc(json);
	normal[0] = next_num(json);
	fgetc(json);
	normal[1] = next_num(json);
	fgetc(json);
	normal[2] = next_num(json);
	//fprintf(stdout, "Normal Array: %lf,%lf,%lf\n", normal[0], normal[1], normal[2]);
      }
      else if(strcmp(val, "") == 0){
        
      }
      else{
	fprintf(stderr, "Error: invalid type. Given: %s\n", val);
      }
    }
    n = fgetc(json);
    
  }
  if(temp_type == 1){//save the camera info
	Camera camera = {0, 0};
	camera.width = width;
	camera.height = height;
	//fprintf(stdout, "Saving Camera Info:\n");
      }
  else if(temp_type == 2){//save the sphere information
	sphere Sphere = {};
	Sphere.color[0] = colors[0];
	Sphere.color[1] = colors[1];
	Sphere.color[2] = colors[2];
	Sphere.position[0] = position[0];
	Sphere.position[1] = position[1];
	Sphere.position[2] = position[2];
	Sphere.radius = radius;
	sphereArray[sphereCount] = Sphere;
      }
      else if(temp_type == 3){//save the plan information
	plane Plane = {};
	Plane.color[0] = colors[0];
	Plane.color[1] = colors[1];
	Plane.color[2] = colors[2];
	Plane.position[0] = position[0];
	Plane.position[1] = position[1];
	Plane.position[2] = position[2];
	Plane.normal[0] = normal[0];
	Plane.normal[1] = normal[1];
	Plane.normal[2] = normal[2];
	planeArray[planeCount] = Plane;
      }

  
}

void normalizeVector(double *v){//changes the vector given into a unit vector by dividing each coordinate by
  //the length of the vector.
  double length = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
  v[0] = v[0] / length;
  v[1] = v[1] / length;
  v[2] = v[2] / length;
}

double scale(double val){
  if(val < 0){
    return 0;
  }
  else if(val > 1){
    return 1;
  }
  else{
    return val;
  }
}

double sphereIntersect(double *o, double *d, double *S, double r){
  //o = origin of ray points, (x,y,z)
  //d = direction of ray (x,y,z)
  //S = center of sphere (x,y,z)
  //r = sphere radius
  
  double a = (d[0] * d[0]) +(d[1] * d[1]) + (d[2] * d[2]);
  double b = (2 * (o[0] * d[0] - d[0] * S[0] + o[1] * d[1] - d[1] * S[1] + o[2] * d[2] - d[2] * S[2]));
  double c = (o[0] * o[0]) - 2 * o[0] * S[0] + (S[0] * S[0]) + (o[1] * o[1]) - 2 * o[1] * S[1] + (S[1] * S[1]) + (o[2] * o[2]) - 2 * o[2] * S[2] + (S[2] * S[2]) - (r * r);
  double t0;
  double t1;

  double det = pow(b, 2) - 4 * a * c;

  if(det < 0){
    return -1;
  }

  det = sqrt(det);
  t0 = (-b - det) / (2*a);
  if(t0 > 0){
    return t0;
  }
  t1 = (-b + det)/ (2*a);
  if(t1 > 0){
    return t1;
  }
  return -1;
}

double planeIntersect(double *o, double *d, double *c, double *n){
  //o = origin of ray points, (x,y,z)
  //d = direction of the ray (x,y,z)
  //c = coordinates of the plane (x,y,z)
  //n = normal of the plane (x,y,z)
  double t;
  normalizeVector(n);
  t = (n[0] * c[0] + n[1] * c[1] + n[2] * c[2] - n[0] * o[0] - n[1] * o[1] - n[2] * o[2])/(n[0] * d[0] + n[1] * d[1] + n[2] * d[2]);

  if(t > 0){
    return t;
  }
  return -1;
}

void construct_scene(Camera camera, sphere *Sphere, plane *Plane, Pixel *pixel, int width, int height){

  double cam_width = camera.width;
  double cam_height = camera.height;
  double Op[3];
  double Dp[3];
  double pixWidth = cam_width / width;
  double pixHeight = cam_height / height;
  int y, x;

  for(y = 0; y < height; y++){
    for(x = 0; x < width; x++){
      double closest_t = 9007199254740992;//number is highest Double. INFINITY not working?
      sphere closest_sphere;
      plane closest_plane;
      int i;
      int position = (height - (y+1)) * width + x;
      double origin[3] = {0,0,0};
      double d[3] = {0 - (cam_width/2) + pixWidth * (x + 0.5),
		     0 - (cam_height/2) + pixHeight * (y + 0.5),
		     1};
      normalizeVector(d);

      for(i = 0; i < sphereCount; i++){
	double t = 0;
	t = sphereIntersect(origin, d, Sphere[i].position, Sphere[i].radius);

	if(t > 0 && t < closest_t){
	  closest_t = t;
	  closest_sphere = Sphere[i];
	}
	if(closest_t > 0 && closest_t != 9007199254740992){
	  pixel[position].r = Sphere[i].color[0];
	  pixel[position].g = Sphere[i].color[1];
	  pixel[position].b = Sphere[i].color[2];
	  	  
	}
	else{
	  pixel[position].r = 0;
	  pixel[position].g = 0;
	  pixel[position].b = 0;
	}
      }
    }
  }
}

void writeOut(Pixel *pixel, FILE *out_file, int width, int height, int max_color){

  int current_col = 1;
  fprintf(out_file, "P3\n%d %d\n%d\n", width, height, max_color);//write the P3 header to the output file

  
}

int main(int argc, char *argv[]){

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  // FILE* out_file = fopen(argv[4], "w");
  fprintf(stdout, "width = %d\n", width);
  fprintf(stdout, "height = %d\n", height);
  fprintf(stdout, "Beginning file reading...");
  read_json(argv[3]);//call read_json with the input file
  fprintf(stdout, "File reading Completed...");
  exit(0);
}
