#include "stdafx.h"

#include <stdlib.h>
#include <windows.h>
#include <glut.h>
#include "Color.h"
#include "Material.h"
#include "glm.h"
#include <math.h>
// Math //
#define PI 3.141592

float theta = 0;

float kudaphi[4]={0,0,0,0};
float kudarho[4]={0,0,0,0};
float kudatheta[3]={0,0,0};


void kudaHeadUp(float speed)
{
		kudatheta[3]-=speed;
}

void glShadowProjection(float * l, float * e, float * n)   
{   
	float d, c;   
	float mat[16];   

	d = n[0]*l[0] + n[1]*l[1] + n[2]*l[2];   
	c = e[0]*n[0] + e[1]*n[1] + e[2]*n[2] - d;   

	mat[0]  = l[0]*n[0]+c;    
	mat[4]  = n[1]*l[0];    
	mat[8]  = n[2]*l[0];    
	mat[12] = -l[0]*c-l[0]*d;   

	mat[1]  = n[0]*l[1];           
	mat[5]  = l[1]*n[1]+c;   
	mat[9]  = n[2]*l[1];    
	mat[13] = -l[1]*c-l[1]*d;   

	mat[2]  = n[0]*l[2];           
	mat[6]  = n[1]*l[2];    
	mat[10] = l[2]*n[2]+c;    
	mat[14] = -l[2]*c-l[2]*d;   

	mat[3]  = n[0];           
	mat[7]  = n[1];    
	mat[11] = n[2];    
	mat[15] = -d;   

	glMultMatrixf(mat);   
}   

void swap(double *x, double *y) {
	double temp = *x;
	*x = *y;
	*y = temp;
}

double sin_d(double degree) {

	return sin(degree*PI/180.0);

}

double cos_d(double degree) {
	return cos(degree*PI/180.0);
}
// End Math //

// Material //
/*//////////////////////////////////////
CONTROL UNTUK MATERIAL
///////////////////////////////////////*/
//vektor untuk ambiensi material
float material_Ka[4];
//vektor untuk diffusi material
float material_Kd[4];
//vektor untuk spekular material
float material_Ks[4];
//vektor untuk emitansi material
float material_Ke[4];
//variabel untuk kecemerlangan material
float material_Se = 13;
//fungsi untuk mengatur material secara run time
void glEnableMaterial(float* material)
{
	//load all material
	material_Ka[0] = material[0];
	material_Ka[1] = material[1];
	material_Ka[2] = material[2];
	material_Ka[3] = material[3];
	material_Kd[0] = material[4];
	material_Kd[1] = material[5];
	material_Kd[2] = material[6];
	material_Kd[3] = material[7];
	material_Ks[0] = material[8];
	material_Ks[1] = material[9];
	material_Ks[2] = material[10];
	material_Ks[3] = material[11];
	material_Ke[0] = 0;
	material_Ke[1] = 0;
	material_Ke[2] = 0;
	material_Ke[3] = 0;
	material_Se = material[12];

	//here we go
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);
}

void glDisableMaterial() {
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
}
// End Material //

//////////////////////////////////////
/* TEXTURE CODE */
//////////////////////////////////////
GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
// data texture //
GLuint texture[5];
// bool untuk flag apakah texture on atau off //

// untuk load file berekstensi *.raw //
GLuint LoadTextureRAW(const char * filename, int wrap) {
    GLuint texture;
    int width, height;
    BYTE * data;
    FILE * file;

    // open texture data
    file = fopen(filename, "rb");
    if (file == NULL) return 0;

    // allocate buffer
    width = 128;
    height = 128;
    data = (unsigned char *) malloc(width * height * 3);

    // read texture data
    fread(data, width * height * 3, 1, file);
    fclose(file);

    // allocate a texture name
    glGenTextures(1, &texture);

    // select our current texture
    glBindTexture(GL_TEXTURE_2D, texture);
    /* glBindTexture — bind a named texture to a texturing target
     */

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    /* set texture environment parameters
     */

    // when texture area is small, bilinear filter the closest MIP map
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_NEAREST);
    /* set texture parameters
     */

    // when texture area is large, bilinear filter the first MIP map
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
            wrap ? GL_REPEAT : GL_CLAMP);

    // build our texture MIP maps
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width,
            height, GL_RGB, GL_UNSIGNED_BYTE, data);

    // free buffer
    free(data);

    return texture;

}
// method untuk me-relokasi memory //
void FreeTexture(GLuint texture) {
    glDeleteTextures(1, &texture);
}

