# myDES
C++练手小工具，DES加解密，使用Qt creator

# 字符串加解密  
任意长度的字符串，每个字符是一个16进制数

# 文件加解密
任意格式的文件加解密

# 说明  
每个untitled\*原名都为untitled，需修改回原名后才能正确编译

version 1.0
使用bitset  
单线程，不开启 gcc -O3 编译优化的情况下：  
平均加解密速度：100 Kb/s左右  

version 2.0
使用uint_64代替bitset，自己实现位运算  
单线程，不开启 gcc -O3 编译优化的情况下：  
平均加解密速度：1 M/s左右  
若开启开启 gcc -O3 编译 优化，速度  可增至3 M/s  
