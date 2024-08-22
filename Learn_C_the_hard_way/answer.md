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