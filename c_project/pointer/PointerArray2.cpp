#include <iostream>

using namespace std;
const int MAX = 3;

int main ()
{
		int  var[MAX] = {10, 100, 200};
		cout << var[2] << endl;
		*(var + 2) = 500;
		cout << var[2] << endl;
		for (int i = 0; i < MAX; i++)
		{
				*var = i;    // 这是正确的语法
//				var++;       // 这是不正确的
				cout << *var << endl;
		}
		return 0;
}
