//
// Created by AlexFan on 2020/3/2.
//
#include<iostream>
#include<string>
#include"func_filesystem.h"
#include"ui_filesystem.h"
#include<Windows.h>
using namespace std;

void start(){
    int choice = 0;
    char name[30], pass[30], pass1[30];
    system("CLS");
    Sleep(2);
    while (true) {
        system("CLS");
        cout << "\t\t��������������������������������������������������������������������������������\t" << endl;
        cout << "\t\t          Alex Fan File System           \t" << endl;
        cout << "\t\t               1. sign up               \t" << endl;
        cout << "\t\t               2. login                    \t" << endl;
        cout << "\t\t               3. help                    \t" << endl;
        cout << "\t\t               4. sign out                    \t" << endl;
        cout << "\t\t��������������������������������������������������������������������������������\t" << endl;
        cout << "\t\t��ѡ��(���������ֱ��): " ;
        cin >> choice;
        switch (choice) {
            /*ѡ��ע��*/
            case 1: {
                cout << "�������û���:";
                cin >> name;
                cout << "����������: ";
                cin >> pass;
                cout << "��ȷ������: ";
                cin >> pass1;
                while (strcmp(pass, pass1) != 0) {
                    cout << "���벻һ�£�������" << endl;
                    cout << "����������: ";
                    cin >> pass;
                    cout << "��ȷ������: ";
                    cin >> pass1;
                }
                if (regist(name, pass) == 1){
                    cout << "ע��ɹ�.." << endl;
                    Sleep(2);
                }
                else{
                    cout << "ע��ʧ��" << endl;
                    Sleep(2);
                }
            }
                break;

                /*ѡ���¼*/
            case 2: {
                cout << "�������û���:";
                cin >> name;
                cout << "����������:";
                cin >> pass;
                if (login(name, pass) == 1) {
                    cout << "��¼�ɹ�" << endl;
                    operate(name, pass);

                } else{
                    cout << "��¼ʧ�ܣ������û���������" << endl;
                    Sleep(2);
                }}
                break;
            case 3:{
                system("CLS");
                help();
                system("Pause");
            }break;

            case 4: {
                system("CLS");
                cout << "�˳��ļ�ϵͳ" << endl;
                Sleep(3);
                exit(0);
            }
                break;
                /*����ѡ��*/
            default:
                cout << "������ڣ�������ѡ��"<< endl;

                break;
        }
    }

}//��ʼ�ļ�ϵͳ
void help() {
    cout << "-------------------------------------------------------------" << endl;
    cout << "-----------------Alex Fan File System �����ֲ�----------------" << endl;
    cout << " 1.touch + <�ļ���>                                �½��ļ�   " << endl;
    cout << " 2.rm + <�ļ���>                                   ɾ���ļ�   " << endl;
    cout << " 3.cp + <�ļ���>                                   �����ļ�   " << endl;
    cout << " 4.ps                                             ճ���ļ�   " << endl;
    cout << " 5.open + <�ļ���>                                 ���ļ�   " << endl;
    cout << " 6.vi + <�ļ���>                                   �༭�ļ�   " << endl;
    cout << " 7.mkdir + <�ļ���>                                �½�Ŀ¼   " << endl;
    cout << " 8.rmdir + <�ļ���>                                ɾ��Ŀ¼   " << endl;
    cout << " 9.ls                                             չ��Ŀ¼   " << endl;
    cout << " 10.cd + <�ļ���>                                  �����¼�Ŀ¼" << endl;
    cout << " 11.cd..                                          �����ϼ�Ŀ¼" << endl;
    cout << " 12.clear                                         ����       " << endl;
    cout << " 13.exit                                          ע��       " << endl;
    cout << "��������������������������������������������������������������������������������������������������������������������������" << endl
         << endl;

}/*�û�ע��д���ļ�����*/
int regist(char username[30], char password[30]) {

    FILE *cfptr;//�ļ�ָ��
    if ((cfptr = fopen("user.ini", "a+")) == nullptr) {
        printf("File client.txt could not be opened\n");
        fclose(cfptr);
        return 0;
    } else {
        fprintf(cfptr, "%s %s\n", username, password);
        fclose(cfptr);
        return 1;

    }
}/*�û�����������飬�Ƿ��Ѿ�ע�ᣨ���ļ������ݱȽϣ�*/
int login(char username[30], char password[30]) {
    char user[30];
    char pass[30];
    FILE *cfptr;//�ļ�ָ��
    if ((cfptr = fopen("user.ini", "r")) == nullptr) {
        printf("File client.txt could not be opened\n");
        fclose(cfptr);
        return 0;
    } else {
        while (!feof(cfptr)) {
            fscanf(cfptr, "%s%s", user, pass);
            if ((strcmp(username, user) == 0) && (strcmp(password, pass) == 0)) {
                fclose(cfptr);
                return 1;
            }
        }
    }
    fclose(cfptr);
    return 0;
}//��¼
int operate(char name[30], char pass[30]) {
    func_filesystem af_filesystem;
    af_filesystem.setUser(name, pass);
    while (1) {
        system("CLS");
        while (1) {
            cout << endl;
            string choice;
            af_filesystem.showPath();
            cin >> choice;
            if (choice == "mkdir")
                af_filesystem.newDir();
            else if (choice == "touch")
                af_filesystem.newFile();
            else if (choice == "rmdir")
                af_filesystem.deleteDir();
            else if (choice == "rm")
                af_filesystem.deleteFile();
            else if (choice == "cd")
                af_filesystem.open_dir();
            else if (choice == "open")
                af_filesystem.open_file();
            else if (choice == "ls")
                af_filesystem.ls();
            else if (choice == "cp")
                af_filesystem.copyFile();
            else if (choice == "ps") {
                af_filesystem.pasteFile();
            } else if (choice == "vi")
                af_filesystem.write();
            else if (choice == "cd..")
                af_filesystem.goback();
            else if (choice == "pwd")
                af_filesystem.pwd();
            else if (choice == "clear") {
                system("CLS");
                help();
            } else if (choice == "exit") {
                system("CLS");
                cout << "�û�: " << name << "����ע��"
                     << endl;
                Sleep(3);
                return 0;
            } else if (choice == "help") {
                help();
            }
            else
                cout << "������Ч��" << endl;
        }
    }

}//ѡ�����
