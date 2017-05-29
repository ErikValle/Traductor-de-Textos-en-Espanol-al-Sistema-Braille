#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <glibmm/ustring.h>//para usar UTF8
#include <glibmm.h>
#include <wiringPi.h>
#include <vector>
#include "matriz.h"

//gpio readall
//gpio -g mode 2 out

using namespace std;
vector< vector<int> > vec(10000, vector<int>(60,0));//vector de almacenamiento general
vector<int> va;//vector de almacenamiento actual

void conPines(void){
		wiringPiSetup () ;
		for(int pin=0;pin<8;pin++)
			pinMode (pBraille[pin], OUTPUT);
		for(int pin=0;pin<8;pin++)
			pinMode (pMatsel[pin], OUTPUT);
		for(int pin=0;pin<3;pin++)//:3
			pinMode (bSel[pin], INPUT);
	}
void eBraille(int b1,int b2,int b3,int b4,int b5,int b6){//almacenar todas las lineas Braille
	int mBraille[6]={b1,b2,b3,b4,b5,b6};
	for(int e=0;e<6;e++){
		vec[eBl][eBc*6+e]=mBraille[e];
	}
}
void matriz(int est){//escribir en matriz
	if(est==1){
		w=w+1;//linea siguiente
	}
	else if(est==2){
		if(w>0)
			w=w-1;//linea anterior
	}
	else if(est==0){
		w=0;//nada
	}
	int ant,sieoc;
	for(int i=0;i<8;i++){//filas
		if(i<3){
			ant=i; sieoc=0;
		}
		else if((i>=3)&&(i<6)){
			ant=21+i; sieoc=0;
		}
		else if(i==6){
			sieoc=1;
		}
		else{
			sieoc=2;
		}
		if(sieoc==0){
			for(int j=0;j<8;j++){//columnas
				//digitalWrite (LED, HIGH) ; delay (50) ;
				digitalWrite (pMatsel[j],vec[w][ant]);
				ant=ant+3;
			}
		}
		else if(sieoc==1){
			int ant6[]={51,52,53,57,58,59};
			for(int j=0;j<6;j++){//columnas
				//digitalWrite (LED, HIGH) ; delay (50) ;
				digitalWrite (pMatsel[j],vec[w][ant6[j]]);
			}
			digitalWrite (pMatsel[6],LOW);
			digitalWrite (pMatsel[7],LOW);
		}
		else{
			int ant6[]={48,49,50,54,55,56};
			for(int j=0;j<6;j++){//columnas
				//digitalWrite (LED, HIGH) ; delay (50) ;
				digitalWrite (pMatsel[j],vec[w][ant6[j]]);
			}
			digitalWrite (pMatsel[6],LOW);
			digitalWrite (pMatsel[7],LOW);
		}
		for(int k=0;k<8;k++){//filas
				if(k==i)
					digitalWrite (pBraille[k], LOW);
				else
					digitalWrite (pBraille[k], HIGH);
			}
		delay (10);
	}
}

void sBraille(){//leer botones
	int est,w1;
	while(w1<eBl){
		int sig=digitalRead (bSel[0]);
		int atr=digitalRead (bSel[1]);
		if(sig==1 && atr==0){//siguiente
			cout<<"siguiente"<<endl;
			est=1;
			w1=w1+1;
		}
		else if(sig==0 && atr==1){//anterior
			cout<<"atras"<<endl;
			est=2;
			if(w1>0)
				w1=w1-1;
		}
		else if(sig==0 && atr==0){
			cout<<"ambos"<<endl;
			break;
		}
		else{//quieto
			est=3;
		}
		matriz(est);
		//delay (10);
	}
//digitalWrite (LED,  LOW) ; delay (50) ;
}

