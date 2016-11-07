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
				    
	cout << c << endl ;
	f = 70.0/3.0;
	cout << f << endl ;
					   
	return 0;
}

