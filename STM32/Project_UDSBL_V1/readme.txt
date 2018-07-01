2017-02-09
已知问题
1.
SecurityAccess的seed产生方式有误，EXCD_NUM_ATMP和REQD_TD_NOT_EXPR否定回复未做处理
2.
功能地址（1对多通信）仅仅对单帧的通信支持 单目前程序，未对接收到的功能地址多帧进行屏蔽
3.
N_Br N_Cs N_As N_Ar未做处理
其他说明
1.
不支持全双工
2.
CAN发送使用查询发送

2017-02-22
更新
1.
文件组织结构变更
2.
SecurityAccess服务seed产生方式修正，增加否定响应EXCD_NUM_ATMP和REQD_TD_NOT_EXPR
3.
应用程序有效标识存储位置由后备寄存器改为模拟eeprom
4.
增加S3_Server超时功能，增加Hook函数ReceiveMessageHook实现
5.
在NWS_ReceiveFirstFrame函数中，屏蔽功能地址接收
6.
增加服务TesterPresent(2E hex)
7.
增加驱动文件下载接口，但未实现驱动文件下载功能
8.
增加ReadDataByIdentifier 0x22，读0xF193，0xF195标识。
9.
安全访问的加密掩码更改，APP和Bootloader加密掩码不同
10.
RoutineControl的擦除内存历程0xFF00，其中表示地址和数据所占字节数的格式位，应为0x44。
但知豆诊断仪做成0x00了。为了兼容如果是0x00也认为地址和数据所占字节数都是4。
11.
修复RequestDownload Response 0x74中的maxNumberOfBlockLength的处理问题，所指是TransferData 0x36服务包含服务ID和数据参数的总长度。
12.
擦除内存历程，增加NRC0x78 requestCorrectlyReceived-ResponsePending 否定响应，当擦除内存处理完成后发送肯定响应。
已知问题
1.
N_Br N_Cs N_As N_Ar未做处理
2.
ReadDataByIdentifier，需要根据不同的DID标识配置权限，暂未实现。
3.
NRC0x78 后续处理需要优化
其他说明
1.
不支持全双工
2.
CAN发送使用查询发送
3.
RequestDownload 0x34.知豆诊断仪，dataFormatIdentifier只支持0x00，即非压缩非加密模式。
