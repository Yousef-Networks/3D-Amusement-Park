#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>


#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

int windowWidth = 800;
int windowHeight = 550;

float colorR = 0.0f;
float colorG = 0.5f;
float colorB = 1.0f;

bool switchColor = false;

bool animation = false;

float carColorR = 1.0f;
float carColorG = 0.0f;
float carColorB = 0.0f;
float PlayerX = 0.3f, PlayerY = 0.0f, PlayerZ = 0.4f, playerAngle = 0.0f;

float diskYOffset = 0.0;
float objXOffset = 0.0;

bool gameOver = false;

enum GameState { PLAYING, WIN, LOSE };
GameState gameState = PLAYING;

// Define game time limit in seconds
const int gameTimeLimit = 30; // Change as needed

// Track elapsed time
int elapsedTime = 0;



void changeColor(int value) {
	// Change color components based on a gradual change

	switchColor = !switchColor; // Toggle between two colors
	if (switchColor) {
		// First shade of blue
		colorR = 0.0f;
		colorG = 0.0f;
		colorB = 0.8f;
	}
	else {
		// Second shade of blue
		colorR = 0.4f;
		colorG = 0.5f;
		colorB = 0.9f;
	}

	carColorR += 0.25f;
	carColorG += 0.1f;
	carColorB += 0.1f;

	if (carColorR > 1.0f) carColorR = 0.5f;
	if (carColorG > 0.5f) carColorG = 0.1f;
	if (carColorB > 0.5f) carColorB = 0.1f;


	glutPostRedisplay();
	glutTimerFunc(3000, changeColor, 0); // Change color every 3 seconds (3000 milliseconds)
}


class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;


void Timer(int value) {
	if (gameState == PLAYING) {
		elapsedTime++;

		if (elapsedTime >= gameTimeLimit) {
			gameOver = true;
			if (gameState != WIN)
				gameState = LOSE;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(1000, Timer, 0); // Update timer every second (1000 milliseconds)
}



void drawWall(double thickness) {
	glColor3f(colorR, colorG, colorB);
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);

	glTranslatef(0, 0, 0.1); // Move slightly forward to draw clouds





	glPopMatrix();
}
void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawTable(double topWid, double topThick, double legThick, double legLen) {

	glColor3f(1.0f, 0.843f, 0.0f);
	glPushMatrix();

	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void drawFloor() {
	glColor3f(0.6f, 0.3f, 0.1f); // Brown color for the ground

	glPushMatrix();
	glTranslated(0.4, 0.0, 0.4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.5, 0.5 * 0.02, 0.5);
	glScaled(1.0, 0.02, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}


void drawFences() {

	glColor3f(0.5f, 0.8f, 1);


	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWall(0.02);

	glPushMatrix();
	glColor3f(1.0, 0.8, 0.0);  // Light gray color for clouds
	glTranslated(0.5, 0, 0.85);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 0.8, 0.0);  // Light gray color for clouds
	glTranslated(0.45, 0, 0.8);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 0.8, 0.0);  // Light gray color for clouds
	glTranslated(0.55, 0, 0.8);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();


	glPopMatrix();



	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);

	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);  // Red color for the sun
	glTranslated(0.9, 0, 0.9);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();

	glPopMatrix();


	glPushMatrix();
	glRotated(90, 0, 0, 1.0);

	glTranslatef(0, -1.0, 0);
	drawWall(0.02);

	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);  // Light gray color for clouds
	glTranslated(0.85, 0, 0.5);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);  // Light gray color for clouds
	glTranslated(0.8, 0, 0.45);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);  // Light gray color for clouds
	glTranslated(0.8, 0, 0.55);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
	glPopMatrix();



	glPopMatrix();

}

