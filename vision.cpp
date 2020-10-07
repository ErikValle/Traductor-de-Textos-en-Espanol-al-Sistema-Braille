#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc.hpp>
#include <highgui.h>
#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include <sstream>

// Umbrales
int lowThreshold;
int ratio = 1;//3,1
int kernel_size = 5;//3,5
int scale = 1;
int delta = 0;
int ddepth = CV_16S;
int c,rec,umbral=7;
int mx=0;
int my=0;
int start,hoja;
#define uget(x,y)    at<unsigned char>(y,x)
#define uset(x,y,v)  at<unsigned char>(y,x)=v;
#define fget(x,y)    at<float>(y,x)
#define fset(x,y,v)  at<float>(y,x)=v;

void sta_callback(const std_msgs::Int32::ConstPtr& msg1)
{
start=msg1->data;
}

using namespace std; 

double calcLocalStats (cv::Mat &im, cv::Mat &map_m, cv::Mat &map_s, int winx, int winy) {    
    cv::Mat im_sum, im_sum_sq;
    cv::integral(im,im_sum,im_sum_sq,CV_64F);

	double m,s,max_s,sum,sum_sq;	
	int wxh	= winx/2;
	int wyh	= winy/2;
	int x_firstth= wxh;
	int y_lastth = im.rows-wyh-1;
	int y_firstth= wyh;
	double winarea = winx*winy;

	max_s = 0;
	for	(int j = y_firstth ; j<=y_lastth; j++){   
		sum = sum_sq = 0;

        sum = im_sum.at<double>(j-wyh+winy,winx) - im_sum.at<double>(j-wyh,winx) - im_sum.at<double>(j-wyh+winy,0) + im_sum.at<double>(j-wyh,0);
        sum_sq = im_sum_sq.at<double>(j-wyh+winy,winx) - im_sum_sq.at<double>(j-wyh,winx) - im_sum_sq.at<double>(j-wyh+winy,0) + im_sum_sq.at<double>(j-wyh,0);

		m  = sum / winarea;
		s  = sqrt ((sum_sq - m*sum)/winarea);
		if (s > max_s) max_s = s;

		map_m.fset(x_firstth, j, m);
		map_s.fset(x_firstth, j, s);

		// Shift the window, add and remove	new/old values to the histogram
		for	(int i=1 ; i <= im.cols-winx; i++) {

			// Remove the left old column and add the right new column
			sum -= im_sum.at<double>(j-wyh+winy,i) - im_sum.at<double>(j-wyh,i) - im_sum.at<double>(j-wyh+winy,i-1) + im_sum.at<double>(j-wyh,i-1);
			sum += im_sum.at<double>(j-wyh+winy,i+winx) - im_sum.at<double>(j-wyh,i+winx) - im_sum.at<double>(j-wyh+winy,i+winx-1) + im_sum.at<double>(j-wyh,i+winx-1);

			sum_sq -= im_sum_sq.at<double>(j-wyh+winy,i) - im_sum_sq.at<double>(j-wyh,i) - im_sum_sq.at<double>(j-wyh+winy,i-1) + im_sum_sq.at<double>(j-wyh,i-1);
			sum_sq += im_sum_sq.at<double>(j-wyh+winy,i+winx) - im_sum_sq.at<double>(j-wyh,i+winx) - im_sum_sq.at<double>(j-wyh+winy,i+winx-1) + im_sum_sq.at<double>(j-wyh,i+winx-1);

			m  = sum / winarea;
			s  = sqrt ((sum_sq - m*sum)/winarea);
			if (s > max_s) max_s = s;

			map_m.fset(i+wxh, j, m);
			map_s.fset(i+wxh, j, s);
		}
	}

	return max_s;
}

