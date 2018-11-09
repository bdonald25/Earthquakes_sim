#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cmath>
#include <time.h>


using namespace std;

//Global Variables
int tmax = 100000;           //total time steps
int L = 200;                //System Size
float alpha = 0.01;          //Dissipation rate
int R = 32;                 //Range
float minStress = 1.0;      //Residual stress
float maxStress = 2.0;      //Threshold stress
float ds = 0.01;            //driving rate
float eta = 0.25;           //noise Amplitude
//End Global Variables


//driving function
float *doPush(float stressArray[]){
    for (int i=0; i<L; ++i){
        stressArray[i] = stressArray[i] + ds;
    }
    return stressArray;
}

//Flat random distribution
float createFlatRandom(){

    float LO = -1.0; float HI = 1.0;
    float number = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    //cout << number << "\n";

    return number;
}

//initialize system stress
float* doFillup(float filledArray[], int Size){

    for(int i=0; i<Size; ++i){
        filledArray[i] = minStress + eta * (createFlatRandom());
    }
return filledArray;
}

// stress fail at node i
float* doFail(float stressArray[], int i){

    stressArray[i] = minStress + eta * (createFlatRandom());

    for(int m=1;m<(R+1);++m){

        if((i + m)<L){
            stressArray[i + m] = stressArray[i + m] + (maxStress-minStress)*(1-alpha)/(2.*static_cast<float>(R));
        }
        if((i - m)>=0){
            stressArray[i - m] = stressArray[i - m] + (maxStress-minStress)*(1-alpha)/(2.*static_cast<float>(R));
        }

    }

    return stressArray;
}

// printout stress for each node
void doPrintSysStress(float stressArray[]){
    for (int elem = 0; elem<L; ++elem){
        cout << stressArray[elem] << " ";
    }
    cout << "\n";
}

// printout when a node fails
void doPrintNumfails(int fails, int t){
    if (fails>0){
        cout << "Failed " << fails << " times at t = " << t << "\n";
    }
}

//main function
int main()
{
    //initialize
    float stressArray[L]; int failArray[L];
    int pass = 0; int fails = 0;

    //fill Array
    stressArray[L] = *doFillup(stressArray, L);

    //if you want, print out stress of each node
    //doPrintSysStress(stressArray);

    //make output file
    ofstream numfail_file;
    numfail_file.open ("time_fails_R32_a01.csv");
    ofstream sitefail_file;
    sitefail_file.open ("site_fails_R32_a01.csv");

    //loop over time
    for(int t=0;t<tmax;++t){

        fails = 0; pass = 0;
        //drive the board
        stressArray[L] = *doPush(stressArray);

        // Scan for Stress failures
        while(pass==0){

            pass = 1;

            // Check for Failures
            for(int i=0;i<L;++i){
                if (stressArray[i] > maxStress){
                    //if node is above threshold, then fail
                    stressArray[L] = *doFail(stressArray, i);
                    pass = 0;
                    //record fails
                    ++fails;
                    ++failArray[i];
                }
            }
        }

        //outputs # fails and at what times
        //doPrintNumfails(fails, t);

        // write to file for graphing

        numfail_file << t << "," << fails << endl;

    }

    //write to file each site's # of failures
    for(int k=0;k<L;++k){
            sitefail_file << failArray[k] << endl;
        }

    //close the files
    numfail_file.close();
    sitefail_file.close();

    //if you want to print out node stresses
    //doPrintSysStress(stressArray);

    return 0;
}
