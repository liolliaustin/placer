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
#include "graphics.h"
//#include "drawscreen.h"
using namespace std;

void delay (void);
void drawscreen (void);
void act_on_new_button_func (void (*drawscreen_ptr) (void));
void act_on_button_press (float x, float y);
void act_on_mouse_move (float x, float y);
void act_on_key_press (char c);

vector<float> xPos;
vector<float> yPos;
float Num;

// For example showing entering lines and rubber banding
bool rubber_band_on = false;
bool have_entered_line, have_rubber_line;
bool line_entering_demo = false;
float x1, y3, x2, y4;  
int num_new_button_clicks = 0;

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

	//Graphics graph(net);

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
	// net.getN();
	// net.establishNetlist();
	// net.createClique();
	// net.defineMatrix();
	// net.defineBforX();
	// net.defineBforY();
	// net.UMFPACKIO();
	// net.computeLocation();
	// net.HPWL();
	net.runStep1();
	xPos = net.getxPositions();
	yPos = net.getyPositions();
	Num = net.returnN();
	printf ("About to start graphics.\n");
	init_graphics("Some Example Graphics", WHITE);

	/* still picture drawing allows user to zoom, etc. */
	// Set-up coordinates from (xl,ytop) = (0,0) to 
	// (xr,ybot) = (1000,1000)
	init_world (0.,0.,1000.,1000.);
	update_message("Interactive graphics example.");

	// Pass control to the window handling routine.  It will watch for user input
	// and redraw the screen / pan / zoom / etc. the graphics in response to user
	// input or windows being moved around the screen.  This is done by calling the
	// four callbacks below.  mouse movement and key press (keyboard) events aren't 
	// enabled by default, so they aren't on yet.
	event_loop(act_on_button_press, NULL, NULL, drawscreen);   

	/* animation section */
	clearscreen();
	update_message("Non-interactive (animation) graphics example.");
	setcolor (RED);
	setlinewidth(1);
	setlinestyle (DASHED);
	init_world (0.,0.,Num*100,Num*100);
	for (int i=0; i<50; i++) {
	  drawline ((float)i,(float)(10.*i),(float)(i+500.),(float)(10.*i+10.));
	  flushinput();
	  delay(); 
	}

	/* Draw an interactive still picture again.  I'm also creating one new button. */

	init_world (0.,0.,1000.,1000.);
	update_message("Interactive graphics #2. Click in graphics area to rubber band line.");
	create_button ("Window", "0 Clicks", act_on_new_button_func);

	// Enable mouse movement (not just button presses) and key board input.
	// The appropriate callbacks will be called by event_loop.
	set_keypress_input (true);
	set_mouse_move_input (true);
	line_entering_demo = true;

	// draw the screen once before calling event loop, so the picture is correct 
	// before we get user input.
	drawscreen(); 
	event_loop(act_on_button_press, act_on_mouse_move, act_on_key_press, drawscreen);

	close_graphics ();

	//drawGrid(net.getxPositions(), net.getyPositions(), net.returnN());



	return 0;
}

void drawscreen (void) {

/* redrawing routine for still pictures.  Graphics package calls  *
 * this routine to do redrawing after the user changes the window *
 * in any way.                                                    */
	set_draw_mode (DRAW_NORMAL);  // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();  /* Should precede drawing for all drawscreens */


	float bound = Num*100.;
	float div = bound/Num;
	float center = div/2;

	setfontsize (10);
	setlinestyle (SOLID);
	setlinewidth (1);
	setcolor (BLACK);
	drawrect (0,0,bound,bound);
	for(int i=0; i<Num; i++){
	  drawline (i*div, 0, i*div, bound);
	  drawline (0, i*div, bound, i*div);
	}
	setcolor(BLUE);
	//fillarc (center,center,20.,0.,360.);

	//float positions[5][2] = {{4.5, 0.5},{0.5, 0.5},{4.5, 4.5},{0.5, 4.5},{4.5, 2.5}};

	for(int i=0;i<xPos.size();i++){
		cout << i <<": " << xPos[i] <<", " << yPos[i] << endl;
		fillarc (xPos[i]*100,yPos[i]*100,20.,0.,360.);
		// fillarc (positions[i][0]*100,positions[i][1]*100,20.,0.,360.);

	}
}


void delay (void) {

/* A simple delay routine for animation. */

   int i, j, k, sum;

   sum = 0;
   for (i=0;i<100;i++) 
      for (j=0;j<i;j++)
         for (k=0;k<1000;k++) 
            sum = sum + i+j-k; 
}


void act_on_new_button_func (void (*drawscreen_ptr) (void)) {

   char old_button_name[200], new_button_name[200];
   printf ("You pressed the new button!\n");
   setcolor (MAGENTA);
   setfontsize (12);
   drawtext (500., 500., "You pressed the new button!", 10000.);
   sprintf (old_button_name, "%d Clicks", num_new_button_clicks);
   num_new_button_clicks++;
   sprintf (new_button_name, "%d Clicks", num_new_button_clicks);
   change_button_text (old_button_name, new_button_name);
}


void act_on_button_press (float x, float y) {

/* Called whenever event_loop gets a button press in the graphics *
 * area.  Allows the user to do whatever he/she wants with button *
 * clicks.                                                        */

   printf("User clicked a button at coordinates (%f, %f)\n", x, y);

   if (line_entering_demo) {
      if (rubber_band_on) {
         rubber_band_on = false;
         x2 = x;
         y4 = y;
         have_entered_line = true;  // both endpoints clicked on --> consider entered.

         // Redraw screen to show the new line.  Could do incrementally, but this is easier.
         drawscreen ();  
      }
      else {
         rubber_band_on = true;
         x1 = x;
         y3 = y;
         have_entered_line = false;
         have_rubber_line = false;
      }
   }

}



void act_on_mouse_move (float x, float y) {
	// function to handle mouse move event, the current mouse position in the current world coordinate
	// as defined as MAX_X and MAX_Y in init_world is returned

   printf ("Mouse move at (%f,%f)\n", x, y);
   if (rubber_band_on) {
      // Go into XOR mode.  Make sure we set the linestyle etc. for xor mode, since it is 
      // stored in different state than normal mode.
      set_draw_mode (DRAW_XOR); 
      setlinestyle (SOLID);
      setcolor (WHITE);
      setlinewidth (1);

      if (have_rubber_line) {
         // Erase old line.  
         drawline (x1, y3, x2, y4); 
      }
      have_rubber_line = true;
      x2 = x;
      y4 = y;
      drawline (x1, y3, x2, y4);  // Draw new line
   }
}


void act_on_key_press (char c) {
	// function to handle keyboard press event, the ASCII character is returned
   printf ("Key press: %c\n", c);
}