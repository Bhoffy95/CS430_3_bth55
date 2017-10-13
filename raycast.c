#include <stdio.h>
#include <stdlib.h>


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
int next_char(FILE* file);

void read_json(char* filename);

void expected_char(FILE* file, int n);

void skip_ws(FILE* file);

char* next_string(FILE* file);

double next_num(FILE* file);

double* next_vector(FILE* file);

int line = 1;



int next_char(FILE* file){
  int n = fgetc(file);

  if(n == '\n'){
    line += 1;
  }
  /*if (n == EOF){
    fprintf(stderr, "Error: File end reached unexpectedly.");
    fclose(file);
    exit(1);
    }*/
  return n;
}

void skip_to(FILE* file, int n){
  int x = next_char(file);
  if(x == n){
    return;
  }
  else{
    fprintf(stderr, "Error: expected '%c' on line %d.\n", n, line);
    fclose(file);
    exit(1);
    }
}

void skip_space(FILE* file){
  int n = next_char(file);
  while(n = 10){
    line += 1;
    n = next_char(file);
  }
  ungetc(n, file);
}

char* next_string(FILE *file){
  char buffer[129];
  int n;
  int i = 0;
  /* if(n != '"'){
    fprintf(stderr, "Error: Expected string beginning on line %d.\n", line);
    fclose(file);
    exit(1);
    }*/
  //skip_space(file);
  n = next_char(file);
  
  while(n != ',' && n != ':' && n != '['){
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
    n = next_char(file);
  }
  buffer[i] = 0;
  return (char *)strdup(buffer);
}

double next_num(FILE* file){
  double value;
  int count = fscanf(file, "%lf", &value);
  if(count != 1){
    fprintf(stderr, "Error: Failed to read number.");
    fclose(file);
    exit(1);
  }
  return value;
}

