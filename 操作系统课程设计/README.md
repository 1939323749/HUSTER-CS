# 操作系统课程设计🔧

## 一.目的

- 掌握Linux操作系统的使用方法
- 了解Linux系统内核代码结构
- 掌握实例操作系统的实现方法

## 二.设计内容

1. Linux用户界面的使用

> * 编一个C程序，其内容为实现文件拷贝的功能.
>
> - [x] 基本要求:使用系统调用*open/read/write...*; 选择:容错、*cp*。
>
> * 编一个C程序，其内容为分窗口同时显示三个并发进程的运行结果。要求用到Linux下的图形库。 (gtk/Qt) 
>
> - [x] 基本要求:三个独立子进程各自窗口显示;选择:三个进程誊抄演示。

2. 掌握系统调用的实现过程，通过编译内核方法，增加一个新的系统调用。另编写一个应用程序，使用新增加的系统调用。

> * 内核编译、生成,用新内核启动;
> * 新增系统调用实现:文件拷贝或P、V操作。

3. 掌握增加设备驱动程序的方法。通过模块方法，增加一个新的字符设备驱动程序，其功能可以简单,基于内核缓冲区。

> - [x] 基本要求:演示实现字符设备读、写;
> - [x] 选择:键盘缓冲区，不同进程、追加、读取。

4. 使用GTK/QT实现系统监控器

> * 了解/proc文件的特点和使用方法
> * 监控系统中进程运行情况
> * 用图形界面实现系统资源的监控

5. 设计并实现一个模拟的文件系统

> * 基于一大文件(10M或100M)，模拟磁盘;
> * 格式化，建立文件系统管理数据结构; 
> * 基本操作，实现文件、目录相关操作。



## 三.实验环境

- Windows10 +虚拟机Ubuntu19.10 (60G)+内核Linux 5.4.21

## 四.实验步骤

### 4.1 Linux用户界面的使用

