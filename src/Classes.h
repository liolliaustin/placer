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
	double *result;
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
			if (add[i] > maxnet)
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
			// cout << "Net " << i << ": ";
			// for(int j=0; j<netsBtwnBlocks[i].size(); j++){
			// 	//cout << netsBtwnBlocks[i][j] << " ";
			// }
			cout << endl << "The Size is: " << netsBtwnBlocks[i].size() << endl;
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
	for(int i=2; i<netsFile[block_ID-1].size(); i++){
		netsToCheck.push_back(netsFile[block_ID-1][i]);
	}

	float matrixValue=0;

	for(int i=0; i<netsToCheck.size(); i++){
		matrixValue += edgeWeights[netsToCheck[i]-1];
	}

	netsToCheck.clear();

	return matrixValue;
}

float Objects::setRestofMatrix(int block_ID, int column){
	
	float matrixValue = 0;
	vector<int> netsToCheck;
	vector<int> netsCompare;
	for(int i=2; i<netsFile[block_ID-1].size(); i++){
		netsToCheck.push_back(netsFile[block_ID-1][i]);
	}

	for(int i=2; i<netsFile[column-1].size(); i++){
		netsCompare.push_back(netsFile[column-1][i]);
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
	vector<vector<float> > Q(netsFile.size(), vector<float>(netsFile.size(), 0));
	
	int nz=0;
	

	for(int i=0; i<Q.size(); i++){
		for(int j=0; j<Q.size(); j++){
			if(j!=i){
				float QValue = setRestofMatrix(j+1,i+1);
				Q[j][i] = QValue;
				Q[i][j] = QValue;
			}
			else
				Q[i][j] = setMatrixDiagonal(i+1);

			if(Q[i][j] != 0)
				nz += 1;
		}
	}

	this -> Q = Q;
	this -> nz=nz;
	int n = Q.size();
	this -> n=n;
	
	//cout << "nz = " << nz << endl;
	for(int i=0; i<Q.size(); i++){
		//for(int j=0; j<Q.size(); j++){
		cout << Q[i][0] << endl;
		//}
		// cout << endl;
	}
}

void Objects::defineBforX(){
	vector<float> BX(netsFile.size(), 0);

	vector<int> netsToCheck;
	vector<int> netsCompare;
	float xPosition, matrixValue=0;
	int stopValue = (int)fixed[0][0];
	for(int i=netsFile.size()-1; i>stopValue-1; i--){
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
		BX[i] = matrixValue;
		matrixValue = 0;
		netsToCheck.clear();
	}
	this -> BX = BX;
	cout << "BX: " << endl;
	for(int i=0; i<BX.size(); i++){
		cout << BX[i] << endl;
	}
	cout << endl;
	
}

void Objects::defineBforY(){
	vector<float> BY(netsFile.size(), 0);
	
	float fixedWeight;
	int block_ID;
	for(int i=0; i<fixedAmount(); i++){
		vector<float> current = getFixedObject(i);
		block_ID = (int)current[0];
		fixedWeight = setMatrixDiagonal(block_ID);
		BY[current[0]-1] = fixedWeight*current[2];
		current.clear();
	}
	this -> BY = BY;
	cout << "BY: " << endl;
	for(int i=0; i<BY.size(); i++){
		cout << BY[i] << endl;
	}
	cout << endl;
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
	double *result, *Axarr, *BXarr, *BYarr;
	int *Aparr, *Aiarr;
	result = new double [netsFile.size()];
	Aparr = new int [Q.size() + 1];
	Axarr = new double [nz];
	Aiarr = new int [nz];
	BXarr = new double [netsFile.size()];
	BYarr = new double [netsFile.size()];

	for(int i=0; i<netsFile.size(); i++){
		BXarr[i] = BX[i];
		BYarr[i] = BY[i];
	}
	// for(int i=0; i<netsFile.size(); i++){
	// 	cout << BXarr[i] << " ";
	// }
	// cout << endl << endl;
	// for(int i=0; i<netsFile.size(); i++){
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
	void *Symbolic, *Numeric ;
	(void) umfpack_di_symbolic (n, n, Aparr, Aiarr, Axarr, &Symbolic, null, null) ;
	(void) umfpack_di_numeric (Aparr, Aiarr, Axarr, Symbolic, &Numeric, null, null) ;
	umfpack_di_free_symbolic (&Symbolic) ;

	(void) umfpack_di_solve (UMFPACK_A, Aparr, Aiarr, Axarr, result, BXarr, Numeric, null, null) ;
	umfpack_di_free_numeric (&Numeric) ;
	for (i = 0 ; i < n ; i++) printf ("x [%d] = %g\n", i, result[i]) ;

	cout << endl << endl;

	// (void) umfpack_di_solve (UMFPACK_A, Aparr, Aiarr, Axarr, result, BYarr, Numeric, null, null) ;
	// umfpack_di_free_numeric (&Numeric) ;
	// for (i = 0 ; i < n ; i++) printf ("Y [%d] = %g\n", i, result[i]) ;

	
	

	this -> result = result;

	delete [] Aparr;
	delete [] Aiarr;
	delete [] Axarr;
	delete [] BXarr;
	delete [] BYarr;

}

#endif

