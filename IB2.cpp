#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

struct TrainItem{
   int classAttr;
   double *attr;
   int aa;
   char SX;
};

unsigned long long int computations;

int ATTRIBUTES;
int CLASSES;

void readTrainData(TrainItem[], char[], int);
int NearestNeighbor(TrainItem, TrainItem[], int);
int countLinesAttrs(char[], int&, int&);


int main(int argc, char *argv[]){
   static TrainItem *trainData;
   int i, j, nn, c, f;
   int dataNumV;

   unsigned long long int sumd = 0;
   int sumc = 0;

  if (argc != 4){
      cout<<"ERROR. Number of parameters"<<endl;
      return 1;
   }

   char *fileName = (char*) malloc( sizeof(char) * strlen(argv[1])+2);
   char *fileName2 = (char*) malloc( sizeof(char) * strlen(argv[2])+2);

   strcpy(fileName,argv[1]);
   strcat(fileName,"X");
   strcpy(fileName2,argv[2]);
   strcat(fileName2,"X");

   if (!isdigit(*argv[3])){
      cout<<"ERROR. Folds number must be numeric"<<endl;
      return 1;
   }

   int FOLDS=atoi(argv[3]);
   if (FOLDS < 1){
       cout<<"Error: number of FOLDS"<<endl;
       return 1;
   }


   for (f=1; f<=FOLDS; f++){
      fileName[strlen(fileName)-1]=f+'0';
      fileName2[strlen(fileName2)-1]=f+'0';

      if (countLinesAttrs(fileName,dataNumV,ATTRIBUTES)){
         cout<<"File "<<fileName<<" does not exist"<<endl;
         return 1;
      }

      trainData = new TrainItem[dataNumV];
      readTrainData(trainData, fileName, dataNumV);

      computations = 0;

      //Put first item into S
      trainData[0].SX='S';

      for (i=0; i<dataNumV; i++){
         if (trainData[i].SX == 'X'){ //for each item belong to X, ibl2 searches in S
            nn=NearestNeighbor(trainData[i], trainData, dataNumV);
            if (nn!=-1){
               if (trainData[nn].classAttr != trainData[i].classAttr){
                  trainData[i].SX='S';
               }
            }
         }
      }

      ofstream out1;
      out1.open(fileName2);

      c=0;
      for (i=0; i<dataNumV; i++){
         if (trainData[i].SX == 'S'){
            for (j=0; j<ATTRIBUTES; j++){
               out1<<trainData[i].attr[j]<<"\t";
            }
            out1<<trainData[i].classAttr<<endl;
            c++;
         }
      }


      out1.close();
      if (f == 1){
         cout<<"Classes: "<<CLASSES<<endl;
         cout<<"Attributes: "<<ATTRIBUTES<<endl;
      }

      cout<<"Fold: "<<f<<endl;
      cout<<"Data counter: "<<dataNumV<<endl;;
      cout<<"CS size: "<<c<<endl;
      cout<<"Distance computations: "<<computations<<endl;

      sumd = sumd + computations;
      sumc = sumc + c;
   }

   cout.precision(3);
   cout.setf(ios::fixed);
   cout<<endl;
   cout<<"Avg. CS size: "<<(double)sumc/(double)FOLDS<<endl;
   cout<<"Avg. Reduction rate: "<<(double)(1-((double)sumc/(double)FOLDS/(double)dataNumV))*100<<endl;
   cout<<"Avg. distance computations: "<<(double)sumd/(double)FOLDS<<endl;

   return 0;
}


int NearestNeighbor(TrainItem ti, TrainItem trainData[], int dataNumV){
   int g, k, j ;
   double u, x, min;

   double *dist = new double[dataNumV];

   min=9999999999999;
   g=-1;

   for (k=0; k<dataNumV; k++){
      //search for the NN in S
      if (trainData[k].SX == 'X'){
         continue;
      }
      if (ti.aa == k){
         continue;
      }
      x=0;

      computations++;

      for (j=0; j<ATTRIBUTES; j++){
         u = (double)ti.attr[j] - (double)trainData[k].attr[j];
         x=x + u * u;
      }

      dist[k]=x;
      if (dist[k]<min){
         min=dist[k];
         g=k;
      }
   }

   free(dist);

   return g;
}


void readTrainData(TrainItem trainData[], char fileName[], int n){
   int i,j;

   ifstream dat;
   dat.open(fileName);
   CLASSES=0;
   bool fl;

   for (i=0; i<n; i++){
      trainData[i].attr = new double[ATTRIBUTES];
      for (j=0; j<ATTRIBUTES; j++){
         dat>>trainData[i].attr[j];
      }
      dat>>trainData[i].classAttr ;
      trainData[i].SX = 'X';
      trainData[i].aa=i;
      fl = false;
      for (j=0; j<i; j++){
         if (trainData[i].classAttr == trainData[j].classAttr){
            fl = true;
            break;
         }
      }
      if (!fl){
         CLASSES++;
      }
   }

   dat.close();
}

int countLinesAttrs(char fileName[], int &lines, int &attrs){
   unsigned int i, c;

   ifstream dat;
   dat.open(fileName);

   if (!dat){
      return 1;
   }

   string lline;

   getline(dat, lline);
   c=0;
   for (i=0; i<lline.length(); i++){
      if (lline[i] == '\t'){
         c++;
      }
   }
   dat.close();
   attrs = c;

   dat.open(fileName);
   i=0;
   while (!dat.eof()){
      getline(dat, lline);
      i++;
   }

   lines = i-1;
   dat.close();
   return 0;
}

