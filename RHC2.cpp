#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <cstring>
#include <cstdlib>


struct TrainItem{
   int classAttr;
   double *attr;
   double dist;
};

struct Representative{
   int classAttr;
   double *attr;
   TrainItem *data;
   int dataCounter;
   bool homogeneous;
};

unsigned long long int kmeansComputations;
int ATTRIBUTES;
int CLASSES;

using namespace std;

void readTrainData(TrainItem[], char[], int);
void readTrainData2(TrainItem[], char[], int);
bool checkIfHomogenous(Representative);
int DistancesComputationKMeans(TrainItem&, Representative[], int, int);
void initial_kMeans(TrainItem[], Representative[], int);
void kMeans(Representative, Representative[], int&);
void setInitialClassesCentroid(TrainItem[], Representative[], int);
int countLinesAttrs(char[], int&, int&);

int main(int argc, char *argv[]){
   TrainItem *TrD;
   Representative *Repr;
   int f, i, j, c, back;
   int dataNumV;
   int sumc=0, sumd=0;

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
      cout<<"Folds:"<<f<<endl;

      fileName[strlen(fileName)-1]=f+'0';
      fileName2[strlen(fileName2)-1]=f+'0';

      if (countLinesAttrs(fileName,dataNumV,ATTRIBUTES)){
         cout<<"File "<<fileName<<" does not exist"<<endl;
         return 1;
      }

      kmeansComputations=0;

      TrD = new TrainItem[dataNumV];
      Repr = new Representative[dataNumV];

      readTrainData2(TrD, fileName, dataNumV);
      setInitialClassesCentroid(TrD, Repr, dataNumV);
      initial_kMeans(TrD, Repr, dataNumV);
      back=CLASSES;

      ofstream o;
      o.open(fileName2);

      i=0;
      c=0;

      while (i <= back-1){
         if ((!Repr[i].homogeneous) && (Repr[i].dataCounter>1)){
            kMeans(Repr[i], Repr, back);
         }
         else if (Repr[i].homogeneous){
            if (Repr[i].dataCounter>0){
               Repr[i].classAttr = Repr[i].data[0].classAttr;
               c++;
               for (j=0; j<ATTRIBUTES; j++){
                  o<<Repr[i].attr[j]<<"\t";
               }
               o<<Repr[i].classAttr<<endl;
            }
         }
         i++;
         //if ((i >= dataNumV*100) || (back >= dataNumV*100)){
         //   cout<<"Queue overflow"<<endl;
         //   return 1;
         //}
      }

      o.close();

      if (f == 1){
         cout<<"Classes: "<<CLASSES<<endl;
         cout<<"Attributes: "<<ATTRIBUTES<<endl;
         cout<<"Data counter: "<<dataNumV<<endl;
      }
      cout<<"Prototypes: "<<c<<endl;
      cout<<"Distance computations: "<<kmeansComputations<<endl;
      sumc += c;
      sumd += kmeansComputations;
   }
   cout.precision(3);
   cout.setf(ios::fixed);
   cout<<endl;
   cout<<"Avg. prototypes: "<<(double)sumc/(double)FOLDS<<endl;
   cout<<"Avg. Reduction rate: "<<(double)(1-((double)sumc/(double)FOLDS/(double)dataNumV))*100<<endl;
   cout<<"Avg. distance computations: "<<(double)sumd/(double)FOLDS<<endl;

   return 0;
}


void setInitialClassesCentroid(TrainItem TrD[], Representative R[], int dataNumV){
   int c, i, j, *mx;
   double **sumAttr;

   mx = new int[CLASSES];
   sumAttr = new double* [CLASSES];

   for (i=0; i<CLASSES; i++){
      sumAttr[i] = new double[ATTRIBUTES];
      mx[i]=0;
      for (j=0; j<ATTRIBUTES; j++){
         sumAttr[i][j]=0;
      }
   }

   for (i=0; i<dataNumV; i++){
      c = TrD[i].classAttr;
      mx[c]++;
      for (j=0; j<ATTRIBUTES; j++){
         sumAttr[c][j] += TrD[i].attr[j];
      }
   }

   for (i=0; i<CLASSES; i++){
      R[i].classAttr = i;
      if (mx[i] > 0){
         R[i].data = new TrainItem[dataNumV];
         R[i].attr = new double[ATTRIBUTES];
         for (j=0; j<ATTRIBUTES; j++){
            R[i].attr[j] = (double)sumAttr[i][j] / (double)mx[i];
         }
      }
   }

   delete []mx;
   delete []sumAttr;
}


