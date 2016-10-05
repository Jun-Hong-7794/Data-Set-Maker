#pragma once
#include <iostream>

#include <QDir>
#include <QString>
#include <QTime>
#include <QDateTime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video.hpp>

#define IMG_TYPE_ORIGINAL           1
#define IMG_TYPE_SEGMENT            2
#define IMG_TYPE_TRAFFIC_SIGNAL     3
#define IMG_TYPE_SIGN               4
#define IMG_TYPE_PEDESTRIAN         5

class CSaveImg{

private:
    QDir m_qdir;

    QString m_current_dir;
    QString m_folder_path;

    bool fl_time_stamp_option;
    int m_file_index;
public:
    CSaveImg();
    ~CSaveImg();

private:
    QString Current_Time_to_String();
    QString Current_Time_to_String_Just_Time();

public:
    void Save_Folder_Init(QString _folder_name, int _img_type, bool _using_time_stamp_name = false);
    void Save_Folder_Init(QString _upper_folder_name, QString _under_folder_name);
    void Save_Folder_Init(QString _folder_path);

    void Set_Folder_Output_Dir(QString _dir);

    QString Save_Image(cv::Mat _img, int _time_stamp = 0);
    bool Save_Image(cv::Mat _img, QString _under_folder_name);

    QString Save_Dir();

    void Close_Folder();

};
