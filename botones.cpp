#include <string>
#include <iostream>
#include <wiringPi.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32.h"
#include <sstream>

//gpio readall
//gpio -g mode 2 out
int bSel[]={1,4,5,6};//siguiente, atras y ok.

using namespace std;

void conPines(void){
		wiringPiSetup () ;
		for(int pin=0;pin<3;pin++)//:3
			pinMode (bSel[pin], INPUT);
	}

int main(int argc, char* argv[]) {
	conPines();
	ros::init(argc, argv, "botones");
  	ros::NodeHandle n;
  	ros::Publisher btn_p = n.advertise<std_msgs::Int32>("/botones",1);
  	ros::Rate loop_rate(10);
  	while (ros::ok())
	{
		int menu=digitalRead (bSel[0]);
		int ok=digitalRead (bSel[1]);
		int sig=digitalRead (bSel[2]);
		int atr=digitalRead (bSel[3]);
		int est; //0 es presionado, 1 no presionado
		if(sig==1 && atr==0 && menu==1 && ok==1){//anterior
			cout<<"anterior"<<endl;
			est=4;
		}
		else if(sig==0 && atr==1 && menu==1 && ok==1){//siguiente
			cout<<"siguiente"<<endl;
			est=3;
		}
		else if(sig==1 && atr==1 && menu==0 && ok==1){ //menu
			cout<<"menu"<<endl;
			est=1;
		}
		else if(sig==1 && atr==1 && menu==1 && ok==0){ //okay
			cout<<"okay"<<endl;
			est=2;
		}
		else{//quieto
			est=0;
		}
		cout<<"menu "<<menu<<" ok "<<ok<<" sig "<<sig<<" atr "<<atr<<endl;
		std_msgs::Int32 msgbtn;
		msgbtn.data = est;//boton presionado
		btn_p.publish(msgbtn);
		delay (150);
  	}

	return 0;
}