void drawPlayer() {
	// Set Color for Player Body
	glColor3f(0.2f, 0.4f, 0.8f); // RGB for blue color

	// Draw Player Head (Changed to Cone Shape)
	glPushMatrix();
	glTranslated(PlayerX, 0.5, PlayerZ);
	GLUquadricObj* headQuadric = gluNewQuadric();
	gluCylinder(headQuadric, 0.0, 0.03, 0.06, 20, 20);
	glPopMatrix();

	// Set Color for Other Body Parts
	glColor3f(0.8f, 0.2f, 0.4f); // RGB for pink color

	// Draw Player Torso (Changed to Sphere)
	glPushMatrix();
	glTranslated(PlayerX, 0.44, PlayerZ);
	glutSolidSphere(0.04, 20, 20);
	glPopMatrix();

	// Draw Player Arms (Changed to Cubes)
	glPushMatrix();
	glTranslated(PlayerX - 0.03, 0.48, PlayerZ);
	glScaled(0.02, 0.02, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(PlayerX + 0.03, 0.48, PlayerZ);
	glScaled(0.02, 0.02, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw Player Legs (Changed to Cylinders)
	glPushMatrix();
	glTranslated(PlayerX - 0.015, 0.42, PlayerZ);
	glRotated(90, 1, 0, 0);
	GLUquadricObj* legQuadric = gluNewQuadric();
	gluCylinder(legQuadric, 0.01, 0.01, 0.1, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslated(PlayerX + 0.015, 0.42, PlayerZ);
	glRotated(90, 1, 0, 0);
	gluCylinder(legQuadric, 0.01, 0.01, 0.1, 10, 10);
	glPopMatrix();

	gluDeleteQuadric(headQuadric);
	gluDeleteQuadric(legQuadric);
}




void drawSwingSet() {
	// Frame (slanted rectangles)
	glColor3f(1.0f, 0.84f, 0.0f); // Golden color

	glPushMatrix();
	glTranslatef(-0.5f, 0.0f, 0.0f);
	glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.015f, 0.65f, 0.03f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3f, 0.0f, 0.0f);
	glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.015f, 0.65f, 0.03f);
	glutSolidCube(1.0f);
	glPopMatrix();



	// Top bars (vertical rectangles)
	glColor3f(1.0f, 0.84f, 0.0f); // Golden color

	glPushMatrix();
	glTranslatef(-0.425f, 0.02, 0.025f);
	glScalef(0.004f, 0.3f, 0.002f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glColor3f(1.0f, 0.84f, 0.0f); // Golden color

	glPushMatrix();
	glTranslatef(-0.375f, 0.02, 0.025f);
	glScalef(0.004f, 0.3f, 0.002f);
	glutSolidCube(1.0f);
	glPopMatrix();



	// Seat (cube)
	glColor3f(0.0f, 0.0f, 1.0f); // Red color for the seat
	glPushMatrix();
	glTranslatef(-0.405f, -0.035, 0.025f);
	glScalef(0.06f, 0.05f, 0.05f);
	glutSolidCube(1.0f);
	glPopMatrix();

}





void drawTree() {


	static float angle = 90.0f;
	// Draw the trunk (brown rectangular prism)
	glColor3f(0.54f, 0.27f, 0.07f); // Brown color for the trunk
	glPushMatrix();
	glTranslated(0.1, 0.0, 0.1); // Position the tree in the corner
	glRotated(angle, 0.0f, 1.0f, 0.0f); // Rotate to stand upright
	glScalef(0.1, 0.1, 0.4); // Adjust the dimensions of the trunk
	glutSolidCube(1.0); // Draw the trunk
	glPopMatrix();

	// Draw the branches (brown rectangular prisms)
	glColor3f(0.54f, 0.27f, 0.07f); // Brown color for the branches
	glPushMatrix();
	glTranslated(0.1, 0.2, 0.1); // Position the branches on top of the trunk

	// Draw three branches
	for (int i = 0; i < 3; ++i) {
		glPushMatrix();
		glRotated(i * 45, 0, 1, 0); // Rotate branches around the trunk
		glTranslated(0, 0, 0.15); // Move to the end of the trunk
		glScalef(0.05, 0.05, 0.3); // Adjust the dimensions of the branches
		glutSolidCube(1.0); // Draw the branches
		glPopMatrix();
	}

	glPopMatrix();

	// Draw the leaves (green cone)
	glColor3f(0.0f, 0.5f, 0.0f); // Green color for the leaves
	glPushMatrix();
	glTranslated(0.1, 0.2, 0.1); // Position the leaves above the branches
	glRotated(-90, 1, 0, 0); // Rotate to stand upright
	glutSolidCone(0.2, 0.4, 10, 10); // Draw the leaves as a cone
	glPopMatrix();
	angle += 0.2f;
	if (angle >= 360.0f) {
		angle -= 360.0f;
	}
	glutPostRedisplay();
}




void drawLighthouse() {
	// Update rotation angle for animation
	static float angle = 60.0f;
	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	// Base of the lighthouse (cylinder)
	glColor3f(0.5f, 0.5f, 0.5f); // Gray color for the base
	glPushMatrix();
	glTranslated(0.0, -0.1, 0.0); // Adjust position to stand upright
	glRotated(-90, 1, 0, 0); // Rotate to stand upright
	glScaled(1.2, 1.2, 1.2); // Increase the size of the base
	GLUquadricObj* base = gluNewQuadric();
	gluCylinder(base, 0.08, 0.08, 0.2, 20, 20); // Draw the base cylinder
	glPopMatrix();

	// Top of the lighthouse (cone)
	glColor3f(0.8f, 0.8f, 0.0f); // Yellow color for the top
	glPushMatrix();
	glTranslated(0.0, 0.1, 0.0); // Adjust position above the base
	glRotated(-90, 1, 0, 0); // Rotate to stand upright
	glutSolidCone(0.08, 0.2, 20, 20); // Draw the cone
	glPopMatrix();

	// Stripes on the lighthouse
	glColor3f(1.0f, 1.0f, 1.0f); // White color for the stripes
	glPushMatrix();
	for (int i = 0; i < 3; ++i) {
		glutSolidCube(0.08); // Draw three white stripes on the base
		glTranslated(0.0, 0.03, 0.0); // Move to the next stripe position
	}
	glPopMatrix();
	glPopMatrix();
	angle += 0.3f;
	if (angle >= 360.0f) {
		angle -= 360.0f;
	}
	glutPostRedisplay();
}









void drawTower() {
	// Base cylinder
	glColor3f(0.0f, 0.7f, 0.0f); // Green color for the base

	glPushMatrix();
	glTranslated(0.0, -0.25, 0.0); // Translate the tower down by half of its height
	glRotated(-90, 1.0, 0.0, 0.0); // Orient the cylinder upright
	GLUquadricObj* cylinder = gluNewQuadric();
	gluCylinder(cylinder, 0.05, 0.05, 0.5, 10, 10); // Adjust cylinder size and height
	glPopMatrix();

	// Disk
	glColor3f(0.0f, 0.7f, 0.0f); // Green color for the base

	glPushMatrix();
	glTranslated(0.0, 0.4 - diskYOffset, 0.0);
	glRotated(90, 1.0, 0.0, 0.0); // Orient the disk properly
	glutSolidTorus(0.04, 0.1, 10, 10); // Adjust the torus parameters for the disk size
	glPopMatrix();

	// Hanging triangles
	glColor3f(0.9f, 0.5f, 0.8f); // Green color for the triangles
	glPushMatrix();
	glTranslated(0.0, 0.6, 0.0); // Position the triangles above the disk
	for (int i = 0; i < 4; ++i) {
		glPushMatrix();
		glRotated(90 * i, 0.0, 1.0, 0.0); // Rotate each triangle around the center
		glBegin(GL_TRIANGLES);
		glVertex3f(0.0f, 0.0f, 0.0f); // Top point of the triangle
		glVertex3f(0.075f, -0.1f, 0.0f); // Bottom right point
		glVertex3f(-0.075f, -0.1f, 0.0f); // Bottom left point
		glEnd();
		glPopMatrix();
	}
	glPopMatrix();
}





void drawChair() {
	// Seat of the chair
	glColor3f(0.0f, 0.0f, 0.0f); // Brown color for the seat
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f); // Position the chair
	glScalef(0.1f, 0.01f, 0.1f); // Adjust the size
	glutSolidCube(1.0f);
	glPopMatrix();

	// Backrest of the chair
	glColor3f(0.0f, 0.0f, 0.0f); // Brown color for the backrest
	glPushMatrix();
	glTranslatef(0.0f, 0.05f, -0.05f); // Position the backrest
	glScalef(0.1f, 0.1f, 0.01f); // Adjust the size
	glutSolidCube(1.0f);
	glPopMatrix();

	// Legs of the chair
	glColor3f(0.0f, 0.0f, 0.0f); // Brown color for the legs

	glPushMatrix();
	glTranslatef(-0.04f, -0.05f, 0.04f); // Position the legs
	glScalef(0.005f, 0.1f, 0.005f); // Adjust the size
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.04f, -0.05f, 0.04f); // Position the legs
	glScalef(0.005f, 0.1f, 0.005f); // Adjust the size
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.04f, -0.05f, -0.04f); // Position the legs
	glScalef(0.005f, 0.1f, 0.005f); // Adjust the size
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.04f, -0.05f, -0.04f); // Position the legs
	glScalef(0.005f, 0.1f, 0.005f); // Adjust the size
	glutSolidCube(1.0f);
	glPopMatrix();
}





