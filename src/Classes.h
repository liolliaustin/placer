#ifndef __CLASSES_INCLUDED__
#define __CLASSES_INCLUDED__
#include "umfpack.h"
using namespace std;

class Objects{
private:
	vector<vector<float> > fixed;
	vector<vector<int> > netsFile;
	int maxnet = 0; 
	vector<vector<int> > netsBtwnBlocks;
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

	double *resultx, *resulty;
	int nz, n;
	

public:
	vector<float> Ap;
	vector<float> Ai;
	vector<float> Ax;
	float fixedX, fixedY, fixedBlockNum;

//Fixed Block Definitions
	void addFixedObject(vector<float> next){fixed.push_back(next);}
	int fixedAmount(){return fixed.size();}
	vector<float> getFixedObject(int x){
		return fixed[x];
	}

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

//Each net with all blocks it contains
	void establishNetlist();

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
	void createEdges(int iteration, vector<int>& verticies);
	float setMatrixDiagonal(int block_ID);
	float setRestofMatrix(int block_ID, int column);
	void outputEdges(int iteration, vector<vector<int> >& Edges);
	void defineMatrix();
	void defineBforX();
	void defineBforY();
	void UMFPACKIO();
	void computeLocation();
	void HPWL();
};

void Objects::establishNetlist(){
	vector<vector<int> > netsBtwnBlocks(maxnet+1, vector<int>());
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

	// for(int i=0; i<edgeWeights.size(); i++){
	// 	cout << "Net " << i +1 << " weight: " << edgeWeights[i] << endl;
	// }
	
	this -> p = p;
	this -> edgeNum = edgeNum;
	this -> edgeWeights = edgeWeights;

	// for(int i=0; i<p.size(); i++){
	// 	createEdges(i, netsBtwnBlocks[i+1]);
	// }

}
//somehow recursively create edges for given list of blocks attached to net
void Objects::createEdges(int iteration, vector<int>& verticies){
	int location = 0;
	int connected = 1;
	// cout << "verticies: ";
	// for(int i=0; i<verticies.size(); i++){
	// 	cout << verticies[i] << " ";
	// }
	// cout << endl;

	// cout << "Number of Edges: " << edgeNum[iteration] << endl;
	// cout << "Number of verticies: " << p[iteration] << endl;
	// cout << "Edge Weight: " << edgeWeights[iteration] << endl;
	vector<vector<int> > Edges(edgeNum[iteration], vector<int>());
	for(int i=0; i<edgeNum[iteration]; i++){
		Edges[i].push_back(verticies[location]);
		Edges[i].push_back(verticies[connected]);
		if(connected == verticies.size() - 1){
			location++;
			connected = location + 1;
		}
		else
			connected++;
		
	}
	
	//outputEdges(iteration, Edges);
	Edges.clear();
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

void Objects::defineMatrix(){
	int size = netsFile.size() - fixed.size();
	vector<vector<float> > Q(size, vector<float>(size, 0));
	
	int nz=0;
	

	for(int i=0; i<Q.size(); i++){
		for(int j=0; j<Q.size(); j++){
			if(j!=i){
				float QValue = setRestofMatrix(j+fixed.size(),i+fixed.size());
				Q[j][i] = QValue;
				Q[i][j] = QValue;
			}
			else
				Q[i][j] = setMatrixDiagonal(i+fixed.size());

			if(Q[i][j] != 0)
				nz += 1;
		}
	}

	this -> Q = Q;
	this -> nz=nz;
	int n = Q.size();
	this -> n=n;
	
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

}

void Objects::HPWL(){
	vector<float> xPositions;
	vector<float> yPositions;
	vector<float> hpwl;

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

	// for(int i=0; i<xPositions.size(); i++){
	// 	cout <<"x[" << i << "] = " << xPositions[i] << endl;
	// }
	// for(int i=0; i<yPositions.size(); i++){
	// 	cout <<"y[" << i << "] = " << yPositions[i] << endl;
	// }

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

	cout << "Total: " << total << endl;
}

#endif

