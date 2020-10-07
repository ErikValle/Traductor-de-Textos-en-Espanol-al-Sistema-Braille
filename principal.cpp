#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32.h"
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h> 
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <sys/statvfs.h>
struct statvfs diskData;
std::vector<std::string> v;

int inf,btn,cap,opcion,menu,lee_opc;
int vo=80,vo1=1;

void inf_callback(const std_msgs::Int32::ConstPtr& msg1)
{
inf=msg1->data;
}

void btn_callback(const std_msgs::Int32::ConstPtr& msg2)
{
btn=msg2->data;
}

void cap_callback(const std_msgs::Int32::ConstPtr& msg3)
{
cap=msg3->data;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "principal");
  ros::NodeHandle n;
  //ros::Publisher linea_matp = n.advertise<std_msgs::String>("lineaA", 1000);
  ros::Publisher vel_m = n.advertise<std_msgs::Int32>("/vel",1);
  ros::Publisher capturar_p = n.advertise<std_msgs::Int32>("/capturar",1);
  ros::Publisher dato_p = n.advertise<std_msgs::Int32>("/escM",1);
  ros::Subscriber bot = n.subscribe("botones", 150, btn_callback);
  ros::Subscriber scn = n.subscribe("escaneando", 150, cap_callback);
  ros::Rate loop_rate(10);
  DIR           *d;
  struct dirent *dir;
  d = opendir("./documentos");
  int count = 0,fo=0,actar=5,sela=0,archivos=0,indar=0,inda=0;
  int oo=0, actop=0,selop=0,indop=0,indo=0;
  int vis=0;
  int datom=1;
  //system("rostopic list");
  system("echo \"Traductor de textos en espaniol al sistema braille\" | festival --tts");
  std_msgs::Int32 msgscn;
  std_msgs::Int32 msgbra;
  msgscn.data = 0;//inicia
  capturar_p.publish(msgscn);
  //ESCRIBIR ANTERIOR

  while (ros::ok())
  {
	msgscn.data = vis;//inicia
	capturar_p.publish(msgscn);
	if(cap==0){
	//CHECAR BOTONES
	if(btn==1){//menu
		if(menu==1){
			menu=0; 
			opcion=0;
			vo1=1;vis=0; datom=0;
			fo=0;actar=5;sela=0;indar=0,inda=0;
			oo=0;actop=5;selop=0;indop=0,indo=0;
			}
		else{
			menu=1;
			system("echo \"Menu principal\" | festival --tts");
			datom=2;
		}
	}
	else if(btn==2){//ok
		if(menu==1)
		{
			if(opcion==0)
			{
				menu=2;
			}
			else if(opcion==1)
			{	
				menu=3;
			}
			else if(opcion==2)
			{
				menu=4;
			}
			else if(opcion==3)
			{
				menu=5;
			}
		}
		else if(menu==3){//1 abrir, 2 guardar, 3 cerrar
			if(sela==0){
				if(fo==0){
					while ((dir = readdir(d)) != NULL)
	    				{
						if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0 ){//eliminar puntitos
							printf ("%s\n", dir->d_name);
							v.push_back(dir->d_name);
						}
	    				}
					archivos=v.size();
					sela=1;
					datom=10;
					system("echo \"Presione los botones adelante o atras para seleccionar archivo\" | festival --tts");
				}
				else if(fo==1){//guardar
					sela=2;
					datom=11;
					//cp out.txt out1.txt
					system("cp out.txt out1.txt");
					system("echo \"Guardado exitosamente\" | festival --tts");
					std::cout<<"Guardado exitosamente \n";
					menu=0;
				}
				else if(fo==2){//cerrar
					sela=3;menu=0;datom=11;
					system("> out.txt");
					system("echo \"Archivo cerrado\" | festival --tts");
				}
			}
			else if(sela==1){
				std::cout<<"Convirtiendo... \n";
				if((v[inda].back()=='f')||(v[inda].back()=='F')){
					system(("pdftotext ./documentos/"+v[inda]+" out.txt").c_str());	
				}
				if((v[inda].back()=='x')||(v[inda].back()=='X')){
					system(("docx2txt ./documentos/"+v[inda]+" out.txt").c_str());	
				}
				system("echo \"Cargado exitosamente\" | festival --tts");
				std::cout<<"Cargado exitosamente \n";
				menu=0;sela=0;fo=0;datom=0;
			}
		}
		else if(menu==5){//1 velocidad, 2 almacenamiento, 3 about
			if(selop==0){
				if(oo==0){
					selop=1;datom=1;
					system("echo \"Presione los botones adelante o atras para cambiar velocidad\" | festival --tts");
				}
				else if(oo==1){//almacenamiento
					selop=2;datom=14;
					statvfs("/home", &diskData);
    				unsigned long available = (diskData.f_bavail * diskData.f_bsize) / (1024 * 1024* 1024);
    				system(("echo \" almacenamiento disponible "+ std::to_string(available) +" gigabytes \" | festival --tts").c_str());
					system("echo \" Total 14.2 gigabytes \" | festival --tts");
					menu=0;
				}
				else if(oo==2){//about
					selop=3;menu=0;datom=62;
					system("echo \"Para recibir el titulo de ingeniero en mecatronica presentan los alumnos Misael Alarcon, Anayeli Cervantes, Alejandra Falcon y Erik Valle\" | festival --tts");
				}
			}
			else if(selop==1){
				system("echo \"velocidad configurada exitosamente\" | festival --tts");
				//publicar mensaje en ros
				std::cout<<"velocidad configurada exitosamente \n";
				menu=0;selop=0;oo=0;datom=11;
			}
		}
		opcion=0;
	}
	else if(btn==3){//siguiente
		lee_opc=0;
		opcion=opcion+1;
		if(opcion>3)
			opcion=0;
		if(menu==3){//archivos
			fo=fo+1;
			if(fo>2)
				fo=0;
			if(sela==1){
				system(("echo \""+v[indar]+"\" | festival --tts").c_str());
				inda=indar;
				indar=indar+1;
				if(indar>=v.size())
					indar=0;
			}
		}
		else if(menu==5){//archivos
			oo=oo+1;
			if(oo>2)
				oo=0;
			if(selop==1){
				system(("echo \" velocidad "+std::to_string(indop)+"\" | festival --tts").c_str());
				indo=indop;
				indop=indop+1;
				if(indop>=5)
					indop=0;
			}
		}
	}
	else if(btn==4){//anterior
		lee_opc=0;
		if(opcion>0)
			opcion=opcion-1;
		else
			opcion=3;
		if(menu==3){
			fo=fo-1;
			if(fo<0)
				fo=2;
			if(sela==1){
				system(("echo \""+v[indar]+"\" | festival --tts").c_str());
				inda=indar;
				indar=indar-1;
				if(indar<0)
					indar=v.size()-1;
			}
		}
		else if(menu==5){
			oo=oo-1;
			if(oo<0)
				oo=2;
			if(selop==1){
				system(("echo \" velocidad "+std::to_string(indop)+"\" | festival --tts").c_str());
				indo=indop;
				indop=indop-1;
				if(indop<0)
					indop=4;
			}
		}
	}
	else{}//nada
	if(menu==0){//cambio de linea
		menu=0; 
		opcion=0;
		vo1=1;
		fo=0;actar=5;sela=0;indar=0,inda=0;
		oo=0;actop=5;selop=0;indop=0,indo=0;
	}
	else if(menu==1){//despliega menu
		if(lee_opc==0){
			if(opcion==0){
				system("echo \"iniciar captura\" | festival --tts");
				datom=3;
				//system("echo \"por favor coloque la camara al centro del papel\" | festival --tts");
			}
			else if(opcion==1){
				system("echo \"archivo\" | festival --tts");
				datom=4;
			}
			else if(opcion==2){
				system("echo \"volumen\" | festival --tts");
				datom=5;
			}
			else if(opcion==3){
				system("echo \"opciones\" | festival --tts");
				datom=6;
			}
			lee_opc=1;
		}
	}
	else if(menu==2){//iniciar captura
		datom=31;
		system("echo \"Iniciando captura\" | festival --tts");
		system("echo \"Coloque el dispositivo paralelo y al centro de la hoja\" | festival --tts");
		std_msgs::Int32 msgscn;
		vis=1;
		menu=0;
	}
	else if(menu==3){//explorar archivos
		std::cout<<actar<<" fo "<<fo<<" sela "<<sela<<"\n";
		if(fo!=actar && sela==0){
			if(fo==0){//abrir 
				system("echo \"abrir\" | festival --tts");
				datom=41;
			}
			else if(fo==1){//guardar 
				system("echo \"guardar\" | festival --tts");
				datom=42;
			}
			else if(fo==2){//cerrar 
				system("echo \"cerrar\" | festival --tts");
				datom=43;
			}
			actar=fo;
		}
		opcion=0;
		//abrir y guardar
	}
	else if(menu==4){//volumen
		if(vo1==1){
			system("echo \"Presione los botones adelante o atras para cambiar volumen\" | festival --tts");
			btn=0;
			vo1=0;datom=10;
		}
		if(btn==3){//incrementar
			vo=vo+5;
			vo1=2;
			if(vo>100)
				vo=100;
		}
		else if(btn==4){//decrementar
			vo=vo-5;
			vo1=2;
			if(vo<0)
				vo=0;
		}
		if(vo1==2){
			std::string vol = std::to_string(vo);
			std::cout<<"opcion "<<vol<<std::endl;
			system(("amixer cset numid=1 "+vol+"%").c_str());
			system("aplay /usr/share/scratch/Media/Sounds/Instruments/Chord.wav");
			vo1=0;		
		}
		opcion=0;
	}
	else if(menu==5){//opciones
	std::cout<<actop<<" oo "<<oo<<" selop "<<selop<<"\n";
		if(oo!=actop && selop==0){
			if(oo==0){//velocidad 
				system("echo \"velocidad\" | festival --tts");
				datom=61;
			}
			else if(oo==1){//almacenamiento 
				system("echo \"almacenamiento\" | festival --tts");
				datom=63;
			}
			else if(oo==2){//about 
				system("echo \"acerca de nosotros\" | festival --tts");
				datom=62;
			}
			actop=oo;
		}
		opcion=0;

	}
    //++count;
  }
  else if(cap==1){
  	std::cout<<"Buscando hoja... \n";
	datom=32;
  }
  else if(cap==2){
	vis = 0;
	system("tesseract a_w.jpg out");
	system("echo \"captura terminada\" | festival --tts");
	datom=11;
  }
    msgbra.data = datom;//escribe en matriz
    dato_p.publish(msgbra);
    ros::spinOnce();
    loop_rate.sleep();
  }

  return 0;
}
