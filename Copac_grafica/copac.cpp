#include "GLOS.h"

#include "gl.h"
#include "glu.h"
#include "glut.h"
#include "glaux.h"

#include <math.h>
#include <stdio.h>
#include <stdbool.h>


const int x = 0, y = 1, z = 2, w = 3; // used only for indexing
const int A = 0, B = 1, C = 2, D = 3; // used only for indexing

static float lightSourcePosition[4] = { -60, 50, -40, 1 };
static float modelX = 0;
static float angle = 0;
static float angle2 = 0;
float marY = 10.0f;
bool mereCazute = false;

GLfloat punctePlanIarba[][3] = {
	{ -150.0f, -60.0f, -150.0f },
	{ -150.0f, -60.0f, 150.0f },
	{ 150.0f, -60.0f, 150.0f } ,
	{ 150.0f, -60.0f, -150.0f }
};

float coeficientiPlanIarba[4];
float matriceUmbrire[4][4];

void CALLBACK mutaSursaFata() {
	if (lightSourcePosition[z] < 100) {
		lightSourcePosition[z] += 5;
	}
}

void CALLBACK mutaSursaSpate() {
	if (lightSourcePosition[z] > -100) {
		lightSourcePosition[z] -= 5;
	}
}

void CALLBACK mutaSursaDreapta() {
	if (lightSourcePosition[x] < 100) {
		lightSourcePosition[x] += 5;
	}
}

void CALLBACK mutaSursaStanga() {
	if (lightSourcePosition[x] > -100) {
		lightSourcePosition[x] -= 5;
	}
}

void CALLBACK rotireDreapta() {
	angle2 -= 2;
}

void CALLBACK rotireStanga() {
	angle2 += 2;
}

void CALLBACK mutaStanga()
{
	modelX = modelX - 2;
}

void CALLBACK mutaDreapta()
{
	modelX = modelX + 2;
}

void computePlaneCoefficientsFromPoints(float points[3][3]) {
	// calculeaza 2 vectori din 3 puncte
	float v1[3]{ points[0][x] - points[1][x], points[0][y] - points[1][y], points[0][z] - points[1][z] };
	float v2[3]{ points[1][x] - points[2][x], points[1][y] - points[2][y], points[1][z] - points[2][z] };

	// produsul vectorial al celor 2 vectori => al 3lea vector cu componentele A,B,C chiar coef din ec. planului
	coeficientiPlanIarba[A] = v1[y] * v2[z] - v1[z] * v2[y];
	coeficientiPlanIarba[B] = v1[z] * v2[x] - v1[x] * v2[z];
	coeficientiPlanIarba[C] = v1[x] * v2[y] - v1[y] * v2[x];

	// determinam D - ecuatia planului in punctul random ales trebuie sa fie zero
	int randomPoint = 1; // poate fi orice punct de pe planul ierbii, asa ca alegem unul din cele 3 folosite pentru calcule
	coeficientiPlanIarba[D] =
		-(coeficientiPlanIarba[A] * points[randomPoint][x] +
			coeficientiPlanIarba[B] * points[randomPoint][y] +
			coeficientiPlanIarba[C] * points[randomPoint][z]);
}

void computeShadowMatrix(float points[3][3], float lightSourcePosition[4]) {
	// determina coef planului	
	computePlaneCoefficientsFromPoints(points);

	// temp = AxL + ByL + CzL + Dw
	float temp =
		coeficientiPlanIarba[A] * lightSourcePosition[x] +
		coeficientiPlanIarba[B] * lightSourcePosition[y] +
		coeficientiPlanIarba[C] * lightSourcePosition[z] +
		coeficientiPlanIarba[D] * lightSourcePosition[w];

	//prima coloana
	matriceUmbrire[0][0] = temp - coeficientiPlanIarba[A] * lightSourcePosition[x];
	matriceUmbrire[1][0] = 0.0f - coeficientiPlanIarba[B] * lightSourcePosition[x];
	matriceUmbrire[2][0] = 0.0f - coeficientiPlanIarba[C] * lightSourcePosition[x];
	matriceUmbrire[3][0] = 0.0f - coeficientiPlanIarba[D] * lightSourcePosition[x];
	//a 2a coloana
	matriceUmbrire[0][1] = 0.0f - coeficientiPlanIarba[A] * lightSourcePosition[y];
	matriceUmbrire[1][1] = temp - coeficientiPlanIarba[B] * lightSourcePosition[y];
	matriceUmbrire[2][1] = 0.0f - coeficientiPlanIarba[C] * lightSourcePosition[y];
	matriceUmbrire[3][1] = 0.0f - coeficientiPlanIarba[D] * lightSourcePosition[y];
	//a 3a coloana
	matriceUmbrire[0][2] = 0.0f - coeficientiPlanIarba[A] * lightSourcePosition[z];
	matriceUmbrire[1][2] = 0.0f - coeficientiPlanIarba[B] * lightSourcePosition[z];
	matriceUmbrire[2][2] = temp - coeficientiPlanIarba[C] * lightSourcePosition[z];
	matriceUmbrire[3][2] = 0.0f - coeficientiPlanIarba[D] * lightSourcePosition[z];
	//a 4a coloana
	matriceUmbrire[0][3] = 0.0f - coeficientiPlanIarba[A] * lightSourcePosition[w];
	matriceUmbrire[1][3] = 0.0f - coeficientiPlanIarba[B] * lightSourcePosition[w];
	matriceUmbrire[2][3] = 0.0f - coeficientiPlanIarba[C] * lightSourcePosition[w];
	matriceUmbrire[3][3] = temp - coeficientiPlanIarba[D] * lightSourcePosition[w];
}

