#include <iostream>
#include <string>

using namespace std;
double g(double x);
//固定步长,x= x - alpha * g(x)
//alpha为0.01， 经过1000次迭代，极值为0.1112，效果不好
//alpha为0.001，经过20000次迭代，极值为0.0789，效果一般，迭代次数过多
//alpha为0.001，经过40000次迭代，极值为0.0558，效果一般，迭代次数过多
//alpha为0.001，经过60000次迭代，极值为0.0456，效果一般，迭代次数过多
void fix_alpha_regression(){
	double x = 1.5;
	double d; //一阶导数
	double alpha = 0.001; //学习率
	int iterator = 60000;
	for(int i = 0; i < iterator; i++) {
		x = x - alpha * g(x);
	}
	cout << iterator << ":\t" << x << endl;
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