//rutina de binarización
void NiblackSauvolaWolfJolion (cv::Mat im, cv::Mat output,int winx, int winy, double k, double dR) {

	double m, s, max_s;
	double th=0;
	double min_I, max_I;
	int wxh	= winx/2;
	int wyh	= winy/2;
	int x_firstth= wxh;
	int x_lastth = im.cols-wxh-1;
	int y_lastth = im.rows-wyh-1;
	int y_firstth= wyh;
	int mx, my;

	// Crea estadísticas locales y guárdalas en matrices dobles
	cv::Mat map_m = cv::Mat::zeros (im.rows, im.cols, CV_32F);
	cv::Mat map_s = cv::Mat::zeros (im.rows, im.cols, CV_32F);
	max_s = calcLocalStats (im, map_m, map_s, winx, winy);
	
	minMaxLoc(im, &min_I, &max_I);
			
	cv::Mat thsurf (im.rows, im.cols, CV_32F);
			
	// Umbralizar y obtener bordes
	// ----------------------------------------------------

	for	(int j = y_firstth ; j<=y_lastth; j++) {

		// NORMAL, área sin bordes en medio de la ventana. 
		for	(int i=0 ; i <= im.cols-winx; i++) {

			m  = map_m.fget(i+wxh, j);
    		s  = map_s.fget(i+wxh, j);
		
    		// Calcular umbral
    		th = m + k * (s/max_s-1) * (m-min_I);//wolf
    		
    		thsurf.fset(i+wxh,j,th);

    		if (i==0) {
        		// borde izquierdo
        		for (int i=0; i<=x_firstth; ++i)
                	thsurf.fset(i,j,th);

        		// esquina superior izquierda
        		if (j==y_firstth)
        			for (int u=0; u<y_firstth; ++u)
        			for (int i=0; i<=x_firstth; ++i)
        				thsurf.fset(i,u,th);

        		// esquina inferior izquierda
        		if (j==y_lastth)
        			for (int u=y_lastth+1; u<im.rows; ++u)
        			for (int i=0; i<=x_firstth; ++i)
        				thsurf.fset(i,u,th);
    		}

			// borde superior
			if (j==y_firstth)
				for (int u=0; u<y_firstth; ++u)
					thsurf.fset(i+wxh,u,th);

			// borde inferior
			if (j==y_lastth)
				for (int u=y_lastth+1; u<im.rows; ++u)
					thsurf.fset(i+wxh,u,th);
		}

		// borde derecho
		for (int i=x_lastth; i<im.cols; ++i)
        	thsurf.fset(i,j,th);

  		// esquina superior izquierda
		if (j==y_firstth)
			for (int u=0; u<y_firstth; ++u)
			for (int i=x_lastth; i<im.cols; ++i)
				thsurf.fset(i,u,th);

		// esquina inferior derecha
		if (j==y_lastth)
			for (int u=y_lastth+1; u<im.rows; ++u)
			for (int i=x_lastth; i<im.cols; ++i)
				thsurf.fset(i,u,th);
	}
	cerr << "superficie creada" << endl;
	
	
	for	(int y=0; y<im.rows; ++y) 
	for	(int x=0; x<im.cols; ++x) 
	{
    	if (im.uget(x,y) >= thsurf.fget(x,y))
    	{
    		output.uset(x,y,255);
    	}
    	else
    	{
    	    output.uset(x,y,0);
    	}
    }
}

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

std::vector<cv::Point> getChangePoints(const std::vector<cv::Point>& points) {
  std::vector<cv::Point> changePoints;
  cv::Point vec(points[0] - points[3]);
  double h = sqrt(vec.ddot(vec));

  changePoints.push_back(cv::Point(0, 0));
  changePoints.push_back(cv::Point(int(h * 210 / 297), 0));
  changePoints.push_back(cv::Point(int(h * 210 / 297), int(h)));
  changePoints.push_back(cv::Point(0, int(h)));

  return changePoints;
}

void sortPoints(std::vector<cv::Point>& points) {
  double distance[4];

  for (int i = 0; i < 4; ++i) {
	distance[i] = points[i].ddot(points[i]);
  }

  int index = 0;
  double mini = distance[0];

  for (int i = 0; i < 4; ++i) {
	if (distance[i] < mini) {
	  mini = distance[i];
	  index = i;
	}
  } 

  cv::Point temp(points[index]);
  sort(points.begin(), points.end(), [&](cv::Point a, cv::Point b) {
	return (pow(a.x - temp.x, 2) + pow(a.y - temp.y, 2)) < (pow(b.x - temp.x, 2) + pow(b.y - temp.y, 2));
  });

  temp = points[2];
  points[2] = points[3];
  points[3] = temp;

  if (points[1].y > points[3].y && points[1].x < points[3].x) {
    std::vector<cv::Point> vec;

	  // rotate points
	vec.push_back(points[1]);
	vec.push_back(points[0]);
	vec.push_back(points[3]);
	vec.push_back(points[2]);

	points.clear();
	points.assign(vec.begin(), vec.end());
  }
}

