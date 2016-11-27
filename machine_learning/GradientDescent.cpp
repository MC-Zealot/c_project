#include <iostream>
#include <string>

using namespace std;
double g(double x);
//x= x - alpha * g(x)
void fix_alpha_regression(){
	double x = 1.5;
	double d; //一阶导数
	double alpha = 0.01; //学习率
	int iterator = 1000;
	for(int i = 0; i < iterator; i++) {
		x = x - alpha * g(x);
		cout << i << ":\t" << x << endl;
	}
}

//y=x^4的导数，y=4x^3
double g(double x){
	return 4 * x * x * x;
}

int main ()
{

		fix_alpha_regression();
		return 0;
}
