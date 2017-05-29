#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "rotayCorta.h"

using namespace cv; 
using namespace std;


bool filterLines(const vector<PairPoint>& old, vector<PairPoint>& filter) {
  if (old.size() != 0) {
	for (int i = 0; i < old.size(); ++i) {
	  bool isSame = false;
	  for (int j = 0; j < filter.size(); ++j) {
		// cout << old[i].pt1 << ' ' << old[i].pt2 << ' ' << filter[j].pt1 << ' ' << filter[j].pt2 << endl;
		if (sqrt(pow(filter[j].pt1.x - old[i].pt1.x, 2) + pow(filter[j].pt1.y - old[i].pt1.y, 2)) < 300 &&
		  sqrt(pow(filter[j].pt2.x - old[i].pt2.x, 2) + pow(filter[j].pt2.y - old[i].pt2.y, 2)) < 300) {
			filter[j].pt1.x += old[i].pt1.x;
			filter[j].pt1.y += old[i].pt1.y;
			filter[j].pt2.x += old[i].pt2.x;
			filter[j].pt2.y += old[i].pt2.y;

			filter[j].pt1.x /= 2;
			filter[j].pt1.y /= 2;
			filter[j].pt2.x /= 2;
			filter[j].pt2.y /= 2;

			isSame = true;

			break;
		}
	  }

	  if (!isSame) {
		filter.push_back(old[i]);
	  }
	}

	if (filter.size() == 4) {
	  return true;
	} else {
	  vector<PairPoint> vec;

	  for (int i = 0; i < filter.size(); ++i) {
		for (int j = i + 1; j < filter.size(); ++j) {
		  Point p1(filter[i].pt1 - filter[i].pt2);
		  Point p2(filter[j].pt1 - filter[j].pt2);

		  if (abs(abs(p1.x) - abs(p2.x)) < 300 && abs(abs(p1.y) - abs(p2.y)) < 300) {
			vec.push_back(filter[i]);
			vec.push_back(filter[j]);
			break;
		  }
		}
	  }

	  filter.clear();
	  filter.assign(vec.begin(), vec.end());

	  if (filter.size() >= 4) {
		return true;
	  }

	  return false;
	}
  }

  return false;
}

void filterPoint(vector<Point>& points) {
  for (int i = 0; i < points.size(); ++i) {
	for (int j = i + 1; j < points.size(); ++j) {
	  Point vec1(points[i] - points[j]);

	  vector<Point>::iterator iter = points.begin();

	  while (iter != points.end()) {
		if (*iter != points[i] && *iter != points[j]) {
		  Point vec2(points[i] - *iter);

		  if (fabs(vec1.ddot(vec2) / sqrt(vec1.ddot(vec1) * vec2.ddot(vec2))) >= 0.99) {
			iter = points.erase(iter);
		  } else {
		    iter++;
		  }
		} else {
		  iter++;
		}
	  }
	}
  }
}

bool interactionPoint(const PairPoint& p1, const PairPoint& p2, Point& p) {
  Point vec1(p1.pt1 - p1.pt2);
  Point vec2(p2.pt1 - p2.pt2);

  if (!vec1.cross(vec2) == 0) {
	p = p1.pt1;
	double t = ((p1.pt1.x - p2.pt1.x) * (p2.pt1.y - p2.pt2.y) -
	  (p1.pt1.y - p2.pt1.y) * (p2.pt1.x - p2.pt2.x)) /
	  double(((p1.pt1.x - p1.pt2.x) * (p2.pt1.y - p2.pt2.y) -
	  (p1.pt1.y - p1.pt2.y) * (p2.pt1.x - p2.pt2.x)));

	p.x += int((p1.pt2.x - p1.pt1.x) * t);
	p.y += int((p1.pt2.y - p1.pt1.y) * t);

	if (p.x < 0 || p.x > 5000 || p.y < 0 || p.y > 5000) {
	  return false;
	}

	return true;
  }

  return false;
}

