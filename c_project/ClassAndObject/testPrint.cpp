#include<stdio.h> 
int main(void) 
{ 
char s[40];

sprintf(s,"%s%d%c","test",1,'2'); 
/*第一个参数就是指向要写入的那个字符串的指针，剩下的就和printf()一样了

你可以比较一下，这是向屏幕输入 
printf("%s%d%c","test",1,'2');

*/ 
printf("%s%d%c\n","test",1,'2');
char ch[50];
sprintf(ch,"%u-1", 3);//
printf(ch);
return 0; 
}
