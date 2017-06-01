#include "widget.h"
#include <QApplication>
#include "des.h"

// 64位密钥
extern uint64_t key;
// 存放16轮子密钥
extern uint64_t subKey[16];
int main(int argc, char *argv[])
{
    uint64_t mplain = Hex2Bin("5469875321456045");
    key = Hex2Bin("5987423651456987");
    generateKeys();
    string tmpSubKey;
    for (int i=0; i<16; ++i)
    {
        tmpSubKey = Bin2Hex(subKey[i]);
        cout << "Key[" << i << "]: " << tmpSubKey << endl;
    }
    uint64_t mencrypt = encrypt(mplain);
    cout << "encrypt: " << Bin2Hex(mencrypt) << endl;
//    QApplication a(argc, argv);
//    Widget w;
//    w.show();
//    return a.exec();
}
