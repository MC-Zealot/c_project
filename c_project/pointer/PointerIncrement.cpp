#include <iostream>

using namespace std;
const int MAX = 3;

int main ()
{
		int  var[MAX] = {10, 100, 200};
		int  *ptr;

		// 指针中的数组地址, 因为var是一个数组，所以，var为地址,默认第一个元素的地址，&var[0]就是第一个元素的地址
		ptr = var;
		cout << "var: " << &var << endl;
		cout << "var: " << var << endl;
		ptr = &var[0];
		for (int i = 0; i < MAX; i++)
		{
				cout << "Address of var[" << i << "] = ";
				cout << ptr << endl;

				cout << "Value of var[" << i << "] = ";
				cout << *ptr << endl;

				// 移动到下一个位置
				ptr++;
		}
		return 0;
}
