#ifndef __CLASSES_INCLUDED__
#define __CLASSES_INCLUDED__
#include "umfpack.h"
#include "graphics.h"
#include <math.h>
#include <queue>
using namespace std;

// static bool rubber_band_on = false;
// static bool have_entered_line, have_rubber_line;
// static bool line_entering_demo = false;
// //static float x1, y1, x2, y2;  
// static int num_new_button_clicks = 0;

queue<vector<float> > spreadQueueXY;
queue<vector<int> > spreadQueueBlocks;

class Objects{
private:
	vector<vector<float> > fixed;
	vector<vector<int> > netsFile;
	int maxnet = 0; 
	vector<vector<int> > netsBtwnBlocks;
	vector<vector<int> > Edges;
	int block_ID;
	vector<float> edgeWeights;
	vector<int> edgeNum;
	vector<int> p;
	vector<int> verticies;
	vector<vector<float> >  Q;
	vector<float> BX;
	vector<float> BY;
	vector<float>xPositions;
	vector<float>yPositions;
	vector<int> blockstopass;
	

	double *resultx, *resulty;
	int nz, n;
	float N;
	int edgeNumSum;
	

public:
	vector<float> Ap;
	vector<float> Ai;
	vector<float> Ax;
	float fixedX, fixedY, fixedBlockNum;

//Fixed Block Definitions
	void addFixedObject(vector<float> next){fixed.push_back(next);}
	int fixedAmount(){return fixed.size();}
	vector<float> getFixedObject(int x){return fixed[x];}

//Block_ID with nets its attached to
	void addNet(vector<int> add){
		for (int i=0; i<add.size(); i++){
			if (i>1 && add[i] > maxnet)
				maxnet = add[i];
		}
		netsFile.push_back(add);
	}

	int getMaxNet(){return maxnet;}
	int blocksAmount(){return netsFile.size();}
	
	vector<int> getNextNet(int x){return netsFile[x];}
	void getN(){
		int m = netsFile.size();
		float N = ceil(sqrt(m));
		this -> N = N;
	}
	float returnN(){return N;}
	vector<float> getxPositions(){return xPositions;}
	vector<float> getyPositions(){return yPositions;}

//Each net with all blocks it contains
	void establishNetlist(int size);

	void outputNBB(){
		for(int i=1; i<maxnet+1; i++){
			cout << "Net " << i << ": ";
			for(int j=0; j<netsBtwnBlocks[i].size(); j++){
				cout << netsBtwnBlocks[i][j] << " ";
			}
			cout << endl; //<< "The Size is: " << netsBtwnBlocks[i].size() << endl;
		}
	}
	vector<vector<int> > getNetSet(){return netsBtwnBlocks;}

//work with clique model
	void createClique();
	vector<vector<int> > createEdges();
	float setMatrixDiagonal(int block_ID);
	float setRestofMatrix(int block_ID, int column);
	void outputEdges(int iteration, vector<vector<int> >& Edges);
	void defineMatrix(int size1, int size2);
	void defineBforX();
	void defineBforY();
	void UMFPACKIO();
	void computeLocation();
	float HPWL();
	void overlapRemoval(int depth, float centerx, float centery, vector<int> &blocks);
	void executeRecursion(int iteration);

	// void drawGrid();
	// void delay (void);
	// void drawscreen (void);
	// void act_on_new_button_func (void (*drawscreen_ptr) (void));
	// void act_on_button_press (float x, float y);
	// void act_on_mouse_move (float x, float y);
	// void act_on_key_press (char c);

//For each step
	void runStep1();
	void runStep2(int iteration);
	void snap1();
	void snap2();

//Recursion
	void recursiveNetlist(vector<int> &blocks, float centerx, float centery);
	vector<vector<float> > setRecursiveMatrix(int size, int size2, vector<float> &newP, vector<float> &newWeights, vector<vector<int> > &recursiveNetFile);
	float setRecursiveMatrixDiagonal(int position, vector<float> &newP, vector<float> &newWeights, vector<vector<int> > &recursiveNetFile);
	float setRecursiveRestofMatrix(int block_ID, int column, vector<float> &newWeights, vector<vector<int> > &recursiveNetFile);
	vector<float> setRecursiveBX(vector<vector<int> > &recursiveNetFile, vector<vector<float> > &newFixedBlocks, vector<float> &newWeights);
	vector<float> setRecursiveBY(vector<vector<int> > &recursiveNetFile, vector<vector<float> > &newFixedBlocks, vector<float> &newWeights);
	vector<float> getrecursiveEdges(vector<int> &blocks, vector<float> &newP);
	vector<float> getrecursiveP(vector<vector<int> > &recNets, vector<int> &blocks);
	vector<vector<int> > recursiveNetsBtwnBlocks(vector<vector<int> > &recursiveNetFile, int firstnet);
};

void Objects::runStep1(){
	getN();
	establishNetlist(maxnet+1);
	//outputNBB();
	createClique();
	defineMatrix(netsFile.size(),fixed.size());
	UMFPACKIO();
	computeLocation();
}

void Objects::runStep2(int iteration){
	executeRecursion(iteration);
}