cv::Mat getChangePicture(cv::Mat src, int rows, int cols, cv::Mat matrix) {
  cv::Mat dst = cv::Mat(rows, cols, 0);

  for (int i = 0; i < dst.rows; ++i) {
	for (int j = 0; j < dst.cols; ++j) {
	  double x = matrix.at<double>(0, 0) * j + matrix.at<double>(0, 1) * i +
		matrix.at<double>(0, 2);
	  double y = matrix.at<double>(1, 0) * j + matrix.at<double>(1, 1) * i +
		matrix.at<double>(1, 2);

	  int a = int(x);
	  int b = int(y);
	  double u = x - a;
	  double v = y - b;


	  int pixel = int((1 - u) * (1 - v) * src.at<uchar>(b >= src.rows ? src.rows - 1 : b, a >= src.cols ? src.cols - 1 : a) +
		u * (1 - v) * src.at<uchar>(b >= src.rows ? src.rows - 1 : b, a + 1 >= src.cols ? src.cols - 1 : a + 1) +
		(1 - u) * v * src.at<uchar>(b + 1 >= src.rows ? src.rows - 1 : b + 1, a >= src.cols ? src.cols - 1 : a ) +
		u * v * src.at<uchar>(b + 1 >= src.rows ? src.rows - 1 : b + 1, a + 1 >= src.cols ? src.cols - 1 : a + 1));

	  dst.at<uchar>(i, j) = pixel;
	}
  }
  return dst;
}

int main ( int argc,char **argv ) {
    ros::init(argc, argv, "vision");
    ros::NodeHandle n;
    ros::Subscriber ini_v = n.subscribe("capturar", 1, sta_callback);
    ros::Publisher prin_p = n.advertise<std_msgs::Int32>("/escaneando",150);
    raspicam::RaspiCam_Cv Camera;
    std_msgs::Int32 msgsta;
    //set camera params
    Camera.set(CV_CAP_PROP_FRAME_WIDTH,640);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    Camera.set(CV_CAP_PROP_BRIGHTNESS,50);
    Camera.set(CV_CAP_PROP_CONTRAST,50);
    Camera.set(CV_CAP_PROP_SATURATION,50);
    Camera.set(CV_CAP_PROP_GAIN,50);
    Camera.set(CV_CAP_PROP_FORMAT,CV_8UC3);
    Camera.set(CV_CAP_PROP_EXPOSURE,-1);
    Camera.set(CV_CAP_PROP_WHITE_BALANCE_RED_V,-1);
    Camera.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U,-1);
    
    //Open camera
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    cv::namedWindow("Original");
    cv::namedWindow("Gray");
    cv::namedWindow("Edges");
    cv::namedWindow("Cut");
    cv::Mat img,src,src2;
    while(ros::ok()){
    std::cout<<"start "<<start<<"\n";
    if(start==0){ hoja=0; }
    if(start==1 && hoja!=2){
		hoja = 1;//buscando hoja
	Camera.grab();
        Camera.retrieve ( img);
	cv::Mat src,src_gray,detected_edges;
	img.copyTo(src);
	img.copyTo(src2);
	int p1=img.size().width;
	int p2=img.size().height;
	cv::GaussianBlur(src, src, cv::Size(3,3),0,0,cv::BORDER_DEFAULT);//15,15,7
	cv::cvtColor(src,src_gray,CV_BGR2GRAY);
	cv::cvtColor(src2,src2,CV_BGR2GRAY);
	/// Generate grad_x and grad_y
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	cv::Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x, abs_grad_x );
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	cv::Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_y, abs_grad_y );
	/// Total Gradient (approximate)
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, detected_edges );
	//Find contours
	cv::imshow("Edges",detected_edges);
	cv::Canny(detected_edges, detected_edges, 10,30);//20,70
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(detected_edges,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);//RETR_CCOMP
	//Approximate contours with linear features
	// Test contours
        