void toBraille(Glib::ustring matriz){
	cout<<matriz.raw()<<endl;
	for (int i=0; i<(matriz.length());i++){
		//int m=matriz[i];
		eBc=i;
		switch(matriz[i]){ //97-122 de a-z
			case 32: //espacio @ 
				{
				eBraille(0,0,0,0,0,0);
				break;
				}
			case 33: //! @ 235
				{
				eBraille(0,1,1,0,1,0);
				break;
				}
			case 34: //" @ 236
				{
				eBraille(0,1,1,0,0,1);
				break;
				}
			case 36: //$ @ 456-15
				{
				eBraille(0,0,0,1,1,1);
				break;
				}
			case 37: //% @ 456-356
				{
				eBraille(0,0,0,1,1,1);
				break;
				}
			case 38: //& @ 6-123456
				{
				eBraille(0,0,0,0,0,1);
				break;
				}
			case 40: //( @ 126
				{
				eBraille(1,1,0,0,0,1);
				break;
				}
			case 41: //) @ 345
				{
				eBraille(0,0,1,1,1,0);
				break;
				}
			case 42: //* @ 35 
				{
				eBraille(0,0,1,0,1,0);
				break;
				}
			case 43: //+ @ 235
				{
				eBraille(0,1,1,0,1,0);
				break;
				}
			case 44: //, @ 2
				{
				eBraille(0,1,0,0,0,0);
				break;
				}
			case 45: //- @ 36
				{
				eBraille(0,0,1,0,0,1);
				break;
				}
			case 46: //. @ 3
				{
				eBraille(0,0,1,0,0,0);
				break;
				}
			case 47: /// @ 6-2
				{
				eBraille(0,0,0,0,0,1);
				break;
				}
			case 48: //0 @ 
				{
				eBraille(0,1,0,1,1,0);
				break;
				}
			case 49: //1 @ 1
				{
				eBraille(1,0,0,0,0,0);
				break;
				}
			case 50: //2 @ 12
				{
				eBraille(1,1,0,0,0,0);
				break;
				}
			case 51: //3 @ 14
				{
				eBraille(1,0,0,1,0,0);
				break;
				}
			case 52: //4 @ 145
				{
				eBraille(1,0,0,1,1,0);
				break;
				}
			case 53: //5 @ 15
				{
				eBraille(1,0,0,0,1,0);
				break;
				}
			case 54: //6 @ 124
				{
				eBraille(1,1,0,1,0,0);
				break;
				}
			case 55: //7 @ 1245
				{
				eBraille(1,1,0,1,1,0);
				break;
				}
			case 56: //8 @ 125
				{
				eBraille(1,1,0,0,1,0);
				break;
				}
			case 57: //9 @ 24
				{
				eBraille(0,1,0,1,0,0);
				break;
				}
			case 58: //: @ 25
				{
				eBraille(0,1,0,0,1,0);
				break;
				}
			case 59: //; @ 23
				{
				eBraille(0,1,1,0,0,0);
				break;
				}
			case 60: //< @ 5-13
				{
				eBraille(0,0,0,0,1,0);
				break;
				}
			case 61: //= @ 2356
				{
				eBraille(0,1,1,0,1,1);
				break;
				}
			case 62: //> @ 46-2
				{
				eBraille(0,0,0,1,0,1);
				break;
				}
			case 63: //? @ 26
				{
				eBraille(0,1,0,0,0,1);
				break;
				}
			case 64: //@ @ 5
				{
				eBraille(0,0,0,0,1,0);
				break;
				}
			case 97: //a @ 1
				{
				eBraille(1,0,0,0,0,0);
				break;
				}
			case 98: //b @ 12
				{
				eBraille(1,1,0,0,0,0);
				break;
				}
			case 99: //c @ 14
				{
				eBraille(1,0,0,1,0,0);
				break;
				}
			case 100: //d @ 145
				{
				eBraille(1,0,0,1,1,0);
				break;
				}
			case 101: //e @ 15
				{
				eBraille(1,0,0,0,1,0);
				break;
				}
			case 102: //f @ 124
				{
				eBraille(1,1,0,1,0,0);
				break;
				}
			case 103: //g @ 1245
				{
				eBraille(1,1,0,1,1,0);
				break;
				}
			case 104: //h @ 125
				{
				eBraille(1,1,0,0,1,0);
				break;
				}
			case 105: //i @ 24
				{
				eBraille(0,1,0,1,0,0);
				break;
				}
			case 106: //j @ 245
				{
				eBraille(0,1,0,1,1,0);
				break;
				}
			case 107: //k @ 13
				{
				eBraille(1,0,1,0,0,0);
				break;
				}
			case 108: //l @ 123
				{
				eBraille(1,1,1,0,0,0);
				break;
				}
			case 109: //m @ 134
				{
				eBraille(1,0,1,1,0,0);
				break;
				}
			case 110: //n @ 1345
				{
				eBraille(1,0,1,1,1,0);
				break;
				}
			case 111: //o @ 135
				{
				eBraille(1,0,1,0,1,0);
				break;
				}
			case 112: //p @ 1234
				{
				eBraille(1,1,1,1,0,0);
				break;
				}
			case 113: //q @ 12345
				{
				eBraille(1,1,1,1,1,0);
				break;
				}
			case 114: //r @ 1235
				{
				eBraille(1,1,1,0,1,0);
				break;
				}
			case 115: //s @ 234
				{
				eBraille(0,1,1,1,0,0);
				break;
				}
			case 116: //t @ 2345
				{
				eBraille(0,1,1,1,1,0);
				break;
				}
			case 117: //u @ 136
				{
				eBraille(1,0,1,0,0,1);
				break;
				}
			case 118: //v @ 1236
				{
				eBraille(1,1,1,0,0,1);
				break;
				}
			case 119: //w @ 2456
				{
				eBraille(0,1,0,1,1,1);
				break;
				}
			case 120: //x @ 1346
				{
				eBraille(1,0,1,1,0,1);
				break;
				}
			case 121: //y @ 12456
				{
				eBraille(1,1,0,1,1,1);
				break;
				}
			case 122: //z @ 1356
				{
				eBraille(1,0,1,0,1,1);
				break;
				}
			case 161: //¡ @ 26
				{
				eBraille(0,1,0,0,0,1);
				break;
				}
			case 168: //" @ 236
				{
				eBraille(0,1,1,0,0,1);
				break;
				}
			case 170: //ª @ 
				{
				eBraille(1,0,0,0,0,0);
				break;
				}
			case 176: //° @ 356
				{
				eBraille(0,0,1,0,1,1);
				break;
				}
			case 186: //º @ 135
				{
				eBraille(1,0,1,0,1,0);
				break;
				}
			case 191: //¿ @ 26
				{
				eBraille(0,1,0,0,0,1);
				break;
				}
			case 225: //á @ 12356
				{
				eBraille(1,1,1,0,1,1);
				break;
				}
			case 233: //é @ 123456
				{
				eBraille(1,1,1,1,1,1);
				break;
				}
			case 237: //í @ 34
				{
				eBraille(0,0,1,1,0,0);
				break;
				}
			case 241: //ñ @ 12456
				{ 
				eBraille(1,1,0,1,1,1);
				break;
				}
			case 243: //ó @ 346
				{
				eBraille(0,0,1,1,0,1);
				break;
				}
			case 250: //ú @ 23456
				{
				eBraille(0,1,1,1,1,1);
				break;
				}
			default: //
				{
				eBraille(0,1,1,1,1,1);
				break;
				}
		}
/*
		for(int x=0;x<(matriz.length());x++){//barrido de seleccion
			int pM = pMatsel[x];
			int edo;
			if(x==i){
				edo=1;
			}
			else{
				edo=0;
			}
			digitalWrite(pM,edo);
		}
*/
		//delay(100);//ms
	}
	eBl=eBl+1;
}

