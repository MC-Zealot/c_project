#include <iostream>
#include <string>

using namespace std;
double g(double x);
double get_Alpha_Armijo(double x, double d, double alpha);//获取alpha步长
double f(double x);
//固定步长,x= x - alpha * g(x)
//alpha为0.01， 经过1000次迭代，极值为0.1112，效果不好
//alpha为0.001，经过20000次迭代，极值为0.0789，效果一般，迭代次数过多
//alpha为0.001，经过40000次迭代，极值为0.0558，效果一般，迭代次数过多
//alpha为0.001，经过60000次迭代，极值为0.0456，效果一般，迭代次数过多
void fix_alpha_regression(){
	double x = 1.5;
	double d; //一阶导数
	double alpha = 0.001; //学习率
	int iterator = 20;
	for(int i = 0; i < iterator; i++) {
		d=g(x);
		alpha = get_Alpha_Armijo(x,d,alpha);
		x = x - alpha * d;
		cout << i << ", " << alpha << ", " << x << endl;
	}
//	cout << iterator << ":\t" << x << endl;
}

//y=x^4的导数，y=4x^3
double g(double x){
	return 4 * x * x * x;
}
double f(double x){
	return x * x * x* x;	
}
/**
 *基于Armijo准则，获取步长
 *
 */
double get_Alpha_Armijo(double x, double d, double alpha){
	double c1 = 0.3;
	double now = f(x);
	double next = f(x - alpha * d);
	//如果h'(x)<0
	int count = 30;
	while(next < now && count >= 0){
		alpha *= 2;
		next = f(x - alpha * d);
		count--;
	}
	
	count = 50;

	while(next > now - c1 * alpha * d * d && count >= 0){
		alpha = alpha/2;
		next = f(x - alpha * d);
		count--;
	}
	return alpha;
}
int main ()
{

		fix_alpha_regression();
		return 0;
}
