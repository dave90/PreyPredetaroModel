/*
 * PreyPredatorAutoma.cpp
 *
 *  Created on: 28/ago/2013
 *      Author: Davide
 */

#include "PreyPredatorAutoma.h"
#include <iostream>
#include <math.h>
#include <omp.h>

using namespace std;

PreyPredatorAutoma::PreyPredatorAutoma(int N, int mat_prey, int mat_predator,int fasting_predator,double preyPerc,double predatorPerc,double percPreyZone) {
	srand(time(0));
	this->N=N;
	this->mat_predator=mat_predator;
	this->mat_prey=mat_prey;
	this->fasting_predator=fasting_predator;
	this->fasting_eat_predator=this->fasting_predator/2;
	this->preyPerc=preyPerc;
	this->predatorPerc=predatorPerc;
	this->percPreyZone=percPreyZone;
	//TODO Add to input
	this->preyReproductionFactor=1;
	this->preyMoveFactor=10;
	this->preyNoMoveFactor=5;
	this->predatorReproductionFactor=1;
	this->predatorAggressiveFactor=5;

// inizializzazione matrice
	currentMatrix=0;
	automa=new cell**[2];
	automa[0]=new cell*[N];
	automa[1]=new cell*[N];

	for(int i=0;i<N;i++){
		automa[0][i]=new cell[N];
		automa[1][i]=new cell[N];
	}
	inizialize();
}

void PreyPredatorAutoma::inizialize() {

//setta tutte le celle a vuote
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			automa[currentMatrix][i][j].type=EMPTY;

//aggiungi prede e predatori in base la percentuale
	if(predatorPerc+preyPerc>100){
		cerr<<"Number percentage of predator and prey is uncorrect"<<endl;
		predatorPerc=100-preyPerc;
	}
	int preyNumber=N*N*preyPerc/100;
	int predatorNumber=N*N*predatorPerc/100;
	while(preyNumber>0){
		int x=rand()%N;
		int y=rand()%N;
		if(automa[currentMatrix][x][y].type==EMPTY){
			automa[currentMatrix][x][y].type=PREY;
			automa[currentMatrix][x][y].direction=0;
			automa[currentMatrix][x][y].age=0;
			preyNumber--;
		}
	}

	while(predatorNumber>0){
		int x=rand()%N;
		int y=rand()%N;
		if(automa[currentMatrix][x][y].type==EMPTY){
			automa[currentMatrix][x][y].type=PREDATOR;
			automa[currentMatrix][x][y].direction=0;
			automa[currentMatrix][x][y].age=0;
			automa[currentMatrix][x][y].fasting=0;
			predatorNumber--;
		}
	}

//inizializzza il terreno
	field=new int*[N];
	for(int i=0;i<N;i++)
		field[i]=new int[N];
//setta tutte le celle come terreno per i predatori
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			field[i][j]=PREDATOR_ZONE;
//aggiungi le aree per le prede
	int preyCell=sqrt(N*N*percPreyZone/100);
	int x=N/2-preyCell/2;
	int y=N/2-preyCell/2;
	for(int i=x;i<x+preyCell;i++)
		for(int j=y;j<y+preyCell;j++)
			field[i][j]=PREY_ZONE;


}

void PreyPredatorAutoma::doStep() {
	//pulisci matrice del modello del passo successivo
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			automa[1-currentMatrix][i][j].type=EMPTY;
		}

	//esegui i sottoprocessi
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++){
			if(automa[currentMatrix][i][j].type==PREY)
				doPreyStep(i,j);
			else if(automa[currentMatrix][i][j].type==PREDATOR)
				doPredatorStep(i,j);
		}

	//risolvi conflitti
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			resolveConflict(i,j);

	//esegui cattura prede
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			if(automa[currentMatrix][i][j].type==PREDATOR && automa[currentMatrix][i][j].action==EAT)
				capturePrey(i,j);

	//esegui spostamenti o riproduzioni
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			if(automa[currentMatrix][i][j].type==PREY || automa[currentMatrix][i][j].type==PREDATOR){
				doAction(i,j);
			}



	currentMatrix=1-currentMatrix;
}

