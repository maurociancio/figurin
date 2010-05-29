
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#ifdef WIN32
#include <cv.h>
#include <highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif
#include <fann.h>
#include <cvblob.h>
using namespace cvb;


//funcion que calcula el cuadrado de la maxima distancia al centroide de un poligono 
double getMaxDistAlVaricentro(double xc, double yc, const CvContourPolygon & poly){
		double max =0.0f;
		std::vector<CvPoint>::const_iterator it = poly.begin();
		while(it!=poly.end()){
			double x1 = (*it).x-xc;
			double y1 = (*it).y-yc;
			double sqNorma = x1*x1+y1*y1;
			if (sqNorma>max) max=sqNorma;	
			it++;
		}
	return max;
}

//funcion que calcula el promedio del cuadrado de la distancia al centroide de un poligono 
double getPromedioSqDistancias(double xc, double yc, const CvContourPolygon & poly){

	double suma=0;
		std::vector<CvPoint>::const_iterator it = poly.begin();
		while(it!=poly.end()){
			double x1 = (*it).x-xc;
			double y1 = (*it).y-yc;
			double sqNorma = x1*x1+y1*y1;
			suma+=sqNorma;
			it++;
		}
	return suma/poly.size();
}

//funcion que devuelve el nombre de la figura que corresponde a la salida de la red neuronal
string getNombreFigura(const double & s1,const double & s2,const double & s3){
	if((s1>s2)&&(s1>s3)) return "Cuadrado";
	if((s2>s1)&&(s2>s3)) return "Triangulo";
	if((s3>s1)&&(s3>s2)) return "Circulo";
}

//funcion que calcula el area del minimo rectangulo que contiene al blob
double calcularAreaMinBoundingBox(const CvContourPolygon & poly){
	double area=0.0f;
	CvMemStorage* MemStorage = cvCreateMemStorage(0);
	CvSeq * seq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq),	sizeof(CvPoint), MemStorage);
	cvSeqPushMulti( seq,&poly[0],poly.size());
	CvBox2D minBoundingBox = cvMinAreaRect2(seq);
	return (minBoundingBox.size.width*minBoundingBox.size.height);
}


int main(int argc, char** argv)
{

	if((argv[1]==NULL)||(argv[2]==NULL)){
		cerr << "Debe especificar un archivo de imagen a analizar y la red neuronal a utilizar."<<endl;
		exit(1);
	};
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
	//TODO definir limites como un const o tomarlos como parametros  
	cvFilterByArea(blobs, 250,50000);

	//creo imagen con los datos de salida
  IplImage *imgOut = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
  cvRenderBlobs(labelImg, blobs, img, imgOut,CV_BLOB_RENDER_BOUNDING_BOX |
																						 /*CV_BLOB_RENDER_TO_STD |*/
																						 CV_BLOB_RENDER_CENTROID
																							|CV_BLOB_RENDER_COLOR );

	//Inicializo fuente de texto
  CvFont fuente;
  cvInitFont(&fuente, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, CV_AA);

	
	//archivo para el output de datos
	//fstream datos;
  //datos.open("datos.txt",ios::out);
	/*datos << blobs.size() <<" 3 3" << endl << endl;*/
  //myfile << "Writing this to a file.\n";

	//Red Neuronal
	//estructura para pasar los datos a la red
	fann_type entradas[3];
	//estructura para obtener el resultado
	fann_type *resultado;
	//creo la red neuronal
	struct fann *ann = fann_create_from_file(archivoRed);

	int contador = 1;

  // Render contours:
  for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
  {
		ostringstream label; 
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
    CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, 3.0f);

		//obtengo cantidad de vertices del poligono
		size_t verticesPoligono = polygon->size();

		//obtengo cantidad de vertices del poligono simplificado
		size_t verticesPoligonoSimple = sPolygon->size();
  
		//obtengo area del blob
		double areaBlob = (*it).second->area;

		//obtengo area del minimo rectangulo que contiene al blob
		double areaMinimoRect = calcularAreaMinBoundingBox(*sPolygon);

		//relacion area blob /area del minimo rectangulo que lo contiene
		double relArea = areaBlob/areaMinimoRect;

		//relacion nº de vertices del poligono simple/100
		double relVertices = verticesPoligonoSimple/100.0f;

/*
	 std::cout << "relacion promedio distancias /max dist al centroide: "<< relDistancias<< std::endl; 
	 std::cout << "cantidad de vertices poligono: "<< verticesPoligono<< std::endl; 
	 std::cout << "cantidad de vertices poligono simplificado: "<< verticesPoligonoSimple << std::endl ; 
*/	 std::cout << "area de poligono simplificado: "<< areaBlob<< std::endl;
/*	 std::cout << "area de la boundingBox: "<< areaMinimoRect << std::endl;
	 std::cout << "relacion area poligono : "<< areaBlob/areaMinimoRect<< std::endl;
	 std::cout << "vertices / 50: " << verticesPoligonoSimple/100.0f<< std::endl ; 
*/

   std::cout << relVertices << ";"<<relArea<<";"<<relDistancias<<";"<<endl;
/*
	 datos  << relVertices  <<" " 
				  << (relArea>1?1:relArea) <<" "
				  << relDistancias << endl <<endl;
*/
 //   cvRenderContourChainCode(&(*it).second->contour, imgOut);
 //   cvRenderContourPolygon(sPolygon, imgOut, CV_RGB(0, 255,255));
 //   cvRenderContourPolygon(cPolygon, imgOut, CV_RGB(0, 255, 0));

	//Determino de que tipo de figura se trata
	entradas[0] = (verticesPoligonoSimple/100.0f);
	entradas[1] = relArea;
	entradas[2] = relDistancias;

	resultado = fann_run(ann, entradas);
	cout << resultado[0] <<";"<<resultado[1] <<";"<<resultado[2] <<";"<<endl;
	label << contador << ". " << getNombreFigura(resultado[0],resultado[1],resultado[2]);
	cout<< "es: " << label.str() << endl<<endl;


	CvPoint blobPos = cvPoint((*it).second->minx,(*it).second->maxy+14);
	CvPoint rectInicio= cvPoint(blobPos.x,blobPos.y+2);
	CvPoint rectFin= cvPoint(blobPos.x+9*label.str().size(),blobPos.y-12);
  cvRectangle(imgOut, rectInicio,rectFin, cvScalar(200, 200, 200, 0), CV_FILLED, 8, 0);
  cvPutText(imgOut,label.str().c_str(),blobPos, &fuente, cvScalar(0, 0, 0, 0));

    delete sPolygon;
    delete polygon;
		contador++;

  }


 // datos.close();

  //creo ventanas
  cvNamedWindow("test", 1);
//  cvNamedWindow("imagen", 1);

	//renderizo imagenes
  cvShowImage("test", imgOut);
//  cvShowImage("grey", grey);
//  cvShowImage("imagen", img);
  cvWaitKey(0);
	//elimino ventana
  cvDestroyWindow("test");

	//libero recursos
	fann_destroy(ann);
  cvReleaseImage(&imgOut);
  cvReleaseImage(&grey);
  cvReleaseImage(&labelImg);
  cvReleaseImage(&img);
  cvReleaseBlobs(blobs);

  return 0;
}