//fix for fixed blocks
void Objects::snap1(){
	int count = 0;
	vector<vector<int> > gridspace(N,vector<int>(N,0));
	
	float xValue, yValue;
	float val2snap=0, differenceMin=0, differenceCurrent;

	for(int i=0; i< fixed.size(); i++){
		gridspace[fixed[i][1] - 0.5 ][fixed[i][2] - 0.5] = fixed[i][0];
	}

	for(int i=0; i<gridspace.size(); i++){
		for(int j=0; j<gridspace.size(); j++){
			if(gridspace[i][j] == 0){	
				xValue = i+0.5;
				yValue = j+0.5;
				count++;

				for(int k=fixed.size(); k<xPositions.size(); k++){
					differenceCurrent = (yPositions[k]-yValue)/(xPositions[k]-xValue);
					if(differenceCurrent<0)
						differenceCurrent *= -1;
					
					if(val2snap == 0 && differenceMin == 0){
						if(xPositions[k] > 0 && yPositions[k]>0){
							differenceMin = differenceCurrent;
							val2snap = k + 1;
						}
						else if(k==xPositions.size()-1)
							goto end;
					}
					else if(differenceCurrent < differenceMin){
						if(xPositions[k] > 0 && yPositions[k]>0){
							differenceMin = differenceCurrent;
							val2snap = k + 1;
						}
						
					}
				}

				gridspace[i][j] = val2snap;
				
				xPositions[val2snap-1] *= -1;
				yPositions[val2snap-1] *= -1;
				val2snap = 0;
				differenceMin = 0;
			}
		}
	}

	end:
	for(int i=fixed.size(); i<xPositions.size(); i++){
		xPositions[i] *= -1;
		yPositions[i] *= -1;
	}


	for(int i=0; i<gridspace.size(); i++){
		for(int j=0; j<gridspace[i].size(); j++){
			//cout << gridspace[i][j] << " ";
			if(gridspace[i][j] > fixed.size()){
				xPositions[gridspace[i][j]-1] = i+0.5;
				yPositions[gridspace[i][j]-1] = j+0.5;
			}
		}
		//cout << endl;
	}

	//cout << fixed.size() << " " << xPositions.size() << " " << count;
	this -> xPositions = xPositions;
	this -> yPositions = yPositions;
}

void Objects::snap2(){
	vector<vector<int> > gridspace(N,vector<int>(N,0));
}

void Objects::establishNetlist(int size){
	vector<vector<int> > netsBtwnBlocks(size, vector<int>());
	vector<int> currentNet;
	int value;

	int block_ID, block_type;
	for(int i=0; i<netsFile.size(); i++){
		currentNet = netsFile[i];
		block_ID = currentNet[0];
		block_type = currentNet[1];
		for(int j=2; j<currentNet.size(); j++){
			value = currentNet[j];
			netsBtwnBlocks[value].push_back(block_ID);
		}
	}

	this -> netsBtwnBlocks = netsBtwnBlocks;
}

void Objects::createClique(){
	vector<float> edgeWeights;
	vector<int> edgeNum;
	vector<int> p;
	for(int i=1; i<netsBtwnBlocks.size(); i++){
		float current = netsBtwnBlocks[i].size();
		p.push_back(current);
		edgeNum.push_back((current*(current - 1))/2);
		edgeWeights.push_back(2/current);
	}

	int edgeNumSum=0;
	for(int i=0; i<edgeNum.size(); i++){
		edgeNumSum += edgeNum[i];
	}
	
	this -> edgeNumSum = edgeNumSum;
	this -> p = p;
	this -> edgeNum = edgeNum;
	this -> edgeWeights = edgeWeights;

	// for(int i=0; i<p.size(); i++){
	// 	createEdges(i, netsBtwnBlocks[i+1]);
	// }
}

vector<vector<int> > Objects::createEdges(){
	
	int k=0;
	vector<vector<int> > Edges(edgeNumSum, vector<int>());

	vector<int> verticies;
	for(int j=1; j<netsBtwnBlocks.size(); j++){
		int location = 0;
		int connected = 1;
		verticies = netsBtwnBlocks[j];
		for(int i=0; i<edgeNum[j-1]; i++){
			Edges[k].push_back(verticies[location]);
			Edges[k].push_back(verticies[connected]);
			if(connected == verticies.size() - 1){
				location++;
				connected = location + 1;
				k++;
			}
			else{
				k++;
				connected++;
			}

			
		}
		verticies.clear();
	}

	// for(int i=0; i<Edges.size(); i++){
	// 	for(int j=0; j<Edges[i].size(); j++){
	// 		cout << Edges[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	
	return Edges;
}

