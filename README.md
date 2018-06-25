# ipc_learning
实践进程间通信(IPC)的代码，主要参考*The Linux Programming Interface*(简称`TLPI`)的示例代码，使用自定义的简易错误处理库。
# 自定义编码风格
1. 命名风格
类命名采用大驼峰命名法，比如`MyData`;
函数命名采用驼峰命名法，比如`getName()`;
变量命名采用帕斯卡命名法，比如`my_data`;
宏/常量均采用大写字母和下划线集合的形式，比如`BUFFER_SIZE`;
不严格禁止缩写，比如`fd`代表`file descriptor`，`epfd`代表`epoll file descriptor`，但是像`num_read`这种并未缩写。
2. 大括号
左括号放在语句行末，右括号根据块内内容决定是否换行，若在同一行，需要和语句用空格隔开
```
void func() { printf("hello\n"); }
void func(int i, double d, const char* s) {
    printf("%d %f %s\n", i, d, s);
}
```
3. 缩进采用4个空格，双目运算符左右均留出一个空格。
4. 关于goto语句，仅在处理信号时使用goto语句。
5. 对于简单变量的检查，把if语句和xxxExit语句放在同一行；对于包含函数调用的条件语句，把if语句和xxxExit语句放在两行。
```
if (ret == -1) errorExit("func");
if (doSystemCall() == -1)
    errorSystemCall(errno, "doSystemCall");
```

# 简易错误处理库([error_handler.hpp](include/error_handler.hpp))
- `errorMsg(const char*, ...)`：相当于`fprintf`打印参数内容后再打印换行符；
- `errorMsg(error, const char*, ...)：在打印换行符之前打印`": *errormsg*"`，*erromsg*是错误码`error`对应的信息；
- `errorExit(...)`：将参数传入`errorMsg`打印信息后，`exit(1)`终止程序。

最开始考虑过`errorExitIf(bool, ...)`这种API，但发现挤在一行会使代码变得难以阅读，于是放弃之，若用户想自定义这样的API，可以简单地用下述宏定义的方式
```
#define errorExitIf(b, ...) if (b) { errorExit(__VA_ARGS__); }
```
