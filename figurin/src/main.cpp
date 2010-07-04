
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <fann.h>
#include <cvblob.h>

using namespace std;
using namespace cvb;

#define AREA_MAX 50000
#define AREA_MIN 250
#define MAX_VERTICES 100.0f
#define ENTRADAS_RED 3
#define CONST_SIMP 3.0f

//funcion que calcula el cuadrado de la maxima distancia al centroide de un poligono
double getMaxDistAlVaricentro(const double & xc, const double & yc, const CvContourPolygon & poly) {
    double max =0.0f;
    std::vector<CvPoint>::const_iterator it = poly.begin();
    while (it!=poly.end()) {
        double x1 = (*it).x-xc;
        double y1 = (*it).y-yc;
        double sqNorma = x1*x1+y1*y1;
        if (sqNorma>max) max=sqNorma;
        it++;
    }
    return max;
}

//funcion que calcula el promedio del cuadrado de la distancia al centroide de un poligono
double getPromedioSqDistancias(const double & xc, const double & yc, const CvContourPolygon & poly) {

    double suma=0;
    std::vector<CvPoint>::const_iterator it = poly.begin();
    while (it!=poly.end()) {
        double x1 = (*it).x-xc;
        double y1 = (*it).y-yc;
        double sqNorma = x1*x1+y1*y1;
        suma+=sqNorma;
        it++;
    }
    return suma/poly.size();
}


//funcion que devuelve el nombre de la figura que corresponde a la salida de la red neuronal
string getNombreFigura(int tipo) {
	switch (tipo){
		case 1: return "Cuadrado";
		case 2: return "Triangulo";
		case 3: return "Circulo";
		case 4: return "Rectangulo";
		case 5: return "Elipse";
	}
}

//funcion que determina el tipo de figura a partir del output de la red
int getTipoFigura(const double & s1,const double & s2,const double & s3) {
    if ((s1>s2)&&(s1>s3)) return 1;
    if ((s2>s1)&&(s2>s3)) return 2;
    if ((s3>s1)&&(s3>s2)) return 3;
}

int getTipoFiguraBinario(const double & s1,const double & s2) {
    if (s1>s2) return 1;
    if (s2>s1) return 2;
	if (s1==s2) return 1;
}

CvBox2D calcularRectanguloAreaMinima(const CvContourPolygon & poly){
    CvMemStorage* MemStorage = cvCreateMemStorage(0);
    CvSeq * seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),sizeof(CvPoint), MemStorage);
    cvSeqPushMulti( seq,&poly[0],poly.size());
    return cvMinAreaRect2(seq);
}

//funcion que calcula el area del minimo rectangulo que contiene al blob
double calcularAreaMinBoundingBox(const CvContourPolygon & poly) {
    CvBox2D minBoundingBox = calcularRectanguloAreaMinima(poly);
    return (minBoundingBox.size.width*minBoundingBox.size.height);
}

//funcion que calcula la relacion entre ancho y largo del minimo rectangulo que contiene al blob
double calcularElongatedness(const CvContourPolygon & poly) {
    CvBox2D minBoundingBox = calcularRectanguloAreaMinima(poly);
    double height = minBoundingBox.size.height;
    double width = minBoundingBox.size.width;
    if( height == width) 
        return 1;
    else if (height > width)
         return width/height;
    else 
         return height/width;
}

