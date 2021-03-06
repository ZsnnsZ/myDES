#include <fstream>
#include <bitset>
#include <string>
#include <sstream> 
#include <vector>

// split string
using std::getline;
using std::back_inserter;
using std::vector;

// file
using std::ifstream;
using std::ofstream;
using std::ios;

using std::bitset;
using std::string;
using std::stringstream;
using std::hex;

// 64位密钥
bitset<64> key;

// 存放16轮子密钥
bitset<48> subKey[16];

// 初始置换表 IP-initial permutation
const int IP[] = { 58, 50, 42, 34, 26, 18, 10, 2,
                   60, 52, 44, 36, 28, 20, 12, 4,
                   62, 54, 46, 38, 30, 22, 14, 6,
                   64, 56, 48, 40, 32, 24, 16, 8,
                   57, 49, 41, 33, 25, 17, 9,  1,
                   59, 51, 43, 35, 27, 19, 11, 3,
                   61, 53, 45, 37, 29, 21, 13, 5,
                   63, 55, 47, 39, 31, 23, 15, 7};


// 尾置换表 the inverse of IP
const int IP_1[] = {40, 8, 48, 16, 56, 24, 64, 32,
                    39, 7, 47, 15, 55, 23, 63, 31,
                    38, 6, 46, 14, 54, 22, 62, 30,
                    37, 5, 45, 13, 53, 21, 61, 29,
                    36, 4, 44, 12, 52, 20, 60, 28,
                    35, 3, 43, 11, 51, 19, 59, 27,
                    34, 2, 42, 10, 50, 18, 58, 26,
                    33, 1, 41,  9, 49, 17, 57, 25};

// 密钥置换表，将64位密钥变成56位
const int PC_1[] = {57, 49, 41, 33, 25, 17, 9,
                    1, 58, 50, 42, 34, 26, 18,
                    10,  2, 59, 51, 43, 35, 27,
                    19, 11,  3, 60, 52, 44, 36,
                    63, 55, 47, 39, 31, 23, 15,
                    7, 62, 54, 46, 38, 30, 22,
                    14,  6, 61, 53, 45, 37, 29,
                    21, 13,  5, 28, 20, 12,  4};

// 压缩置换，将56位密钥压缩成48位子密钥
const int PC_2[] = {14, 17, 11, 24,  1,  5,
                     3, 28, 15,  6, 21, 10,
                    23, 19, 12,  4, 26,  8,
                    16,  7, 27, 20, 13,  2,
                    41, 52, 31, 37, 47, 55,
                    30, 40, 51, 45, 33, 48,
                    44, 49, 39, 56, 34, 53,
                    46, 42, 50, 36, 29, 32};