void read_json(char *file){
  
  int n;
  int temp_type;
  int item_count = 0;
  FILE* json = fopen(file, "r");
  char* key;
  char* val;
  if (json == NULL){
    fprintf(stderr, "Error: No file found named \"%s\"\n", file);
    fclose(json);
    exit(1);
  }
  else{
    fprintf(stdout, "File not empty.\n");
  }
  n = fgetc(json);
  if(n < 97 || n > 122){
    fprintf(stderr, "Error: non-letter charcter. %s.\n", n);
    fclose(json);
    exit(1);
  }
  else{
    ungetc(n, json);
  }
  key = next_string(json);
  fprintf(stdout, "String = %s \n", key);
  if(strcmp(key, "camera") == 0){
    Camera camera = {0, 0};
    fprintf(stdout, "Found a camera object...\n");
    val = next_string(json);
    fprintf(stdout, "Second string: %s\n", val);
    while(n != 10){
      if(strcmp(val, "width") == 0){
	fprintf(stdout, "Found 'width' id of camera.\n");
	camera.width = next_num(json);
	fprintf(stdout, "Camera width: %lf\n", camera.width);
	skip_to(json, ',');
        val = next_string(json);
	fprintf(stdout, "third value: %s\n", val);
      }
      else if(strcmp(val, "height") == 0){
	fprintf(stdout, "Found 'height' id of camera.\n");
	camera.height = next_num(json);
	fprintf(stdout, "Camera height: %lf\n", camera.height);
	n = next_char(json);
	val = next_string(json);
      }
      else{
	fprintf(stderr, "Error: invalid property found for camera. Found: %s.\n", val);
	fclose(json);
	exit(1);
      }
   } 
  }
  else if(strcmp(key, "sphere") == 0){
    sphere Sphere = {};
    fprintf(stdout, "Got Here.....\n");
    sphereArray[item_count] = Sphere;
    fprintf(stdout, "Got Here.....\n");
    sphereArray[item_count].type = 0;
    fprintf(stdout, "Found a sphere object...\n");
    val = next_string(json);
    fprintf(stdout, "After Sphere Value: %s\n", val);
    item_count++;
    while(n != 10){
      if(strcmp(val, "position") == 0){
	fprintf(stdout, "Position Sphere Found.\n");
	Sphere.position[0] = next_num(json);
	next_string(json);
	Sphere.position[1] = next_num(json);
	next_string(json);
	Sphere.position[2] = next_num(json);
	next_string(json);
	fprintf(stdout, "sphere position 1: %lf, pos 2: %lf, pos 3: %lf\n", Sphere.position[0], Sphere.position[1], Sphere.position[2]);
	val = next_string(json);
      }
      else if(strcmp(val, "radius") == 0){
	fprintf(stdout, "radius Sphere Found.\n");
	Sphere.radius = next_num(json);
	n = next_char(json);
	fprintf(stdout, "Rad next string: %d\n", n);
	fprintf(stdout, "sphere radius: %lf", Sphere.radius);
	val = next_string(json);
      }
      else if(strcmp(val, "color") == 0){
	fprintf(stdout, "Color Sphere Found.\n");
	skip_to(json, '[');
	Sphere.color[0] = next_num(json);
	next_string(json);
	Sphere.color[1] = next_num(json);
	next_string(json);
	Sphere.color[2] = next_num(json);
	next_string(json);
	fprintf(stdout, "sphere color 1: %lf, pos 2: %lf, pos 3: %lf\n", Sphere.color[0], Sphere.color[1], Sphere.color[2]);
	val = next_string(json);
      }
      else{
	fprintf(stderr, "Error: Invalid properties for Sphere.\n");
	fclose(json);
	exit(1);
      }
    }
  }
  else if(strcmp(key, "plane") == 0){
    plane Plane = {};
    planeArray[item_count] = Plane;
    planeArray[item_count].type = 1;
    fprintf(stdout, "Found a plane object...");
    val = next_string(json);
    item_count++;
    while(n != 10){
      if(strcmp(val, "position") == 0){
	fprintf(stdout, "Position Plane Found.\n");
	next_char(json);
	fprintf(stdout, "position first char: %s\n", val);
	Plane.position[0] = next_num(json);
	next_string(json);
	Plane.position[1] = next_num(json);
	next_string(json);
	Plane.position[2] = next_num(json);
	next_string(json);
	fprintf(stdout, "plane position 1: %lf, pos 2: %lf, pos 3: %lf\n", Plane.position[0], Plane.position[1], Plane.position[2]);
	val = next_string(json);
      }
      else if(strcmp(val, "normal") == 0){
	fprintf(stdout, "normal Plane Found Val: %s.\n", val);
	fprintf(stdout, "%s\n", next_string(json));
	Plane.normal[0] = next_num(json);
	fprintf(stdout, "normal num1: %d\n", Plane.normal[0]);
	next_string(json);
	Plane.normal[1] = next_num(json);
	next_string(json);
	Plane.normal[2] = next_num(json);
	skip_to(json, ']');
	n = next_char(json);
	if(n = -1){
	  break;
	}
	fprintf(stdout, "n value: %d", n);
	fprintf(stdout, "plane normal 1: %lf, pos 2: %lf, pos 3: %lf\n", Plane.normal[0], Plane.normal[1], Plane.normal[2]);
      }
      else if(strcmp(val, "color") == 0){
	fprintf(stdout, "Color plane Found.\n");
	next_string(json);
	Plane.color[0] = next_num(json);
	next_string(json);
	Plane.color[1] = next_num(json);
	next_string(json);
	Plane.color[2] = next_num(json);
	next_string(json);
	fprintf(stdout, "Plane color 1: %lf, pos 2: %lf, pos 3: %lf\n", Plane.color[0], Plane.color[1], Plane.color[2]);
	val = next_string(json);
      }
      else{
	fprintf(stderr, "Error: Invalid properties for Plane.\n");
	fclose(json);
	exit(1);
      }
    }
  }
  else{
    fprintf(stderr, "Error: Invalid object type.\n");
    fclose(json);
    exit(1);
  }
}

int main(int argc, char *argv[]){

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  // FILE* out_file = fopen(argv[4], "w");
  fprintf(stdout, "width = %d\n", width);
  fprintf(stdout, "height = %d\n", height);
  fprintf(stdout, "Beginning file reading...");
  read_json(argv[3]);
  fprintf(stdout, "File reading Completed...");
  exit(0);
}
