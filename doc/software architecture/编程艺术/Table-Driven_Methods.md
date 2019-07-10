# what

表驱动方法（Table-Driven Methods），在《Unix 编程艺术》中有提到，《代码大全》的第十八章对此进行了详细地讲解。

> 表驱动法是一种编程模式（Scheme），从表里面查找信息而不使用逻辑语句（if 和case）
> 它的好处是消除代码里面到处出现的if、else、swith语句，让凌乱代码变得简明和清晰。
> 对简单情况而言，表驱动方法可能仅仅使逻辑语句更容易和直白，但随着逻辑的越来越复杂，表驱动法就愈发有吸引力。

# why

先通过一个简单的例子体验下，在某些情况下，如果不使用表驱动方法，代码会如何地难看。

假设让你实现一个返回每个月天数的函数（为简单起见不考虑闰年）。

初级码农的笨方法是马上摆出 12 副威武雄壮的 if-else 组合拳：

    int iGetMonthDays(int iMonth){
        int iDays;

        if(1 == iMonth) {iDays = 31;}
        else if(2 == iMonth) {iDays = 28;}
        else if(3 == iMonth) {iDays = 31;}
        else if(4 == iMonth) {iDays = 30;}
        else if(5 == iMonth) {iDays = 31;}
        else if(6 == iMonth) {iDays = 30;}
        else if(7 == iMonth) {iDays = 31;}
        else if(8 == iMonth) {iDays = 31;}
        else if(9 == iMonth) {iDays = 30;}
        else if(10 == iMonth) {iDays = 31;}
        else if(11 == iMonth) {iDays = 30;}
        else if(12 == iMonth) {iDays = 31;}

        return iDays;
    }

稍微机灵点的码农发现每月天数无外乎 28、30、31 三种，或许会用 switch-case “裁剪”下：

    int iGetMonthDays(int iMonth){
        int iDays;

        switch (iMonth) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:{iDays = 31;break;}
            case 2:{iDays = 28;break;}
            case 4:
            case 6:
            case 9:
            case 11:{iDays = 30;break;}
        }

        return iDays;
    }

这两种方法充斥了大量的逻辑判断，还凭空冒出了一大堆1，2，...，11，12这样的 Magic Number（魔鬼数字公然出现在程序里是很 ugly 的做法），不利于代码的维护与扩展。

表驱动处理起来就赏心悦目得多了：

    static int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    int iGetMonthDays(int iMonth){
        return monthDays[(iMonth - 1)];
    }

表驱动可以使你的代码更简洁，结构更加灵活，多用于逻辑性不强但是分支多的情况。

如何使用表驱动法？需要明确两个关键问题：

* 表的形式及表中放什么内容
    * 表形式可以为一维数组、二维数组和结构体数组。
    * 表中可以存放数值、字符串或函数指针等数据。
* 如何去访问表。

下面介绍表的三种访问方式：

* 直接访问

    直接根据“键”来获得“值”，给定下标 index，然后array[index]就获得数组在相应下标处的数值。例如前面这个根据月份取天数的例子。

* 索引访问

    它适用于这样的情况：假设你经营一家商店，有 100 种商品，每种商品都有一个 ID 号，但很多商品的描述都差不多，所以只有 30 条不同的描述，如何建立建立商品与商品描述的表？

    还是同上面做法来一一对应吗？那样描述会扩充到 100 个，会有 70 个描述是重复的！太浪费了。

    方法是建立一个 100 长的索引和 30 长的描述，然后这些索引指向相应的描述（不同的索引可以指向相同的描述），这样就解决了表数据冗余的问题啦。

        struct product_t {
            char * id;
            int desc_index;
        };

        const char * desc[] = {
            "description_1",
            "description_2",
            ...
            "description_29",
            "description_30"
        };

        const product_t goods [] = {
            {"id_1", 3},
            {"id_2", 1},
            ...
            {"id_99", 12},
            {"id_100", 5}
        };

        const char* desc_product (const char* id) {
            for (const product_t & p : goods) {
                if (strcmp(p.id, id) == 0) {
                    return desc[p.desc_index - 1];
                }
            }

            return NULL;
        }

* 阶梯访问

    例子：将百分制成绩转成五级分制（我们用的优、良、中、合格、不合格，西方用的 A、B、C、D和F），假定转换关系:

    | Score | Degree |
    | ------ | ------ |
    | [90-100] | A |
    | [80,90) | B |
    | [70,80) | C |
    | [60,70) | D |
    | [0,60) | E |

    如何用表格表示这些范围？你当然可以用第一种直接访问的方法：申请一个 100 长的表，然后在这个表中填充相应的等级。很明显，也会浪费大量空间，有没有更好的方法？

    对于这种“某个范围区间内，对应某个值”的逻辑规则，可用阶梯访问的方式。

        const char gradeTable[] = {
        'A', 'B', 'C', 'D', 'F'
        };

        const int downLimit[] = {
            90, 80, 70, 60
        };

        int degree(int score)
        {
            int gradeLevel = 0;
            char lowestDegree = gradeTable[sizeof(gradeTable)/sizeof(gradeTable[0]) - 1];

            // 这里可用二分查找优化
            while (gradeTable[gradeLevel] != lowestDegree) {
                if(score < downLimit[gradeLevel]) {
                    ++ gradeLevel;
                } else {
                    break;
                }
            }

            return gradeTable[gradeLevel];
        }

    将来如果等级规则变了（比如 85～100 分为等级 A，或添加 50～60 分为等级 E），只需要修改 gradeTable 和 downLimit 表就行，degree 函数可以保持一行都不改动。

    更进一步地，gradeTable 和 downLimit 表还可以配置文件的形式表示，主程序从外部文件 load 进来就行，程序灵活性大大增加。


# review

伟大的 C 语言大师 Rob Pike 有句话说的好：

    数据压倒一切。如果选择了正确的数据结构并把一切组织的井井有条，正确的算法就不言自明。编程的核心是数据结构，而不是算法。

对人类来说，数据比编程逻辑更容易驾驭。在复杂数据和复杂代码中选择，宁可选择前者。

更进一步，在设计中，应该主动将代码的复杂度转移到数据中去。

这里谈到了 Unix 哲学之分离原则：

    策略同机制分离

机制，即提供的功能。

策略，即如何使用功能。

以百分制转五级分制为例，机制就是 degree 函数：你给一个百分制分数给它，它吐出来一个五级分制给你。策略就是gradeTable 和 downLimit 这两个表，它规定了哪个区间的分数对应哪个等级。

从 degree 的实现可以看出:对机制而言，策略是透明的（degree 完全看不到 gradeTable 和 downLimit 这两个表的内部规则）。

将两者分离，可以使机制(degree)相对保持稳定，而同时支持策略(表)的变化。