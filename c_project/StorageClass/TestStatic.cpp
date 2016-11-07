#include <iostream>
 
 // 函数声明 
void func(void);
 
static int count = 10; /* 全局变量 */ //变量的作用域限制在声明它的文件内
   
int main()
{
   	while(count--)
   	{
	   func();
	}
	return 0;
}
// 函数定义
void func( void )
{
	static int i = 5; // 局部静态变量   在函数调用之间保持局部变量的值 
	i++;
	std::cout << "局部静态变量 i 为 " << i ;
    std::cout << " , 全局静态变量 count 为 " << count << std::endl;
}
