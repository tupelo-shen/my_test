[TOC]

响应时间（或延迟）对于深入理解系统性能是一个很重要的维度。但是，大部分的命令行表示方式无法清晰有效地展现出来。而Latency热图是展示的一种有效方式：显示分布情况、异常值和其它的一些细节。

当然有一些提供热点图的工具。但是有些时候，我想单独的跟踪某个输出，并将结果转换成热图。

本文我们以磁盘I/O延迟作为示例，进行讲解。

## 1 问题

As an example of disk I/O latency (aka storage I/O latency), I have a single disk system with a single process performing a sequential synchronous write workload. The storage device is a hard disk (rotational, magnetic). While this sounds like a very simple workload, the resulting latency profile is more interesting than you might expect.

Using iostat(1) to examine average latency (await) on Linux:

