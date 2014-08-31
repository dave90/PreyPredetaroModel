/*
 * PreyPredatorAutoma.h
 *
 *  Created on: 28/ago/2013
 *      Author: Davide
 */

#ifndef PREYPREDATORAUTOMA_H_
#define PREYPREDATORAUTOMA_H_

#define EMPTY 0
#define PREY 1
#define PREDATOR 2
#define PREY_ZONE 0
#define PREDATOR_ZONE 1
#define GRASS 2
#define MOVE 0
#define REPRODUCT 1
#define EAT 2
#define DEAD 3

#include <stdlib.h>
#include <time.h>

using namespace std;


//singola cella che compone la matrice
struct cell{
	int age;
	int type;
	int fasting;
	int direction;
	int action;
	int grass;
};

class PreyPredatorAutoma {


public:
	PreyPredatorAutoma(int N, int mat_prey, int mat_predator,int fasting_predator,int fasting_prey,int preyReproductionFactor,int preyMoveFactor,int preyNoMoveFactor,int predatorReproductionFactor,int predatorMoveFactor,int predatorNoMoveFactor,int predatorAggressiveFactor,double growthGrassFactor,int preyEatGrassFactor,int grassPerc,double preyPerc,double predatorPerc,double percPreyZone);
	void inizialize();
	void doStep();
	//dalla direzione ritorna le cordinate
	void getCordinate(int direction,int x,int y,int &newX,int &newY);
	//dalle cordinate ritorna la direzione
	int getDirection(int x,int y,int newX,int newY);
	//calcola i vicini della cella (x,y) e mette le cordinate nei vettori nx e ny e la dimensione in N
	void getNeighborhood(int x,int y,int *&nx,int *&ny,int& N);
	//ritorna true se le celle sono vicine
	bool isNeighborhood(int x1,int y1,int x2,int y2);
	//cerca una cella vicina la cella (x,y) di tipo type
	int searchCell(int x,int y,int type);
	//muovi cella in base la direzione
	void moveCell(int x,int y);
	//riproduci preda/predatore
	void reproductCell(int x,int y);
	//risolvi conflitto direzione delle celle adiacenti a (x,y) che vogliono andare in essa
	void resolveConflict(int x,int y);
	//step della preda
	void doPreyStep(int x,int y);
	//step dell predatore
	void doPredatorStep(int x,int y);
	//esegui l'azione scelta della preda/predatore
	void doAction(int x,int y);
	//esegue la crescita delle piante
	void grassGrowth(int i,int j);
	//esegue l'azione di diminuire la pianta e il fasting della preda
	void preyEatGrass(int i,int j);
	//cattura della preda da parte del predatore
	void capturePrey(int x,int y);
	//avvista predatore e restituisce la direzione per scappare
	int sightsPredator(int x,int y);
	cell** getCurrentMatrix();
	int**  getField();
	int getNumberCurrentPrey(int zone);
	int getNumberCurrentPredator(int zone);

private:
// dimensione automa e variabile per swap della matrice
	int N;
	int currentMatrix;

//costanti per il modello
	//maturità riproduzione
	int mat_prey;
	int mat_predator;
	//digiuno massimo predatore/preda
	int fasting_predator;
	int fasting_prey;
	// Ricerca cibo predatore dopo fasting passi
	int fasting_eat_predator;
	//Fattore crescita piante
	double growthGrassFactor;
	//parametri per l'inizializzazione
	double preyPerc;
	double predatorPerc;
	double percPreyZone;
	//erba iniziale
	double grassPerc;
	// Fattori di riproduzione e aggressione preda-predatore
	int preyReproductionFactor;
	int preyMoveFactor;
	int preyNoMoveFactor;
	int preyEatGrassFactor;
	int predatorReproductionFactor;
	int predatorMoveFactor;
	int predatorNoMoveFactor;
	int predatorAggressiveFactor;

//matrici dell'automa
	cell ***automa;
	int **field;

};

#endif /* PREYPREDATORAUTOMA_H_ */
