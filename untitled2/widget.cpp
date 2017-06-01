#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <des.h>

// 64位密钥
extern uint64_t key;
// 存放16轮子密钥
extern uint64_t subKey[16];


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_key_clicked()
{
    QString keyStr = ui->lineEdit_key->text();
    key = Hex2Bin(keyStr.toStdString());
    generateKeys();
    cout << "The Key in hex: " << keyStr.toStdString() << endl;
    string tmpSubKey;
    cout << "The subKey in hex: " << endl;
    for (int i=0; i<16; ++i)
    {
        tmpSubKey = Bin2Hex(subKey[i]);
        cout << "Key[" << i << "]: " << tmpSubKey << endl;
    }
    cout << "The Key and SubKeys have been calculated"<< endl;
    cout << "-----------------------------------------------------" << endl;
}

void Widget::on_pushButton_e_clicked()
{
    QString plainQStr = ui->lineEdit_plain->text();
    string plainStr = plainQStr.toStdString();
    int round = plainStr.size() / 16;// 商
    string cipher = "";
    if(plainStr.size()%16 == 0)// 余数
        round -= 1;
    vector<string> vs;
    for(int i=0; i<=round; ++i)
    {
        string subs = "";
        for(int j=0; j<16; ++j)
            subs = subs + plainStr[i*16+j];
        vs.push_back(subs);
    }
    for(size_t i=0; i<vs.size(); ++i)
    {
        uint64_t subPlain = Hex2Bin(vs[i]);
        uint64_t subCipher = encrypt(subPlain);
        cipher = cipher + Bin2Hex(subCipher);
    }
    QString s = QString::fromStdString(cipher);
    ui->lineEdit_cipher->setText(s);
    cout << "The Plain in hex: " << plainStr << endl;
    cout << "The Cipher in hex: " << cipher << endl;
    cout << "-----------------------------------------------------" << endl;
}

void Widget::on_pushButton_d_clicked()
{
    QString cipherQStr = ui->lineEdit_cipher->text();
    string cipherStr = cipherQStr.toStdString();
    int round = cipherStr.size() / 16;
    string plain = "";
    if(cipherStr.size()%16 == 0)
        round -= 1;
    vector<string> vs;
    for(int i=0; i<=round; ++i)
    {
        string subs = "";
        for(int j=0; j<16; ++j)
            subs = subs + cipherStr[i*16+j];
        vs.push_back(subs);
    }
    for(size_t i=0; i<vs.size(); ++i)
    {
        uint64_t subCipher = Hex2Bin(vs[i]);
        uint64_t subPlain = decrypt(subCipher);
        plain = plain + Bin2Hex(subPlain);
    }
    QString s = QString::fromStdString(plain);
    ui->lineEdit_plain->setText(s);
    cout << "After encrypt and decrypt: " << plain << endl;
    cout << "-----------------------------------------------------" << endl;
}

void Widget::on_pushButton_fe_clicked()
{
    QString fplainStr = ui->lineEdit_fplain->text();
    if(fplainStr != "")
    {
        string saveFilePath = encryptFile(fplainStr.toStdString());
        QMessageBox::information(NULL, tr("Notice"), tr("Your File has been encrypted to: ") + QString::fromStdString(saveFilePath));
        ui->lineEdit_fcipher->setText(QString::fromStdString(saveFilePath));
        cout << "Your Encrypted File is: " << saveFilePath << endl;
        cout << "-----------------------------------------------------" << endl;
    }
    else
    {
        QMessageBox::information(NULL, tr("Notice"), tr("Please Select File To Be Encrypted!"));
    }
}

void Widget::on_pushButton_fd_clicked()
{
    QString fcipherStr = ui->lineEdit_fcipher->text();
    if(fcipherStr != "")
    {
        string saveFilePath = decryptFile(fcipherStr.toStdString());
        QMessageBox::information(NULL, tr("Notice"), tr("Your File has been decrypted to: ") + QString::fromStdString(saveFilePath));
        ui->lineEdit_fplain->setText(QString::fromStdString(saveFilePath));
        cout << "Your Decrypted File is: " << saveFilePath << endl;
        cout << "-----------------------------------------------------" << endl;
    }
    else
    {
        QMessageBox::information(NULL, tr("Notice"), tr("Please Select File To Be Decrypted!"));
    }
}

void Widget::on_toolButton_1_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select Cipher File"),
                                                "/Users/macbookair/Desktop",
                                                tr("Files (*.txt *.md *.pdf *.doc *.docx *.csv *.zip *.rar);;"
                                                   "A/V (*.mp3 *.wav *.wma *.mp4 *.avi);;"
                                                   "Images (*.jpg *.png *.bmp *.gif);;"
                                                   "Codes (*.py *.cpp *.html *.xml *.js *.java *.bat)"));
    if(path.length() == 0)
    {
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    else
    {
        ui->lineEdit_fplain->setText(path);
        cout << "Your Selected File is: " << path.toStdString() << endl;
        cout << "-----------------------------------------------------" << endl;
    }
}

void Widget::on_toolButton_2_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select Cipher File"),
                                                "/Users/macbookair/Desktop",
                                                tr("Files (*.txt *.md *.pdf *.doc *.docx *.csv *.zip *.rar);;"
                                                   "A/V (*.mp3 *.wav *.wma *.mp4 *.avi);;"
                                                   "Images (*.jpg *.png *.bmp *.gif);;"
                                                   "Codes (*.py *.cpp *.html *.xml *.js *.java *.bat)"));
    if(path.length() == 0)
    {
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    else
    {
        ui->lineEdit_fcipher->setText(path);
        cout << "Your Selected File is: " << path.toStdString() << endl;
        cout << "-----------------------------------------------------" << endl;
    }
}

void Widget::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked())
    {
        cout << "check box" << endl;
    }
}
