//
//  VarType.cpp
//  c_project
//
//  Created by Zealot on 2016/11/7.
//  Copyright © 2016年 Zealot. All rights reserved.
//

#include <stdio.h>

#include "iostream"

using namespace std;
// 变量声明
extern int a, b;
extern int c;
extern float f;
int a,b;//全局变量
static int a1,b1;//静态全局变量
const int a2=23;//全局常量
const static int a3=22;//静态全局常量
char*s3;
char*s="hello";//s是全局变量，"hello"在常量区
int main()
{
    cout << "Hello World!!\n";
	// 变量定义
	int a, b;
	int c;
	float f;
		   
	// 实际初始化
	a = 10;
	b = 20;
	c = a + b;
				    
	cout << c << endl;
	f = 70.0/3.0;
	cout << f << endl;
	cout << a3 << endl;				   
	return 0;
}

