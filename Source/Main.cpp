#include <fstream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector> 
#include <queue>
#include <deque>
#include "Classes.h"
using namespace std;


class Utils {
private:
	Objects& net;

public:
	
	Utils(Objects &nets): net(nets){this-> net = net;}
	
	void get_configuration(char * file_name) {
		FILE *fp;
		char line[256];
		int stage =0, size, width;
		int negativeCount=0;
		fp = fopen(file_name, "r"); 
		vector<float> ports;
		vector<int> netPorts;

		if(fp){
			while(fgets(line, 256, fp) != NULL){

				char * value = strtok(line, " ");
				
				switch(negativeCount){
					case 0:
						while (value != NULL){
							if(atoi(value) == -1.0){
								if(netPorts.empty()){
									negativeCount ++;
									break;
								}
								else{
									net.addNet(netPorts);
									netPorts.clear();
									break;
								}
							}
							else	
								netPorts.push_back(atoi(value));
					
							value = strtok(NULL, " ");
						}
						break;

					case 1:
						while (value != NULL){
							if(atof(value) == -1.0){
								break;
							} else{
								ports.push_back(atof(value));
							}

							if(ports.size() == 3){
								net.addFixedObject(ports);
								ports.clear();
								break;
							} 
						
							value = strtok(NULL, " ");
							
						}
						break;
				
				}
	
			}
			
			fclose(fp);
			
		}
		else{
			cout << "Couldn't open file." << endl;
			exit(-1);
		}
	}
};


int main(int argc, char * argv[]) {
	Objects net;

	Utils * ut = new Utils(net);
	ut->get_configuration(argv[1]);

	// for(int i=0; i<net.blocksAmount(); i++){
	// 	vector<int> current = net.getNextNet(i);
	// 	for(int j=0; j<current.size(); j++){
	// 		cout << current[j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// for(int i=0; i<net.fixedAmount(); i++){
	// 	vector<float> current = net.getFixedObject(i);
	// 	for(int j=0; j<current.size(); j++){
	// 		cout << current[j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// cout << "The amount of blocks are: " << net.blocksAmount() << endl;

	// int maxnet = net.getMaxNet();
	// cout << endl << "The Largest Net is: " << maxnet;
	// cout << endl;

	net.establishNetlist();
	//net.outputNBB();
	net.createClique();
	//loat val;
	// for(int i=0; i<20; i++){
	// 	val = net.setMatrixDiagonal(i+1);
	// 	cout <<"Value " << i+1 <<": " << val << endl;
	// }
	net.defineMatrix();


	return 0;
}