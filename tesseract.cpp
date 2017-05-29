#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <fstream>
#include "tesseract.h"

int imgatxt()
{
    char *outText;

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Inicializar Tesseract en españolsin especificar una dirección
    if (api->Init(NULL, "spa")) {
        fprintf(stderr, "No se puede iniciar tesseract.\n");
        exit(1);
    }

    // Abrir imagen con la libreria leptonica
    Pix *image = pixRead("a_w.jpg");
    api->SetImage(image);
    // Obtener resultado del OCR
    outText = api->GetUTF8Text();
    printf("OCR output:\n%s", outText);
    std::ofstream archivo_salida("out.txt"); // Se abre el archivo para escribir.
    archivo_salida<<outText;
    // Destructor y liberar memoria
    api->End();
    delete [] outText;
    pixDestroy(&image);
    archivo_salida.close();  // Se cierra el archivo
    return 0;
}