void myInit(void) {
	glClearColor(0.7f, 0.8f, 0.9f, 1.0f);

	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// sursa lumina
	float lightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);

	// material
	float materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float materialShininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

	// use current color set with glColor3f() to directly set the material ambient and diffuse
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
}


void desenareCopac() {
	glPushMatrix();
	glTranslated(0, 20.0, 5.0);
	glColor3f(0.5, 0.25, 0.1);
	glPushMatrix();
	auxSolidCylinder(7, 90);
	glPopMatrix();

	glTranslated(0, 10, 5.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(3, 0, 5.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(-5, -10, 5.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(-5, 0, 0.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(15, 0, 0.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(15);
	glPopMatrix();

	glTranslated(10, 0.0, -15.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(-25, 0.0, -15.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(15, 0.0, 0.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(5, 10, 5.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(-25, -7, 10.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glTranslated(5, 13, -10.0);
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
	auxSolidSphere(17);
	glPopMatrix();

	glPopMatrix();
}

void desenareMar() {

	glPushMatrix(); 
	glPushMatrix();
	glTranslated(-20, marY, 0.0);
	glColor3f(1.0, 0.0, 0.0);
    auxSolidSphere(5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(30, marY+10, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-5, marY+20, -20.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(4);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslated(5, marY + 5, -25.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(5.0, marY + 5, 33.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(25.0, marY + 20, 17.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(4);
	glPopMatrix();


	glPushMatrix();
	glTranslated(-20, marY + 20, -10.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-15, marY + 20,30.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-7, marY + 30, 20.0);
	glColor3f(1.0, 0.0, 0.0);
	auxSolidSphere(3);
	glPopMatrix();

	
	

	glPopMatrix();

}


void desenareModel(bool umbra) {
	if (umbra) {
		glColor3f(0, 0.15, 0.05);
	}
	else {
		glColor3f(0, 0.3, 1);
	}
	glPushMatrix();
	glTranslatef(modelX, -30, 0);
	glRotatef(angle2, 0, 1, 0);
	desenareCopac();
	desenareMar();
	glPopMatrix();
}


void desenareIarba() {
	glPushMatrix();
	glColor3f(0.0, 0.5, 0);
	glTranslatef(0, -0.9, 0);
	glBegin(GL_QUADS);
	{
		glNormal3f(0, 1, 0);
		for (int i = 0; i < 4; i++) {
			glVertex3fv(punctePlanIarba[i]);
		}
	}
	glEnd();
	glPopMatrix();
}

void deseneazaLumina()
{
	glPushMatrix();
	glTranslatef(lightSourcePosition[x], lightSourcePosition[y], lightSourcePosition[z]);
	glColor3f(1.0, 1.0, 0);
	auxSolidSphere(10);
	glPopMatrix();
}

void CALLBACK display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -150);

	glRotatef(15, 1, 0, 0);


	computeShadowMatrix(punctePlanIarba, lightSourcePosition);

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);

	desenareIarba();

	desenareModel(false);
	glPopMatrix();

	//deseneaza umbra
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glPushMatrix();


	glMultMatrixf((GLfloat*)matriceUmbrire); // se inmulteste matricea curenta cu matricea de umbrire
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	desenareModel(true);
	glPopMatrix();
	glDisable(GL_BLEND);
	deseneazaLumina();

	glEnable(GL_LIGHTING);

	auxSwapBuffers();

}

void CALLBACK myReshape(GLsizei w, GLsizei h) {
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0 * (GLfloat)w / (GLfloat)h, 10, 300.0);
	glMatrixMode(GL_MODELVIEW);
}

void CALLBACK IdleFunction()
{

	if (!mereCazute) {
		angle += 1;

		marY -= 0.1f;

	}
	

	if (marY < -35.0f) {
		marY = 10.0;
		mereCazute = true;
	}
	
	display();

	//Sleep(1000 / 60);
}

int main(int argc, char** argv) {
	auxInitDisplayMode(AUX_DOUBLE | AUX_DEPTH16 | AUX_RGB);
	auxInitPosition(10, 10, 1200, 800);
	auxKeyFunc(AUX_RIGHT, mutaDreapta);
	auxKeyFunc(AUX_LEFT, mutaStanga);
	auxKeyFunc(AUX_UP, rotireDreapta);
	auxKeyFunc(AUX_DOWN, rotireStanga);
	auxKeyFunc(AUX_w, mutaSursaSpate);
	auxKeyFunc(AUX_s, mutaSursaFata);
	auxKeyFunc(AUX_d, mutaSursaDreapta);
	auxKeyFunc(AUX_a, mutaSursaStanga);
	auxInitWindow("Copac");
	myInit();
	auxIdleFunc(IdleFunction);
	auxReshapeFunc(myReshape);
	auxMainLoop(display);
	return 0;
}