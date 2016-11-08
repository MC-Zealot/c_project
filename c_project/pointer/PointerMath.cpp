#include <iostream>

using namespace std;

int main()
{
	//int a = 1;
	char cc[] = {'a','b'};
	cout << cc<< endl;
	int *p;
	p=&cc;

	cout << "cc address: "<<&cc << endl;
	cout << &p << ": " << *p << endl;
	p++;
	cout << &p << ": " << *p << endl;
	cout << cc << endl;
	return 0;
}
