#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//create structs
typedef struct Sphere{
  double position[3];
  double diffuse_color[3];
  double specular_color[3];
  double radius;
}sphere;

typedef struct Plane{ 
  double position[3];
  double diffuse_color[3];
  double specular_color[3];
  double normal[3];
}plane;

typedef struct Light{
  double color[3];
  double position[3];
  double direction[3];
  double radial_a0;
  double radial_a1;
  double radial_a2;
  double theta;
  double angular_a0;
}Light;

typedef struct Camera{
  double width;
  double height;
}Camera;

typedef struct{
  unsigned char r, g, b;
}Pixel;

sphere sphereArray[128];
plane planeArray[128];
Light lightArray[128];
Camera camera;


//create json reading functions

void read_json(char* filename);

void skip_to(FILE* file, int n);

char* next_string(FILE* file, int n);

double next_num(FILE* file);

void normalizeVector(double *v);

double scale(double val);

double sphereIntersect(double *o, double *d, double *Cam, double r);

double planeIntersect(double *o, double *d, double *c, double *n);

double dot_product(double *a, double *b);

void reflection(double *v1, double *v2, double *array);

void sub_vector(double *v1, double *v2, double *array);

double vector_length(double *v);

void scale_vector(double *v1, double a, double *array);

double intensity(Light light, double t);

double diffuse_lighting(double d_color, double l_color, double *n, double *l);

double specular_lighting(double s_color, double l_color, double *n, double *l, double *r, double *c);

void construct_scene(Camera camera, Pixel *pixel, int width, int height);

int line = 1;

int sphereCount = 0;

int planeCount = 0;

