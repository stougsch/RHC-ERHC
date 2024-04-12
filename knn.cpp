#include <stdlib.h>
#include<stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <cstring>
#include <cstdlib>

struct TrainItem{
   int classAttr;
   double *attr;
};

struct TestItem{
   int classAttr;
   double *attr;
   int pred;
};

using namespace std;

int convKNN;
int NumberOfAttributes;
int classes;

int findClassConv(int votingArray[], TrainItem trainData[], int pos[], int max, int p, int U){
   int i, j, cb=0;
   int *snov;

   snov = new int[classes];

   for (i=0; i<classes; i++){
      if (votingArray[i] == max){
         snov[cb]= i;
         cb++;
      }
   }
   if (cb == 1){
      return p;
   }
   else{
      for (i=0; i<U; i++){
         for (j=0; j<cb; j++){
            if (trainData[pos[i]].classAttr == snov[j]){
               return trainData[pos[i]].classAttr;
            }
         }
      }
   }
   return -1;
}

int findMax(int votingArray[]){
   int max, p, i;
   max=votingArray[0];
   p=0;
   for (i=1; i<classes; i++){
      if (votingArray[i]>max){
         max=votingArray[i];
         p=i;
      }
   }
   return p;
}

void readTestDataFile(TestItem testData[], char fileName[], int n){
   int i, j;
   ifstream dat;
   dat.open(fileName);

   i=0;
   for (i=0; i<n; i++){
      testData[i].attr = new double[NumberOfAttributes];
      for (j=0; j<NumberOfAttributes; j++){
         dat>>testData[i].attr[j];
      }
      dat>>testData[i].classAttr;
   }
   dat.close();
}

void readTrainData(TrainItem trainData[], char fileName[], int n){
   int i,j;

   ifstream dat;
   dat.open(fileName);
   classes=0;
   bool fl;

   for (i=0; i<n; i++){
      trainData[i].attr = new double[NumberOfAttributes];
      for (j=0; j<NumberOfAttributes; j++){
         dat>>trainData[i].attr[j];
      }
      dat>>trainData[i].classAttr ;
      fl = false;
      for (j=0; j<i; j++){
         if (trainData[i].classAttr == trainData[j].classAttr){
            fl = true;
            break;
         }
      }
      if (!fl){
         classes++;
      }
   }

   dat.close();
}


int conventialKNNSearch(TrainItem trainData[], TestItem queryPoint, int n){
   int h, g, k, j, pos, *posMinKArray;
   double u,x, *min;
   static double *dist;

   min = new double[convKNN];
   posMinKArray = new int[convKNN];
   dist = new double[n];

   for (h=0; h<convKNN; h++){
      min[h]=999999999999999999;
      posMinKArray[h]=-1;
   }

   for (k=0; k<n; k++){
      x=0;

      for (j=0; j<NumberOfAttributes; j++){
         u = (double)queryPoint.attr[j] - (double)trainData[k].attr[j];
         x=x + u*u;
      }
      dist[k]=x;
      for (h=0; h<convKNN; h++) {
         if (dist[k]<min[h]){
            for (g=convKNN-1; g>h; g--){
               min[g]=min[g-1];
               posMinKArray[g]=posMinKArray[g-1];
            }
            min[h]=dist[k];
            posMinKArray[h]=k;
            break;
         }
      }
   }

   free(dist);

   if (convKNN==1){
      return trainData[posMinKArray[0]].classAttr;
   }
   else{
      int max, *votingArray;

      votingArray = new int[classes];

      for (k=0; k<classes; k++){
         votingArray[k]=0;
      }
      for (k=0; k<convKNN; k++){
         votingArray[trainData[posMinKArray[k]].classAttr]++;
      }
      pos=findMax(votingArray);
      max=votingArray[pos];

      return findClassConv(votingArray, trainData, posMinKArray, max, pos, convKNN);
   }
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



//==============================================================================
int main(int argc, char *argv[]){

   static TrainItem *trainData;
   static TestItem *testData;

   unsigned long long int comp, sumComp;

   int i, n, n2, f;
   int correctclassified;
   double acc, sumAcc = 0;

   if (argc != 5){
      cout<<"ERROR. Number of parameters"<<endl;
      return 1;
   }

   char *fileName = (char*) malloc( sizeof(char) * strlen(argv[1])+2);
   char *fileName2 = (char*) malloc( sizeof(char) * strlen(argv[2])+2);

   strcpy(fileName,argv[1]);
   strcat(fileName,"X");
   strcpy(fileName2,argv[2]);
   strcat(fileName2,"X");

   if ((!isdigit(*argv[3])) || (!isdigit(*argv[4]))){
      cout<<"ERROR. parameter must be numeric"<<endl;
      return 1;
   }

   int FOLDS=atoi(argv[3]);
   if (FOLDS < 1){
       cout<<"Error: number of FOLDS"<<endl;
       return 1;
   }
   convKNN=atoi(argv[4]);

   sumComp = 0;

   for (f=1; f<=FOLDS; f++){

      fileName[strlen(fileName)-1]=f+'0';
      fileName2[strlen(fileName2)-1]=f+'0';

      if (countLinesAttrs(fileName,n,NumberOfAttributes)){
         cout<<"File "<<fileName<<" dos not exist"<<endl;
         return 1;
      }

      if (countLinesAttrs(fileName2,n2,NumberOfAttributes)){
         cout<<"File "<<fileName2<<" does not exist"<<endl;
         return 1;
      }

      trainData = new TrainItem[n];
      readTrainData(trainData, fileName, n);

      testData = new TestItem[n2];
      readTestDataFile(testData, fileName2 ,n2);


      if (f == 1){
         cout<<"Classes: "<<classes<<endl;
         cout<<"Attributes: "<<NumberOfAttributes<<endl;
      }

      correctclassified=0;
      for (i=0; i<n2; i++){
         testData[i].pred=conventialKNNSearch(trainData, testData[i], n);
         if (testData[i].pred == testData[i].classAttr){
            correctclassified++;
         }
      }

      free(trainData);
      free(testData);

      acc = (double)correctclassified/(double)(n2)*100;

      comp = n * n2;

      cout<<"Fold"<<f<<". Accuracy: "<<acc<<" Computations: "<<comp<<endl;
      sumAcc = sumAcc + acc;
      sumComp = sumComp + comp;
   }
   cout.precision(3);
   cout.setf(ios::fixed);
   cout<<"Avg. accuracy: "<<(double)sumAcc/(double)FOLDS<<endl;
   cout<<"Avg. computations: "<<(double)sumComp/(double)FOLDS<<endl;
}
