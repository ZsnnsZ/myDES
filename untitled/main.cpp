#include "widget.h"
#include <QApplication>
/*
 * 测试
#include <iostream>
#include <string>
#include <bitset>

using std::cout;
using std::endl;
using std::bitset;
using std::string;

// 声明
void generateKeys();
bitset<64> charToBitset(const char s[8]);
string bitsetToChar(const bitset<64> b);
bitset<64> encrypt(bitset<64>& plain);
bitset<64> decrypt(bitset<64>& cipher);
extern bitset<64> key;
extern bitset<48> subKey[16];
bitset<64> Hex2Bin(const string& s);
string Bin2Hex(const string& s);
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