void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (gameOver) {
		// Display game win or game lose screen
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0f, 1.0f, 1.0f); // White text color

		std::string message;
		if (gameState == WIN) {
			message = "Game Win!";
		}
		else {
			message = "Game Lose!";
		}

		// Display game win or game lose message
		glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) / 2 - 50, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		for (const char& c : message) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
		}
	}
	else {


		// Display elapsed time
		glColor3f(1.0f, 1.0f, 1.0f); // White text color
		glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) / 2 - 20, glutGet(GLUT_WINDOW_HEIGHT) / 2 - 50);
		std::string timeMsg = "Time: " + std::to_string(elapsedTime) + "s";
		for (const char& c : timeMsg) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
		}



		drawFloor();
		drawFences();

		// Check collision with house
		float dx = PlayerX - 0.2f;
		float dz = PlayerZ - 0.8f;
		float distance = sqrt(dx * dx + dz * dz);

		if (distance < 0.125f) {
			// Collision detected, the player collided with the house
			gameState = WIN;
			gameOver = true;
		}

		glPushMatrix();
		glTranslatef(0, -0.3f, 0);
		glTranslatef(PlayerX, PlayerY, PlayerZ);  // Translate to player position
		glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);
		glTranslatef(-PlayerX, -PlayerY, -PlayerZ); // Rotate around Y-axis (for example)
		drawPlayer();

		glPopMatrix();



		glPushMatrix();
		glTranslatef(objXOffset, 0, 0);
		drawTree();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.7f + objXOffset, 0, 0.1f);
		drawTable(0.2f, 0.02f, 0.03f, 0.1f);
		glPopMatrix();

		if (gameState != WIN) {
			glPushMatrix();
			glTranslatef(0.2f, 0.05f, 0.8f);
			drawLighthouse();
			glPopMatrix();
		}


		glPushMatrix();
		glTranslatef(0.8f, 0.2f, 0.6f);
		drawTower();
		glPopMatrix();


		glPushMatrix();
		glTranslatef(0.5f + objXOffset, 0.07f, 0.1f);
		drawChair();
		glPopMatrix();

		// Place the swing set
		glPushMatrix();
		glTranslatef(1.2f, 0.13f, 0.8f + objXOffset);
		drawSwingSet();
		glPopMatrix();

	}

	glFlush();
	glutSwapBuffers();

}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 'W':
		if (PlayerZ <= 0.95) {
			PlayerZ += d; // Move character forward along the Z-axis
			playerAngle = 0;
		}

		break;
	case 'S':
		if (PlayerZ >= 0.05) {
			PlayerZ -= d; // Move character backward along the Z-axis
			playerAngle = 180;
		}
		break;
	case 'A':
		//least possible x==-0.1 max x=1.1

		if (PlayerX >= 0.05) {
			PlayerX -= d; // Move character left along the X-axis
			playerAngle = 90;
		}
		break;
	case 'D':
		if (PlayerX <= 0.95) {
			PlayerX += d; // Move character right along the X-axis
			playerAngle = -90;
		}
		break;
	case '1':
		// Set the camera to front view
		camera = Camera(0.5f, 0.7f, 1.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		break;
	case '2':
		// Set the camera to top view
		camera = Camera(0.4f, 1.5f, 0.4f, 0.4f, 0.0f, 0.4f, 0.0f, 0.0f, -1.0f);
		break;
	case '3':
		// Set the camera to side view
		camera = Camera(0.98f, 0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f);
		break;
	case 'v':
		animation = true;
		break;
	case 'b':
		animation = false;
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	float dx = PlayerX - 0.2f;
	float dz = PlayerZ - .8f;
	float distance = sqrt(dx * dx + dz * dz);

	// Assuming the user and car have a radius of 0.1f
	if (distance < 0.125f) {
		// Collision detected, the user collided with the car
		gameState = WIN;
	}


	glutPostRedisplay();
}


void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void animate(int value) {

	if (animation) {

		diskYOffset = 0.1 * sin(glutGet(GLUT_ELAPSED_TIME) * 0.01); // Adjust speed and range as needed
		objXOffset = 0.03 * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001); // Adjust speed and range as needed


		glutPostRedisplay();
	}

	glutTimerFunc(30, animate, 0);

}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Amusement park");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutTimerFunc(0, Timer, 0); // Start the timer for game time

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutTimerFunc(0, changeColor, 0); // Start the timer for color change
	glutTimerFunc(0, animate, 0);

	glutMainLoop();
}