int txtabraille() {
	conPines();
	string STRING;
	ifstream infile;
	infile.open("out.txt");
	
        while(getline(infile,STRING)) // para obtener todas las lineas
        {
		int a1= Glib::ustring(STRING).length();//cantidad de caracteres
		Glib::ustring str1= STRING;
		Glib::ustring STRING1 = str1.lowercase();//convertir a minusculas
		int a=STRING.size();		
		cout << a1 <<" "<<STRING<<" "<<endl;
		Glib::ustring s10;//buffer de 10 caracteres
		if(a1>10){
			int c=(int)a1/10;//decenas
			int d = a1 % 10;//unidades
			for (int i=0; i<c;i++){
				if(i==0){
					s10=STRING1.substr(0,10);//obtener 10 caracteres comenzando desde posicion 0
					toBraille(s10);
					//bool bo=s10[0]==105;
					//cout<<bo<<endl;
				}
				else{
					int sub=10*(i);
					s10=STRING1.substr(sub,10);
					toBraille(s10);
				}
			}
			if(d>0){
				s10=STRING1.substr(c*10,d);
				string spa="";
				int rellena=10-d;
				for(int r=0;r<rellena;r++){
					spa+=" ";
				}
				s10+=spa;
				toBraille(s10);
			}
		}
		else{
			s10=STRING1.substr(0,10);
			string spa="";
			int rellena=10-a1;
			for(int r=0;r<rellena;r++){
				spa+=" ";
			}
			s10+=spa;
			toBraille(s10);
		}

        }
	sBraille();//ESCRIBE EN LA MATRIZ
	//cout<<vec[1][6]<<" "<<vec[1][7]<<" "<<vec[1][8]<<" "<<vec[1][9]<<" "<<vec[1][10]<<" "<<vec[1][11]<<" "<<endl;
	infile.close();
	return 0;
}
