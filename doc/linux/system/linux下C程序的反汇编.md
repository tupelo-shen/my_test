Linux下反汇编的2种方法：
    
    gcc -S -o main.s main.c             // 直接编译生成汇编文件

或，

    objdump -s -d main > main.txt       // 可执行文件的反汇编
    objdump -s -d main.o > main.o.txt   // 目标文件的反汇编