**1.配置虚拟机**
[Vmware安装虚拟机Ubuntu知乎教程链接🔗](https://zhuanlan.zhihu.com/p/38797088)
[VMware虚拟机扩展Ubuntu系统磁盘空间教程🔗](https://blog.csdn.net/daemon_2017/article/details/80660372)
其他具体教程就按照个人需求Google就好咯. 

**2.(task1)编写Get_Copy_Get文件誊抄程序**
这是一个样例,不涉及并发.

```C++
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <errno.h>
#include <string.h>
#define BUFFER_SIZE 1024 
int main(int argc,char **argv) {
//缓冲区大小
    int from_fd,to_fd;
    int bytes_read,bytes_write;
    char buffer[BUFFER_SIZE];
    char *ptr;
    if(argc!=3) //三个参数
    {
    fprintf(stderr,"Usage:%s fromfile tofile\n\a",argv[0]);
    return(-1);
    }
    /* 打开源文件 */
    if((from_fd=open(argv[1],O_RDONLY))==-1)
    {
        fprintf(stderr,"Open %s Error:%s\n",argv[1],strerror(errno));
        return(-1);
    }
/* 创建目的文件 */ 
    if((to_fd=open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1) {
        fprintf(stderr,"Open %s Error:%s\n",argv[2],strerror(errno));
        //设定一个缓冲区
        return(-1); 
    }
/* 以下代码是一个经典的拷贝文件的代码 */ 
    while(bytes_read=read(from_fd,buffer,BUFFER_SIZE)) {
/* 一个致命的错误发生了 */ 
        if((bytes_read==-1)&&(errno!=EINTR)) break;
        else if(bytes_read>0)
        {
            ptr=buffer; 
            while(bytes_write=write(to_fd,ptr,bytes_read))
            {
/* 一个致命错误发生了 */ 
            if((bytes_write==-1)&&(errno!=EINTR))break;
        /* 写完了所有读的字节 */
            else if(bytes_write==bytes_read) break;
        /* 只写了一部分,继续写 */
            else if(bytes_write>0)
            {
                ptr+=bytes_write;
                bytes_read-=bytes_write;
            }
        }
/* 写的时候发生的致命错误 */ if(bytes_write==-1)break;
        }
        }
        close(from_fd);
        close(to_fd);

        return(1); 
    }
```

**3.(task2)编一个C程序，其内容为分窗口同时显示三个并发进程的运行结果。要求用到Linux下的图形库。 (gtk/Qt) **
因为GTK之前没有接触过,而且有一点点古老,所以我选择了Qt5.
我一共做了四个并发进程,通过fork()来实现.分别监视系统世界、CPU使用率、内存使用率、磁盘使用率.

![F803A68E974232A903522C7D7E91BA06](http://alexfanblog.oss-cn-beijing.aliyuncs.com/2020/02/29/f803a68e974232a903522c7d7e91ba06.jpg?image/auto-orient,1/interlace,1/quality,q_100)
(源码放在了Github仓库里面,欢迎大家一起学习交流)

### 4.2新增系统调用实现

**以下内容基于Ubuntu19.10 + 内核Linux5.4.21,如果是4.x.x版本的内核,参考以下下文链接**
👉[基于Linux4代内核的新增系统调用步骤](https://blog.csdn.net/qq_41175905/article/details/80529245)

**1.配置虚拟机**
[Vmware安装虚拟机Ubuntu知乎教程链接🔗](https://zhuanlan.zhihu.com/p/38797088)
[VMware虚拟机扩展Ubuntu系统磁盘空间教程🔗](https://blog.csdn.net/daemon_2017/article/details/80660372)



**2.更换Ubuntu源为清华源**
[更换镜像源链接🔗](https://blog.csdn.net/yumei1998/article/details/83214433)
注意你的Ubuntu版本,选择适配的镜像源,可加快安装包的速度.



**3.一键配置所有所需要的包**
换好镜像源后直接在命令行里配置就行了.

```
apt-get install libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf
```

后续如果缺其他包就安装系统提示下载就好了



**4.内核下载**
别去www.kernel.org下了,速度实在不忍直视
直接通过下述链接下载配置.
[kernel镜像源链接🔗](http://ftp.sjtu.edu.cn/sites/ftp.kernel.org/pub/linux/kernel/)
选择合适的版本挑一个下载即可.



**5.安装内核**

- ctrl+alt+t快速打开CMD,并且输入sudo su获得管理员权限.
- 将下载好的内核move到/usr/src目录下,使用命令

```
sudo mv **/**/**/linux***  /usr/src/
//**代表你刚下载的linux内核的路径
```

- 进入/usr/src/ 解压文件

```
sudo tar -xvf  linux****
//linux***代表你刚下载的linux内核的文件名,可以用tab自动补齐.
```

- 进入解压出来的内核文件中的kernel目录

```
cd linux***/kernel
```

- 修改sys.c文件

```
sudo gedit sys.c
//在最末尾加上你想加的新的系统调用
//举个例子,helloworld程序
SYSCALL_DEFINE1(helloworld,int,number){
        printk("Hello,world!");
        return number+1
}
//对系统调用的编写 注意此处与前版本内核的不同
//上述程序中,1代表这个函数只有一个参数,有两个参数就换成2
//helloworld是新增系统调用函数名
//int是参数类型,number是形参.
//函数功能就是打印Hello,world!,并且返回1+number.可以通过dmesg看到打印的句子.
```

- 增加系统调用号

```
//从kernel返回上一级目录
cd /arch/x86/entry/syscalls
sudo gedit syscall_64.tbl
//增加一个新的系统调用
//例如 335    64  helloworld      __x64_sys_helloworld
```

- 增加系统调用头文件

```
//返回/usr/src/linux***目录
cd include/linux
sudo gedit syscalls.h
//例如asmlinkage long sys_helloworld(int number);
```

- 最后按照上述步骤编写你的文件copy和P、V操作系统调用程序吧



**6.编译内核**

```
sudo make mrproper //删除之前编译的残余文件.只在第一次编译时执行即可,以免每次编译把重复文件重复编译.
sudo make clean //可执行可不执行,因为上一个命令似乎是涵盖了clean的
sudo make menuconfig//在general setup哪里给你的内核换个名字,比如AlexKernel
sudo make -j4 //根据你给虚拟机分配的核心数来适配.我分了4核心.
sudo make modules_install // 安装内核模块
sudo make install //安装内核
```

**7.更换系统默认启动内核**
[更换启动内核链接🔗](https://blog.csdn.net/cf_wu95/article/details/85984956)



**8.重启,编写你的测试c文件.**

(PS:不嫌麻烦的可以直接看官方文档。[新增系统调用官方文档🔗](https://www.kernel.org/doc/html/latest/process/adding-syscalls.html))


### 4.3增加设备驱动程序

[设备驱动程序编写官方教程链接🔗](https://www.tldp.org/LDP/lkmpg/2.4/html/c577.htm)
[推荐相关博客教程🔗](https://www.cnblogs.com/lidabo/p/5312827.html)

> 建议看看我GitHub中的操作系统课程设计指导书📖.里面关于设备驱动程序有很详细的例子.

系统调用是操作系统内核和应用程序之间的接口，设备驱动程序是操作系统内核和机器硬件之间的接口。设备驱动程序为应用程序屏蔽了硬件的细节，这样在应用程序看来，硬件设备只是一个设备文件，应用程序可以象操作普通文件一样对硬件设备进行操作。设备驱动程序是内核的一部分，它完成以下的功能:

  　　1. 对设备初始化和释放。
  　　2. 把数据从内核传送到硬件和从硬件读取数据。
  　　3. 读取应用程序传送给设备文件的数据和回送应用程序请求的数据。
  　　4. 检测和处理设备出现的错误。
	在Linux操作系统下有三类主要的设备文件类型，一是字符设备，二是块设备，三是网络设备。字符设备和块设备的主要区别是:在对字符设备发出读/写请求时，实际的硬件I/O一般就紧接着发生了，块设备则不然，它利用一块系统内存作缓冲区，当用户进程对设备请求能满足用户的要求，就返回请求的数据，如果不能，就调用请求函数来进行实际的I/O操作。块设备是主要针对磁盘等慢速设备设计的，以免耗费过多的CPU时间来等待。

**1.编写字符驱动程序,功能是向虚拟出的虚拟设备传入一连串的数字字符.**
(以下是概要代码,详情请前往GitHub仓库阅览)
**2.字符设备驱动代码**

```c
#include  <linux/modules.h>   
#include  <linux/version.h>
#include <linux/types.h>   
#include <linux/fs.h>   
#include <linux/mm.h>   
#include <linux/errno.h>   
#include <asm/segment.h>
//以上是我们需要的所有头文件
long int ALEX_MAJOR = 0;
static ssize_t read_alex(struct file *flip,char *buf,size_t count, loff_t*f_pos)
{ 
	int left;   
	for(left = count ; left > 0 ; left--)   
{   
	put_user(9,buf);   
	buf++;   
}   
	return count; 
} //read操作定义
static ssize_t write_alex(struct file *flip,const char *file,size_t count, loff_t*f_pos) 
{ 
	return count; 
} //write操作的定义
 
static int open_alex(struct inode *inode,struct file *file ) 
{
	
	return 0; 
} //open操作的定义
 
static int release_alex(struct inode *inode,struct file *file )
{

}//release操作的定义

struct file_operations alex_fops={
	.read = read_alex,
	.write=write_alex,
	.open=open_alex,
	.release=release_alex
};//操作顺序定义

static int init_mymodule(void) 
{ 
	int result;
	result = register_chrdev(0, "alex", &alex_fops); 
	if (result < 0) { 
		printk(KERN_INFO "alex: can't get major number\n"); 
		return result;
	} 
	if (ALEX_MAJOR == 0) ALEX_MAJOR = result; /* dynamic */ 
	return 0; 
}//初始化驱动程序
static void cleanup_mymodule(void) 
{ 
	unregister_chrdev(ALEX_MAJOR,"alex"); 
} //终止驱动程序

MODULE_LICENSE("GPL");
module_init(init_mymodule);
module_exit(cleanup_mymodule);
```

**3.编译步骤**
a. 将上述程序复制到/usr/src/linux**/drivers/misc下
b.在Makefile文件中加入一句

```c
obj-m +=alex_driver.0
```

c.执行

```
make -C /usr/src/linux SUBDIR=$PWD modules
```

得到.ko文件
d.挂载

```
insmod ./alex_driver.ko
```

e.查看系统分配的设备号

```
cat /proc/devices
```

发现出现了一个设备叫alex,记下它前面的设备号,我的是241.

f.创建新的虚拟设备文件

```
mknod /dev/alex c 241 0
chmod 666 /dev/alex
```

执行结束后,会在/dev中看到一个新的设备文件alex

**4.编写测试程序**

```c
#include <stdio.h>   
#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
int main()   
{   
	int testdev;   
	int i;   
	char buf[10];   
	testdev = open("/dev/alex",O_RDWR);   
	if ( testdev == -1 )   
	{   
		printf("Can't open file \n");   
		exit(0);   
		}   
	read(testdev,buf,10);   
	for (i = 0; i < 10;i++)
	printf("%d\n",buf[i]);  
	close(testdev);   
}  
```

**5.测试成功后卸载模块**

```
rmmod alex-drive
rm /dev/alex
```

以上步骤中如果权限不够请在命令前加上sudo.

### 4.4使用GTK/QT实现系统监控器

同4.1中的题目其实十分相似,只是不用强制要求设计并发进程了.
但是一定要制作图形界面,这里我们仍然选用Qt5来做.
**1.功能清单🧾**

```
- 1.获取并显示主机名
- 2.获取并显示系统启动的时间
- 3.显示系统到目前为止持续运行的时间
- 4.显示系统的版本号
- 5.显示CPU的型号和主频大小
- 6.通过pid或者进程名查询一个进程,并显示该进程的详细信息,提供杀掉该进程的功能
- 7.显示系统所有进程的一些信息,包括pid,ppid,占用内存大小,优先级等
- 8.cpu使用率的图形化显示(2分钟内的历史记录曲线)
- 9.内存和交换分区(swap)使用率的图形化显示(2分钟内的历史记录曲线)
- 10.在状态栏显示当前时间
- 11.在状态栏显示当前CPU使用率
- 12.在状态栏显示当前内存使用情况
- 13.用新进程运行一个其他程序
- 14.关机功能
```

**2.设计流程**
整个任务管理器的设计主要分成两个步骤.

- 功能函数的设计
- Qt UI界面设计

### 4.5小型模拟文件系统

1.linux文件系统概述
文件系统指文件存在的物理空间，linux系统中每个分区都是一个文件系统，都有自己的目录层次结构。linux会将这些分属不同分区的、单独的文件系统按一定的方式形成一个系统的总的目录层次结构。一个操作系统的运行离不开对文件的操作，因此必然要拥有并维护自己的文件系统。 　　 
linux文件系统使用**索引节点**来记录文件信息，作用像windows的文件分配表。索引节点是一个结构，它包含了一个文件的**长度、创建及修改时间、权限、所属关系、磁盘中的位置**等信息。一个文件系统维护了一个索引节点的数组，每个文件或目录都与索引节点数组中的唯一一个元素对应。系统给每个索引节点分配了一个号码，也就是该节点在数组中的索引号，称为索引节点号。 　　

linux文件系统将文件索引节点号和文件名同时保存在目录中。所以，目录只是将文件的名称和它的索引节点号结合在一起的一张表，**目录中每一对文件名称和索引节点号称为一个连接**。 　　
对于一个文件来说有唯一的索引节点号与之对应，对于一个索引节点号，却可以有多个文件名与之对应。因此，在磁盘上的同一个文件可以通过不同的路径去访问它。可以用ln命令对一个已经存在的文件再建立一个新的连接，而不复制文件的内容。 　　

连接有软连接和硬连接之分，软连接又叫符号连接。它们各自的特点是： 　　
硬连接：原文件名和连接文件名都指向相同的物理地址。目录不能有硬连接；　　
硬连接不能跨越文件系统（不能跨越不同的分区）文件在磁盘中只有一个拷贝，节省硬盘空间；　　
由于删除文件要在同一个索引节点属于唯一的连接时才能成功，因此可以防止不必要的误删除。 　　
符号连接：用ln -s命令建立文件的符号连接符号连接是linux特殊文件的一种，作为一个文件，它的数据是它所连接的文件的路径名。类似windows下的快捷方式。可以删除原有的文件而保存连接文件，没有防止误删除功能.