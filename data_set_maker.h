#ifndef DATA_SET_MAKER_H
#define DATA_SET_MAKER_H

#include <QMainWindow>
#include <QtWidgets>
#include <QKeyEvent>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>

/////////////////////////////////////////////////
///////////////////OpenCv////////////////////////
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video.hpp>

#include "CSaveImg/CSaveImg.h"

#define FRAME_HEIGHT_OFFSET 45

namespace Ui {
class Data_Set_Maker;
}

class Data_Set_Maker : public QMainWindow
{
    Q_OBJECT

public:
    explicit Data_Set_Maker(QWidget *parent = 0);
    ~Data_Set_Maker();


private:
    Ui::Data_Set_Maker *ui;

    QGraphicsScene *mp_org_qgraphic;
    QGraphicsScene *mp_crop_qgraphic;
private:
    cv::Mat m_org_image;
    cv::Mat m_crop_image;

    bool fl_mouse_pressed;
    bool fl_load_path;
    bool fl_load_save_path;

    QPointF m_mouse_pressed_point;
    QPointF m_mouse_released_point;
private:
    void keyPressEvent(QKeyEvent *);
    bool eventFilter(QObject *, QEvent *);

    QPointF Get_Valid_Mouse_Point_Valid(QMouseEvent* _mouse_event);

    QImage Mat2QImage(cv::Mat src);
    void Display_Image(cv::Mat _img, QGraphicsScene* _graphics_scene,QGraphicsView * _graphics_view);

    void Image_Crop(QPointF _top_left_point,QPointF _bottom_right_point);
    void Image_Capture();

    QPointF ViewPoint_To_ImagePoint(QPointF _view_point);
    bool Get_Crop_ROI(QPointF _top_left_point,QPointF _bottom_right_point,cv::Rect &_crop_roi);
private:
    CSaveImg m_save_image;
private:
    bool Save_Image(QString _folder_name);

private:
    QStringList m_image_list;

    int m_org_image_number;
    int m_current_image_number;

    QString m_org_img_folder_path;
    QString m_save_img_folder_path;
public slots:
    void Button_Org_Image_Path();
    void Button_Save_Image_Path();
    void Button_Go_To_The_Index();
};

#endif // DATA_SET_MAKER_H