void Objects::outputEdges(int iteration, vector<vector<int> >& Edges){
	for(int i=0; i<edgeNum[iteration]; i++){
		int current = Edges[i].size();
		cout << "Edge " << i << ": ";
		for(int j=0; j<current; j++){
			cout << Edges[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl << endl;
}

float Objects::setMatrixDiagonal(int block_ID){
	vector<int> netsToCheck;
	for(int i=2; i<netsFile[block_ID].size(); i++){
		netsToCheck.push_back(netsFile[block_ID][i]);
	}

	float matrixValue=0;

	for(int i=0; i<netsToCheck.size(); i++){
		matrixValue += (p[netsToCheck[i]-1] - 1)*edgeWeights[netsToCheck[i]-1];
	}

	netsToCheck.clear();

	return matrixValue;
}

float Objects::setRestofMatrix(int block_ID, int column){
	
	float matrixValue = 0;
	vector<int> netsToCheck;
	vector<int> netsCompare;
	for(int i=2; i<netsFile[block_ID].size(); i++){
		netsToCheck.push_back(netsFile[block_ID][i]);
	}

	for(int i=2; i<netsFile[column].size(); i++){
		netsCompare.push_back(netsFile[column][i]);
	}

	for(int i=0; i<netsToCheck.size(); i++){
		for(int j=0; j<netsCompare.size(); j++){
			if(netsToCheck[i] == netsCompare[j])
				matrixValue += edgeWeights[netsToCheck[i]-1];
		}
	}
	if(matrixValue)
		matrixValue = (-1)*matrixValue;
	netsCompare.clear();
	netsToCheck.clear();
	return matrixValue;
}

void Objects::defineMatrix(int size1, int size2){
	int size = size1 - size2;
	vector<vector<float> > Q(size, vector<float>(size, 0));
	
	int nz=0;
	

	for(int i=0; i<Q.size(); i++){
		for(int j=0; j<Q.size(); j++){
			if(j!=i){
				float QValue = setRestofMatrix(j+size2,i+size2);
				Q[j][i] = QValue;
				Q[i][j] = QValue;
			}
			else
				Q[i][j] = setMatrixDiagonal(i+size2);

			if(Q[i][j] != 0)
				nz += 1;
		}
	}

	this -> Q = Q;
	this -> nz=nz;
	int n = Q.size();
	this -> n=n;
	
	defineBforX();
	defineBforY();
	//cout << "nz = " << nz << endl;
	// for(int i=0; i<Q.size(); i++){
	// 	for(int j=0; j<Q.size(); j++){
	// 		cout << Q[i][i] << endl;
	// 	}
	// 	cout << endl;
	// }
}

void Objects::defineBforX(){
	int size = netsFile.size() - fixed.size();
	vector<float> BX(size, 0);

	vector<int> netsToCheck;
	vector<int> netsCompare;
	float xPosition, matrixValue=0;
	int stopValue = (int)fixed[0][0];
	for(int i=fixed.size(); i<netsFile.size(); i++){
		for(int j=2; j<netsFile[i].size(); j++)
			netsToCheck.push_back(netsFile[i][j]);

		for(int j=0; j<fixedAmount(); j++){
			int row = fixed[j][0] - 1;
			for(int k=2; k<netsFile[j].size(); k++)
				netsCompare.push_back(netsFile[row][k]);

			xPosition = fixed[j][1];
			for(int k=0; k<netsToCheck.size(); k++){
				for(int l=0; l<netsCompare.size(); l++){
					if(netsToCheck[k] == netsCompare[l]){
						matrixValue += edgeWeights[netsCompare[l] -1]*xPosition;
					}
				}
			}
			
			netsCompare.clear();

		}
		
		BX[i-fixed.size()] = matrixValue;
		matrixValue = 0;
		netsToCheck.clear();
	}
	this -> BX = BX;
	// cout << "BX: " << endl;
	// for(int i=0; i<BX.size(); i++){
	// 	cout << BX[i] << endl;
	// }
	// cout << endl;

	int startValue = (int)fixed[0][0];	
}

void Objects::defineBforY(){
	int size = netsFile.size() - fixed.size();
	vector<float> BY(size, 0);

	vector<int> netsToCheck;
	vector<int> netsCompare;
	float yPosition, matrixValue=0;
	int stopValue = (int)fixed[0][0];
	for(int i=fixed.size(); i<netsFile.size(); i++){
		for(int j=2; j<netsFile[i].size(); j++)
			netsToCheck.push_back(netsFile[i][j]);

		for(int j=0; j<fixedAmount(); j++){
			int row = fixed[j][0] - 1;
			for(int k=2; k<netsFile[j].size(); k++)
				netsCompare.push_back(netsFile[row][k]);

			yPosition = fixed[j][2];
			for(int k=0; k<netsToCheck.size(); k++){
				for(int l=0; l<netsCompare.size(); l++){
					if(netsToCheck[k] == netsCompare[l]){
						matrixValue += edgeWeights[netsCompare[l] -1]*yPosition;
					}
				}
			}
			
			netsCompare.clear();

		}
		BY[i-fixed.size()] = matrixValue;
		matrixValue = 0;
		netsToCheck.clear();
	}
	this -> BY = BY;
	// cout << "BY: " << endl;
	// for(int i=0; i<BY.size(); i++){
	// 	cout << BY[i] << endl;
	// }
	// cout << endl;
}

void Objects::UMFPACKIO(){
	vector<float> Ap;
	vector<float> Ai;
	vector<float> Ax;
	
	bool first = true;

	for(int column=0; column<Q.size(); column++){
		for(int row=0; row<Q.size(); row++){
			if(Q[row][column] != 0){
				Ai.push_back(row);
				Ax.push_back(Q[row][column]);
				if(first){
					Ap.push_back(Ax.size() - 1);
					first = false;
				}
			}
			
		}
		first = true;
	}
	Ap.push_back(nz);

	if(Ai.size() != nz)
		cout << "Did not get right row vector"<< endl;
	// else{
	// 	cout << "Row Vector: ";
	// 	for(int i=0; i<Ai.size(); i++)
	// 		cout << Ai[i] << " ";
	// }
	// cout << endl;

	if(Ax.size() != nz)
		cout << "Did not get right values vector"<< endl;
	// else{
	// 	cout << "Values Vector: ";
	// 	for(int i=0; i<Ax.size(); i++)
	// 		cout << Ax[i] << " ";
	// }
	// cout << endl;

	if(Ap.size() != Q.size() + 1)
		cout << "Did not get right column vector"<< endl;
	// else{
	// 	cout << "Column Vector: ";
	// 	for(int i=0; i<Ap.size(); i++)
	// 		cout << Ap[i] << " ";
	// }
	// cout << endl;

	this -> Ap = Ap;
	this -> Ai = Ai;
	this -> Ax = Ax;
}

void Objects::computeLocation(){
	double *null = (double *) NULL ;
	double *resultx, *resulty, *Axarr, *BXarr, *BYarr;
	int *Aparr, *Aiarr;
	resultx = new double [Q.size()];
	resulty = new double [Q.size()];
	Aparr = new int [Q.size() + 1];
	Axarr = new double [nz];
	Aiarr = new int [nz];
	BXarr = new double [Q.size()];
	BYarr = new double [Q.size()];

	for(int i=0; i<Q.size(); i++){
		BXarr[i] = BX[i];
		BYarr[i] = BY[i];
	}
	// for(int i=0; i<Q.size(); i++){
	// 	cout << BXarr[i] << " ";
	// }
	// cout << endl << endl;
	// for(int i=0; i<Q.size(); i++){
	// 	cout << BYarr[i] << " ";
	// }
	// cout << endl << endl;

	for(int i=0; i<nz; i++){
		Axarr[i] = Ax[i];
		Aiarr[i] = Ai[i];
	}

	// for(int i=0; i<nz; i++){
	// 	cout << Aiarr[i] << " ";
	// }
	// cout << endl << endl;
	// for(int i=0; i<nz; i++){
	// 	cout << Axarr[i] << " ";
	// }
	// cout << endl << endl;

	for(int i=0; i<Q.size() + 1; i++){
		Aparr[i] = Ap[i];
	}
	// for(int i=0; i<Q.size() + 1; i++){
	// 	cout << Aparr[i] <<" ";
	// }
	// cout << endl << endl;


	int i ;
	int n=Q.size();
	void *Symbolicx, *Numericx, *Symbolicy, *Numericy  ;
	(void) umfpack_di_symbolic (n, n, Aparr, Aiarr, Axarr, &Symbolicx, null, null) ;
	(void) umfpack_di_numeric (Aparr, Aiarr, Axarr, Symbolicx, &Numericx, null, null) ;
	umfpack_di_free_symbolic (&Symbolicx) ;

	(void) umfpack_di_solve (UMFPACK_A, Aparr, Aiarr, Axarr, resultx, BXarr, Numericx, null, null) ;
	umfpack_di_free_numeric (&Numericx) ;
	//for (i = 0 ; i < n ; i++) printf ("x [%d] = %g\n", i, resultx[i]) ;

	//cout << endl << endl;

	(void) umfpack_di_symbolic (n, n, Aparr, Aiarr, Axarr, &Symbolicy, null, null) ;
	(void) umfpack_di_numeric (Aparr, Aiarr, Axarr, Symbolicy, &Numericy, null, null) ;
	umfpack_di_free_symbolic (&Symbolicy) ;

	(void) umfpack_di_solve (UMFPACK_A, Aparr, Aiarr, Axarr, resulty, BYarr, Numericy, null, null) ;
	umfpack_di_free_numeric (&Numericy) ;
	//for (i = 0 ; i < n ; i++) printf ("Y [%d] = %g\n", i, resulty[i]) ;

	this -> resultx = resultx;
	this -> resulty = resulty;

	delete [] Aparr;
	delete [] Aiarr;
	delete [] Axarr;
	delete [] BXarr;
	delete [] BYarr;

	vector<float> xPositions;
	vector<float> yPositions;
	
	vector<int> blockstopass;

	for(int i=fixed.size()-1; i>=0; i--){
		xPositions.push_back(fixed[i][1]);
		yPositions.push_back(fixed[i][2]);
	}

	for(int i=0; i<Q.size(); i++){
		xPositions.push_back(resultx[i]);
		yPositions.push_back(resulty[i]);
	}

	this -> xPositions = xPositions;
	this -> yPositions = yPositions;

	for(int i=0; i<xPositions.size(); i++){
		if(i>=fixed.size())
			blockstopass.push_back(i);
		//cout <<"Block " << i << ": = (" << xPositions[i] << "," << yPositions[i] <<")" << endl;
	}

	this -> blockstopass = blockstopass;

	cout << endl;
}

float Objects::HPWL(){
	vector<float> hpwl;

	float currentBlock, total;

	for(int i=1; i<netsBtwnBlocks.size();i++){
		float yMax=0, yMin, xMax=0, xMin;
		for(int j=0; j<netsBtwnBlocks[i].size(); j++){
			currentBlock = netsBtwnBlocks[i][j];
			//cout << currentBlock << " ";

			if(j==0){
				xMin = xPositions[currentBlock-1];
				yMin = yPositions[currentBlock-1];
			}

			if(xPositions[currentBlock-1] < xMin){
				if(xMin > xMax){
					xMax = xMin;
					xMin = xPositions[currentBlock-1];
				}
				else
					xMin = xPositions[currentBlock-1];
			}

			else if(xPositions[currentBlock-1] > xMax)
				xMax = xPositions[currentBlock-1];

			if(yPositions[currentBlock-1] < yMin){
				if(yMin > yMax){
					yMax = yMin;
					yMin = yPositions[currentBlock-1];
				}
				else
					yMin = yPositions[currentBlock-1];
			}

			else if(yPositions[currentBlock-1] > yMax)
				yMax = yPositions[currentBlock-1];


		}
		hpwl.push_back((xMax - xMin) + (yMax - yMin));

		//cout <<endl << "Net " << i << ": " << endl << "xMax = " << xMax <<  endl << "xMin = " << xMin <<  endl << "yMax = " << yMax <<  endl << "yMin = " << yMin << endl << endl;
	}

	for(int i=0; i<hpwl.size(); i++)
		total += hpwl[i];

	return total;
	
}

void Objects::executeRecursion(int iteration){

	if(iteration == 1)
		overlapRemoval(iteration, N/2, N/2, blockstopass);
	else{
		for(int i=0; i<(iteration-1)*4; i++){
			vector<float> vals = spreadQueueXY.front();
			spreadQueueXY.pop();
			vector<int> blocks = spreadQueueBlocks.front();
			spreadQueueBlocks.pop();
			overlapRemoval(iteration,vals[0],vals[1],blocks);
		}

	}
	
}

//assume blocks stores the actual value of block 1-20
void Objects::overlapRemoval(int depth, float centerx, float centery, vector<int> &blocks){
	bool filled = false;
	float xl, xr;
	float yl, yu;

	xl = centerx - N/(4*depth);
	xr = centerx + N/(4*depth);
	yl = centery - N/(4*depth);
	yu = centery + N/(4*depth);


	vector<float> update_positions_x;
	vector<float> update_positions_y;
	vector<float> unorderedX;
	vector<float> unorderedY;
	vector<float> orderedX;
	vector<float> orderedY;

	float minX, minY, location;
	float mediany, medianx, median;

	for(int i=0; i<blocks.size(); i++){
		update_positions_x.push_back(xPositions[blocks[i]-1]);
		unorderedX.push_back(xPositions[blocks[i]-1]);
		update_positions_y.push_back(yPositions[blocks[i]-1]);
		unorderedY.push_back(yPositions[blocks[i]-1]);
	}

	while(orderedX.size() != unorderedX.size()){
		for(int i=0; i<unorderedX.size(); i++){
			if(unorderedX[i]>0){
				minX = unorderedX[i];
				location = i;
				break;
			}
		}
		for(int i=0; i<unorderedX.size(); i++){
			if(unorderedX[i]<minX && unorderedX[i]>0){
				minX = unorderedX[i];
				location = i;
			}
		}

		orderedX.push_back(minX);
		unorderedX[location] = (-1)*unorderedX[location];
	}

	while(orderedY.size() != unorderedY.size()){
		for(int i=0; i<unorderedY.size(); i++){
			if(unorderedY[i]>0){
				minY = unorderedY[i];
				location = i;
				break;
			}
		}
		for(int i=0; i<unorderedY.size(); i++){
			if(unorderedY[i]<minY && unorderedY[i]>0){
				minY = unorderedY[i];
				location = i;
			}
		}

		orderedY.push_back(minY);
		unorderedY[location] = (-1)*unorderedY[location];
	}

	medianx = orderedX[ceil(orderedX.size()/2)];
	mediany = orderedY[ceil(orderedY.size()/2)];

	unorderedX.clear();
	unorderedY.clear();
	orderedX.clear();
	orderedY.clear();

	vector<int> blocksLowerLeft;
	vector<int> blocksLowerRight;
	vector<int> blocksUpperLeft;
	vector<int> blocksUpperRight;

	for(int i=0; i<update_positions_x.size(); i++){
		if(update_positions_x[i] > medianx){
			if(update_positions_y[i] > mediany)
				blocksUpperRight.push_back(blocks[i]);
			else
				blocksLowerRight.push_back(blocks[i]);
		}
		else{
			if(update_positions_y[i] > mediany)
				blocksUpperLeft.push_back(blocks[i]);
			else
				blocksLowerLeft.push_back(blocks[i]);
		}
	}

	// cout << "Lower Left: " << blocksLowerLeft.size() << endl;
	// for(int i=0; i<blocksLowerLeft.size(); i++){
	// 	cout << blocksLowerLeft[i] << " ";
	// }

	// cout << endl << "Lower Right:" << blocksLowerRight.size()<< endl;
	// for(int i=0; i<blocksLowerRight.size(); i++){
	// 	cout << blocksLowerRight[i] << " ";
	// }

	// cout << endl << "Upper Left: " << blocksUpperLeft.size()<< endl;
	// for(int i=0; i<blocksUpperLeft.size(); i++){
	// 	cout << blocksUpperLeft[i] << " ";
	// }

	// cout << endl << "Upper Right: " << blocksUpperRight.size()<< endl;
	// for(int i=0; i<blocksUpperRight.size(); i++){
	// 	cout << blocksUpperRight[i] << " ";
	// }

	// cout << endl << endl;
	recursiveNetlist(blocksLowerLeft, xl, yl);
	recursiveNetlist(blocksLowerRight, xr, yl);
	recursiveNetlist(blocksUpperLeft, xl, yu);
	recursiveNetlist(blocksUpperRight, xr, yu);


	//queue<vector<float> > spreadQueueXY;
	vector<float> XY;

	XY.push_back(xl);
	XY.push_back(yl);
	spreadQueueXY.push(XY);
	XY.clear();
	XY.push_back(xr);
	XY.push_back(yl);
	spreadQueueXY.push(XY);
	XY.clear();
	XY.push_back(xl);
	XY.push_back(yu);
	spreadQueueXY.push(XY);
	XY.clear();
	XY.push_back(xr);
	XY.push_back(yu);
	spreadQueueXY.push(XY);
	XY.clear();

	spreadQueueBlocks.push(blocksLowerLeft);
	spreadQueueBlocks.push(blocksLowerRight);
	spreadQueueBlocks.push(blocksUpperLeft);
	spreadQueueBlocks.push(blocksUpperRight);


}

void Objects::recursiveNetlist(vector<int> &blocks, float centerx, float centery){
	vector<vector<int> > recursiveNetFile;
	vector<vector<float> > newFixedBlocks;
	vector<float> newFixed;
	vector<int> newNet;
	vector<float> newWeights;
	vector<float> newP;

	for(int i=0; i<fixed.size(); i++){
		recursiveNetFile.push_back(netsFile[i]);
	}

	for(int i=0; i<blocks.size(); i++){
		recursiveNetFile.push_back(netsFile[blocks[i]]);
	}

	for(int i=fixed.size()-1; i>=0; i--){
		newFixedBlocks.push_back(fixed[i]);
	}
	

	newFixed.push_back(netsFile.size() + 1);
	newFixed.push_back(centerx);
	newFixed.push_back(centery);
	newFixedBlocks.push_back(newFixed);
	newFixed.clear();

	newNet.push_back(netsFile.size() + 1);
	newNet.push_back(0);
	int firstnet = netsBtwnBlocks.size();
	for(int i=newFixedBlocks.size()-1; i<recursiveNetFile.size(); i++){
		recursiveNetFile[i].push_back(firstnet);
		newNet.push_back(firstnet);
		firstnet++;
	}
	recursiveNetFile.push_back(newNet);
	newNet.clear();

	vector<vector<int> > recNets = recursiveNetsBtwnBlocks(recursiveNetFile,firstnet);
	newP = getrecursiveP(recNets, blocks);
	newWeights = getrecursiveEdges(blocks, newP);

	// for(int i=0; i<p.size(); i++){
	// 	newP.push_back(p[i]);
	// }

	// for(int i=2; i<recursiveNetFile[recursiveNetFile.size()-1].size(); i++){
	// 	newP.push_back(2);
	// }

	// vector<int> netsToCheck(maxnet+1,0);

	// for(int i=fixed.size(); i<recursiveNetFile.size()-1; i++){
	// 	for(int j=2; j<recursiveNetFile[i].size(); j++)
	// 		netsToCheck[recursiveNetFile[i][j]] += 1;
	// }

	// for(int i=0; i<newWeights.size(); i++){
	// 	cout <<"Edge " << i << " weight: " <<  newWeights[i] << endl;
	// }
	// cout << endl;

	// for(int i=0; i<newP.size(); i++){
	// 	cout <<"P " << i <<": "<<  newP[i] << endl;
	// }
	// cout << endl;
	// for(int i=0; i<recursiveNetFile.size(); i++){
	// 	for(int j=0; j<recursiveNetFile[i].size(); j++)
	// 		cout << recursiveNetFile[i][j] << " ";
	// 	cout << endl;
	// }

	// cout << endl;

	// for(int i=0; i<newFixedBlocks.size(); i++){
	// 	for(int j=0; j<newFixedBlocks[i].size(); j++)
	// 		cout << newFixedBlocks[i][j] << " ";
	// 	cout << endl;
	// }

	// cout << endl;


	// for(int i=0; i<recNets.size(); i++){
	// 	cout << "Net " << i << ": ";
	// 	for(int j=0; j<recNets[i].size(); j++)
	// 		cout << recNets[i][j] << " ";
	// 	cout << endl;
	// }

	vector<vector<float> > Qnew = setRecursiveMatrix(recursiveNetFile.size() - newFixedBlocks.size(), newFixedBlocks.size()-1, newP, newWeights,recursiveNetFile);

	vector<float> recursiveBX = setRecursiveBX(recursiveNetFile, newFixedBlocks, newWeights);

	vector<float> recursiveBY = setRecursiveBY(recursiveNetFile, newFixedBlocks, newWeights);

	vector<float> Aprec;
	vector<float> Airec;
	vector<float> Axrec;
	int nz2=0;
	bool first = true;

	for(int column=0; column<Qnew.size(); column++){
		for(int row=0; row<Qnew.size(); row++){
			if(Qnew[row][column] != 0){
				Airec.push_back(row);
				Axrec.push_back(Qnew[row][column]);
				if(first){
					Aprec.push_back(Axrec.size() - 1);
					first = false;
				}
			}
			
		}
		first = true;
	}
	for(int i=0; i<Qnew.size(); i++){
		for(int j=0; j<Qnew.size(); j++){
			if(Qnew[i][j] != 0)
				nz2 += 1;
		}
	}
	Aprec.push_back(nz2);

	double *null = (double *) NULL ;
	double *recresultx, *recresulty, *Axarr, *BXarr, *BYarr;
	int *Aparr, *Aiarr;
	recresultx = new double [Qnew.size()];
	recresulty = new double [Qnew.size()];
	Aparr = new int [Qnew.size() + 1];
	Axarr = new double [nz2];
	Aiarr = new int [nz2];
	BXarr = new double [Qnew.size()];
	BYarr = new double [Qnew.size()];

	for(int i=0; i<Qnew.size(); i++){
		BXarr[i] = recursiveBX[i];
		BYarr[i] = recursiveBY[i];
	}
	// cout <<"BX" << endl;
	// for(int i=0; i<Qnew.size(); i++){
	// 	cout << BXarr[i] << " ";
	// }
	// cout << endl << endl;
	// cout <<"BY" << endl;
	// for(int i=0; i<Qnew.size(); i++){
	// 	cout << BYarr[i] << " ";
	// }
	// cout << endl << endl;

	for(int i=0; i<nz2; i++){
		Axarr[i] = Axrec[i];
		Aiarr[i] = Airec[i];
	}

	// cout<< "Ai" << endl;
	// for(int i=0; i<nz2; i++){
	// 	cout << Aiarr[i] << " ";
	// }

	// cout << endl << endl << "Ax" << endl;
	// for(int i=0; i<nz2; i++){
	// 	cout << Axarr[i] << " ";
	// }
	// cout << endl << endl;

	for(int i=0; i<Qnew.size() + 1; i++){
		Aparr[i] = Aprec[i];
	}
	// cout << "Ap" << endl;
	// for(int i=0; i<Qnew.size() + 1; i++){
	// 	cout << Aparr[i] <<" ";
	// }
	// cout << endl << endl;

	// cout << "Center: (" << centerx <<"," << centery << ") " << endl;

	int i ;
	int n=Qnew.size();
	void *Symbolicx, *Numericx, *Symbolicy, *Numericy  ;
	(void) umfpack_di_symbolic (n, n, Aparr, Aiarr, Axarr, &Symbolicx, null, null) ;
	(void) umfpack_di_numeric (Aparr, Aiarr, Axarr, Symbolicx, &Numericx, null, null) ;
	umfpack_di_free_symbolic (&Symbolicx) ;

	(void) umfpack_di_solve (UMFPACK_A, Aparr, Aiarr, Axarr, recresultx, BXarr, Numericx, null, null) ;
	umfpack_di_free_numeric (&Numericx) ;
	//for (i = 0 ; i < n ; i++) printf ("x [%d] = %g\n", i, recresultx[i]) ;

	//cout << endl;

	(void) umfpack_di_symbolic (n, n, Aparr, Aiarr, Axarr, &Symbolicy, null, null) ;
	(void) umfpack_di_numeric (Aparr, Aiarr, Axarr, Symbolicy, &Numericy, null, null) ;
	umfpack_di_free_symbolic (&Symbolicy) ;

	(void) umfpack_di_solve (UMFPACK_A, Aparr, Aiarr, Axarr, recresulty, BYarr, Numericy, null, null) ;
	umfpack_di_free_numeric (&Numericy) ;
	//for (i = 0 ; i < n ; i++) printf ("Y [%d] = %g\n", i, recresulty[i]) ;

	for(int i=0; i<blocks.size(); i++){
		xPositions[blocks[i]] = recresultx[i];
		yPositions[blocks[i]] = recresulty[i];
	}

	for(int i=0; i<xPositions.size(); i++){
		if(xPositions[i] <0){
			xPositions[i] *= (-1);
		}
		if(yPositions[i]<0){
			yPositions[i] *= (-1);
		}
	}

	this -> xPositions = xPositions;
	this -> yPositions = yPositions;

	delete [] Aparr;
	delete [] Aiarr;
	delete [] Axarr;
	delete [] BXarr;
	delete [] BYarr;
	delete [] recresultx;
	delete [] recresulty;

	recursiveNetFile.clear();
	newFixedBlocks.clear();
	newFixed.clear();
	newNet.clear();
	newWeights.clear();
	newP.clear();
	Qnew.clear();
	recursiveBX.clear();
	recursiveBY.clear();
}

vector<vector<int> > Objects::recursiveNetsBtwnBlocks(vector<vector<int> > &recursiveNetFile, int firstnet){
	vector<vector<int> > recNets(firstnet, vector<int>());
	for(int i=0; i<recursiveNetFile.size(); i++){
		int block_ID = recursiveNetFile[i][0];
		for(int j=2; j<recursiveNetFile[i].size(); j++){
			//cout << "Net: " << recursiveNetFile[i][j] << endl;
			recNets[recursiveNetFile[i][j]].push_back(block_ID);
		}
		//cout << endl;
	}
	return recNets;
}

vector<float> Objects::getrecursiveP(vector<vector<int> > &recNets, vector<int> &blocks){
	vector<float> newP(recNets.size(),0);
	for(int i=0; i<newP.size(); i++){
		if(recNets[i].size() > 1)
			newP[i] = recNets[i].size();
	}

	return newP;
}

vector<float> Objects::getrecursiveEdges(vector<int> &blocks, vector<float> &newP){
	vector<float> newWeights(newP.size(),0);
	for(int i=0; i<newWeights.size(); i++){
		if(newP[i]!=0){
			newWeights[i] = 2/newP[i];

		}
		if(i>netsBtwnBlocks.size()){
			newWeights[i] *= 10;
		}
	}

	return newWeights;
}

vector<vector<float> > Objects::setRecursiveMatrix(int size, int size2, vector<float> &newP, vector<float> &newWeights, vector<vector<int> > &recursiveNetFile){
	vector<vector<float> > Qnew(size, vector<float>(size, 0));
	for(int i=0; i<Qnew.size(); i++){
		for(int j=0; j<Qnew.size(); j++){
			if(j!=i){
				float QValue = setRecursiveRestofMatrix(j+size2,i+size2,newWeights, recursiveNetFile);
				Qnew[j][i] = QValue;
				Qnew[i][j] = QValue;
			}
			else
				Qnew[i][j] = setRecursiveMatrixDiagonal(i+size2, newP, newWeights, recursiveNetFile);

		}
	}

	// for(int i=0; i<Qnew.size(); i++){
	// 	for(int j=0; j<Qnew.size(); j++){
	// 		cout << Qnew[i][j] << endl;
	// 	}
	// 	cout << endl;
	// }

	return Qnew;
}

float Objects::setRecursiveMatrixDiagonal(int position, vector<float> &newP, vector<float> &newWeights, vector<vector<int> > &recursiveNetFile){
	vector<int> netsToCheck;
	for(int i=2; i<recursiveNetFile[position].size(); i++){
		netsToCheck.push_back(recursiveNetFile[position][i]);
	}

	float matrixValue=0;

	for(int i=0; i<netsToCheck.size(); i++){
		matrixValue += (newP[netsToCheck[i]] - 1)*newWeights[netsToCheck[i]];
	}

	netsToCheck.clear();

	return matrixValue;
}

float Objects::setRecursiveRestofMatrix(int block_ID, int column, vector<float> &newWeights, vector<vector<int> > &recursiveNetFile){
	
	float matrixValue = 0;
	vector<int> netsToCheck;
	vector<int> netsCompare;
	for(int i=2; i<recursiveNetFile[block_ID].size(); i++){
		netsToCheck.push_back(recursiveNetFile[block_ID][i]);
	}

	// for(int i=0; i<netsToCheck.size(); i++){
	// 	cout << netsToCheck[i] << " ";
	// }
	// cout << endl;

	for(int i=2; i<recursiveNetFile[column].size(); i++){
		netsCompare.push_back(recursiveNetFile[column][i]);
	}

	// for(int i=0; i<netsCompare.size(); i++){
	// 	cout << netsCompare[i] << " ";
	// }
	// cout << endl;

	for(int i=0; i<netsToCheck.size(); i++){
		for(int j=0; j<netsCompare.size(); j++){
			if(netsToCheck[i] == netsCompare[j])
				matrixValue += newWeights[netsToCheck[i]];
		}
	}
	if(matrixValue)
		matrixValue = (-1)*matrixValue;
	//cout <<"Matrixval: " << matrixValue << endl;
	netsCompare.clear();
	netsToCheck.clear();
	return matrixValue;
}

vector<float> Objects::setRecursiveBX(vector<vector<int> > &recursiveNetFile, vector<vector<float> > &newFixedBlocks, vector<float> &newWeights){
	int size = recursiveNetFile.size() - newFixedBlocks.size();
	vector<float> recursiveBX(size, 0);


	vector<int> netsToCheck;
	vector<int> netsCompare;
	int row;
	float xPosition, matrixValue=0;
	for(int i=newFixedBlocks.size()-1; i<recursiveNetFile.size()-1; i++){
		for(int j=2; j<recursiveNetFile[i].size(); j++)
			netsToCheck.push_back(recursiveNetFile[i][j]);

		for(int j=0; j<newFixedBlocks.size(); j++){
			if(j==newFixedBlocks.size()-1)
				row = recursiveNetFile.size()-1;
			else
				row = j;
			
			for(int k=2; k<recursiveNetFile[row].size(); k++)
				netsCompare.push_back(recursiveNetFile[row][k]);

			xPosition = newFixedBlocks[j][1];
			for(int k=0; k<netsToCheck.size(); k++){
				for(int l=0; l<netsCompare.size(); l++){
					if(netsToCheck[k] == netsCompare[l]){
						matrixValue += newWeights[netsCompare[l]]*xPosition;
					}

				}
			}
			netsCompare.clear();
		}
		
		recursiveBX[i-newFixedBlocks.size()+1] = matrixValue;
		matrixValue = 0;
		netsToCheck.clear();
	}

	
	// cout << "BX: " << endl;
	// for(int i=0; i<recursiveBX.size(); i++){
	// 	cout << recursiveBX[i] << " ";
	// }
	// cout << endl << endl;

	return recursiveBX;
}

vector<float> Objects::setRecursiveBY(vector<vector<int> > &recursiveNetFile, vector<vector<float> > &newFixedBlocks, vector<float> &newWeights){
	int size = recursiveNetFile.size() - newFixedBlocks.size();
	vector<float> recursiveBY(size, 0);


	vector<int> netsToCheck;
	vector<int> netsCompare;
	int row;
	float yPosition, matrixValue=0;
	for(int i=newFixedBlocks.size()-1; i<recursiveNetFile.size()-1; i++){
		for(int j=2; j<recursiveNetFile[i].size(); j++)
			netsToCheck.push_back(recursiveNetFile[i][j]);

		for(int j=0; j<newFixedBlocks.size(); j++){
			if(j==newFixedBlocks.size()-1)
				row = recursiveNetFile.size()-1;
			else
				row = j;
			
			for(int k=2; k<recursiveNetFile[row].size(); k++)
				netsCompare.push_back(recursiveNetFile[row][k]);

			yPosition = newFixedBlocks[j][2];
			for(int k=0; k<netsToCheck.size(); k++){
				for(int l=0; l<netsCompare.size(); l++){
					if(netsToCheck[k] == netsCompare[l]){
						matrixValue += newWeights[netsCompare[l]]*yPosition;
					}
				}
			}
			netsCompare.clear();
		}
		
		recursiveBY[i-newFixedBlocks.size()+1] = matrixValue;
		matrixValue = 0;
		netsToCheck.clear();
	}

	return recursiveBY;


	// cout << "BY: " << endl;
	// for(int i=0; i<recursiveBY.size(); i++){
	// 	cout << recursiveBY[i] << " ";
	// }
	// cout << endl;
}



#endif

