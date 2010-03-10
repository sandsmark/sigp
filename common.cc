#include <fstream>
#include <string>

using namespace std;

int getFileLength(string filename){
	int length;
	ifstream file;

	file.open(filename.c_str(), ios::binary | ios::in);

	file.seekg(0,ios::end);
	length = file.tellg();
	file.close();
	return length;
}

char* copyToMemory(string filename, int length){
	char* buffer;
	ifstream file;

	file.open(filename.c_str(), ios::binary | ios::in);

	buffer = new char[length];
	file.read(buffer,length);
	file.close();
	
	return buffer;
}