// 每轮左移的位数
const int shiftBits[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// 扩展置换表，将 32位 扩展至 48位
const int E[] = {32,  1,  2,  3,  4,  5,
                  4,  5,  6,  7,  8,  9,
                  8,  9, 10, 11, 12, 13,
                 12, 13, 14, 15, 16, 17,
                 16, 17, 18, 19, 20, 21,
                 20, 21, 22, 23, 24, 25,
                 24, 25, 26, 27, 28, 29,
                 28, 29, 30, 31, 32, 1};

// S盒，每个S盒是4x16的置换表，6位 -> 4位
const int S_BOX[8][4][16] = {
    {
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

// P置换，32位 -> 32位
const int P[] = {16,  7, 20, 21,
                 29, 12, 28, 17,
                  1, 15, 23, 26,
                  5, 18, 31, 10,
                  2,  8, 24, 14,
                 32, 27,  3,  9,
                 19, 13, 30,  6,
                 22, 11,  4, 25};

const string C[] = {"1", "22", "333", "4444", "55555",
                    "666666", "7777777", "88888888"};
/**
 *  密码函数f，接收32位数据和48位子密钥，产生一个32位的输出
 */
bitset<32> f(bitset<32> R, bitset<48> K)
{
    bitset<48> expandR;

    // E 变换 32 -> 48
    for(int i=0; i<48; ++i)
        expandR[i] = R[E[i]-1];

    // R XOR K
    expandR = expandR ^ K;

    // 查找S_BOX置换表
    bitset<32> output;
    int x = 0;
    for(int i=0; i<48; i=i+6)
    {
        int row = expandR[47-i]*2 + expandR[47-i-5];
        int col = expandR[47-i-1]*8 + expandR[47-i-2]*4 + expandR[47-i-3]*2 + expandR[47-i-4];
        int num = S_BOX[i/6][row][col];
        bitset<4> binary(num);

        output[31-x] = binary[3];
        output[31-x-1] = binary[2];
        output[31-x-2] = binary[1];
        output[31-x-3] = binary[0];
        x += 4;
    }

    // P-置换，32 -> 32
    bitset<32> tmp = output;
    for(int i=0; i<32; ++i)
        output[31-i] = tmp[32-P[i]];
    return output;
}

/**
 *  对56位密钥的前后部分进行左移
 */
bitset<28> leftShift(bitset<28> k, int shift)
{
    bitset<28> tmp = k;
    for(int i=27; i>=0; --i)
    {
        if(i-shift<0)
            k[i] = tmp[i-shift+28];
        else
            k[i] = tmp[i-shift];
    }
    return k;
}

/**
 *  生成16个48位的子密钥
 */
void generateKeys()
{
    bitset<56> realKey;
    bitset<28> left;
    bitset<28> right;
    bitset<48> compressKey;
    // 去掉奇偶标记位，将64位密钥变成56位
    for (int i=0; i<56; ++i)
        realKey[55-i] = key[64 - PC_1[i]];
    // 生成子密钥，保存在 subKeys[16] 中
    for(int round=0; round<16; ++round)
    {
        // 前28位与后28位
        for(int i=28; i<56; ++i)
            left[i-28] = realKey[i];
        for(int i=0; i<28; ++i)
            right[i] = realKey[i];
        // 左移
        left = leftShift(left, shiftBits[round]);
        right = leftShift(right, shiftBits[round]);
        // 压缩置换，由56位得到48位子密钥
        for(int i=28; i<56; ++i)
            realKey[i] = left[i-28];
        for(int i=0; i<28; ++i)
            realKey[i] = right[i];
        for(int i=0; i<48; ++i)
            compressKey[47-i] = realKey[56 - PC_2[i]];
        subKey[round] = compressKey;
    }
}

/**
 *  工具函数
 */

// 将char字符数组转为二进制
bitset<64> charToBitset(const char s[8])
{
    bitset<64> bits;
    for(int i=0; i<8; ++i)
        for(int j=0; j<8; ++j)
            bits[i*8+j] = ((s[i]>>j) & 1);
    return bits;
}

// 将二进制转为字符串
string bitsetToString(const bitset<64> b)
{
    string s = "";
    bitset<8> temp;
    for(int i=0; i<8; ++i)
    {
        for(int j=0; j<8; ++j)
            temp[j] = b[i*8+j];
        s += char(temp.to_ulong());
    }
    return s;
}

// 将16进制转为2进制
bitset<64> Hex2Bin(const string& s)
{
    stringstream ss;
    ss << hex << s;
    unsigned long long n;
    ss >> n;
    bitset<64> b(n);
    return b;
}

// 将2进制转为16进制
string Bin2Hex(const string& s)
{
    bitset<64> bs(s);
    unsigned long long n = bs.to_ullong();
    stringstream ss;
    ss << hex << n;
    return ss.str();
}

// split a string
template<typename Out>
void split(const string &s, char delim, Out result)
{
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

/**
 *  DES加密
 */
bitset<64> encrypt(bitset<64>& plain)
{
    bitset<64> cipher;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    bitset<32> newLeft;
    // 第一步：初始置换IP
    for(int i=0; i<64; ++i)
        currentBits[63-i] = plain[64-IP[i]];
    // 第二步：获取 Li 和 Ri
    for(int i=32; i<64; ++i)
        left[i-32] = currentBits[i];
    for(int i=0; i<32; ++i)
        right[i] = currentBits[i];
    // 第三步：共16轮迭代
    for(int round=0; round<16; ++round)
    {
        newLeft = right;
        right = left ^ f(right,subKey[round]);
        left = newLeft;
    }
    // 第四步：合并L16和R16，注意合并为 R16L16
    for(int i=0; i<32; ++i)
        cipher[i] = left[i];
    for(int i=32; i<64; ++i)
        cipher[i] = right[i-32];
    // 第五步：结尾置换IP-1
    currentBits = cipher;
    for(int i=0; i<64; ++i)
        cipher[63-i] = currentBits[64-IP_1[i]];
    // 返回密文
    return cipher;
}

/**
 *  DES解密
 */
bitset<64> decrypt(bitset<64>& cipher)
{
    bitset<64> plain;
    bitset<64> currentBits;
    bitset<32> left;
    bitset<32> right;
    bitset<32> newLeft;
    // 第一步：初始置换IP
    for(int i=0; i<64; ++i)
        currentBits[63-i] = cipher[64-IP[i]];
    // 第二步：获取 Li 和 Ri
    for(int i=32; i<64; ++i)
        left[i-32] = currentBits[i];
    for(int i=0; i<32; ++i)
        right[i] = currentBits[i];
    // 第三步：共16轮迭代（子密钥逆序应用）
    for(int round=0; round<16; ++round)
    {
        newLeft = right;
        right = left ^ f(right,subKey[15-round]);
        left = newLeft;
    }
    // 第四步：合并L16和R16，注意合并为 R16L16
    for(int i=0; i<32; ++i)
        plain[i] = left[i];
    for(int i=32; i<64; ++i)
        plain[i] = right[i-32];
    // 第五步：结尾置换IP-1
    currentBits = plain;
    for(int i=0; i<64; ++i)
        plain[63-i] = currentBits[64-IP_1[i]];
    // 返回明文
    return plain;
}

#include <iostream>

string encryptFile(const string& path)
{
    ifstream in;
    ofstream out;
    in.open(path, ios::binary);
    in.seekg(0, std::ios_base::end);
    std::streampos sp = in.tellg();// 文件字节数
    in.seekg(0, ios::beg);
    vector<string> x = split(path, '/');
    string ends = x[x.size()-1];// ends表示原文件名
    string savePath = "/Users/macbookair/Desktop/encrypted_" + ends;
    out.open(savePath, ios::binary);
    bitset<64> plain;
    int round = sp/8;
    for(int i=0; i < round; ++i)//对8的整数倍的字节进行正常加密
    {
        in.read((char*)&plain, 8);
        bitset<64> cipher  = encrypt(plain);
        out.write((char*)&cipher, 8);
        plain.reset();
    }
    //对于不足8个字节的缺几个字节就在后边补几个几，如5个字节需要补3个3，特别的，补8个8
    in.read((char*)&plain, sp%8);
    string completion = C[7-sp%8];
    string temp = "";
    // bitsetToString(plain)返回的是八个字节，需筛选真正内容
    for(auto c: bitsetToString(plain))
        if(isprint(c))
            temp = temp + c;
    plain = charToBitset((temp+completion).c_str());
    bitset<64> cipher  = encrypt(plain);
    out.write((char*)&cipher, 8);
    in.close();
    out.close();
    return savePath;
}

string decryptFile(const string& path)
{
    ifstream in;
    ofstream out;
    bitset<64> cipher;
    in.open(path, ios::binary);
    in.seekg(0, std::ios_base::end);
    std::streampos sp = in.tellg();
    in.seekg(0, ios::beg);
    vector<string> x = split(path, '/');
    string ends = x[x.size()-1];
    string savePath = "/Users/macbookair/Desktop/decrypted_" + ends;
    out.open(savePath, ios::binary);
    int round = sp/8;
    for(int i=0; i < round-1; ++i)// 解密时注意循环上界
    {
        in.read((char*)&cipher, 8);
        bitset<64> plain = decrypt(cipher);
        out.write((char*)&plain, 8);
        cipher.reset();
    }
    in.read((char*)&cipher, 8);
    bitset<64> plain = decrypt(cipher);
    string lastEight = bitsetToString(plain);
    int deleteCount = lastEight[7] - '0';// 不能用(int)强制转换，这样得到的是对应的ascii码
    out.write((char*)&plain, 8-deleteCount);// 删除(不写入)末尾的几个几
    in.close();
    out.close();
    return savePath;
}

