#ifndef __CLASSES_INCLUDED__
#define __CLASSES_INCLUDED__
using namespace std;

class Objects{
private:
	vector<vector<float> > fixed;
	vector<vector<int> > nets;

public:
	float fixedX, fixedY, fixedBlockNum;

	void addFixedObject(vector<float> next){
		fixed.push_back(next);
	}
	
	int fixedAmount(){return fixed.size();}
	
	vector<float> getFixedObject(int x){
		return fixed[x];
	}

	void addNet(vector<int> add){
		nets.push_back(add);
	}
	
	int netsAmount(){return nets.size();}
	
	vector<int> getNextNet(int x){
		return nets[x];
	}

};

class currentNet{
private:
	int block_ID, block_type;
	vector<int> newNets;

public:
	currentNet(vector<int>& netIn){
		vector<int> newNets;
		this -> block_ID = netIn[0]; 
		this -> block_type = netIn[1];
		for(int i=2; i<netIn.size(); i++)
			newNets.push_back(netIn[i]);
		this -> newNets = newNets;
	}


};
















#endif