void PreyPredatorAutoma::doPreyStep(int x, int y) {

	//incremento età preda
	automa[currentMatrix][x][y].age=automa[currentMatrix][x][y].age+1;

//	//muore dopo tot anni
//	if(automa[currentMatrix][x][y].age>mat_prey){
//		automa[currentMatrix][x][y].action=DEAD;
//		return;
//	}

	//avvista il predatore se nella zona prey_zone
	if(field[x][y]==PREY_ZONE){
		int dir=sightsPredator(x,y);
		if(dir!=-1){
			automa[currentMatrix][x][y].action=MOVE;
			automa[currentMatrix][x][y].direction=dir;
			return ;
		}
	}

	// Fattore riproduzione movimento e stare fermo (quest'ultimi pari a 1)
	int choose=0;
	if(automa[currentMatrix][x][y].age>=mat_prey)
		choose=rand()%(preyMoveFactor+preyNoMoveFactor+preyReproductionFactor);
	else
		choose=rand()%(preyMoveFactor+preyNoMoveFactor);

	if(choose<preyMoveFactor){
		//spostamento preda
		automa[currentMatrix][x][y].action=MOVE;
		automa[currentMatrix][x][y].direction=searchCell(x,y,EMPTY);
	}else if(choose<preyMoveFactor+preyNoMoveFactor){
		//la preda non fa niente
		automa[currentMatrix][x][y].action=MOVE;
		automa[currentMatrix][x][y].direction=0;
	}else {
		//riproduzione della preda
		automa[currentMatrix][x][y].action=REPRODUCT;
		automa[currentMatrix][x][y].direction=searchCell(x,y,EMPTY);
	}
}

int PreyPredatorAutoma::sightsPredator(int x, int y) {
	int *nx=0;
	int *ny=0;
	int N;

	getNeighborhood(x,y,nx,ny,N);
	int emptyX[N];
	int emptyY[N];
	int cont=0;

	//calcola le celle libere nell'intorno
	for(int i=0;i<N;i++){
		if(automa[currentMatrix][nx[i]][ny[i]].type==EMPTY){
			emptyX[cont]=nx[i];
			emptyY[cont]=ny[i];
			cont++;
		}
	}

	//calcola le celle che nel loro vicinato non ci sono predatori
	int safeX[N];
	int safeY[N];
	int notSafeX[N];
	int notSafeY[N];
	int safeCont=0;
	int predator=0;
	for(int i=0;i<cont;i++){
		if(searchCell(emptyX[i],emptyY[i],PREDATOR)!=0){
			notSafeX[predator]=emptyX[i];
			notSafeY[predator]=emptyY[i];
			predator++;
		}else{
			safeX[safeCont]=emptyX[i];
			safeY[safeCont]=emptyY[i];
			safeCont++;
		}
	}

	delete []nx;
	delete []ny;

	if(predator>=1 && safeCont>0){
		int selectCellX[N];
		int selectCellY[N];
		int selectCount=0;
		//Elimina le celle vicine alle celle not safe
		for(int i=0;i<safeCont;i++){
			bool add=true;
			for(int j=0;j<predator;j++)
				if(isNeighborhood(safeX[i],safeY[i],notSafeX[j],notSafeY[j])){
					add=false;
					break;
				}
			if(add){
				selectCellX[selectCount]=safeX[i];
				selectCellY[selectCount]=safeY[i];
				selectCount++;
			}
		}

		// Se tutte le celle sono vicine alla cella not safe prendine una a caso tra le empty
		if(selectCount==0){
			int choose=rand()%safeCont;
			return getDirection(x,y,safeX[choose],safeY[choose]);
		}

		int choose=rand()%selectCount;
		return getDirection(x,y,selectCellX[choose],selectCellY[choose]);

	}
	return -1;
}

