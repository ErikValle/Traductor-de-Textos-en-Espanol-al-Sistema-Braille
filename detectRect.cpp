#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "detectRect.h"
#include <highgui.h>

using namespace std; 

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int detectRec() {
    raspicam::RaspiCam_Cv Camera;//nuevo objeto cámara

    //Parámetros de cámara
    Camera.set(CV_CAP_PROP_FRAME_WIDTH,320);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    Camera.set(CV_CAP_PROP_BRIGHTNESS,50);
    Camera.set(CV_CAP_PROP_CONTRAST,50);
    Camera.set(CV_CAP_PROP_SATURATION,50);
    Camera.set(CV_CAP_PROP_GAIN,50);
    Camera.set(CV_CAP_PROP_FORMAT,CV_8UC3);
    Camera.set(CV_CAP_PROP_EXPOSURE,-1);
    Camera.set(CV_CAP_PROP_WHITE_BALANCE_RED_V,-1);
    Camera.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U,-1);
    
    //Abrir cámara
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    cv::namedWindow("Original");
    cv::namedWindow("Gray");
    cv::namedWindow("Edges");
    cv::Mat img,src;
    while(1){
	Camera.grab();
        Camera.retrieve ( img);
	cv::Mat src,src_gray,detected_edges;
	img.copyTo(src);
	cv::GaussianBlur(src, src, cv::Size(3,3),0,0,cv::BORDER_DEFAULT);//15,15,7
	cv::cvtColor(src,src_gray,CV_BGR2GRAY);
	/// Generate grad_x and grad_y
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	/// Gradiente X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	cv::Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_x, abs_grad_x );
	/// Gradiente Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	cv::Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
	cv::convertScaleAbs( grad_y, abs_grad_y );
	/// Gradiente total (aproximado)
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, detected_edges );
	// Encuentra contornos
	cv::imshow("Edges",detected_edges);
	cv::Canny(detected_edges, detected_edges, 10,30);//20,70
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(detected_edges,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);//RETR_CCOMP
	//Aproximar contornos con líneas
        std::vector<cv::Point> approx,approx2;
	std::vector<std::vector<cv::Point> > squares;
//**************************************************************
	for (size_t i = 0; i < contours.size(); i++)
        {
                cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);//true
		//obtener polígonos
                if (approx.size() == 4 &&
                        fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) //¿tiene 4 lados?
                {
			//std::cout<<"tamaño "<<approx.size()<<" \n";
			std::cout<<"approx "<<approx<<" \n";
			std::cout<<"area "<<fabs(cv::contourArea(cv::Mat(approx)))<<" \n";
                        double maxCosine = 0;
			//std::cout<<"pasa"<<"\n \n";
                        for (int j = 2; j < 5; j++)//obtener angulos entre vertices
                        {
                                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                                maxCosine = MAX(maxCosine, cosine);
				//std::cout<<maxCosine<<"\n";
                        }
                        if (maxCosine < 0.6){
                                squares.push_back(approx);
				hdet=1;
			}
		}
	}
//*********************************************
	//Dibujar rectángulo
	for ( int i = 0; i< squares.size(); i++ ) {
        // draw contour
        cv::drawContours(img, squares, i, cv::Scalar(0,255,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
    	}

	cv::imshow("Original",img);
	cv::imshow("Gray",src_gray);
	if (hdet==1){//¿Detectó hoja?	
		// guardar imagen
    		imwrite ("a.jpg", img);	
		break;
	}	
	if((cv::waitKey(3) & 255) == 27){break;}//¿presionaron el botón esc?
    }
    
    Camera.release();
    return 0;
}