// mengenable texture 2d //
void globalTextureEnabled() {
	
	//to handle the textures generation and coordinates
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); // Set The Texture Generation Mode
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); // Set The Texture Generation Mode
	glTexGenfv(GL_S, GL_OBJECT_PLANE, slanted);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, slanted);

	glEnable(GL_TEXTURE_2D); //enable texture

}
// mengdisable texture 2d //
void globalTextureDisable() {
	glDisable(GL_TEXTURE_2D);
}

// Camera //
/* 
* CAMERA_MODE 1 = Free Roaming  
* MODE 2 = Model Utama 1 
* MODE 3 = Model Utama 2
*/

GLenum CAMERA_MODE = 1;
GLenum MODE_OBJECT_TYPE = 1;
GLdouble degree = 0.0;
GLdouble constanta_zoom = 5;
GLdouble camera_pos[9] = {0, 5, 5, 0, 0, 0, 0, 1, 0};
void event_camera(GLint key) {
	if (CAMERA_MODE==1) {
		if (key == GLUT_KEY_LEFT) {
			degree -= 1.0f;			
		} else if (key == GLUT_KEY_RIGHT) {
			degree += 1.0f;			
		} else if (key == GLUT_KEY_UP) {
			constanta_zoom -= 0.5;
		} else if (key == GLUT_KEY_DOWN) {
			constanta_zoom += 0.5;
		}
		camera_pos[0] =  constanta_zoom * sin_d(degree);
		camera_pos[2] =  constanta_zoom * cos_d(degree);
	} else if (CAMERA_MODE==2) {

	} else if (CAMERA_MODE==3) {

	}

	// Normalize degree //
	if (degree>360.0) {
		degree-=360.0f;
	} else if (degree<0.0) {
		degree+=360.0f;
	}
	if (constanta_zoom < 1) {
		constanta_zoom = 1;
	} else if (constanta_zoom>20) {
		constanta_zoom = 20;
	}
	//printf("Camera Mode : %d -- Degree : %f -- Zoom Level = %f\n", CAMERA_MODE, degree, constanta_zoom);
}
static float ypoz = 0, zpoz = 0;
static int width = 800, height = 600;
// You need to declare your self a pointer to the model
// This is where the data will be loaded
GLMmodel* modelkuda[12];

GLMmodel* modelgajah[11];


GLMmodel* model_stone = NULL;
GLMmodel* model_tree = NULL;
GLMmodel* model_mountain = NULL;
GLuint ground_texture = NULL;
GLMmodel* model_ground_plane;
// World //
// World - Drawing Border //
float floor_pos_x[4] = {-500, 500, 500, -500}, floor_pos_z[4] = {-500, -500, 500, 500}; 

void draw_fileobj(char * filename, GLMmodel*& model) {
	if (!model) 
	{
		model = glmReadOBJ(filename);	
		if (!model) exit(0);	
		glmUnitize(model);	
		glmFacetNormals(model);        
		glmVertexNormals(model, 90.0);
	}
	glmDraw(model, GLM_SMOOTH | GLM_TEXTURE);
}

