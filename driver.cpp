#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

//Function that gets all files from the given directory and adds them to a file vector to be used later
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

//Takes a file from the file vector and adds all of the words from that file to a different vector for future usage
void getAllWords(string fileName, vector<string> &words, string path)
{
    string pathName = path + "/" + fileName;
    ifstream inFile;
    string word;
    inFile.open(pathName);


    inFile >> word;
    while(inFile)
    {
        words.push_back(word);
        inFile >> word;
    }

    inFile.close();
}

//Helper function that fixes all 'chunks' of string so that only valid characters are considered and also makes all
//letters lowercase
string fixString(string s)
{
    string value;
    for(int i = 0; i < s.length(); i++)
    {
        if((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= '0' && s[i] <= '9'))
            value+=s[i];
        else
            if(s[i] >= 'A' && s[i] <= 'Z'){
                s[i]+=32;
                value+=s[i];
            }
            else
                if(s[i] == '<')
                    i+=2;
    }

    return value;
}

//Takes a given set of words from a file and concatenates them into chunks of n size based on user input before storing
//them in
void processChunks(vector<string> words, int n, vector<string> &chunks)
{
    string chunk;
    for(int i = 0; i < words.size() - n + 1; i++)
    {
        chunk = "";
        for(int y = i; y < i + n; y++)
        {
            chunk+= words[y];
        }
        chunk = fixString(chunk);
        chunks.push_back(chunk);
    }
}

//Helper function that takes two integers and takes returns the value of one raised to the other
int power(int base, int exp)
{
    if(exp == 0)
        return 1;
    else
        if(exp == 1)
            return base;
        else
            return base * power(base,exp-1);
}

//Hashing function that takes all chunks of a certain file and hashes them into the hash table
void hashValues(vector<string> chunks, vector<vector<int>> &hashTable, int tableSize, int fileNumber)
{
    unsigned int hashValue;
    bool contained;
    string chunk;

    for(int i = 0; i < chunks.size(); i++)
    {
        contained = false;
        hashValue = 0;
        chunk = chunks[i];
        for(int j = 0; j < chunk.length(); j++)
        {
            hashValue += (int)(chunk[chunk.length()-j-1]) * power(11,j);
        }
        if(hashValue > tableSize)
            hashValue = hashValue % tableSize;

        for(int k = 0; k < hashTable[hashValue].size(); k++)
            if(hashTable[hashValue][k] == fileNumber)
                contained = true;
        if(!contained)
            hashTable[hashValue].push_back(fileNumber);
    }
}

//Checks the hash table and records collisions of files in a matrix
void checkCollisions(vector<vector<int>> &collisions, vector<vector<int>> &hashTable, int hashSize)
{
    for(int i = 0; i < hashSize; i++)
    {
        if(!hashTable[i].empty())
        {
            for (int j = 0; j < hashTable[i].size() - 1; j++)
                for (int k = j + 1; k < hashTable[i].size(); k++) {
                    collisions[hashTable[i][j]][hashTable[i][k]]++;
                }
        }
    }
}

//Checks the collisions matrix and outputs the file pairs that show high amounts of plagiarism
void outputResults(vector<vector<int>> &collisions, int numFiles, int plagiarismCutoff, vector<string> &files)
{
    int counter = 0;
    int max;
    int maxR, maxC;

    for(int i = 0; i < numFiles; i++)
    {
        for (int j = 0; j < numFiles; j++)
        {
            if (collisions[i][j] >= plagiarismCutoff){
                counter++;
            }
        }
    }

    while(counter > 0)
    {
        max = 0;
        for(int r = 0; r < numFiles; r++)
            for(int c = 0; c < numFiles; c++)
                if(collisions[r][c] > max)
                {
                    max = collisions[r][c];
                    maxR = r;
                    maxC = c;
                }
        cout << max << ": " << files[maxR] << ", " << files[maxC] << endl;
        collisions[maxR][maxC] = 0;
        counter--;
    }

}

int main(int argc, char *argv[])
{
    const unsigned int HASH_SIZE = 3000017;                     //Constant size or hash table
    string dir = string(argv[1]);
    vector<string> files;
    vector<string> words;
    vector<string> chunked;
    vector<vector<int>> hashTable(HASH_SIZE,vector<int>(0));
    int chunkSize = atoi(argv[2]);
    int plagSize = atoi(argv[3]);

    getdir(dir,files);

    files.erase(files.begin());                                 //First two files are unnecessary
    files.erase(files.begin());
    vector<vector<int>> collisions(files.size(),vector<int>(files.size(),0));

    for (int i = 0; i < files.size();i++) {                     //Loop that hashes every file from the directory
        words.clear();
        chunked.clear();
        getAllWords(files[i],words,dir);
        processChunks(words,chunkSize,chunked);
        hashValues(chunked,hashTable,HASH_SIZE,i);
    }

    checkCollisions(collisions,hashTable,HASH_SIZE);            //Check the resulting collisions and display
    outputResults(collisions,files.size(),plagSize,files);

    return 0;
}