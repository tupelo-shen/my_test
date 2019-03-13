在C++中，*boost::asio* 把串口当做一种文件流来使用。

C++的Boost库中，通信库都在asio下，串口类为 *boost::asio::serial_port*。串口通信由asio组件的 *serial_port* 类完成。 Boost库下的串口通信 *serial_port*类 的使用跟网络通信相似[网络通信]，只是在进行串口通信前需要初始化串口。下面记录使用步骤：

# 1. 头文件和命名空间

serial_port类头文件和命名空间声明
    
    #include <boost::asio.hpp>

# 2. serial_port类实现串口通信步骤

1. 定义 *io_service* 对象
    
    *io_service* 对象是使用 *boost::asio* 库的必有对象。

        io_service io_s;

2. 创建串口对象，传入 *io_service* 对象，打开串口
    
        serial_port sp(io_s, "COM1");

    io_s是步骤1中定义的 *io_service* 对象，*COM1*为串口名称（计算机->属性->设备管理器->端口）。一旦串口被打开，则此串口就会被当成 *stream*来处理。

    1. 如果串口端口不存在，则 *try-catch* 能够获取“系统找不到指定文件”文件异常。
    2. 如果串口端口没有和实际的串口连接，则try-catch能够获取“设备没有连接”的异常。
    3. 如果在电脑之上连接一个串口线，则用此函数打开对应的端口（如COM4 ）就不会出现以上两个异常。

3. 初始化
    
    利用串口类对象初始化串口。串口类对象就是(2.)中定义的 *sp*，串口就是上面的 *COM1*。

        sp.set_option(serial_port::baud_rate(9600));                                //比特率
        sp.set_option(serial_port::flow_control(serial_port::flow_control::none));  //流量控制
        sp.set_option(serial_port::parity(serial_port::parity::none));              //奇偶校验
        sp.set_option(serial_port::stop_bits(serial_port::stop_bits::one));         //停止位
        sp.set_option(serial_port::character_size(8));                              //数据位

4. 利用 *serial_port* 的成员函数进行串口通信

    如前所述，*serial_port* 类成功打开 *COM1* 后， *COM1*对于 *serial_port* 类来说就成了一种文件流。咱们就可以使用 *serial_port* 类中的成员函数对流进行读写数据操作。

    向串口发送数据时是采用 *boost::asio::serial_port* 下含 *write* 字符串的函数将程序中的数据写入到串口流， 接收串口数据时是用含 *read* 字符串的函数从串口读出数据，再放到程序变量中。 比如用串口对象调用的 *write_some()*， *read_some()*之类属于 *serial_port*的成员函数，还有在函数内部指明串口对象的 *write()*，*read()*等非 *serial_port*类的成员函数，但它们是 *boost::asio*下的函数。看名就知道 *write_some/read_some* 的函数（ 比如读到空格或者其它特殊字符就会停止读下去）不如“*write/read*”函数功能完善。所以，咱都还是用 *write()，read()* 一类的函数从串口写、读完整的数据串吧。

    1. 向串口写数据
        
        *write* 有4个重载函数，有2种都有接收异常的参数。

            size_t data_len;
            boost::system::error_code ec;
            data_len = write(*pSerialPort, buffer(data), ec);

        *write* 的第一个参数表示 *serial_port* 对象， 第二个参数是写向(传输)串口的数据，第三个参数是boost库下的异常参数，如果 *write* 函数传输数据发生什么异常就会自动抛出此异常给 *catch* 。向串口成功传进数据则返回写入数据data的长度，*buffer*是boost库的函数，一般的参数都需要buffer一下。

    2. 读取、接收串口数据

        如果直接用 *read*函数来读取串口流中的数据，则 *read*函数在读满缓存后才会返回(比如， *char a[6]*,则会读满6个后才会返回 )。而且返回输出字符串的时候还是乱码。如此使用 *read*函数就会阻塞后面代码的执行。

        可以使用异步读取/接收串口的方式：就算未完全读/接收到串口数据，异步读取函数依旧会马上返回执行后续代码， 等串口数据读取完毕，或者发生异常时 *io_service::run()*函数会等待异步读取串口的数据操作，然后调用异步函数指定的回调函数。

        提到异步操作，它包含三部分：

        1. 异步操作函数
        2. 异步操作函数以形参方式指定的回调函数
        3. io_service::run函数的调用

        这三个部分对应的执行流程为：

        1. 程序执行到异步操作函数处，异步操作函数立即返回，程序继续执行后续代码。
        2. 异步操作函数的功能完成[如读取到与设定缓冲区长度大小一致的数据时]或者出现异常时，*io_service::run()*函数机制会自动调用异步操作函数指定的回调函数。如果不用 *io_service::run()*函数， 异步操作函数依然可以实现异步操作流程，只是回调函数不会被执行。


                void handle_read(boost::system::error_code ec, std::size_t bytes_transferred);
                // 如果不使用bind，则async_read()函数的回调函数必须为下面的形式
                async_read( *pPort, buffer(v),  handle_read );
                …
                ios.run();

        在此种情形下，虽然可以使异步操作函数后续代码被执行。但在没有发生异常或者没有读满设定缓冲区大小时，回调函数不会被调用。所以可以使用boost库下的 *deadline_timer*为异步操作定时,如果超过定时的时间就结束异步函数的异步操作去执行回调函数。

                eadline_timer timer(ios);
                timer.expires_from_now(boost::posix_time::millisec(100)); 
                //超时后调用pSerialPort的cancel()方法放弃读取更多字符
                timer.async_wait(boost::bind(&serial_port::cancel,  boost::ref(*pSerialPort)));