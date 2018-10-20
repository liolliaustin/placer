#ifndef __CLASSES_INCLUDED__
#define __CLASSES_INCLUDED__
using namespace std;

class Objects{
private:
	vector<vector<float> > fixed;
	vector<vector<int> > netsFile;
	int maxnet = 0; 
	vector<vector<int> > netsBtwnBlocks;

public:
	float fixedX, fixedY, fixedBlockNum;

	void addFixedObject(vector<float> next){fixed.push_back(next);}

	void outputNBB(){
		for(int i=1; i<maxnet+1; i++){
			cout << "Net " << i << ": ";
			for(int j=0; j<netsBtwnBlocks[i].size(); j++){
				cout << netsBtwnBlocks[i][j] << " ";
			}
			cout << endl;
		}
	}
	
	int fixedAmount(){return fixed.size();}
	
	vector<float> getFixedObject(int x){
		return fixed[x];
	}

	void addNet(vector<int> add){
		for (int i=0; i<add.size(); i++){
			if (add[i] > maxnet)
				maxnet = add[i];
		}
		netsFile.push_back(add);
	}

	int getMaxNet(){return maxnet;}
	int netsAmount(){return netsFile.size();}
	
	vector<int> getNextNet(int x){return netsFile[x];}

	void establishNetlist();
	vector<int> getNetSet(int x){return netsBtwnBlocks[x];}

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

class Clique{
private:
	int block_ID, edgeWeight, p, edgeNum;
	vector<int> verticies;
	vector<vector<int> > Edges;

public:
	Clique(vector<int>& netIn){
		p=netIn.size();
		edgeNum = (p*(p-1))/2;
		edgeWeight= (2/p);
		this -> verticies = netIn;
		vector<vector<int> > Edges(edgeNum, vector<int>());
		this -> Edges = Edges;
	}

	void printVerticies(){
		cout << "Verticies: ";
		for(int i=0; i<verticies.size(); i++){
			cout << verticies[i] << " ";
		}
		cout << endl;
	}

	void createEdges();
	void outputEdges();


};

//somehow recursively create edges for given list of blocks attached to net
void Clique::createEdges(){
	int location = 0;
	int connected = 1;
	for(int i=0; i<edgeNum; i++){
		Edges[i].push_back(verticies[location]);
		Edges[i].push_back(verticies[connected]);
		if(connected == verticies.size() - 1){
			location++;
			connected = location + 1;
		}
		else
			connected++;
		
	}
}

void Clique::outputEdges(){
	for(int i=0; i<edgeNum; i++){
		int current = Edges[i].size();
		cout << "Edge " << i << ": ";
		for(int j=0; j<current; j++){
			cout << Edges[i][j] << " ";
		}
		cout << endl;
	}
}


struct Edges{
	int *p1, *p2;
	Edges(int edgeWeight){

		
	}
};
















#endif