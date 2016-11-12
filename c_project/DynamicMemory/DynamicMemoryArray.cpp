#include <iostream>
using namespace std;
namespace MySpace{
	int count = 0;
}
class Box
{
		public:
				Box() { 
						cout << "调用构造函数！" << MySpace::count++ <<endl; 
				}
				~Box() { 
						cout << "调用析构函数！" << MySpace::count << endl; 
				}
};

int main( )
{
		Box* myBoxArray = new Box[4];

		delete [] myBoxArray; // Delete array

		return 0;
}
