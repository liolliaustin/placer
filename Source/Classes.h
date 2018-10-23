#ifndef __CLASSES_INCLUDED__
#define __CLASSES_INCLUDED__
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

public:
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

	for(int i=0; i<p.size(); i++){
		createEdges(i, netsBtwnBlocks[i+1]);
	}

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
	bool contains = false;
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
	this -> Q = Q;

	for(int i=0; i<Q.size(); i++){
		Q[i][i] = setMatrixDiagonal(i+1);
	}

	for(int i=0; i<Q.size(); i++){
		for(int j=0; j<Q.size(); j++){
			if(j!=i){
				float QValue = setRestofMatrix(j+1,i+1);
				Q[j][i] = QValue;
				Q[i][j] = QValue;
			}
		}
	}
	// for(int i=0; i<Q.size(); i++){
	// 	for(int j=0; j<Q.size(); j++){
	// 		cout << Q[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
}

// class QMatrix{
// private:
// 	int size;
// 	vector<vector<float> >  Q;
// public:
// 	QMatrix(int size){
// 		this -> size = size;
// 		vector<vector<float> >  Q(size, vector<float>(size, 0));
// 		this -> Q = Q;
// 	}
// 	void setMatrixDiagonal(int block_ID);

// };





#endif










