void PreyPredatorAutoma::doPredatorStep(int x, int y) {

	//incremento età predatore
	automa[currentMatrix][x][y].age=automa[currentMatrix][x][y].age+1;
	//incremento digiuno
	automa[currentMatrix][x][y].fasting=automa[currentMatrix][x][y].fasting+1;


	//morte del predatore se fasting>fasting_predator, non mangia da fasting passi
	if(automa[currentMatrix][x][y].fasting>fasting_predator){
		automa[currentMatrix][x][y].action=DEAD;
		return ;
	}


	//ricerca preda
	int dir=searchCell(x,y,PREY);


	bool isMaturity=automa[currentMatrix][x][y].age>=mat_predator;
	bool isPresentPrey=dir!=0;

	int totalProbability=1+1;
	if(isMaturity)
		totalProbability+=predatorReproductionFactor;
	if(isPresentPrey)
		totalProbability+=predatorAggressiveFactor;

	int choose=rand()%totalProbability;

	if(choose<1){
		//spostamento predatore
		automa[currentMatrix][x][y].action=MOVE;
		automa[currentMatrix][x][y].direction=searchCell(x,y,EMPTY);
	}else if(choose<2){
		//il predatore non fa niente
		automa[currentMatrix][x][y].action=MOVE;
		automa[currentMatrix][x][y].direction=0;
	}else if(isMaturity && choose<2+predatorReproductionFactor){

		//riproduzione dell redatore
		automa[currentMatrix][x][y].action=REPRODUCT;
		automa[currentMatrix][x][y].direction=searchCell(x,y,EMPTY);

	}else{

		//ha localizzato una preda
		automa[currentMatrix][x][y].action=EAT;
		automa[currentMatrix][x][y].direction=dir;

	}
}

void PreyPredatorAutoma::resolveConflict(int x, int y) {

	int *nx=0;
	int *ny=0;
	int N;



	getNeighborhood(x,y,nx,ny,N);
	int conflictX[N];
	int conflictY[N];
	int cont=0;
	for(int i=0;i<N;i++){
		if(automa[currentMatrix][nx[i]][ny[i]].type==EMPTY)continue;
		int dirCell=automa[currentMatrix][nx[i]][ny[i]].direction;
		int newX,newY;
		getCordinate(dirCell,nx[i],ny[i],newX,newY);
		if(newX==x && newY==y){
//		int dir=getDirection(x,y,nx[i],ny[i]);
//		if(dirCell+dir==5){
			conflictX[cont]=nx[i];
			conflictY[cont]=ny[i];
			cont++;
		}
	}
	if(cont>1){
		int choose=rand()%cont;
		for(int i=0;i<cont;i++)
			if(i!=choose){
				automa[currentMatrix][conflictX[i]][conflictY[i]].direction=0;
			}
	}
	delete []nx;
	delete []ny;

}

void PreyPredatorAutoma::capturePrey(int x, int y) {
	//se c'è stato un conflitto tra due predatori la direzione è 0
	if(automa[currentMatrix][x][y].direction==0){
		automa[currentMatrix][x][y].action=MOVE;
		return ;
	}

	int nx,ny;
	getCordinate(automa[currentMatrix][x][y].direction,x,y,nx,ny);
	automa[currentMatrix][nx][ny].action=DEAD;
	automa[currentMatrix][x][y].fasting=0;
	automa[currentMatrix][x][y].action=MOVE;
}

void PreyPredatorAutoma::doAction(int x, int y) {
	if(automa[currentMatrix][x][y].action==MOVE){
		moveCell(x,y);
	}else if(automa[currentMatrix][x][y].action==REPRODUCT){
		reproductCell(x,y);
	}
}

void PreyPredatorAutoma::reproductCell(int x, int y) {
	automa[1-currentMatrix][x][y]=automa[currentMatrix][x][y];
	automa[1-currentMatrix][x][y].direction=0;
	if(automa[currentMatrix][x][y].direction==0)
		return ;
	int newX,newY;
	getCordinate(automa[currentMatrix][x][y].direction,x,y,newX,newY);
	//creazione figlio
	automa[1-currentMatrix][newX][newY].type=automa[1-currentMatrix][x][y].type;
	automa[1-currentMatrix][newX][newY].age=0;
	automa[1-currentMatrix][newX][newY].fasting=0;
	automa[1-currentMatrix][newX][newY].direction=0;
}

void PreyPredatorAutoma::moveCell(int x, int y) {
	int direction=automa[currentMatrix][x][y].direction;
	int nx,ny;
	getCordinate(direction,x,y,nx,ny);
	automa[1-currentMatrix][nx][ny]=automa[currentMatrix][x][y];
	automa[1-currentMatrix][nx][ny].direction=0;
}



