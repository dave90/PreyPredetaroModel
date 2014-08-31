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
#include <list>


using namespace std;

//dimension automata & constant
int N = 200;
int mat_prey=20;
int mat_predator=20;
int fasting_prey=8;
int fasting_predator=15;
double preyPerc=20;
double predatorPerc=4;
double grassPerc=100;
double percPreyZone=0;

// Fattori di riproduzione e aggressione preda-predatore
int preyReproductionFactor=6;
int preyMoveFactor=10;
int preyNoMoveFactor=2;
int predatorMoveFactor=10;
int predatorNoMoveFactor=1;
int predatorReproductionFactor=8;
int predatorAggressiveFactor=12;
double growthGrassFactor=8.0;
int preyEatGrassFactor=90;

PreyPredatorAutoma *model;

double cellSize = 2;
double sleepTime = 500.0;
bool pause =true;
double lastTime = 0;
int iteration=0;

// Plot data
list<pair<int,int> > graphPrey;
list<pair<int,int> > graphPredator;


int maxSizeGraph=500;

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
int preyReproductionFactorGUI=preyReproductionFactor;
int preyMoveFactorGUI=preyMoveFactor;
int preyNoMoveFactorGUI=preyNoMoveFactor;
int predatorMoveFactorGUI=predatorMoveFactor;
int predatorNoMoveFactorGUI=predatorNoMoveFactor;
int predatorReproductionFactorGUI=predatorReproductionFactor;
int predatorAggressiveFactorGUI=predatorAggressiveFactor;
float growthGrassFactorGUI=growthGrassFactor;
int preyEatGrassFactorGUI=preyEatGrassFactor;
float grassPercGUI=grassPerc;
int fasting_preyGUI=fasting_prey;
int zoneGraphGUI;





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
	preyReproductionFactor=preyReproductionFactorGUI;
	preyMoveFactor=preyMoveFactorGUI;
	preyNoMoveFactor=preyNoMoveFactorGUI;
	predatorMoveFactor=predatorMoveFactorGUI;
	predatorNoMoveFactor=predatorNoMoveFactorGUI;
	predatorReproductionFactor=predatorReproductionFactorGUI;
	predatorAggressiveFactor=predatorAggressiveFactorGUI;
	growthGrassFactor=growthGrassFactorGUI;
	int preyEatGrassFactor=preyEatGrassFactorGUI;
	double grassPerc=grassPercGUI;
	int fasting_prey=fasting_preyGUI;

	graphPredator.clear();
	graphPrey.clear();
	iteration=0;

	model=new PreyPredatorAutoma( N,  mat_prey,  mat_predator, fasting_predator, fasting_prey, preyReproductionFactor, preyMoveFactor, preyNoMoveFactor, predatorReproductionFactor, predatorMoveFactor, predatorNoMoveFactor, predatorAggressiveFactor, growthGrassFactor, preyEatGrassFactor, grassPerc, preyPerc, predatorPerc, percPreyZone);
	glutReshapeWindow(N*cellSize,N*cellSize);
}



// ------------ //


