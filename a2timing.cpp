
#include "table.h"
#include "timer.h"
#include <string>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>


using namespace std;
const int NUMTESTS=13;
const int MAX=200000;
const int numRandomOperations=10000000;

const char* filename="dictionary.txt";

//A struct to simplify error recording process.
struct Error{
    char msg_[150];
    int passed_;
    Error(const char* msg, int p){
        strcpy(msg_,msg);
        passed_=p;
    }
    void set(const char* msg, int p){
        strcpy(msg_,msg);
        passed_=p;
    }
};


void cleanWordList(char* wordlist[]);
void setData(int data[],int sz);
void printTiming(double timing[],int numOps[]);

int main(int argc, char* argv[]){

    FILE* fp=fopen(filename,"r");

    int testnumber=0;
    if(!fp){
        cout << "could not open file " << filename << ".  Make sure it is in the" << endl;
        cout << "same directory as this program and try again" << endl;
        return 0;
    }
    char curr[50];          //used to read in words from text file

    bool oprc;              //return value from an operation

 
    int numremoved=0;   //number of words removed from the table;

    string* wordlist;        //list of test cases. 
    int* data;
    string* removedList;     //tracks records removed

    int numRecords;
    int currValue;
    Timer t;
    double lambda = 0.65;
    if(argc != 1){
        lambda=atof(argv[1]);
    }
    else{
        cout << "no value supplied for maxLoadFactor, using default of 0.65" << endl;
        cout << "use: a.out <maxLoadFactor> to supply value maximum load factor" << endl;
    }    
    try{
        data=new int[MAX];
        wordlist= new string[MAX];
        removedList = new string[MAX];

        for(int i=0;i<MAX;i++){
            fscanf(fp,"%s\n",curr);                 //read in a word from data file
            wordlist[i]=curr;                       //record it
         }/*end for:  populate table*/
        setData(data,MAX);

        cout << "* * * * * * * * * * * * Testing HashTable Table* * * * * * * * * * * * * * * *"<< endl;
        cout << "Initialization and update" << endl;
        int hashtableCapacity=ceil(MAX/lambda);
        int operationDone[4]={0,0,0,0};   //number of times each operation
                                          //was done.  0 update-new
                                          // 1 update-mod, 2 find, 3 remove

        int hashtableMax = MAX;
        LPTable<int> hashtable(hashtableCapacity,lambda);
        Timer t2;
        double hashTiming[4]={0,0,0,0}; //timing of all ops 0 update-new
                                        // 1 update-mod, 2 find, 3 remove


        t2.reset();
        for(int i=0;i<hashtableMax;i++){
            t2.start();
            oprc=hashtable.update(wordlist[i],data[i]);             
            t2.stop();
            operationDone[0]++;
            if(!oprc){                              //check return code (should be true)
                throw Error("hashtabletable::update is returning false when table is not full",testnumber);
            }
        }/*end for:  populate table*/
        hashTiming[0]=t2.currtime();
        testnumber++;


        cout << "Checking hashtableTable::find() and that records were properly recorded..." << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i++){
            int currValue;
            t2.start();
            oprc=hashtable.find(wordlist[i],currValue);                
            t2.stop();
            operationDone[2]++;
            if(!oprc){                                          //check return code (should be true)
                throw Error("hashtable::find is returning false for a word that should be in the table",testnumber);
            }
            if(currValue!=data[i]){
                cout << "search key:" << wordlist[i] << endl;
                cout << "correct value: "<< data[i] << endl;
                cout << "your record's value: " << currValue << endl;
                throw Error("hashtable::find is not passing back the correct data",testnumber);
            }
        }
        hashTiming[2]=t2.currtime();
        testnumber++;


        cout << "Change some records in hashtable table and see if it still works" << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i++){
            data[i]+=1;                                              //modify the data (but key is same)
            t2.start();
            oprc=hashtable.update(wordlist[i],data[i]);                          //put it back in the table
            t2.stop();
            operationDone[1]++;

            if(!oprc){
                throw Error("hashtable::update is returning false for updating an existing record",testnumber);
            }
        }
        hashTiming[1]=t2.currtime();
        testnumber++;

        cout << "Checking hashtable::find() and whether or not records were properly updated..." << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i++){
            t2.start();
            oprc=hashtable.find(wordlist[i],currValue);                //search the linear probing table
            t2.stop();
            operationDone[2]++;

            if(!oprc){                                          //check return code (should be true)
                throw Error("hashtable::find is returning false for a word that should be in the table",testnumber);
            }
            if(data[i]!=currValue){
                cout << "search key:" << wordlist[i] << endl;
                cout << "correct value: "<< data[i] << endl;
                cout << "your record's value: " << currValue << endl;
                throw Error("hashtable::find is not passing back the correct data",testnumber);
            }
        }
        hashTiming[2]+=t2.currtime();
        testnumber++;



        cout << "Remove some records that exist from hashtable table..." << endl;
        numremoved=0;
        t2.reset();
        for(int i=0;i<hashtableMax;i+=10){
            t2.start();
            oprc=hashtable.remove(wordlist[i]);                          //put it back in the table
            t2.stop();
            operationDone[3]++;
            numremoved++;
            if(!oprc){
                throw Error("hashtable::remove is returning false for removing an existing record",testnumber);
            }
        }
        hashTiming[3]=t2.currtime();
        testnumber++;


        cout << "Check that removed records are gone and others are unaffected in hashtable table..." << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i++){
            t2.start();
            oprc=hashtable.find(wordlist[i],currValue);                          //put it back in the table
            t2.stop();
            operationDone[2]++;

            if(i%10){
                if(!oprc){
                    throw Error("hashtable::find() could not find record that was not removed",testnumber);
                }
                if(data[i]!=currValue){
                    cout << "search key:" << wordlist[i] << endl;
                    cout << "correct value: "<< data[i] << endl;
                    cout << "your record's value: " << currValue << endl;
                    throw Error("hashtable::find is not passing back the correct data",testnumber);
                } 
            }
            else{
                if(oprc){
                    throw Error("hashtable::find() found a record that was removed",testnumber);
                }
            }
        }
        hashTiming[2]+=t2.currtime();
        testnumber++;

        cout << "Attempt to remove some records that do not exist from the table" << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i+=10){
            t2.start();
            oprc=hashtable.remove(wordlist[i]);                          //put it back in the table
            t2.stop();
            operationDone[3]++;

            if(oprc){
                throw Error("hashtable::remove is returning true for removing non-existing record",testnumber);
            }
        }
        hashTiming[3]=t2.currtime();
        testnumber++;



        cout << "Add the removed items back to hashtable table..." << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i+=10){
            data[i]-=1;
            t2.start();
            oprc=hashtable.update(wordlist[i],data[i]);          //put it in the chaining table
            t2.stop();
            operationDone[0]++;            
            if(!oprc){                             //check return code (should be true)
                throw Error("hashtable::update is returning false when table is not full",testnumber);
            }
        }/*end for*/
        testnumber++;
        hashTiming[0]+=t2.currtime();

        cout << "Checking that everything in hashtable was properly updated adding adding removed records back" << endl;
        t2.reset();
        for(int i=0;i<hashtableMax;i++){
            t2.start();
            oprc=hashtable.find(wordlist[i],currValue);                        
            t2.stop();
            operationDone[2]++;

            if(!oprc){
                throw Error("hashtable::find() could not find record that should be there",testnumber);
            }
            if(data[i]!=currValue){
                cout << "search key:" << wordlist[i] << endl;
                cout << "correct value: "<< data[i] << endl;
                cout << "your record's value: " << currValue << endl;
                throw Error("hashtable::find is not passing back the correct data",testnumber);
            }    
        }
        hashTiming[2]+=t2.currtime();
        testnumber++;

        cout << "Random operation timing" <<endl;
        numremoved=0;
        numRecords=MAX;
        int choice;
        for(int i=0;i<numRandomOperations;i++){
            int operation=rand()%4;
            if(operation==0){
                if(numremoved != 0){
                    choice=rand()%numremoved;
                    wordlist[numRecords]=removedList[choice];
                    data[numRecords]=rand()%100;
                    t2.reset();
                    t2.start();
                    oprc=hashtable.update(removedList[numRecords],data[numRecords]);
                    t2.stop();
                    if(numremoved > 1 && choice!=numremoved-1){
                        removedList[choice]=removedList[numremoved-1];
                    }
                    numremoved--;
                    numRecords++;
                }
            }
            else if(operation==1){
                choice=rand()%numRecords;
                data[choice]+=1;
                t2.reset();
                t2.start();
                oprc=hashtable.update(wordlist[choice],data[choice]);            
                t2.stop();
            }
            else if(operation==2){
                choice=rand()%numRecords;
                t2.reset();
                t2.start();
                oprc=hashtable.find(wordlist[choice],currValue);            
                t2.stop();
            }
            else{
                if(numRecords > 0){
                    choice=rand()%numRecords;
                    removedList[numremoved]=wordlist[choice];
                    t2.reset();
                    t2.start();
                    oprc=hashtable.remove(wordlist[choice]);
                    t2.stop();
                    if(numRecords > 1 && choice!=numRecords-1){
                        wordlist[choice]=wordlist[numRecords-1];
                    }
                    numremoved++;
                    numRecords--;                    
                }

            }
            operationDone[operation]++;
            hashTiming[operation]+=t2.currtime();
        }



        cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"<< endl;
        cout << endl;

        cout << "Timing summary: Hash Table" << endl;
        printTiming(hashTiming,operationDone);
        cout << endl;

        fclose(fp);
    }
    catch(Error e){
        cout << e.msg_ << endl;
        cout << e.passed_ << " tests of "<< NUMTESTS <<" passed" << endl;
    }
}


void printTiming(double timing[],int numOps[]){
    double totalTime=0;
    for(int i=0;i<4 ;i++){
        totalTime+=timing[i];
    }
    cout << endl;


    cout << "     Operation                |    Time     | # of Operations done" << endl;
    cout << "------------------------------|-------------|---------------------" << endl;
    cout << " update, new record           |  ";
    cout.width(10);
    cout << timing[0];
    cout << " |  " << numOps[0]<< endl;
    cout << " update, modify record        |  ";
    cout.width(10);
    cout << timing[1];
    cout << " |  " << numOps[1] << endl;

    cout << " find                         |  ";
    cout.width(10);
    cout << timing[2] ;
    cout << " |  " << numOps[2]  << endl;

    cout << " remove                       |  ";
    cout.width(10);
    cout << timing[3];
    cout << " |  " << numOps[3] << endl;

    cout << "------------------------------|-------------|-----------------" << endl;
    cout << " Total:                       |  ";
    cout.width(10);
    cout << totalTime << endl;
}

void setData(int data[],int sz){
    for(int i=0;i<sz;i++){
        data[i]=rand()%100;
    }
}