void PreyPredatorAutoma::getCordinate(int direction, int x, int y, int& newX,int& newY) {
	if(direction==0){
		newY=y;
		newX=x;
	}else if(direction==1){
		newY=y;
		newX=x-1;
	}else if(direction==2){
		newY=y-1;
		newX=x-1;
	}else if(direction==3){
		newY=y-1;
		newX=x;
	}else if(direction==4){
		newY=y-1;
		newX=x+1;
	}else if(direction==5){
		newY=y;
		newX=x+1;
	}else if(direction==6){
		newY=y+1;
		newX=x+1;
	}else if(direction==7){
		newY=y+1;
		newX=x;
	}else if(direction==8){
		newY=y+1;
		newX=x-1;
	}
}

int PreyPredatorAutoma::getDirection(int x, int y, int newX, int newY) {
	if(newY == y  && newX==x){

		return 0;
	}else if(newY == y  && newX==x-1){

		return 1;
	}else if(newY == y-1  && newX==x-1){

		return 2;
	}else if(newY == y-1  && newX==x){

		return 3;
	}else if(newY == y-1  && newX==x+1){

		return 4;
	}else if(newY == y  && newX==x+1){

		return 5;
	}else if(newY == y+1  && newX==x+1){

		return 6;
	}else if(newY == y+1  && newX==x){

		return 7;
	}else if(newY == y+1  && newX==x-1){

		return 8;
	}
	return -1;
}

void PreyPredatorAutoma::getNeighborhood(int x, int y, int  *&nx, int *&ny,int& N) {
	nx=new int[8];
	ny=new int[8];
	N=0;
	if(x>0){
		nx[N]=x-1;
		ny[N]=y;
		N++;
	}
	if(y>0){
		nx[N]=x;
		ny[N]=y-1;
		N++;
	}
	if(y>0 && x>0){
		nx[N]=x-1;
		ny[N]=y-1;
		N++;
	}
	if(y>0 && x<this->N-1){
		nx[N]=x+1;
		ny[N]=y-1;
		N++;
	}
	if(y<this->N-1 && x>0){
		nx[N]=x-1;
		ny[N]=y+1;
		N++;
	}
	if(x<this->N-1){
		nx[N]=x+1;
		ny[N]=y;
		N++;
	}
	if(y<this->N-1){
		nx[N]=x;
		ny[N]=y+1;
		N++;
	}
	if(x<this->N-1 && y<this->N-1){
		nx[N]=x+1;
		ny[N]=y+1;
		N++;
	}

}

int PreyPredatorAutoma::searchCell(int x, int y, int type) {
	int *nx=0;
	int *ny=0;
	int N;

	int directions[8];
	int cont=0;

	getNeighborhood(x,y,nx,ny,N);
	for(int i=0;i<N;i++){
		if(automa[currentMatrix][nx[i]][ny[i]].type==type){
			directions[cont]=getDirection(x,y,nx[i],ny[i]);
			cont++;
		}

	}
	delete []nx;
	delete []ny;
	if(cont>0)
		return directions[rand()%cont];
	return 0;
}




int PreyPredatorAutoma::getNumberCurrentPrey() {
	int contPrey=0;
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			if(automa[currentMatrix][i][j].type==PREY)
				contPrey++;
	return contPrey;

}

bool PreyPredatorAutoma::isNeighborhood(int x1, int y1, int x2, int y2) {
	int *nx=0;
	int *ny=0;
	int N;
	bool returnValue=false;

	getNeighborhood(x1,y1,nx,ny,N);
	for(int i=0;i<N;i++)
		if(nx[i]==x2 && ny[i]==y2){
			returnValue=true;
			break;
		}
	delete nx;
	delete ny;

	return returnValue;
}

int PreyPredatorAutoma::getNumberCurrentPredator() {
	int contPredator=0;
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)
			if(automa[currentMatrix][i][j].type==PREDATOR)
				contPredator++;
	return contPredator;
}


int** PreyPredatorAutoma::getField() {
	return field;
}

cell** PreyPredatorAutoma::getCurrentMatrix() {
	return automa[currentMatrix];
}

