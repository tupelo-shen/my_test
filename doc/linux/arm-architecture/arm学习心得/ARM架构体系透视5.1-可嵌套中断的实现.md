传统ARM中IRQ是作为一种系统异常出现的。对于ARM核来说，有且仅有一个称为IRQ的系统异常。而ARM对于IRQ的处理一般通过异常向量找到IRQ的中断处理程序。当进入IRQ中断处理程序之后，ARM自动屏蔽IRQ，也就是说在中断响应过程中是忽略之后到来的中断请求的。即使使用了VIC，VIC也仅仅是悬起后来的中断请求。也就是说，传统ARM的中断是不可嵌套、不可抢占的。

不过，ARM给了我们一种权利，那就是在中断处理程序中可以手动打开IRQ，这样在前一个IRQ响应的过程中，就可以被后来的中断所打断。就给我们提供了一种用软件解决中断嵌套的途径。

中断的过程我们都十分清楚：`保护现场->响应中断->恢复现场`。ARM对于每一种异常都有相应的堆栈寄存器，且会自动切换，互不影响。所以自然而然地，在嵌套中，我们可以用SP_irq来保护现场和恢复现场。流程如下所示：

1. 第一次进入中断
2. ARM自动保存CPSR到SPSR_irq
3. ARM自动保存PC到LR_irq
4. ARM自动关闭IRQ使能
5. 保存通用寄存器、LR_irq、SPSR_irq到SP_irq所指示的堆栈中
6. 打开IRQ使能位
7. 根据中断号进行中断服务
    * 7.1 第二次进入中断，开始嵌套
    * 7.2 ARM自动保存CPSR到SPSR_irq
    * 7.3 ARM自动保存PC到LR_irq
    * 7.4 ARM自动关闭IRQ使能
    * 7.5 保存通用寄存器、LR_irq、SPSR_irq到SP_irq所指示的堆栈中
    * 7.6 打开IRQ使能位
    * 7.7 根据中断号进行中断服务
    * 7.8 关闭IRQ使能位
    * 7.9 从SP_irq堆栈中恢复通用寄存器、LR_irq、SPSR_irq
    * 7.10 通过LR_irq跳转回到第7步
    * 7.11 ARM自动从SPSR_irq恢复CPSR
8. 关闭IRQ使能位
9. 从SP_irq堆栈中恢复通用寄存器、LR_irq、SPSR_irq
10. 通过LR_irq跳转回到到任务程序
11. ARM自动从SPSR_irq恢复CPSR

这样就实现了嵌套，而且只要堆栈够大，可以嵌套很多层。不考虑优先级，或者把优先级教给中断控制器管理，这样已经不错了吧，虽然不愿意这么说，但是问题还是来了。

在上面的流程中，有一步是根据中断号进行中断服务。对于不同的中断源，我们一般都会用不同函数来写中断服务，这样不仅清晰，也利于将不同功能的模块分割开。这样我们就需要将这步变为：根据中断号进入服务子程序。这步中，我们会牵涉到函数调用。在函数调用过程中，一般都会先将PC保存在LR_irq中，在返回时，再将LR_irq恢复到PC。这也正是LR的作用所在。

正是这个事实，导致了问题的发生。想象这种情况：当我们进入服务子程序后，此时LR_irq正是我们程序的返回地址。这时，第二个中断到来了，回忆一下中断发生时ARM自动做了什么，ARM将PC保存到了LR_irq中！就这样，LR_irq被篡改了，因为我们无法预料到中断什么时候到来，我们也就根本无法保存这个被篡改的LR_irq。程序响应好第二个中断后，一路返回到这个LR_irq，毫无意外的，就跑飞了。

很扫兴吧，不过我们自然有办法解决这个问题。办法就是在进入服务子程序之前，先将系统转换到SVC状态，这样，子程序被调用时返回地址就会被保存在LR_svc中，也就不会再被第二个中断所篡改。流程如下，和第一次不同的地方都用红色标注。

为什么要保存r0-r3, 保存lr_svc不就够了吗？

1. 第一次进入中断
2. ARM自动保存CPSR到SPSR_irq
3. ARM自动保存PC到LR_irq
4. ARM自动关闭IRQ使能
5. 保存通用寄存器、LR_irq、SPSR_irq到SP_irq所指示的堆栈中
6. <font color="blue">更改系统状态为SVC</font>
7. <font color="blue">保存R0-R3，LR_svc到SP_svc所指示的堆栈中</font>
8. <font color="blue">打开IRQ使能位</font>
9. <font color="blue">根据中断号进行中断服务</font>
    * 9.1 第二次进入中断，开始嵌套
    * 9.2 ARM自动保存CPSR到SPSR_irq
    * 9.3 ARM自动保存PC到LR_irq
    * 9.4 ARM自动关闭IRQ使能
    * 9.5 保存通用寄存器、LR_irq、SPSR_irq到SP_irq所指示的堆栈中
    * 9.6 <font color="blue">更改系统状态为SVC</font>
    * 9.7 <font color="blue">保存R0-R3，LR_svc到SP_svc所指示的堆栈中</font>
    * 9.8 打开IRQ使能位
    * 9.9 根据中断号进行中断服务
    * 9.10 关闭IRQ使能位
    * 9.11 <font color="blue">从SP_svc所指示的堆栈中恢复R0-R3，LR_svc</font>
    * 9.12 <font color="blue">更改系统状态为IRQ</font>
    * 9.13 从SP_irq堆栈中恢复通用寄存器、LR_irq、SPSR_irq
    * 9.14 通过LR_irq跳转回到第9步
    * 9.15 ARM自动从SPSR_irq恢复CPSR
10. 关闭IRQ使能位
11. <font color="blue">从SP_svc所指示的堆栈中恢复R0-R3，LR_svc</font>
12. <font color="blue">更改系统状态为IRQ</font>
13. 从SP_irq堆栈中恢复通用寄存器、LR_irq、SPSR_irq
14. 通过LR_irq跳转回到到任务程序
15. ARM自动从SPSR_irq恢复CPSR

这样我们既可以用中断服务子程序，也不怕LR被篡改了。我们再来看一下嵌套过程中的堆栈使用情况。在进入SVC状态之前，使用IRQ的堆栈，保存嵌套所需的通用寄存器、LR_irq和SPSR_irq。进入SVC状态之后，使用SVC堆栈，需要保存调用函数规定的R0-R3，LR_svc。当然在中断服务例程中，也是使用SVC堆栈。可见两个状态的堆栈都被使用了。当然，因为中断服务例程使用SVC堆栈，我们也可以考虑将嵌套所需的堆栈也放到SVC中，这样就不需要IRQ堆栈了。流程上和前面这种方法很相似，只不过要将保存LR_irq和SPSR_irq的时间放到进入SVC态之后，方法可以是通过通用寄存器拷贝。最后也不必再返回IRQ态，可以直接通过SPSR_svc和LR_svc来推出中断处理程序。

程序贴在下面，用的是堆栈分开的方法，只是示例。

```c
__asm void IRQ_Handler(void){
      PRESERVE8
      IMPORT handler1    
     
      // STORE LR_irq & SPSR_irq
      SUB             LR, LR, #4
      MRS             R0, SPSR
      STMFD           SP!, {R0, LR}
 
      // INTO SVC MODE
      MRS             R0, CPSR
      BIC             R0, #0x1f
      ORR             R0, #0x13
      MSR             CPSR_C, R0
     
      // STORE REGISTORS OF SVC MODE
      STMFD           SP!, {R0-R3,LR}
     
      // ENABLE IRQ
      MRS             R0, CPSR
      BIC             R0, #0x80
      MSR             CPSR_C, R0
     
      // GO TO HANDLER
      BL              handler1
     
      // RESTORE REGISTORS OF SVC MODE
      LDMFD           SP!, {R0-R3,LR}
     
      // DISABLE IRQ
      MRS             R0, CPSR
      ORR             R0, #0x80
      MSR             CPSR_C, R0      
     
      // INTO IRQ MODE
      MRS             R0, CPSR
      BIC             R0, #0x1f
      ORR             R0, #0x12
      MSR             CPSR_C, R0
     
      // RESTORE LR_irq & SPSR_irq
      LDMFD           SP!, {R0, LR}
      MSR             SPSR_CFX, R0
     
  // EXIT IRQ
      MOVS            PC, LR 
}
```