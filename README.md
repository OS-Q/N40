# [Xmodem升级](https://github.com/OS-Q/W40)

[![sites](http://182.61.61.133/link/resources/OSQ.png)](http://www.OS-Q.com)

### [Xmodem描述](https://github.com/OS-Q/W40/wiki)

[Xmodem](https://github.com/OS-Q/W40) 是一种在串口通信中广泛使用的异步文件传输协议，由于使用了CRC错误侦测方法，传输的准确率可高达99.6%。分为XModem和1k-XModem协议两种，前者使用128字节的数据块，后者使用1024字节即1k字节的数据块。

 XModem工作时，先由收方发出NAK，然后等待发方的包开始SOH。收到SOH后，即 将整个包收完，检查是否有错。如出错，则向发方发出一个NAK，请求发方重发； 否则发出ACK,表示接收正确，请发下个包。XModem检查包编号和checksum来确定 该包是否传送有问题。编号是发方送得出包的顺序。当XModem发送完最后一个包 时，收方会收到文件结束符(EOT)，此时，收方会送出一个ACK通知发方传送结束。

### [收录资源](https://github.com/OS-Q/)

* [文档](docs/)
* [工程](project/)
* [工具](software/)

### [OS-Q = Open Solutions | Open Source | Operating System ](http://www.OS-Q.com/Xmodem)
