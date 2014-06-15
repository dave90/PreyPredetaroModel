/*
 * PreyPredatorModel.cpp
 *
 *  Created on: 27/ago/2013
 *      Author: Davide
 */

#include <iostream>
#include <GL/glut.h>
#include <omp.h>
#include "PreyPredatorAutoma.h"



using namespace std;

//dimension automata & constant
const int N = 100;
const int mat_prey=10;
const int mat_predator=20;
const int fasting_predator=12;
double preyPerc=80;
double predatorPerc=20;
double percPreyZone=40;

PreyPredatorAutoma *model;

double cellSize = 5;
double sleepTime = 100;
double lastTime = 0;
int iteration=0;



void init() {
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	model=new PreyPredatorAutoma(N,mat_prey,mat_predator,fasting_predator,preyPerc,predatorPerc,percPreyZone);

}

void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	cell **matrixModel=model->getCurrentMatrix();
	int **field=model->getField();

	double span=cellSize/10;

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++) {
			if (matrixModel[i][j].type==EMPTY){
				if(field[i][j]==PREDATOR_ZONE)
					glColor3f(0.6f, 0.2f, 0.0f);
				else
					glColor3f(0.4f, 0.6f, 0.0f);
				span=0;
			}else if(matrixModel[i][j].type==PREY){
				if(field[i][j]==PREDATOR_ZONE)
					glColor3f(0.0f, 1.0f, 0.8f);
				else
					glColor3f(0.0f, 1.0f, 0.8f);
				span=cellSize/10;
			}else if(matrixModel[i][j].type==PREDATOR){
				glColor3f(1.0f, 1.0f, 0.0f);
				span=cellSize/10;
			}

			glBegin(GL_QUADS);
				glVertex2f(j * cellSize+span, i * cellSize+span);
				glVertex2f(j * cellSize+span, i * cellSize + cellSize-span);
				glVertex2f(j * cellSize + cellSize-span, i * cellSize + cellSize-span);
				glVertex2f(j * cellSize + cellSize-span, i * cellSize+span);
			glEnd();
		}
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();

	if (glutGet(GLUT_ELAPSED_TIME)  - lastTime > sleepTime) {
		lastTime = glutGet(GLUT_ELAPSED_TIME) ;
		model->doStep();
		cout<<string(50,'\n');
		cout<<"Number Prey "<<model->getNumberCurrentPrey()<<endl;
		cout<<"Number Predator "<<model->getNumberCurrentPredator()<<endl;
		cout<<"Iteration "<<iteration<<endl;
		iteration++;
	}
}



int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(N * cellSize, N * cellSize);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();


	return 0;
}