vector<Point> getChangePoints(const vector<Point>& points) {
  vector<Point> changePoints;
  Point vec(points[0] - points[3]);
  double h = sqrt(vec.ddot(vec));

  changePoints.push_back(Point(0, 0));
  changePoints.push_back(Point(int(h * 210 / 297), 0));
  changePoints.push_back(Point(int(h * 210 / 297), int(h)));
  changePoints.push_back(Point(0, int(h)));

  return changePoints;
}

Mat getChangePicture(Mat src, int rows, int cols, Mat matrix) {
  Mat dst = Mat(rows, cols, 0);

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

void sortPoints(vector<Point>& points) {
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

  Point temp(points[index]);
  sort(points.begin(), points.end(), [&](Point a, Point b) {
	return (pow(a.x - temp.x, 2) + pow(a.y - temp.y, 2)) < (pow(b.x - temp.x, 2) + pow(b.y - temp.y, 2));
  });

  temp = points[2];
  points[2] = points[3];
  points[3] = temp;

  if (points[1].y > points[3].y && points[1].x < points[3].x) {
    vector<Point> vec;

	  // rotate points
	vec.push_back(points[1]);
	vec.push_back(points[0]);
	vec.push_back(points[3]);
	vec.push_back(points[2]);

	points.clear();
	points.assign(vec.begin(), vec.end());
  }
}

int rotaCorta() {

	Mat img = imread("a.jpg", 0);//leer imagen

	Mat gaussian;
	Mat canny;

	GaussianBlur(img, gaussian, Size(15, 15), 7);
	Canny(gaussian, canny, 20, 70);

	vector<Vec2f> lines;
	vector<PairPoint> pairPoints;
	vector<PairPoint> filter;

	HoughLines(canny, lines, 1, CV_PI / 180, 240);

	for (size_t j = 0; j < lines.size(); ++j) {
	  double rho = lines[j][0];
	  double theta = lines[j][1];
	  Point pt1;
	  Point pt2;

	  if (theta < CV_PI / 4.0 || theta > 3 * CV_PI / 4.0) {
		pt1.x = int(rho / cos(theta));
		pt1.y = 0;
		pt2.x = int((rho - canny.size[0] * sin(theta)) / cos(theta));
		pt2.y = canny.size[0];
	  } else {
		pt1.x = 0;
		pt1.y = int(rho / sin(theta));
		pt2.x = canny.size[1];
		pt2.y = int((rho - canny.size[1] * cos(theta)) / sin(theta));
	  }

	  pairPoints.push_back(PairPoint(pt1, pt2));

	  // line(canny, pt1, pt2, Scalar(255, 0, 0), 3);
	}

	bool check = filterLines(pairPoints, filter);

	if (check) {
	  cout << "Yes" << endl;
	  // cout << j << ' ' << filter[j].pt1 << ' ' << filter[j].pt2 << endl;
	  vector<Point> points;
	  Point point;

	  for (int x = 0; x < filter.size(); ++x) {
		for (int y = x + 1; y < filter.size(); ++y) {
		  if (interactionPoint(filter[x], filter[y], point)) {
			bool isSame = false;

			for (int z = 0; z < points.size(); ++z) {
			  if (points[z] == point) {
				isSame = true;
				break;
			  }
			}

			if (!isSame) {
			  points.push_back(point);
			}
		  }
		}
	  }

	  if (points.size() >= 4) {
	    filterPoint(points);
	  }

	  sortPoints(points);
	  vector<Point> changePoints = getChangePoints(points);
	  Mat matrix = findHomography(changePoints, points);
	  Mat transform = getChangePicture(img, changePoints[3].y, changePoints[1].x, matrix);
	  imwrite("a1.jpg", transform, compression);
	} else {
	  cout << "No" << endl;
	}
  
  return 0;
}
