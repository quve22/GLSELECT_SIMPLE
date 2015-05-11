#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <stdio.h>

#define MAXHITS	200 // size of array to hold hit records for selection

GLfloat color[2][4] = { 
	{ 0.0, 0.0, 1.0, 1.0 },
	{ 1.0, 0.0, 0.0, 1.0 } 
};

int colorFlag = 0;

void init(void)
{
	glClearColor(0.2, 0.5, 0.5, 0.0);
	glShadeModel(GL_SMOOTH);

	GLfloat light_position[] = { 10.0, 10.0, -10.0, 1.0 };
	GLfloat light_color[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ambient_color[] = { 0.2, 0.2, 0.2, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

	GLfloat mat_shininess[] = { 50.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void render(GLenum mode)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (mode == GL_SELECT)
		glLoadName(0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color[0]);
	glutSolidCube(5);

	glTranslated(-10, 0, -10);

	if (mode == GL_SELECT)
		glLoadName(1);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color[1]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color[1]);
	glutSolidSphere(5, 100, 100);

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void processHit(unsigned int hits, unsigned int *pSelectionBuffer)
{
	for (int i = 0; i < hits; i++)
	{
		unsigned int nameStackDepth = *(pSelectionBuffer++);
		float minZ = ((float)*(pSelectionBuffer++)) / (float)0xffffffff;
		float maxZ = ((float)*(pSelectionBuffer++)) / (float)0xffffffff;
		unsigned int stackContents = *(pSelectionBuffer++);

		color[stackContents][1] >= 1.0 ? color[stackContents][1] = 0 : color[stackContents][1] += 0.2;
	}
}

void select(GLint mouseX, GLint mouseY)
{
#define SELECTION_AREA_SIZE 1
#define MAX_HITS 200

	GLint m_viewport[4];
	GLuint m_select_buf[MAX_HITS] = { 0, };
	static unsigned int hits;

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glSelectBuffer(MAXHITS, m_select_buf);
	glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(mouseX, (double)(m_viewport[3] - mouseY), SELECTION_AREA_SIZE, SELECTION_AREA_SIZE, m_viewport);
	gluPerspective(60.0, 1.0, 1.0, 30.0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	gluLookAt(10.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	render(GL_SELECT);

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	hits = glRenderMode(GL_RENDER);
	if (hits > 0) {
		processHit(hits, m_select_buf);
	}

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void Mouse(int button, int state, int mouseX, int mouseY)
{
	if (state == GLUT_DOWN) 
	{
		select((GLint)mouseX, (GLint)mouseY);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render(GL_RENDER);
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1.0, 30.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(10.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(70, 70);
	glutCreateWindow("GL_SELECT Example");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(Mouse);

	init();

	glutMainLoop();

	return 0;
}