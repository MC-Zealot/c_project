#include <iostream>

using namespace std;

int main()
{
	//int a = 1;
	int cc[] = {1,2,3};
	cout << cc<< endl;
	int *p;
	p=cc;

	cout << "cc address: "<<&cc << endl;
	cout << &p << ": " << *p << endl;
	p++;
	cout << &p << ": " << *p << endl;
	cout << cc << endl;
	return 0;
}
