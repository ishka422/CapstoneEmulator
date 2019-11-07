#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

typedef unsigned char BYTE;

struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
	return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
	return HexCharStruct(_c);
}


long getFileSize(FILE *file)

{
	
		long lCurPos, lEndPos;
	
		lCurPos = ftell(file);
	
		fseek(file, 0, 2);
	
		lEndPos = ftell(file);
	
		fseek(file, lCurPos, 0);
	
		return lEndPos;
	
}


int main(int argc, char *argv[]) {

	if (argc > 1) {


		cout << argc << "\n";
		cout << argv[0] << "\n";
		ifstream gameROM;
		gameROM.open(argv[1], ios::in | ios::binary | ios::ate);

		int size = gameROM.tellg();

		char *memblock = new char[size];

		gameROM.seekg(0, ios::beg);

		gameROM.read(memblock, size);

		for (int i = 0; i < size; i++) {
			if (memblock[i] == 0x7e) {
				cout << hex(memblock[i]) << endl;
			}
		}
		
		//char contents[static_cast<size_t>(end - start)];

		//contents.resize(static_cast<size_t>(end - start));
		
		getchar();
		return 0;
	}
	else {
		cout << "Must enter file";
		cout << argv[0] << "\n";
		getchar();
		return 0;
	}


}