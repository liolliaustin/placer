#ifndef __CLASSES_INCLUDED__
#define __CLASSES_INCLUDED__
using namespace std;

class Objects{
private:
	vector<vector<float> > fixed;
	vector<vector<int> > nets;

public:
	float fixedX, fixedY, fixedBlockNum;
	int block_ID, block_type;

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

public:
	currentNet(block_ID, block_type){this -> block_ID = block_ID; this -> block_type = block_type;}


};
















#endif