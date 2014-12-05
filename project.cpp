#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <GL/glew.h> 
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
// #include "imageloader.h" //used this and imageloader.cpp to handle loading of BMP textures. I did not write it.
using namespace std;


//Menu Options List
enum MenuOptions
{
	MENU_LIGHT,
	MENU_TEXTURE,
	MENU_BOUNCE,
	MENU_ROLL,
	MENU_EXIT,
	NOTHING
};

//This class is used to create an array of pixels for the image
class Image {
	public:
		Image(char* ps, int w, int h);
		~Image();
		char* pixels;
		int width;
		int height;
};



//static declarations and definitions
typedef int BOOL; //define type boolean for easier statements
	#define TRUE 1
	#define FALSE 0
static float g_lightPos[4] = { 100, 100, -10, 1 };
static BOOL bounce = false;
static GLfloat translation = 4.0;
static BOOL up = false;
static float maxUp = 4;
static float zoom = 10;
static float rotate = 1.5;
static int r = 1;
static float t = 0;
static BOOL roll = false;
static int xmouse = 0; 
static int ymouse = 0;
static float rotatePot = 0;
static int windowWidth = 800;
static int windowHeight = 600;
static BOOL lightSwitch = true;
GLuint _textureId;
///////////////////////////

//method declerations
void Display(void);
void RenderObjects(void);
void AnimateScene(void);
void Reshape(GLint width, GLint height);
void Bounce(void);
void Keyboard(unsigned char key, int x, int y);
void Reset(void);
void Roll(void);
void MouseButton(int button, int state, int x, int y);
void Zoom(int z);
void Roll(float r);
void SelectFromMenu(int idCommand);
int BuildPopupMenu();
void processSpecialKeys(int key, int x, int y);
Image* loadBMP(const char* filename);
GLuint loadTextureFromImage(Image* image);
int toInt(const char* bytes);
int readInt(ifstream &input);
///////////////////////////

//main method
int main(int argc, char** argv) {
	// GLUT Window Initialization:
	glutInit (&argc, argv);
	glutInitWindowSize (windowWidth, windowHeight);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("David & Taylor's Amazing OpenGL Project - A++++++");

	// Initialize OpenGL. Put it here instead for my easier reading.
	glEnable(GL_DEPTH_TEST);
	glutMouseFunc (MouseButton);
	BuildPopupMenu ();
	glutAttachMenu (GLUT_RIGHT_BUTTON);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    //load texture. Uses imageloader.h which uses imageloader.cpp
	Image* img = loadBMP("ball.bmp");
    _textureId = loadTextureFromImage(img);
    delete img;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glutDisplayFunc (Display);
	glutReshapeFunc (Reshape);
	glutKeyboardFunc (Keyboard);
	glutSpecialFunc(processSpecialKeys);
    glutIdleFunc (AnimateScene);

	// Start the main loop
	glutMainLoop ();
	return 0;
}
///////////////////////////

//display method
void Display(void) {
	// Clear frame buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set up the view
	glLoadIdentity();
	gluLookAt(cos(rotate) * zoom , 3,(zoom * -sin(rotate)), 0, 0, 0, 0, 1, 0);	// Set up the camera and allow for zoom and rotation :)
	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);

	// Render the scene
	RenderObjects();
	glutSwapBuffers();

	// Render Bounce if bounce == true
	if (bounce) {
	   if (maxUp <= 0) {
	    	bounce = false;
	    }

		if (!up && translation <= 1.5 ) {
	    	up = true;
	    	maxUp -= 0.50;
	    }
	    if (up && translation > maxUp) {
	    	up = false;
	    }

		if(up == false) {
			translation -=  0.1; //0.02
		} else {
			translation +=  0.1;
		} 

	}

	//Render roll effect if roll = true
	if (roll) {
		if (maxUp <=0) {
			if (r <= 355) {
				r+=3;
				t+=.05;
			} else {
				roll = !roll;
			}
		} else { //Don't roll if it isn't on the ground!!
			roll = !roll;
		}
	}
}
///////////////////////////

//Object renderer
void RenderObjects(void) {
	float colorBronzeDiff[4] = { 0.8, 0.6, 0.0, 1.0 }; //borrowed these colors from your sample code :)
	float colorBronzeSpec[4] = { 1.0, 1.0, 0.4, 1.0 };
	float colorNone[4]       = { 0.0, 0.0, 0.0, 0.0 };
	float colorWhite[4]       = { 1.0, 1.0, 1.0, 1.0 };


	glMatrixMode(GL_MODELVIEW);

	//make lines for plane
	glPushMatrix();
		glBegin(GL_LINES);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorNone);
		for (GLfloat i = -10.5; i <= 10.5; i += 0.25) {
			glVertex3f(i, 0, 10.5); glVertex3f(i, 0, -10.5);
			glVertex3f(10.5, 0, i); glVertex3f(-10.5, 0, i);
		}
		glEnd();
	glPopMatrix();

	//Making the Sphere
	glPushMatrix();
		GLUquadricObj *qObj = gluNewQuadric();
		gluQuadricNormals(qObj, GLU_SMOOTH);
		gluQuadricTexture(qObj, GL_TRUE);
		glTranslatef(xmouse, translation + ymouse, 0 + t);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorWhite);

		//enable texture
		glEnable(GL_TEXTURE_2D); 
		glBindTexture(GL_TEXTURE_2D,_textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glRotated(r,r,0,0);
		gluSphere(qObj, 1.0f, 24, 24);
		glDisable(GL_TEXTURE_2D); //disable texture so teapot looks bronze still!
	glPopMatrix();


	//Making the teapot
	glPushMatrix();
		glTranslatef(4, .75, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorBronzeDiff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorBronzeSpec);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
		glRotatef(rotatePot,0,1,0);
		glColor4fv(colorBronzeDiff);
		glTranslatef(xmouse, ymouse, 0);
		glutSolidTeapot(1);
	glPopMatrix();

}
///////////////////////////

