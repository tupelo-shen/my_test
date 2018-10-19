Python 中，一个变量的作用域总是由在代码中被赋值的地方所决定的。

* 函数定义了本地作用域，而模块定义的是全局作用域。如果想要在函数内定义全局作用域，需要加上global修饰符。

* 变量名解析：LEGB原则

    当在函数中使用未认证的变量名时，Python搜索４个作用域:

    * 本地作用域(L)(函数内]部声明但没有使用global的变量)，
    * 之后是上一层结构中def或者lambda的本地作用域(E),
    * 之后是全局作用域(G)（函数中使用global声明的变量或在模块层声明的变量），
    * 最后是内置作用域(B)（即python的内置类和函数等）

    并且在第一处能够找到这个变量名的地方停下来。如果变量名在整个的搜索过程中都没有找到，Python就会报错。

    **补：上面的变量规则只适用于简单对象，当出现引用对象的属性时，则有另一套搜索规则:属性引用搜索一个或多个对象，而不是作用域，并且有可能涉及到所谓的"继承"**

以上基于http://blog.csdn.net/carolzhang8406/article/details/6855525

下面探讨`global`修饰符的用法：

首先是`python`的一个奇异现象，在模块层面定义的变量（无需`global`修饰），如果在函数中没有再定义同名变量， 可以在函数中当做全局变量使用：

    hehe=6
    def f():
        print(hehe)
    f()
    print(hehe)

上述代码可以正常运行且输出为6和6

但如果在函数中有再赋值/定义（因为`python`是弱类型语言，赋值语句和其定义变量的语句一样），则会产生引用了未定义变量的错误：

    hehe=6
    def f():
        print(hehe)
        hehe=2
    f()
    print(hehe)

抛出的错误信息为：

    UnboundLocalError: local variable 'hehe' referenced before assignment

而如果在函数中的定义在引用后定义，那么会正常运行但函数中的变量和模块中定义的全局变量不为同一个

    hehe=6
    def f():
        hehe=2
        print(hehe)
    f()
    print(hehe)

上述输出是`2`和`6`，也即`f`函数中`print`使用的是局部变量`hehe`，而最后一个`print`语句使用的全局`hehe`。

那么我们会有疑问，如果我可能在函数使用某一变量后又对其进行修改（也即再赋值），怎么让函数里面使用的变量是模块层定义的那个全局变量而不是函数内部的局部变量呢？这时候`global`修饰符就派上用场了。

    hehe=6
    def f():
        global hehe
        print(hehe)
        hehe=3
    f()
    print(hehe)

在用`global`修饰符声明`hehe`是全局变量的`hehe`后（注意，`global`语句不允许同时进行赋值如`global hehe=3`是不允许的），上述输出是`6`和`3`，得到了我们想要的效果。