int lightCount = 0;





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
  double s_color[3];
  double d_color[3];
  double position[3];
  double direction[3];
  double normal[3];
  double radius;
  double width;
  double height;
  double theta;
  double a0;
  double a1;
  double a2;
  double angular_a0;
  

  n = fgetc(json);
  while(n != EOF && (sphereCount + planeCount + 1) < 129){
    //fprintf(stdout, "Char found: %d\n", n);
    if(n == 10){
      if(temp_type == 1){
	camera.width = width;
	camera.height = height;
      }
      else if(temp_type == 2){
	sphere Sphere = {};
	Sphere.specular_color[0] = s_color[0];
	Sphere.specular_color[1] = s_color[1];
	Sphere.specular_color[2] = s_color[2];
	Sphere.diffuse_color[0] = d_color[0];
	Sphere.diffuse_color[1] = d_color[1];
	Sphere.diffuse_color[2] = d_color[2];
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
	Plane.specular_color[0] = s_color[0];
	Plane.specular_color[1] = s_color[1];
	Plane.specular_color[2] = s_color[2];
	Plane.diffuse_color[0] = d_color[0];
	Plane.diffuse_color[1] = d_color[1];
	Plane.diffuse_color[2] = d_color[2];
	Plane.position[0] = position[0];
	Plane.position[1] = position[1];
	Plane.position[2] = position[2];
	Plane.normal[0] = normal[0];
	Plane.normal[1] = normal[1];
	Plane.normal[2] = normal[2];
	planeArray[planeCount] = Plane;
	planeCount++;
      }
      else if(temp_type == 4){
	Light light = {};
	light.color[0] = colors[0];
	light.color[1] = colors[1];
	light.color[2] = colors[2];
	light.position[0] = position[0];
	light.position[1] = position[1];
	light.position[2] = position[2];
	light.direction[0] = direction[0];
	light.direction[1] = direction[1];
	light.direction[2] = direction[2];
	light.radial_a0 = a0;
	light.radial_a1 = a1;
	light.radial_a2 = a2;
	light.angular_a0 = angular_a0;
	light.theta = theta;
	lightArray[lightCount] = light;
	lightCount++;
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
      else if(strcmp(val, "light") == 0){
	temp_type = 4;//4 is a light type
      }
      else if(strcmp(val, "direction") == 0){
	fgetc(json);
	direction[0] = next_num(json);
	fgetc(json);
	direction[1] = next_num(json);
	fgetc(json);
	direction[2] = next_num(json);
      }
      else if(strcmp(val, "radial-a0") == 0){
	a0 = next_num(json);
      }
      else if(strcmp(val, "radial-a1") == 0){
	a1 = next_num(json);
      }
      else if(strcmp(val, "radial-a2") == 0){
	a2 = next_num(json);
      }
      else if(strcmp(val, "theta") == 0){
	theta = next_num(json);
      }
      else if(strcmp(val, "angular-a0") == 0){
	angular_a0 = next_num(json);
      }
      else if(strcmp(val, "diffuse_color") == 0){
	fgetc(json);
	d_color[0] = next_num(json);
	fgetc(json);
	d_color[1] = next_num(json);
	fgetc(json);
	d_color[2] = next_num(json);
      }
      else if(strcmp(val, "specular_color") == 0){
	fgetc(json);
	s_color[0] = next_num(json);
	fgetc(json);
	s_color[1] = next_num(json);
	fgetc(json);
	s_color[2] = next_num(json);
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
	camera.width = width;
	camera.height = height;
	//fprintf(stdout, "Saving Camera Info:\n");
      }
  else if(temp_type == 2){//save the sphere information
	sphere Sphere = {};
        Sphere.diffuse_color[0] = d_color[0];
	Sphere.diffuse_color[1] = d_color[1];
	Sphere.diffuse_color[2] = d_color[2];
	Sphere.specular_color[0] = s_color[0];
	Sphere.specular_color[1] = s_color[1];
	Sphere.specular_color[2] = s_color[2];
	Sphere.position[0] = position[0];
	Sphere.position[1] = position[1];
	Sphere.position[2] = position[2];
	Sphere.radius = radius;
	sphereArray[sphereCount] = Sphere;
	sphereCount++;
      }
      else if(temp_type == 3){//save the plan information
	plane Plane = {};
	Plane.diffuse_color[0] = d_color[0];
	Plane.diffuse_color[1] = d_color[1];
	Plane.diffuse_color[2] = d_color[2];
	Plane.specular_color[0] = s_color[0];
	Plane.specular_color[1] = s_color[1];
	Plane.specular_color[2] = s_color[2];
	Plane.position[0] = position[0];
	Plane.position[1] = position[1];
	Plane.position[2] = position[2];
	Plane.normal[0] = normal[0];
	Plane.normal[1] = normal[1];
	Plane.normal[2] = normal[2];
	planeArray[planeCount] = Plane;
	planeCount++;
      }
      else if(temp_type == 4){//save the light information
	Light light = {};
	light.color[0] = colors[0];
	light.color[1] = colors[1];
	light.color[2] = colors[2];
	light.position[0] = position[0];
	light.position[1] = position[1];
	light.position[2] = position[2];
	light.direction[0] = direction[0];
	light.direction[1] = direction[1];
	light.direction[2] = direction[2];
	light.radial_a0 = a0;
	light.radial_a1 = a1;
	light.radial_a2 = a2;
	light.angular_a0 = angular_a0;
	lightArray[lightCount] = light;
	lightCount++;
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
  t1 = -((-b - det)/ (2*a));
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
  t = ((n[0]*c[0] + n[1]*c[1] + n[2]*c[2] - n[0]*o[0] - n[1]*o[1] - n[2]*o[2])/(n[0]*d[0] + n[1]*d[1] + n[2]*d[2]));
  //printf("plane t-val: %lf\n", t);
  if(t > 0){
    return t;
  }
  return -1;
}

double dot_product(double *a, double *b){//returns the dot product for vector a and b
  return ((a[0]*b[0]) + (a[1]*b[1]) + (a[2]*b[2]));
}

void reflection(double *v1, double *v2, double *array){

  double dot;
  double temp_v[3];
  dot = 2*dot_product(v1, v2);
  scale_vector(v1, dot, temp_v);
  sub_vector(v2, temp_v, array);
}

void scale_vector(double *v1, double a, double *array){
  array[0] = v1[0]*a;
  array[1] = v1[1]*a;
  array[2] = v1[2]*a;
}

void sub_vector(double *v1, double *v2, double *array){
  array[0] = v1[0] - v2[0];
  array[1] = v1[1] - v2[1];
  array[2] = v1[2] - v2[2];
}

double diffuse_lighting(double d_color, double l_color, double *n, double *l){

  double val;
  val = -(dot_product(n, l));
  if(val > 0){
    return d_color*l_color*val;
  }
  else{
    return 0;
  }
}

double specular_lighting(double d_color, double l_color, double *n, double *l, double *r, double *c){

  double val_1;//c dot  r
  double val_2;//n dot l
  val_1 = -(dot_product(c, r));
  val_2 = -(dot_product(n, l));
  printf("val_1: %lf, val_2: %lf\n", val_1, val_2);
  if(val_1 > 0 && val_2 > 0){
    return d_color*l_color*(pow(val_1, 20));
  }
  else{
    return 0;
  }
}

double intensity(Light light, double t){//calculates intensity of a light source
  //1/(a2(t^2)+a1(t)+a0)
  return (1/(light.radial_a2*t*t+light.radial_a1*t+light.radial_a0));
}

double vector_length(double *v){//returns the length of a vector
  //sqrt((x^2)+(y^2)+(z^2))
  return sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
}

void construct_scene(Camera camera, Pixel *pixel, int width, int height){

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
      char *closestObject;
      sphere closest_sphere;
      plane closest_plane;
      int closest_shadow;
      int i = 0;
      int j = 0;
      int position =(height-(y+1))*width+x;
      double origin[3] = {0,0,0};
      double d[3] = {0 - (cam_width/2) + pixWidth * (x + 0.5),
		     0 - (cam_height/2) + pixHeight * (y + 0.5),
		     1};
      normalizeVector(d);

     for(i = 0; i < sphereCount; i++){
	double t = 0;
	t = sphereIntersect(origin, d, sphereArray[i].position, sphereArray[i].radius);

	if(t > 0 && t < closest_t){
	  fprintf(stdout, "found a sphere intersection!\n");
	  closest_t = t;
	  closest_sphere = sphereArray[i];
	  
	}
	if(closest_t > 0 && closest_t != 9007199254740992){
	  double color[3];
	  int k = 0;
	  color[0] = 0;
	  color[1] = 0;
	  color[2] = 0;
	  for(k = 0; k < lightCount; k++){
	    double light_distance;
	    double shadow_val;
	    int m;
	    Op[0] = closest_t * d[0] + origin[0];
	    Op[1] = closest_t * d[1] + origin[1];
	    Op[2] = closest_t * d[2] + origin[2];
	    Dp[0] = lightArray[j].position[0] - Op[0];
	    Dp[1] = lightArray[j].position[1] - Op[1];
	    Dp[2] = lightArray[j].position[2] - Op[2];
	    light_distance = vector_length(Dp);
	    normalizeVector(Dp);
	    closest_shadow = 0;
	    for(m = 0; m < sphereCount; m++){
	      if(sphereArray[m].position[2] == closest_sphere.position[2]){
		printf("Found Closest Sphere...\n");
		continue;
	      }
	      shadow_val = sphereIntersect(Op, Dp, sphereArray[m].position, sphereArray[m].radius);
	      if(0 < shadow_val && shadow_val < light_distance){
		closest_shadow = 1;
		break;
	      }
	    }
	    if(closest_shadow == 0){
	      double normal[3];
	      double light[3];
	      double reflection_light[3];
	      double camera_pos[3];
	      double radial;
	      double angular;
	      double diffuse[3];
	      double specular[3];
	      printf("normal created...\n");
	      normal[0] = Op[0] - closest_sphere.position[0];
	      normal[1] = Op[1] - closest_sphere.position[1];
	      normal[2] = Op[2] - closest_sphere.position[2];
	      normalizeVector(normal);
	      printf("light created...\n");
	      light[0] = Dp[0];
	      light[1] = Dp[1];
	      light[2] = Dp[2];
	      normalizeVector(light);
	      printf("reflection created...\n");
	      reflection(normal, light, reflection_light);
	      normalizeVector(reflection_light);
	      printf("camera created...\n");
	      camera_pos[0] = -1*d[0];
	      camera_pos[1] = -1*d[1];
	      camera_pos[2] = -1*d[2];
	      normalizeVector(camera_pos);
	      printf("diffuse creating...\n");
	      diffuse[0] = diffuse_lighting(closest_sphere.diffuse_color[0], lightArray[k].color[0], normal, light);
	      diffuse[1] = diffuse_lighting(closest_sphere.diffuse_color[1], lightArray[k].color[1], normal, light);
	      diffuse[2] = diffuse_lighting(closest_sphere.diffuse_color[2], lightArray[k].color[2], normal, light);
	      printf("specular creating...\n");
	      specular[0] = specular_lighting(closest_sphere.specular_color[0], lightArray[k].color[0], normal, light, reflection_light, camera_pos);
	      specular[1] = specular_lighting(closest_sphere.specular_color[1], lightArray[k].color[1], normal, light, reflection_light, camera_pos);
	      specular[2] = specular_lighting(closest_sphere.specular_color[1], lightArray[k].color[2], normal, light, reflection_light, camera_pos);

	      radial = intensity(lightArray[k], light_distance);
	      printf("radial=%lf\ndiffuse=%lf\nspecular=%lf\n", radial, diffuse[0], specular[0]);
	      //add angular
	      printf("color calculations...\n");
	      color[0] += radial * (diffuse[0] + specular[0]);
	      color[1] += radial * (diffuse[1] + specular[1]);
	      color[2] += radial * (diffuse[2] + specular[2]);
	      printf("colors: r: %lf, g: %lf, b: %lf\n", color[0], color[1], color[2]);
	    }
	    printf("got here...position: %d\n", position);
	  }
	  printf("writing pixels...\n");
	  pixel[position].r = (255 * scale(color[0]));
	  pixel[position].g = (255 * scale(color[1]));
	  pixel[position].b = (255 * scale(color[2]));
	  printf("pixels: r: %lf, g: %lf, b: %lf\n ", pixel[position].r, pixel[position].g, pixel[position].b);
	  }
	
	else{
	  //printf("set pixel to black...\n");
	  pixel[position].r = 0;
	  pixel[position].g = 0;
	  pixel[position].b = 0;
	}
      }
      
     /* for(j = 0; j < planeCount; j++){
	double t= 0;
	t = planeIntersect(origin, d, planeArray[i].position, planeArray[i].normal);
	//fprintf(stdout, "Plane intersection run: %lf\n", t);

	if(t > 0 && t < closest_t){
	  fprintf(stdout, "found a plane intersection!\n");
	  closest_t = t;
	  closest_plane = planeArray[i];
	  pixel[position].r = planeArray[i].color[0] * 255;
	  pixel[position].g = planeArray[i].color[1] * 255;
	  pixel[position].b = planeArray[i].color[2] * 255;
	}
	if(closest_t > 0 && closest_t != 9007199254740992){
	 
	
	  }
	else{
	  pixel[position].r = 0;
	  pixel[position].g = 0;
	  pixel[position].b = 0;
	  }
	 }*/
    }
  }
}

void writeOut(Pixel *pixel, FILE *out_file, int width, int height, int max_color){

  int current_col = 1;
  int i;
  fprintf(out_file, "P3\n%d %d\n%d\n", width, height, max_color);//write the P3 header to the output file
  
  for(i = 0; i < width*height; i++){
    fprintf(out_file, "%d %d %d ", pixel[i].r, pixel[i].g, pixel[i].b);
    if(current_col >= 70%20){
      fprintf(out_file, "\n");
      current_col = 1;
    }
    else{
      current_col++;
    }
  }
}

void testGet(sphere *Sphere){
  double radius;
  radius = Sphere[2].radius;
  fprintf(stdout, "TestGet Radius: %lf\n", radius);
}

int main(int argc, char *argv[]){

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  double radius;
  FILE* out_file = fopen(argv[4], "w");
  //fprintf(stdout, "width = %d\n", width);
  //fprintf(stdout, "height = %d\n", height);
  Pixel *pixel;
  pixel = (Pixel *)malloc(width*height*sizeof(Pixel));
  fprintf(stdout, "Beginning file reading...\n");
  read_json(argv[3]);//call read_json with the input file
  fprintf(stdout, "File reading Completed...\n");
  printf("sphereCount: %d\n", sphereCount);
  printf("planeCount: %d\n", planeCount);
  //radius = sphereArray[2].radius;
  // fprintf(stdout, "Test Radius: %lf\n", radius);
  fprintf(stdout, "Creating Scene...\n");
  construct_scene(camera, pixel, width, height);
  fprintf(stdout, "finished creating scene...\n");
  fprintf(stdout, "Wrinting to ppm...\n");
  writeOut(pixel, out_file, width, height, 255);
  fprintf(stdout, "finished writing...\n");
  exit(0);
}