void init() {
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	model=new PreyPredatorAutoma( N,  mat_prey,  mat_predator, fasting_predator, fasting_prey, preyReproductionFactor, preyMoveFactor, preyNoMoveFactor, predatorReproductionFactor, predatorMoveFactor, predatorNoMoveFactor, predatorAggressiveFactor, growthGrassFactor, preyEatGrassFactor, grassPerc, preyPerc, predatorPerc, percPreyZone);



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

void printGraph() {

	graphPrey.push_back( { iteration, model->getNumberCurrentPrey(zoneGraphGUI-1) });
	graphPredator.push_back( { iteration, model->getNumberCurrentPredator(zoneGraphGUI-1) });

	//Print le ultime tot iterazioni
	if (graphPrey.size() > maxSizeGraph) {
		graphPrey.pop_front();
		graphPredator.pop_front();


	};


	cout<<"set terminal wxt 0"<<endl;
	cout << "set title ' Prey-Predator Number" <<  endl;
	cout << "set xlabel 'iteration'" << endl;
	cout << "set ylabel 'Number'" << endl;
	cout << "set xrange [" << graphPrey.begin()->first << ":" << graphPrey.begin()->first+maxSizeGraph << "]" << endl;
	cout << "set yrange [" << 0 << ":" <<   "]" << endl;
//	cout << "plot '-' using 1:2 title 'Prey' with lines lt rgb '#00FFFF', '-' using 1:3 title 'Predator' with lines lt rgb '#FFFF00'" << endl;
	cout << "plot '-' using 1:2 title 'Prey' with lines, '-' using 1:3 title 'Predator' with lines " << endl;
	for (list<pair<int, int> >::iterator it1 = graphPrey.begin(), it2 =	graphPredator.begin(); it1 != graphPrey.end(); it1++, it2++)
		cout << it1->first << "	" << it1->second << "	" << it2->second << endl;
	cout << "e" << endl;


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
			if (matrixModel[i][j].type==EMPTY && matrixModel[i][j].grass==0){
				if(field[i][j]==PREDATOR_ZONE)
					glColor3f(0.6f, 0.2f, 0.0f);
				else
					glColor3f(0.4f, 0.6f, 0.0f);
				span=0;
			}else if (matrixModel[i][j].type==EMPTY && matrixModel[i][j].grass!=0){
				glColor3f(0.0f, 0.8f, 0.0f);
				span=0;
			}else if(matrixModel[i][j].type==PREY){
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

		printGraph();
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
	glutInitWindowPosition(200, 10);
	main_window=glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	  /****************************************/
	  /*         Here's the GLUI code         */
	  /****************************************/

	  GLUI *glui = GLUI_Master.create_glui( "Automata Parameter", 0, 10, 10 ); /* name, flags, x, and y */

	  new GLUI_StaticText( glui, "Prey Factor" );
	  new GLUI_Separator( glui );
	  GLUI_Spinner* maturityPreyText = new GLUI_Spinner( glui, "Maturity:", &mat_preyGUI, 2 );
	  maturityPreyText->set_int_limits( 1, 10000 );

	  GLUI_Spinner* fastingPreyText = new GLUI_Spinner( glui, "Fasting:", &fasting_preyGUI, 2 );
	  fastingPreyText->set_int_limits( 0, 10000 );

	  GLUI_Spinner* preyReproductionText = new GLUI_Spinner( glui, "Reproduction:", &preyReproductionFactorGUI, 2 );
	  preyReproductionText->set_int_limits( 1, 100 );

	  GLUI_Spinner* preyMoveText = new GLUI_Spinner( glui, "Move:", &preyMoveFactorGUI, 2 );
	  preyMoveText->set_int_limits( 1, 100 );

	  GLUI_Spinner* preyStationaryText = new GLUI_Spinner( glui, "Stationary:", &preyNoMoveFactorGUI, 2 );
	  preyStationaryText->set_int_limits( 1, 100 );

	  GLUI_Spinner* preyEatGrassText = new GLUI_Spinner( glui, "Eat grass :", &preyEatGrassFactorGUI, 2 );
	  preyEatGrassText->set_int_limits( 1, 100 );

	  new GLUI_Separator( glui );
	  new GLUI_StaticText( glui, "Predator Factor" );
	  new GLUI_Separator( glui );
	  GLUI_Spinner* maturityPredatorText = new GLUI_Spinner( glui, "Maturity:", &mat_predatorGUI, 2 );
	  maturityPredatorText->set_int_limits( 1, 10000 );

	  GLUI_Spinner* fastingPredatorText = new GLUI_Spinner( glui, "Fasting:", &fasting_predatorGUI, 2 );
	  fastingPredatorText->set_int_limits( 1, 10000 );

	  GLUI_Spinner* predatorReproductionText = new GLUI_Spinner( glui, "Reproduction:", &predatorReproductionFactorGUI, 2 );
	  predatorReproductionText->set_int_limits( 1, 100 );

	  GLUI_Spinner* predatorMoveText = new GLUI_Spinner( glui, "Move:", &predatorMoveFactorGUI, 2 );
	  predatorMoveText->set_int_limits( 1, 100 );

	  GLUI_Spinner* predatorStationaryText = new GLUI_Spinner( glui, "Stationary:", &predatorNoMoveFactorGUI, 2 );
	  predatorStationaryText->set_int_limits( 1, 100 );

	  GLUI_Spinner* predatorAggressiveText = new GLUI_Spinner( glui, "Aggression:", &predatorAggressiveFactorGUI, 2 );
	  predatorAggressiveText->set_int_limits( 1, 100 );

	  new GLUI_Separator( glui );
	  new GLUI_StaticText( glui, "Field & Model Parameters" );
	  new GLUI_Separator( glui );
	  GLUI_Spinner* sleepSpinner  = new GLUI_Spinner( glui, "Sleep time:", &sleepTimeGUI, 2 ,control_cb);
	  sleepSpinner->set_int_limits( 0, 1000 );

	  GLUI_Spinner* dimText = new GLUI_Spinner( glui, "Dimension:", &NGUI, 2 );
	  dimText->set_int_limits( 1, 1000 );

	  GLUI_Spinner* cellSize = new GLUI_Spinner( glui, "Cell size:", &cellSizeGUI, 2 );
	  cellSize->set_float_limits( 0, 50 );

	  GLUI_Spinner* preyZoneText = new GLUI_Spinner( glui, "Prey zone:", &percPreyZoneGUI, 2 );
	  preyZoneText->set_float_limits( 0, 100 );

	  GLUI_Spinner* predatorPercText = new GLUI_Spinner( glui, "Initial predator:", &predatorPercGUI, 2 );
	  predatorPercText->set_float_limits( 0, 100 );

	  GLUI_Spinner* preyPercText = new GLUI_Spinner( glui, "Initial prey:", &preyPercGUI, 2 );
	  preyPercText->set_float_limits( 0, 100 );

	  GLUI_Spinner* grassPercText = new GLUI_Spinner( glui, "Initial grass:", &grassPercGUI, 2 );
	  grassPercText->set_float_limits( 0, 100 );

	  GLUI_Spinner* grassGrowthText = new GLUI_Spinner( glui, "Grass growth:", &growthGrassFactorGUI, 2 );
	  grassGrowthText->set_float_limits( 0, 100 );

	  new GLUI_Separator( glui );
	  new GLUI_StaticText( glui, "Zone to visualize the variation " );
	  new GLUI_Separator( glui );
	  GLUI_Listbox* listGraph=new GLUI_Listbox(glui,"Zone",&zoneGraphGUI,2);
	  // Poichè riceve int e -1 è tutta la zone, sommiamo +1 alle costanti
	  listGraph->add_item(0,"ALL");
	  listGraph->add_item(PREY_ZONE+1,"PREY");
	  listGraph->add_item(PREDATOR_ZONE+1,"PREDATOR");


	  new GLUI_Separator( glui );
	  new GLUI_Button( glui, "Start/Pause", 0,setPause );
	  new GLUI_Button( glui, "Restart", 0,restartModel );
	  new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );


	  glui->set_main_gfx_window( main_window );

	  /* We register the idle callback with GLUI, *not* with GLUT */
	  //GLUI_Master.set_glutIdleFunc( myGlutIdle );
	  GLUI_Master.set_glutIdleFunc( NULL );

	glutMainLoop();


	return 0;
}