void kMeans(Representative R, Representative *Repr, int &back){
   int c, i, j, beg, pos;

   static double **sumAttr;
   static int *mx;

   mx = new int[CLASSES];
   sumAttr = new double* [CLASSES];

   beg=back;

   for (i=0; i<CLASSES; i++){
      sumAttr[i] = new double[ATTRIBUTES];
      mx[i]=0;
      for (j=0; j<ATTRIBUTES; j++){
         sumAttr[i][j]=0;
      }
   }

   for (i=0; i<R.dataCounter; i++){
      c = R.data[i].classAttr;
      mx[c]++;
      for (j=0; j<ATTRIBUTES; j++){
         sumAttr[c][j] += R.data[i].attr[j];
      }
   }


   for (i=0; i<CLASSES; i++){
      if (mx[i] > 0){
         Repr[back].classAttr = i;
         Repr[back].data = new TrainItem[R.dataCounter];
         Repr[back].attr = new double[ATTRIBUTES];
         for (j=0; j<ATTRIBUTES; j++){
            Repr[back].attr[j] = (double)sumAttr[i][j] / (double)mx[i];
         }
         back++;
      }
   }





  delete []sumAttr;
  delete [] mx;

  for (i=beg; i<back; i++){
      Repr[i].dataCounter=0;
  }


   for (i=0; i<R.dataCounter; i++){
      pos = DistancesComputationKMeans(R.data[i], Repr, beg, back);
      Repr[pos].data[Repr[pos].dataCounter] = R.data[i];
      Repr[pos].dataCounter++;
   }

   for (i=beg; i<back; i++){
      //if (Repr[i].dataCounter == 0){cout<<"aaaa"<<endl;}
      Repr[i].homogeneous = checkIfHomogenous(Repr[i]);
   }

}

void initial_kMeans(TrainItem TrD[], Representative Repr[], int dataNumV){
   int i,  pos;

   for (i=0; i<CLASSES; i++){
      Repr[i].dataCounter=0;
   }
   for (i=0; i<dataNumV; i++){
      pos = DistancesComputationKMeans(TrD[i], Repr, 0, CLASSES);
      Repr[pos].data[Repr[pos].dataCounter] = TrD[i];
      Repr[pos].dataCounter++;
   }

   for (i=0; i<CLASSES; i++){
      Repr[i].homogeneous = checkIfHomogenous(Repr[i]);
   }

}


int DistancesComputationKMeans(TrainItem &ti, Representative Repr[], int b, int e){
   int pos=-1, k, j, m;
   double min, u, x;
   double *dist;
   bool minInit = false;

   dist = new double[e-b+1];

   for (k=b; k<e; k++){

      kmeansComputations++;
      x=0;
      for (j=0; j<ATTRIBUTES; j++){
         u = (double)ti.attr[j] - (double)Repr[k].attr[j];
         x=x + u * u;
      }
      dist[k-b]=x;
      if (!minInit){
         min = dist[k-b];
         pos=k;
         minInit = true;
      }

      if (dist[k-b]<min){
         min=dist[k-b];
         pos=k;
      }
   }
   ti.dist = min;

   if (e-b > 2){
      return pos;
   }
   else {
      m=0;
      for (k=0; k<e-b; k++){
          if (dist[k] == min) {
             m++;
          }
      }
      if (m==1){
         return pos;
      }
      else{
         int n = rand() % 2;
         if (n == 0)
            return pos;
         else
            return pos+1;
      }
   }
}


void readTrainData2(TrainItem trainData[], char fileName[], int n){
   int i,j;

   ifstream dat;
   dat.open(fileName);
   CLASSES=0;

   int xmax = -1;

   for (i=0; i<n; i++){
      //trainData[i].attr = (double*) malloc (ATTRIBUTES * sizeof(double));
      trainData[i].attr = new double[ATTRIBUTES];
      for (j=0; j<ATTRIBUTES; j++){
         dat>>trainData[i].attr[j];
      }
      dat>>trainData[i].classAttr ;
      if (trainData[i].classAttr > xmax){
         xmax = trainData[i].classAttr;
      }
   }
   CLASSES = xmax + 1;
   dat.close();
}

bool checkIfHomogenous(Representative R){
   if (R.dataCounter <= 1){
      return true;
   }

   int c = R.data[0].classAttr;

   for (int i=1; i<R.dataCounter; i++){
      if (c != R.data[i].classAttr){
         return false;
      }
   }

   return true;
}

int getNumberOfClasses(Representative R){
   int i, n, *array;

   array = (int*) malloc (CLASSES * sizeof(int));

   for (i=0; i<CLASSES; i++){
      array[i]=0;
   }

   for (i=0; i<R.dataCounter; i++){
      array[R.data[i].classAttr]++;
   }

   n=0;
   for (i=0; i<CLASSES; i++){
      if (array[i]>0){
         n++;
      }
   }
   return n;
}

int countLinesAttrs(char fileName[], int &lines, int &attrs){
   int i, c;

   ifstream dat;
   dat.open(fileName);

   if (!dat){
      return 1;
   }

   string lline;

   getline(dat, lline);
   c=0;
   for (i=0; i<(int)lline.length(); i++){
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

