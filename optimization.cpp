#include"global.h"
/*
传参数问题：



外层的参数作为全局变量。。
if_used:
	
将整个程序变成一个个过程。
全局寄存器：ESI,EDI,EBX;
特殊寄存器：ESP,EBP;
临时寄存器：EAX，EDX，ECX
x86maker里面：每个变量都有了他的地址，或者提示他变量在寄存器里。已经准备填写全部地址这样可以用地址123分别标记寄存器用一个map做标记
map<int,string>

在后层用到的变量不参与全局寄存器的分配;如果寄存器足够，就放到寄存器里，否则不参与分配，提出去
在后层用到的变量不参与全局寄存器的分配;
在后层用到的变量不参与全局寄存器的分配;


在符号表中给每个局部变量做标记，标记他们是否在子模块中被使用。如果没有在子模块中使用，并且活跃完了，就不需要在栈上分配空间了，这些在优化的时候就可以知道。

对于临时变量，如果在消除了公共子表达式之后还存在的，就给他在栈上分配一块空间，否则久不分配。

对于局部变量并且没有分配寄存器的，就在栈上分配一块空间。

函数四元式容器。`

//////////////////////////////
保留Display区?
他可不是自己就能跑上去的。
会有一个循环在的。
避免一个循环查找的过程就是
牺牲内存了拉。。。
所以还是要保留Display区。

真不知道你还有自虐倾向，明明可以早睡却还不。
display区;
参数区[ebp+8...]
上层全局寄存器区域[ebp+8+4*N...]
返回地址[ebp+4]
saved ebp.
ret value.[ebp-4]
局部变量区域[ebp-8]
临时变量区域
本来在全局寄存器不得已到这的局部变量区域




先找出所有能分配全局寄存器的临时变量。
然后选几个放到
给所有
栈向下增长




Saved esp


*/