void draw_glmobj(GLMmodel*& model, float x, float y, float z, float deg, float degx, float degy, float degz, float scale) {
	if (!model) return;
	glPushMatrix();
	glRotatef(deg, degx, degy, degz);
	glTranslatef(x, y, z);
	glPushMatrix();
	glScalef(scale, scale, scale);
	glmDraw(model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
}

void draw_floor() {
	/*globalTextureEnabled();
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, ground_texture);

	glPushMatrix();
	glBegin(GL_POLYGON);
	glColor3ubv(BROWN);
	for (int n=0; n<4; n++) {
		glVertex3f(floor_pos_x[n], 0, floor_pos_z[n]);
	}
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	globalTextureDisable();*/
	glPushMatrix();
		glScalef(10.0,10.0,10.0);
		draw_fileobj("obj-ground.obj", model_ground_plane);
	glPopMatrix();
	
}



// End World //
void init(void) 
{
	glClearColor (0.0, 0.0, 0.0, 0.0);   
	glEnable(GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);

	// Load texture //
	ground_texture = LoadTextureRAW("ground-plane-texture.raw", true);
}


/*
void drawmodel_horse(void)
{
	glPushMatrix();
	glTranslatef(0, 1, 0);
	glPushMatrix();	
	glRotatef(-90, 1, 0, 0);
	
	// Load the model only if it hasn't been loaded before
	// If it's been loaded then pmodel1 should be a pointer to the model geometry data...otherwise it's null
	if (!pmodel1) 
	{
		// this is the call that actualy reads the OBJ and creates the model object
		pmodel1 = glmReadOBJ("horse.obj");	
		if (!pmodel1) exit(0);
		// This will rescale the object to fit into the unity matrix
		// Depending on your project you might want to keep the original size and positions you had in 3DS Max or GMAX so you may have to comment this.
		glmUnitize(pmodel1);
		// These 2 functions calculate triangle and vertex normals from the geometry data.
		// To be honest I had some problem with very complex models that didn't look to good because of how vertex normals were calculated
		// So if you can export these directly from you modeling tool do it and comment these line
		// 3DS Max can calculate these for you and GLM is perfectly capable of loading them
		glmFacetNormals(pmodel1);        
		glmVertexNormals(pmodel1, 90.0);
	}
	// This is the call that will actualy draw the model
	// Don't forget to tell it if you want textures or not :))
	glmDraw(pmodel1, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glPopMatrix();
}*/
void draw_tree(void) {
	glPushMatrix();	
	glTranslatef(2.3, 2, 0.9);
	glPushMatrix();	
	if (!model_tree) 
	{
		model_tree = glmReadOBJ("obj-palmtree.obj");	
		if (!model_tree) exit(0);		
		glmUnitize(model_tree);		
		glmFacetNormals(model_tree);        
		glmVertexNormals(model_tree, 90.0);
	}
	glmDraw(model_tree, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glPopMatrix();
}

void draw_stone(float x, float y, float z, float deg, float degx, float degy, float degz, float scale) {
	glPushMatrix();
	glRotatef(deg, degx, degy, degz);
	glTranslatef(x, y, z);
	glPushMatrix();
	glScalef(scale, scale, scale);
	if (!model_stone) 
	{		
		model_stone = glmReadOBJ("obj-stone.obj");	
		if (!model_stone) exit(0);		
		glmUnitize(model_stone);		
		glmFacetNormals(model_stone);        
		glmVertexNormals(model_stone, 90.0);
	}
	glmDraw(model_stone, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
}
void draw_mountain(float x, float y, float z, float deg, float degx, float degy, float degz, float scale) {
	glPushMatrix();
	glRotatef(deg, degx, degy, degz);
	glTranslatef(x, y, z);
	glPushMatrix();
	glScalef(scale, scale, scale);
	if (!model_mountain) 
	{
		model_mountain = glmReadOBJ("obj-mountain.obj");	
		if (!model_mountain) exit(0);		
		glmUnitize(model_mountain);		
		glmFacetNormals(model_mountain);        
		glmVertexNormals(model_mountain, 90.0);
	}
	glmDraw(model_mountain, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glPopMatrix();
}

GLfloat LightAmbient[]  = {0.0, 0.0,0.0,1};  // Gray (constant factor)
GLfloat LightDiffuse[]  = {1.0,1.0,1.0,1};  // White (all directions)
GLfloat LightSpecular[] = {1.0,1.0,1.0,1};

// default for specular is black
GLfloat LightPos1[] = {-3.0f, 5.0f, 2.0f, 1.0f};  // behind camera (relative to camera) 
GLfloat floorPoint[] = {0, 0+0.02, 0};
GLfloat floorNormal[] = {0, -1, 0};
void lighting() {

}

void shadow_map(void (*f)(char*, GLMmodel*&, float, float, float, float, float, float, float, float), 
	char * filename, GLMmodel*& obj, float x, float y, float z, float deg, float degx, float degy, float degz, float scale) {
	
	//disable lighting dan texture//
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);

	glColor3ubv(BLACK);
	// apply shadow ke lantai //
	glPushMatrix();
	glShadowProjection(LightPos1, floorPoint, floorNormal);
	f(filename, obj, x, y, z, deg, degx, degy, degz, scale);
	glPopMatrix();

	// setelah selesai gambar shadow, enable kembali texture dan lighting //
	glEnable(GL_TEXTURE);
	glEnable(GL_LIGHTING);
}


void debug_light_pos(GLfloat* pos, GLubyte* color) {
	glColor3ub(255,255,255);
	glPushMatrix();
	glEnableMaterial(Bronze);	
	glTranslatef(pos[0], pos[1], pos[2]);
	glutSolidSphere(1.5, 25, 25);
	glDisableMaterial();
	glPopMatrix();
}

void enable_light() {
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos1);
	glEnable(GL_LIGHT0);
	debug_light_pos(LightPos1, WHITE);
	glEnable(GL_LIGHTING);
}

void disable_light() {
	glDisable(GL_LIGHTING);
}

void draw_plane() {
	disable_light();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	draw_floor();
	enable_light();
}

void display(void)
{
	// Clear //
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Put camera //
	glLoadIdentity ();

	

	enable_light();

	gluLookAt(camera_pos[0],camera_pos[1],camera_pos[2],
		camera_pos[3],camera_pos[4],camera_pos[5],
		camera_pos[6],camera_pos[7],camera_pos[8]);
	// Draw plane //
	glPushMatrix();
	draw_plane();
	glPopMatrix();

	// Wireframe or Shading //
	if (MODE_OBJECT_TYPE==0) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}

	glEnable(GL_MODELVIEW);
	
	glPushMatrix();
		glTranslatef(0,1,0);
		draw_fileobj("torso.obj",modelkuda[0]);
		glPushMatrix();
			glTranslatef(0.1,-0.3,0.44);
			draw_fileobj("leg_front_upper_left.obj",modelkuda[1]);
			glPushMatrix();
				glTranslatef(0.0,-0.5,0.0);
				draw_fileobj("leg_front_lower_left.obj",modelkuda[2]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.1,-0.3,0.44);
			draw_fileobj("leg_front_upper_right.obj",modelkuda[3]);
			glPushMatrix();
				glTranslatef(0.0,-0.5,0.0);
				draw_fileobj("leg_front_lower_right.obj",modelkuda[4]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.1,-0.3,-0.44);
			glScalef(0.65,0.65,0.65);
			draw_fileobj("leg_rear_upper_left.obj",modelkuda[5]);
			glPushMatrix();
				glTranslatef(0.0,-0.5,0.0);
				glScalef(0.65,0.65,0.65);
				draw_fileobj("leg_rear_lower_left.obj",modelkuda[6]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.1,-0.3,-0.44);
			glScalef(0.65,0.65,0.65);
			draw_fileobj("leg_rear_upper_right.obj",modelkuda[7]);
			glPushMatrix();
				glScalef(0.65,0.65,0.65);
				glTranslatef(0.0,-0.5,0.0);
				draw_fileobj("leg_rear_lower_right.obj",modelkuda[8]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,-0.3,-0.7);
			draw_fileobj("tail.obj",modelkuda[9]);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0.38,0.60);
			draw_fileobj("neck.obj",modelkuda[10]);
			glTranslatef(0,0.11,0.17);
			glPushMatrix();
				draw_fileobj("head.obj",modelkuda[11]);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(2,1,0);
			glScalef(0.65,0.65,0.65);
		draw_fileobj("ele_torso.obj",modelgajah[0]);
		glPushMatrix();
			glTranslatef(0.25,-0.36,0.58);
			glScalef(0.5,0.5,0.5);
			draw_fileobj("ele_upper_front_left.obj",modelgajah[1]);
			glPushMatrix();
				glTranslatef(0,-1,0);
				glScalef(1,1,1);
				draw_fileobj("ele_lower_front_left.obj",modelgajah[2]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.25,-0.36,0.58);
			glScalef(0.5,0.5,0.5);
			draw_fileobj("ele_upper_front_right.obj",modelgajah[3]);
			glPushMatrix();
				glTranslatef(0,-1,0);
				glScalef(1,1,1);
				draw_fileobj("ele_lower_front_right.obj",modelgajah[4]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.25,-0.36,-0.58);
			glScalef(0.5,0.5,0.5);
			draw_fileobj("ele_upper_rear_left.obj",modelgajah[5]);
			glPushMatrix();
				glTranslatef(0.0,-1.0,0.0);
				glScalef(1,1,1);
				draw_fileobj("ele_lower_rear_left.obj",modelgajah[6]);			
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.25,-0.36,-0.58);
			glScalef(0.5,0.5,0.5);	
			draw_fileobj("ele_upper_rear_right.obj",modelgajah[7]);
			glPushMatrix();
				glTranslatef(0.0,-1,0.0);
				glScalef(1,1,1);		
				draw_fileobj("ele_lower_rear_right.obj",modelgajah[8]);
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,-0.7,-1);
			draw_fileobj("ele_tail.obj",modelgajah[9]);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,-0.2,1.3);
			glRotatef(kudatheta[3],1,0,0);
			draw_fileobj("ele_head.obj",modelgajah[10]);
		glPopMatrix();
	glPopMatrix();

	//draw_tree();
	draw_mountain(7.5, 2.5, 0.2, 0, 0, 0,0,5);
	//glPopMatrix();
	
	glutSwapBuffers();
	
}

void reshape (int w, int h)
{
	width = w;
	height = h;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode (GL_MODELVIEW);
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	}	

}



void sp_keyboard(int key, int x, int y) {
	event_camera(key);
}

void animate()
{
	kudaHeadUp(0.1);
	glutPostRedisplay();
}

void setModeObject(int input) {
	MODE_OBJECT_TYPE = input;
}

void init_menu_select() {
	int id_mode_object = glutCreateMenu(setModeObject);
	glutAddMenuEntry("WireFrame", 0);
	glutAddMenuEntry("Shading", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize (width, height); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init();
	init_menu_select();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(sp_keyboard);
	glutIdleFunc(animate);
	glutMainLoop();
	return 0;
}

