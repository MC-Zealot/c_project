//
//  hello.cpp
//  c_project
//
//  Created by Zealot on 2016/11/7.
//  Copyright © 2016年 Zealot. All rights reserved.
//

#include "hello.hpp"

#include <iostream>
#define MAX_VALUE 100
using namespace std;
int main()
{
    cout << "Hello World!!\n";
    int a = 2147483647;
    unsigned int b = 4294967295;
    wchar_t c = 'A';
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    
    
    cout << "Size of char : " << sizeof(char) << endl;
    cout << "Size of int : " << sizeof(int) << endl;
    cout << "Size of short int : " << sizeof(short int) << endl;
    cout << "Size of long int : " << sizeof(long int) << endl;
    cout << "Size of float : " << sizeof(float) << endl;
    cout << "Size of double : " << sizeof(double) << endl;
    cout << "Size of wchar_t : " << sizeof(wchar_t) << endl;
    
//    您可以使用 typedef 为一个已有的类型取一个新的名字。下面是使用 typedef 定义一个新类型的语法：666
    typedef int feet;
    feet f = 5;
    cout << "feet: " << f << endl;
    
    
//    "枚举"是指将变量的值一一列举出来，变量的值只能在列举出来的值的范围内
    enum color { red, green, blue , yellow = 5} clr;
    
    clr = blue;
    
    cout << clr << endl;
    
    clr = yellow;
    
    cout << clr << endl;
    const string s = "str";
//    s="123";
    cout << "s: " << s << endl;
    cout << MAX_VALUE << endl;
    return 0;
}
