# 基于消息队列的生产者-消费者模式
## 创建消息队列
```
# ./mq_manager -c /xyz
Create mq: "/xyz" success.
    maxmsg:  50
    msgsize: 260
# ./mq_manager -i /xyz
flags:   0
maxmsg:  50
msgsize: 260
curmsgs: 0
```

## 示例消息格式
参考[mq\_protocol.h](mq_protocol.h)
```
#pragma pack(1)
struct Message {
    uint32_t id;
    uint8_t length;
    char info[1];  // flexible array
};
#pragma pack()
```
该结构体仅用于强制转换然后获取各字段，比如用`((Messeage*)buf)->id`取得id。
其中buf为预先分配的足够大的缓冲区，实际消息长度为`sizeof(id) + sizeof(length) + length`。

## 将文件按行生产进消息队列，然后启动消费者消费
```
# cat data.txt
hello, world
你好，世界！This is XYZ！怎么办呢？???
122222222222222222222222345
# cat data.txt | ./mq_producer /xyz
producer stopped.
# ./mq_producer /xyz <data.txt
producer stopped.
# ./mq_consumer /xyz
[Message id: 1] 12 bytes:
00000000  68 65 6c 6c 6f 2c 20 77  6f 72 6c 64              |hello, world|
0000000c
[Message id: 2] 50 bytes:
00000000  e4 bd a0 e5 a5 bd ef bc  8c e4 b8 96 e7 95 8c ef  |................|
00000010  bc 81 54 68 69 73 20 69  73 20 58 59 5a ef bc 81  |..This is XYZ...|
00000020  e6 80 8e e4 b9 88 e5 8a  9e e5 91 a2 ef bc 9f 3f  |...............?|
00000030  3f 3f                                             |??|
00000032
[Message id: 3] 27 bytes:
00000000  31 32 32 32 32 32 32 32  32 32 32 32 32 32 32 32  |1222222222222222|
00000010  32 32 32 32 32 32 32 32  33 34 35                 |22222222345|
0000001b
^Cconsumer stopped.
```
