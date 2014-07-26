/*
 * PreyPredatorModel.cpp
 *
 *  Created on: 27/ago/2013
 *      Author: Davide
 */

#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/glui.h>
#include <omp.h>
#include <windows.h>
#include "PreyPredatorAutoma.h"


using namespace std;

//dimension automata & constant
int N = 100;
int mat_prey=50;
int mat_predator=15;
int fasting_predator=12;
double preyPerc=10;
double predatorPerc=4;
double percPreyZone=40;

PreyPredatorAutoma *model;

double cellSize = 2;
double sleepTime = 500.0;
bool pause =false;
double lastTime = 0;
int iteration=0;
ofstream resultFile;

// UI VARIABLE //

int main_window;

float   sleepTimeGUI = sleepTime;
int NGUI = N;
float cellSizeGUI=cellSize;
int mat_preyGUI=mat_prey;
int mat_predatorGUI=mat_predator;
int fasting_predatorGUI=fasting_predator;
float preyPercGUI=preyPerc;
float predatorPercGUI=predatorPerc;
float percPreyZoneGUI=percPreyZone;



void control_cb( int control )
{
  /********************************************************************
    Here we'll print the user id of the control that generated the
    callback, and we'll also explicitly get the values of each control.
    Note that we really didn't have to explicitly get the values, since
    they are already all contained within the live variables:
    'wireframe',  'segments',  'obj',  and 'text'
    ********************************************************************/

	sleepTime=sleepTimeGUI;

}

void setPause( int control )
{

	pause=!pause;
}

void restartModel( int control )
{
	delete model;

	N=NGUI;
	mat_prey=mat_preyGUI;
	mat_predator=mat_predatorGUI;
	fasting_predator=fasting_predatorGUI;
	preyPerc=preyPercGUI;
	predatorPerc=predatorPercGUI;
	percPreyZone=percPreyZoneGUI;
	cellSize=cellSizeGUI;


	model=new PreyPredatorAutoma(N,mat_prey,mat_predator,fasting_predator,preyPerc,predatorPerc,percPreyZone);
	glutReshapeWindow(N*cellSize,N*cellSize);
}



// ------------ //


void init() {
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	model=new PreyPredatorAutoma(N,mat_prey,mat_predator,fasting_predator,preyPerc,predatorPerc,percPreyZone);
	resultFile.open("result.dat");
	resultFile << "Time " << "Number_Prey" << "Number_Predator" <<endl;

}

void reshape(int w, int h) {
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	glOrtho(0.0, h/w * N*cellSize, h/w, 0.0, -1.0, 1.0);

	float nRange= N*cellSize ;

    if (w <= h)
		glOrtho (0, nRange, -nRange*h/w +2*nRange, nRange*h/w-nRange, -nRange, nRange);
    else
		glOrtho (-nRange*w/h+2*nRange, nRange*w/h-nRange, 0, nRange, -nRange, nRange);



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

	if (glutGet(GLUT_ELAPSED_TIME)  - lastTime > sleepTime && !pause) {
		lastTime = glutGet(GLUT_ELAPSED_TIME) ;
		model->doStep();

		resultFile << iteration << "   " << model->getNumberCurrentPrey() << "   " << model->getNumberCurrentPredator() <<endl;
		iteration++;
	}
}

//void keyboard (unsigned char key, int x, int y)
//{
//   double offset=50;
//   switch (key) {
//      case '+':
//    	 if(sleepTime-offset>0)
//    		 sleepTime -= offset;
//         glutPostRedisplay();
//         break;
//      case '-':
//     	 sleepTime += offset;
//         glutPostRedisplay();
//         break;
//      case 'p':
//    	 pause=!pause;
//         glutPostRedisplay();
//         break;
//      default:
//         break;
//   }
//}



int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(N * cellSize, N * cellSize);
	glutInitWindowPosition(100, 100);
	main_window=glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
//	glutKeyboardFunc(keyboard);

	  /****************************************/
	  /*         Here's the GLUI code         */
	  /****************************************/

	  GLUI *glui = GLUI_Master.create_glui( "Automata Parameter", 0, 400, 50 ); /* name, flags,
									 x, and y */
	  new GLUI_StaticText( glui, "Set parameter" );
	  new GLUI_Separator( glui );
	  GLUI_Spinner* sleepSpinner  = new GLUI_Spinner( glui, "Sleep time:", &sleepTimeGUI, 2 ,control_cb);
	  sleepSpinner->set_float_limits( 0, 500 );

	  GLUI_Spinner* dimText = new GLUI_Spinner( glui, "Dimension:", &NGUI, 2 );
	  dimText->set_int_limits( 1, 1000 );

	  GLUI_Spinner* cellSize = new GLUI_Spinner( glui, "Cell Size:", &cellSizeGUI, 2 );
	  cellSize->set_float_limits( 0, 50 );

	  GLUI_Spinner* maturityPreyText = new GLUI_Spinner( glui, "Maturity Prey:", &mat_preyGUI, 2 );
	  maturityPreyText->set_int_limits( 1, 100 );

	  GLUI_Spinner* maturityPredatorText = new GLUI_Spinner( glui, "Maturity Predator:", &mat_predatorGUI, 2 );
	  maturityPredatorText->set_int_limits( 1, 100 );

	  GLUI_Spinner* fastingPredatorText = new GLUI_Spinner( glui, "Fasting Predator:", &fasting_predatorGUI, 2 );
	  fastingPredatorText->set_int_limits( 1, 100 );

	  GLUI_Spinner* preyZoneText = new GLUI_Spinner( glui, "Prey Zone:", &percPreyZoneGUI, 2 );
	  preyZoneText->set_float_limits( 1, 100 );

	  GLUI_Spinner* predatorPercText = new GLUI_Spinner( glui, "Initial Predator:", &predatorPercGUI, 2 );
	  predatorPercText->set_float_limits( 1, 100 );

	  GLUI_Spinner* preyPercText = new GLUI_Spinner( glui, "Initial Prey:", &preyPercGUI, 2 );
	  preyPercText->set_float_limits( 1, 100 );

	  new GLUI_Button( glui, "Pause", 0,setPause );
	  new GLUI_Button( glui, "Restart", 0,restartModel );
	  new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );


	  glui->set_main_gfx_window( main_window );

	  /* We register the idle callback with GLUI, *not* with GLUT */
	  //GLUI_Master.set_glutIdleFunc( myGlutIdle );
	  GLUI_Master.set_glutIdleFunc( NULL );

	glutMainLoop();


	return 0;
}


