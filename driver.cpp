#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

/*function... might want it in some class?*/
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

void getAllWords(string fileName, vector<string> &words, string path)
{
    string pathName = path + "/" + fileName;
    ifstream inFile;
    inFile.open(pathName);
    string word;

    inFile >> word;
    while(inFile)
    {
        words.push_back(word);
        inFile >> word;
    }

    inFile.close();
}

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
    }

    return value;
}

void processChunks(vector<string> words, int n)
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
        cout << chunk << "\n";
    }
}

int main()
{
    string dir = string("sm_doc_set");
    vector<string> files = vector<string>();
    vector<string> words = vector<string>();
    int chunkSize = 6;

    getdir(dir,files);
/*
    for (int i = 2;i < files.size();i++) {
        words.clear();
        getAllWords(files[i],words,dir);
        processChunks(words,chunkSize);
    } */
    getAllWords(files[2],words,dir);
    processChunks(words,chunkSize);
    return 0;
}