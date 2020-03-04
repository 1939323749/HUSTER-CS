//
// Created by AlexFan on 2020/3/2.
//

#ifndef AF_func_filesystem_FUNC_func_filesystem_H
#define AF_func_filesystem_FUNC_func_filesystem_H
#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
using namespace std;

#define NAME_LENGTH  20    //�ļ�������Ŀ¼��������ֽ���
extern int disk_storage;


typedef struct file         //�ļ��ṹ��
{
    char name[NAME_LENGTH];      //�ļ��� 
    int size;                 //�ļ���С
    struct file *nextFile;  //ָ���ļ��б�����һ���ļ�
    string content;           //�ļ�����
} file;

typedef struct directory          //Ŀ¼�ṹ��
{
    char name[NAME_LENGTH];      //Ŀ¼����
    int size;                 //Ŀ¼��С
    directory *nextDir;           //���Ŀ¼
    directory *preDir;            //ǰ��Ŀ¼
    file *filePtr;          //��Ŀ¼�µ��ļ�����ָ��
    directory *dirPtr;            //��Ŀ¼�µ�Ŀ¼����ָ��
} directory;


class func_filesystem //�ļ�ϵͳ��
{
private:
    directory *currentDir; //��ǰĿ¼
    file *copytempfile; //���ڿ����ļ�ʱ����ʱ�ļ�
    directory *root; //��Ŀ¼
    char password[NAME_LENGTH];   //�û�����
    char name[NAME_LENGTH];   //�û�����
    int size; //�û���ʹ�ÿռ��С

public:
    func_filesystem();//���캯��
    ~func_filesystem();//��������

    /*
    *�ļ�����
    */
public:
    int newFile(); //�����ļ�
    int dele_file(file *file);//ɾ���ļ�
    int deleteFile(); //ɾ���ļ�ǰ���߼��ж�
    int open_file(); //���ļ�
    file *copy_file(file *h); //�����ļ�
    file *copyFile(); //�����ļ�ǰ���߼��ж�
    int pasteFile(); //ճ���ļ�
    int write(); //�༭�ļ�

    /*
     * Ŀ¼����
     */
public:
    int newDir(); //����Ŀ¼
    int dele_dir(directory *d); //ɾ��Ŀ¼
    int deleteDir(); //ɾ��Ŀ¼ǰ���߼��ж�
    int open_dir(); //��Ŀ¼
    int ls(); //��ʾ��ǰĿ¼����

    /*
     * ·������
     */
public:
    int show_path(directory *dir); //��ʾ·���Ĳ���ʵ��
    int showPath(); //��ʾ·��
    int pwd();
    int goback(); //������һ��Ŀ¼

    /*
     * �û�����
     */
public:
    int setUser(char *, char *);//�����û���������
};
#endif //AF_func_filesystem_FUNC_func_filesystem_H