//************************************************************************************************************
	std::vector<cv::Point> approx;
	std::vector<int> areasqr;//vector de areas
	std::vector<std::vector<cv::Point> > squares;//vector de coordenadas
 	rec=0;//para evitar que se muera
        for (size_t i = 0; i < contours.size(); i++)
        {
                cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);//true
		
                if (approx.size() == 4 &&
                        fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx)))
                {
                        double maxCosine = 0;
			//std::cout<<"pasa"<<"\n \n";
                        for (int j = 2; j < 5; j++)
                        {
                                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                                maxCosine = MAX(maxCosine, cosine);
                        }
                        if (maxCosine < 0.6){
				if(rec==0){
					squares.push_back(approx);
					areasqr.push_back(cv::contourArea(cv::Mat(approx)));
					rec=1;
				}
				else{
					int rannx, ranpx,ranny,ranpy;
					//X ***********************************************
					if((squares[rec-1][0].x-umbral)>0 && (squares[rec-1][0].x+umbral)<p1){
						rannx=squares[rec-1][0].x-umbral;
						ranpx=squares[rec-1][0].x+umbral;
					}
					else if((squares[rec-1][0].x-umbral)<0 && (squares[rec-1][0].x+umbral)<p1){ //p1 is width
						rannx=0;
						ranpx=squares[rec-1][0].x+umbral;
					}
					else{
						rannx=squares[rec-1][0].x-umbral;
						ranpx=p1;
					}
					//Y ***********************************************
					if((squares[rec-1][0].y-umbral)>0 && (squares[rec-1][0].y+umbral)<p2){
						ranny=squares[rec-1][0].y-umbral;									
						ranpy=squares[rec-1][0].y+umbral;
					}
					else if((squares[rec-1][0].y-umbral)<0 && (squares[rec-1][0].y+umbral)<p2){ //p2 is length
						ranny=0;
						ranpy=squares[rec-1][0].y+umbral;
					}
					else{
						ranny=squares[rec-1][0].y-umbral;
						ranpy=p2;
					}
					// comparacion de rectangulos
					bool comp=false;
					for(int c=0;c<4;c++){
						bool co=((approx[c].x)>=rannx)&&((approx[c].x)<=ranpx)&&((approx[c].y)>=ranny)&&((approx[c].y)<=ranpy);					
						comp=comp||co;
					}
					if(comp==false)
					{
						squares.push_back(approx);
						areasqr.push_back(fabs(cv::contourArea(cv::Mat(approx))));
						rec=rec+1;
					}
				}
			}
		}
	}
	std::cout<<"cuadrados "<<squares.size()<<" areas " <<areasqr.size()<<"\n";
	if(squares.size()>0){
	for (int c = 0; c < squares.size(); c++)
		std::cout<<c<<" "<<squares[c]<<" area "<<areasqr[c]<<" \n";
	//std::cout<<"cuadrados "<<squares<<" \n";
	//DRAW RECTANGLE
	for ( int i = 0; i< squares.size(); i++ ) {
        // draw contour
        cv::drawContours(img, squares, i, cv::Scalar(0,255,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
    	}	
	//centroid
	int mic = areasqr[0];
	int n=areasqr.size();
	int miar=0;//area maxima
	for(int u=0;u<n;u++)
	    {
		std::cout<<"area for "<<areasqr[u]<<" inicial " <<areasqr[0]<<"\n";
		if(areasqr[u]>mic){
		mic=areasqr[u];
		miar=u;
		}
	    }
	std::cout<<"area mas grande "<<miar<<"\n";
	if(mic>28000){//areas mas grandes para descartar rectangulos dentro de la hoja
		// Crear un rectangulo para definir la region de interes
		//encontrar vertices mas alejados
		int v1x=squares[miar][0].x,v2x=squares[miar][0].x,vxa;
		int v1y=squares[miar][0].y,v2y=squares[miar][0].y,vya;
		for(int i=0; i<4;i++){
			vxa=squares[miar][i].x;
			vya=squares[miar][i].y;
			if(vxa<v1x){//menores
				v1x=vxa;
			}
			if(vya<v1y){
				v1y=vya;
			}
			if(vxa>v2x){//mayores
				v2x=vxa;
			}
			if(vya>v2y){
				v2y=vya;
			}	
			
		}
		std::cout<<"CUMPLE "<<miar<<"\n";
		std::cout<<v1x<<" "<<v1y<<" "<<v2x<<" "<<v2y<<"\n";

		cv::Mat transform = src2(cv::Rect(cv::Point(v1x,v1y),cv::Point(v2x,v2y)));
		
//******************************************************************************************
		//WOLF-JOLION
		char version;
		int c;
		int winx=0, winy=0;
		float optK=0.1;//0.5			
	        // Modificar tamaño de ventana (kernel)
		if (winx==0||winy==0) {
		    winy = (int) (2.0 * transform.rows-1)/3;
		    winx = (int) transform.cols-1 < winy ? transform.cols-1 : winy;
		    if (winx > 70)//100
		       winx = winy = 20;//60
		    cerr << "Definiendo tamaño de ventana a [" << winx<< "," << winy << "].\n";
		 }
	    // Umbral
	    cv::Mat output (transform.rows, transform.cols, CV_8U);
	    NiblackSauvolaWolfJolion (transform, output, winx, winy, optK, 128);
	    // guardar imagen
	    cv::imshow("Cut",output);
	    cv::imwrite ("a_w.jpg", output);
	    hoja=2;//hoja encontrada 
	}
	//centroide
	mx=0;
	my=0;
	for(int v=0;v<4;v++){
		mx=mx+squares[miar][v].x;
		my=my+squares[miar][v].y;
	}
	mx=mx/4;
	my=my/4;
	cv::circle(img,cv::Point(mx,my),3,cv::Scalar( 0, 0, 255 ),8,8,0);
	}
//************************************************************************************************************

	cv::imshow("Original",img);
	cv::imshow("Gray",src_gray);
	
	if((cv::waitKey(10) & 255) == 27){break;}
    }
	msgsta.data = hoja;
	prin_p.publish(msgsta);
	ros::spinOnce();
    }
    Camera.release();
    return 0;

}