//idle animator method
void AnimateScene(void) {
	glClearColor(.8, .8, 1, .8);
	glutPostRedisplay();
}
///////////////////////////

//Shaping function
void Reshape(GLint width, GLint height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (float)width / height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}
///////////////////////////

//Bounce function
void Bounce(void) {
	if (maxUp > 0)
		bounce = !bounce;
	glutPostRedisplay();
}
///////////////////////////

//Zoom function
void Zoom(int z) {
	zoom += z;
	if (zoom > 15)
		zoom = 15;
	if (zoom < 5)
		zoom = 5;
}
///////////////////////////

//Rotate function
void Rotate(float r) {
	rotate += r;
}
///////////////////////////

//Roll function
void Roll(void) {
	roll = !roll;
	glutPostRedisplay();
}
///////////////////////////


//Reset function to reset scene
void Reset(void) {
	maxUp = 4;
	up = FALSE;
	translation = 4;
	zoom = 10;
	rotate = 1.5;
	r = 1;
	t = 0;
	roll = FALSE;
	bounce = FALSE;
	xmouse = 0;
	ymouse = 0;
	rotatePot = 0;
	glEnable(GL_LIGHTING);
}
///////////////////////////


//Register Keyboard Commands
void Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
		case 'd':  
		SelectFromMenu(MENU_BOUNCE);
	break;
		case 'w':
		Zoom(-1);
	break;
		case 's':
		Zoom(1);
	break;
		case 'q':
		Rotate(-.1);
	break;
		case 'e':
		Rotate(.1);
	break;
		case 'r':
		Reset();
	break;
		case 'p':
		SelectFromMenu(MENU_ROLL);
	break;
		case 27:
		exit(0);
	break;
	}
}

void processSpecialKeys(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_LEFT :
				rotatePot-=3;
		break;
		case GLUT_KEY_RIGHT :
				rotatePot+=3;
		break;
		case GLUT_KEY_UP :
				Zoom(-1);
		break;
		case GLUT_KEY_DOWN :
				Zoom(1);
		break;
	}
}
///////////////////////////

//Register Mouse Click for moving objects
void MouseButton(int button, int prostate, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	    {
	    		if (y < (windowWidth/2) && ymouse < 4) {
	    			ymouse++;
	    		} else if (y > (windowWidth/2) && ymouse > -4) {
	    			ymouse--;
	    		}

	    		if (x < (windowHeight/2) && xmouse < 4) {
	    			xmouse++;
	    		} else if (x > (windowHeight/2) && xmouse > -4) {
	    			xmouse--;
	    		}

	    }
}
///////////////////////////

//Create the right click menu
int BuildPopupMenu (void)
{
	int menu;
	menu = glutCreateMenu (SelectFromMenu);
	glutAddMenuEntry ("RIGHT CLICK ON SELECTION", NOTHING);
	glutAddMenuEntry ("Flick the Lights", MENU_LIGHT);
	glutAddMenuEntry ("[D]rop the Ball", MENU_BOUNCE);
	glutAddMenuEntry ("[P]ush the Ball", MENU_ROLL);
	glutAddMenuEntry ("[Esc]ape/Exit", MENU_EXIT);
	return menu;
}

void SelectFromMenu(int idCommand)
{
	switch (idCommand)
	{
		case MENU_LIGHT:
			lightSwitch = !lightSwitch;
			if (lightSwitch)
		 		glEnable(GL_LIGHTING);
			else
		 		glDisable(GL_LIGHTING);
		break;  
		case MENU_BOUNCE:
			Bounce();
		break;
		case MENU_ROLL:
			Roll();
		break;
		case MENU_EXIT:
			exit(0);
		break;
	}
	glutPostRedisplay();
}
///////////////////////////

//Define the Image types for the texture
Image::Image(char* ps, int w, int h) : pixels(ps), width(w), height(h) {

}

Image::~Image() {
	
}
///////////////////////////

//Convert a char to an integer
int toInt(const char* bytes) {
	return (int)(((unsigned char)bytes[3] << 24) |
		((unsigned char)bytes[2] << 16) |
		((unsigned char)bytes[1] << 8) |
		(unsigned char)bytes[0]);
}
///////////////////////////


//Reads the next four bytes as a char and converts them to an integer using toInt
int readInt(ifstream &input) {
	char buffer[4];
	input.read(buffer, 4);
	return toInt(buffer);
}
///////////////////////////

//Load the Bitmap file (found on the web)
Image* loadBMP(const char* filename) {
	ifstream infile(filename, ifstream::binary);
	infile.seekg(10, std::ios::cur);
	int dataOffset = readInt(infile);

	//Read the header
	int headerSize = readInt(infile);
	int width = readInt(infile);
	int height = readInt(infile);

	//Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
	char* pixels = new char[size];
	infile.seekg(dataOffset, ios_base::beg);
	infile.read(pixels, size);
	infile.close();
	//Get the data into the right format
	char* pixels2 = new char[width * height * 3];
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
				pixels2[3*(width*y + x) + c] = pixels[bytesPerRow*y + 3*x + (2 - c)];
			}
		}
	}
	delete[] pixels;
	return new Image(pixels2, width, height);
}
///////////////////////////

//Convert image (bitmap) to OpenGL unsigned integer which will be our actual texture
GLuint loadTextureFromImage(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId);
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D, 
		0,
		GL_RGB,
		image->width, image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);
	return textureId;
}
///////////////////////////

