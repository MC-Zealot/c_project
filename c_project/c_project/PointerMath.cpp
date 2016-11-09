//
//  PointerMath.cpp
//  c_project
//
//  Created by Zealot on 2016/11/8.
//  Copyright © 2016年 Zealot. All rights reserved.
//

#include <stdio.h>
#include <iostream>

using namespace std;

int main()
{
    
    char cc = 'a';
    char *p;
    p=&cc;
    
    cout << "cc address: "<<&cc << endl;
    cout << &p << ": " << *p << endl;
    p++;
    cout << &p << ": " << *p << endl;
    
    cout << cc << endl;
    
    int  var[3] = {10, 100, 200};
    
    
    return 0;
}
