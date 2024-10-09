[Learn_C_the_hard_way](https://wizardforcel.gitbooks.io/lcthw/content/preface.html)
###### 7
Q:把为universe_of_defects赋值的数改为不同的大小，观察编译器的警告。

A:没什么警告

Q:这些巨大的数字实际上打印成了什么？

A:printf将格式化后的字符串输出到标准输出
Q:将long改为unsigned long，并试着找到对它来说太大的数字。

A:ex7.c:10:87: warning: overflow in expression; result is 0 with type 'long' [-Winteger-overflow]
    unsigned long universe_of_defects = 1024L * 1024L * 1024L * 1024L * 1024L * 1024L * 1024L;
                                                                                      ^
1 warning generated.

Q:上网搜索unsigned做了什么。

A:将变量变为了无符号数

Q:试着自己解释（在下个练习之前）为什么char可以和int相乘。

A:This is because, in C programming, characters are internally stored as integer values known as ASCII Values

###### 8
Q:上网搜索在不同的CPU上整数所占的不同大小。

A:
16位处理器中的int 占有16位 即2个字节
32位处理器中int占有32位 即4个字节
64位处理器中int占有64位 即8个字节

###### 9

Q:将一些字符赋给numbers的元素，之后用printf一次打印一个字符，你会得到什么编译器警告？

A:
ex1.c: In function ‘main’:
ex1.c:61:19: warning: format ‘%s’ expects argument of type ‘char *’, but argument 2 has type ‘int’ [-Wformat=]
   61 |     printf("test:%s",numbers[0]);
      |                  ~^  ~~~~~~~~~~
      |                   |         |
      |                   char *    int
      |                  %d
强行运行会有 segmentation fault

Q:对names执行上述的相反操作，把names当成int数组，并一次打印一个int，Valgrind会提示什么？

A:没有提示(?)

Q:有多少种其它的方式可以用来打印它？

A:任何大于int长度的占位符应该都可以用来打印它

Q:如果一个字符数组占四个字节，一个整数也占4个字节，你可以像整数一样使用整个name吗？你如何用黑魔法实现它？

A:没听明白问题

Q:拿出一张纸，将每个数组画成一排方框，之后在纸上画出代码中的操作，看看是否正确。

A:...

Q:将name转换成another的形式，看看代码是否能正常工作。

A:可以输出，但name输出结果很奇怪

###### 11
Q:研究你是否真正复制了这些字符串？答案可能会让你感到意外和困惑。

A:通过打印指针地址可以看出复制的仅仅是地址，只是让两个指针指向了同一个元素而已。

###### 13
Q:在“Y”的例子中，我在if代码块外面写了个break。这样会产生什么效果？如果把它移进if代码块，会发生什么？自己试着解答它，并证明你是正确的。

A:
类似
```
for()
{
   switch():
      if()
         break;
}
```
的结构，其中的break不会使for提前终止循环，其仍起到终止switch的作用。

###### 16
Q:试着传递NULL给Person_destroy来看看会发生什么。如果它没有崩溃，你必须移除Makefile的CFLAGS中的-g选项。

A:ex1: ex1.c:28: Person_destroy: Assertion `who != NULL' failed.
Aborted (core dumped)

Q:在结尾处忘记调用Person_destroy，在Valgrind下运行程序，你会看到它报告出你忘记释放内存。弄清楚你应该向valgrind传递什么参数来让它向你报告内存如何泄露。

A:
==4108539== LEAK SUMMARY:
==4108539==    definitely lost: 48 bytes in 2 blocks
==4108539==    indirectly lost: 21 bytes in 2 blocks
==4108539==      possibly lost: 0 bytes in 0 blocks
==4108539==    still reachable: 0 bytes in 0 blocks
==4108539==         suppressed: 0 bytes in 0 blocks

definitely lost：表示有48字节的内存在程序结束时仍然无法访问到，存在内存泄漏。
indirectly lost：表示有21字节的内存在程序结束时无法访问到，尽管这些内存本身没有泄漏，但与泄漏的内存相关联。

通过Person_create函数创建的两个Person结构体实例中的name成员，每个实例泄漏了24字节（Frank Blank和Joe Alex字符串）。这解释了为什么definitely lost为48字节。
通过strdup函数复制的name字符串，这导致每个实例相关联的21字节的indirectly lost。
要修复这些泄漏问题，需要在适当的时候释放内存。

Q:这一次，向Person_print传递NULL，并且观察Valgrind会输出什么。
A:
==4111521== Invalid read of size 8
==4111521==    at 0x1092CA: Person_print (in /home/lighthouse/ex1)
==4111521==    by 0x109459: main (in /home/lighthouse/ex1)
==4111521==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
这个错误的原因可能是Person_create函数中的strdup函数调用失败，无法为name成员分配内存。这导致who->name被设置为NULL，然后在调用Person_print函数时尝试读取NULL指针。

Q:如何在栈上创建结构体，就像你创建任何其它变量那样。如何使用x.y而不是x->y来初始化结构体。
A:直接在main函数中创建两个结构体变量joe和frank，并使用点操作符（.）来初始化结构体成员。(在栈上直接在函数中使用结构体即可，不用Malloc)


Q:如何不使用指针来将结构体传给其它函数。
A:直接传副本就好
###### 17
实际情况是，除char外，所有其他类型都有“对齐要求”：char可起始于任意字节地址，2字节的short必须从偶数字节地址开始，4字节的int或float必须从能被4整除的地址开始，8比特的long和double必须从能被8整除的地址开始。无论signed（有符号）还是unsigned（无符号）都不受影响。

[失传的C结构体打包技艺](https://www.cnblogs.com/likaiming/p/9009495.html)
###### 18
Q:将错误的函数传给compare_cb，并看看C编辑器会报告什么错误。
A:Incompatible function pointer types passing 'void (const char *)' to parameter of type 'compare_cb' (aka 'int (*)(int, int)')
Q:将NULL传给它，看看程序中会发生什么。然后运行Valgrind来看看它会报告什么。
A:运行会发生segmentation fault。Valgrind显示传递了无效参数，导致内存泄漏。

###### 22
Q:移除ex22.h的extern声明，来观察会得到什么错误或警告。
A:没有错误或警告。。,头文件不去掉不是没效果吗？
Q:编写一个递归调用并导致栈溢出的函数。如果不知道递归函数是什么的话，试着在scope_demo底部调用scope_demo本身，会形成一种循环。
A:segmentation fault