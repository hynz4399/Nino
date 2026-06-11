#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    char inputStr[100]; // 定义一个字符数组来存储输入的内容

    // 提示用户输入
    printf("请输入一些内容: ");
    
    // 接收键盘输入（遇到空格或换行会停止）
    scanf("%99s", inputStr); 

    // 显示输出键盘输入内容
    printf("你刚才输入的内容是: %s\n", inputStr);

    return 0;
}