int main(int argc, char** argv)
{

    if ((argv[1]==NULL)||(argv[2]==NULL)) {
        cerr << "Debe especificar un archivo de imagen a analizar y la red neuronal a utilizar."<<endl;
        exit(1);
    };

    //ruta del archivo de imagen y red neuronal a utilizar
    char * archivoImagen= argv[1];
    char * archivoRed= argv[2];

    //cargo imagen a procesar
    IplImage *img = cvLoadImage(archivoImagen, 1);

    //convierto la imagen a escala de grises
    IplImage *grey = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvCvtColor(img, grey, CV_BGR2GRAY);
    cvThreshold(grey, grey, 100, 255, CV_THRESH_BINARY);
    IplImage *labelImg = cvCreateImage(cvGetSize(grey),IPL_DEPTH_LABEL,1);

    //obtengo blobs de la imagen
    CvBlobs blobs;
    unsigned int result = cvLabel(grey, labelImg, blobs);

    //los filtro por area, elimino los que son muy pequeños
    cvFilterByArea(blobs,AREA_MIN,AREA_MAX);

    //creo imagen con los datos de salida
    IplImage *imgOut = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
    cvRenderBlobs(labelImg, blobs, img, imgOut,CV_BLOB_RENDER_BOUNDING_BOX |
                  /*CV_BLOB_RENDER_TO_STD |*/
                  CV_BLOB_RENDER_CENTROID
                  |CV_BLOB_RENDER_COLOR );

    //Inicializo fuente de texto
    CvFont fuente;
    cvInitFont(&fuente, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, CV_AA);


    //Red Neuronal

    //estructura para pasar los datos a la red
    fann_type entradas[ENTRADAS_RED];

    //estructura para obtener el resultado
    fann_type *resultado=NULL;

    //creo la red neuronal
    struct fann * ann = fann_create_from_file(archivoRed);
	struct fann * annCuadrados = fann_create_from_file("red_cuadrados_rectangulos.net");
	struct fann * annCirculos = fann_create_from_file("red_circulos_elipses.net");
	
    //contador de blobs
    int contador = 1;

    //itero por cada blob
    for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it){

        // cvRenderBlob(labelImg, (*it).second, img, imgOut);

        //obtengo el poligono que define el blob
        CvContourPolygon *polygon = cvConvertChainCodesToPolygon(&(*it).second->contour);

        //obtengo la maxima distancia de un vertice del poligono al centroide del mismo
        double maxDist = getMaxDistAlVaricentro((*it).second->centroid.x,(*it).second->centroid.y,*polygon);

        //obtengo el promedio de la distancia de los vertices del poligono al varicentro
        double sumaDist= getPromedioSqDistancias((*it).second->centroid.x,(*it).second->centroid.y,*polygon);

        //relacion promedio de distancias / maxima distancia
        double relDistancias = sumaDist/maxDist;

        //obtengo el poligono simplificado que define al blob que define al blob
        CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, CONST_SIMP);

        //obtengo cantidad de vertices del poligono
        size_t verticesPoligono = polygon->size();

        //obtengo cantidad de vertices del poligono simplificado
        size_t verticesPoligonoSimple = sPolygon->size();

        //obtengo area del blob
        double areaBlob = (*it).second->area;

        //obtengo area del minimo rectangulo que contiene al blob
        double areaMinimoRect = calcularAreaMinBoundingBox(*sPolygon);

        //relacion area blob /area del minimo rectangulo que lo contiene
        double relArea = ((areaBlob/areaMinimoRect)>1)?1.0f:(areaBlob/areaMinimoRect);

        //relacion nº de vertices del poligono simple/100
        double relVertices = verticesPoligonoSimple/MAX_VERTICES;

        //Determino de que tipo de figura se trata
        entradas[0] = relVertices;
        entradas[1] = relArea;
        entradas[2] = relDistancias;
        resultado = fann_run(ann, entradas);

		int tipo = getTipoFigura(resultado[0],resultado[1],resultado[2]);
		if (tipo == 1){
            double elongatedness = calcularElongatedness(*sPolygon);
            entradas[0] = elongatedness;
	    	cout << "Elongatedness: " << fixed << elongatedness <<  endl;
    	    resultado = fann_run(annCuadrados, entradas);
			if (getTipoFiguraBinario(resultado[0],resultado[2])==1)
				tipo = 1;
			else tipo = 4;	
		} else if (tipo ==3){
        	resultado = fann_run(annCirculos, entradas);
			if (getTipoFiguraBinario(resultado[0],resultado[2])==1)
				tipo = 3;
			else tipo = 5;	
		}

		//Output de los datos en consola
		cout << "Figura " << contador<< ":" <<endl;
		cout.precision(4);
		cout << "s0: " << fixed << entradas[0] << endl;
		cout << "s1: " << fixed << entradas[1] << endl;
		cout << "s2: " << fixed << entradas[2] << endl;
		cout << "o0: " << fixed << resultado[0] << endl;
		cout << "o1: " << fixed << resultado[1] << endl;
		cout << "o2: " << fixed << resultado[2] <<  endl;
        cout<< "Tipo: " << getNombreFigura(tipo) << endl<<endl;



		//dibujo en la imagen el número y el tipo de poligono que se reconoció
        ostringstream label;
        label << contador << ". " << getNombreFigura(tipo);
        CvPoint blobPos = cvPoint((*it).second->minx,(*it).second->maxy+14);
        CvPoint rectInicio= cvPoint(blobPos.x,blobPos.y+2);
        CvPoint rectFin= cvPoint(blobPos.x+9*label.str().size(),blobPos.y-12);
        cvRectangle(imgOut, rectInicio,rectFin, cvScalar(200, 200, 200, 0), CV_FILLED, 8, 0);
        cvPutText(imgOut,label.str().c_str(),blobPos, &fuente, cvScalar(0, 0, 0, 0));

        delete sPolygon;
        delete polygon;
        contador++;

    }

    //creo ventanas
    cvNamedWindow("test", 1);

    //renderizo imagenes
    cvShowImage("test", imgOut);
    cvWaitKey(0);

    //elimino ventana
    cvDestroyWindow("test");

    //libero recursos
    fann_destroy(ann);
    fann_destroy(annCuadrados);
    fann_destroy(annCirculos);
    cvReleaseImage(&imgOut);
    cvReleaseImage(&imgOut);
    cvReleaseImage(&imgOut);
    cvReleaseImage(&grey);
    cvReleaseImage(&labelImg);
    cvReleaseImage(&img);
    cvReleaseBlobs(blobs);

    return 0;
}
