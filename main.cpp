#include"detectRect.h"
#include"rotayCorta.h"
#include"bina_w.h"
#include"tesseract.h"
#include"matriz.h"

#include<algorithm>
using namespace std;

int main() {
	detectRec();//Detectar hoja
	rotaCorta();//Recortar y rotar imagen
	binarizar();//binarizar con Wolf-Jolion
	imgatxt();//tesseract para OCR
	textabraille();//Escribir en matriz
}