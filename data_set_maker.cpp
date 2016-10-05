#include "data_set_maker.h"
#include "ui_data_set_maker.h"

//----------------Dialog----------------//
Data_Set_Maker::Data_Set_Maker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Data_Set_Maker)
{
    ui->setupUi(this);

    m_org_image_number = 0;
    m_current_image_number = 0;

    fl_load_path = false;
    fl_load_save_path = false;
    fl_mouse_pressed = false;

    m_mouse_pressed_point.setX(0);
    m_mouse_pressed_point.setY(0);

    m_mouse_released_point.setX(0);
    m_mouse_released_point.setY(0);

    int orgview_width = ui->view_original->geometry().width();
    int orgview_height = ui->view_original->geometry().height();

    int cropview_width = ui->view_crop->geometry().width();
    int cropview_height = ui->view_crop->geometry().height();

    mp_org_qgraphic = new QGraphicsScene(
                QRectF(0, 0, orgview_width, orgview_height), 0);
    mp_crop_qgraphic = new QGraphicsScene(
                QRectF(0, 0, cropview_width, cropview_height), 0);

    connect(ui->bt_load_path,SIGNAL(clicked()),this,SLOT(Button_Org_Image_Path()));
    connect(ui->bt_load_save_path,SIGNAL(clicked()),this,SLOT(Button_Save_Image_Path()));

    connect(ui->bt_go_to_the_index,SIGNAL(clicked()),this,SLOT(Button_Go_To_The_Index()));

    setMouseTracking(true);
    //ui->view_original->installEventFilter(this);
    qApp->installEventFilter(this);
}

Data_Set_Maker::~Data_Set_Maker()
{
    delete ui;
}

void Data_Set_Maker::Button_Org_Image_Path(){


    QFileDialog dialog(this);
    QStringList fileName;

    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if(dialog.exec())
        fileName = dialog.selectedFiles();
    else
        return;

    m_org_image_number = 0;
    m_current_image_number = 0;

    m_org_img_folder_path = fileName[0];

    ui->ed_org_path->setText(m_org_img_folder_path);

    m_org_image_number = QDir(m_org_img_folder_path).entryList().size() - 2;
    ui->ed_number_of_image->setText(QString::number(m_org_image_number));

    m_image_list = QDir(m_org_img_folder_path).entryList();

    Image_Capture();

    fl_load_path = true;
    //m_selected_folder_name = m_str_folder_path.section("/",-1,-1);

    return;
}

void Data_Set_Maker::Button_Save_Image_Path(){


    QFileDialog dialog(this);
    QStringList fileName;

    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if(dialog.exec())
        fileName = dialog.selectedFiles();
    else
        return;

    m_save_img_folder_path = fileName[0];

    ui->ed_crop_path->setText(fileName[0]);

    m_save_image.Save_Folder_Init(m_save_img_folder_path);

    ui->ed_crop_path->setText(m_save_img_folder_path);

    fl_load_save_path = true;

    //m_org_image_number = QDir(m_org_img_folder_path).entryList().size() - 2;
    //ui->ed_number_of_image->setText(QString::number(m_org_image_number));
    //m_selected_folder_name = m_str_folder_path.section("/",-1,-1);

    return;
}

void Data_Set_Maker::Button_Go_To_The_Index(){

    if(!fl_load_path)
        return;

    m_current_image_number = ui->ed_current_img_index->text().toInt();
    Image_Capture();
}

//----------------Image Display----------------//
void Data_Set_Maker::Image_Capture(){

    if(m_current_image_number < 0)
        m_current_image_number = (m_org_image_number - 1);

    else if(m_current_image_number >= m_org_image_number)
        m_current_image_number = 0;

    std::string str_file_path = (m_org_img_folder_path + "/" + (m_image_list[m_current_image_number + 2]))
           .toLocal8Bit().constData();

    m_org_image = cv::imread(str_file_path);

    Display_Image(m_org_image,mp_org_qgraphic,ui->view_original);

    ui->ed_current_img_index->setText(QString::number(m_current_image_number + 1)); ;

    QString image_size;
    image_size = QString::number(m_org_image.cols) + "X" + QString::number(m_org_image.rows);
    ui->ed_org_size->setText(image_size);
}

void Data_Set_Maker::Display_Image(cv::Mat _img, QGraphicsScene* _graphics_scene,QGraphicsView * _graphics_view){

    if(_img.rows <= 1 || _img.cols <= 1)
        return;

    QImage tmp_qimg;
    QPixmap tmp_qpix;

    tmp_qimg = this->Mat2QImage(_img);
    tmp_qpix.convertFromImage(tmp_qimg);

    _graphics_scene->clear();
    _graphics_scene->addPixmap(tmp_qpix.scaled(QSize(
                        (int)_graphics_scene->width(), (int)_graphics_scene->height()),
                        Qt::KeepAspectRatio, Qt::SmoothTransformation));

    _graphics_view->fitInView(
                QRectF(0, 0, _graphics_view->geometry().width(), _graphics_view->geometry().height()),
                    Qt::KeepAspectRatio);

    _graphics_view->setScene(_graphics_scene);
    _graphics_view->show();

}

QImage Data_Set_Maker::Mat2QImage(cv::Mat src)
{
    cv::Mat temp; // make the same cv::Mat
    cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits(); // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

//----------------Crop Image----------------//
void Data_Set_Maker::Image_Crop(QPointF _top_left_point, QPointF _bottom_right_point){

    cv::Rect crop_roi;

    if(!Get_Crop_ROI(_top_left_point, _bottom_right_point,crop_roi))
        return;

    if(m_org_image.cols <= crop_roi.x + crop_roi.width)
        crop_roi.width = m_org_image.cols - crop_roi.x - 1;

    if(m_org_image.rows <= crop_roi.y + crop_roi.height)
        crop_roi.height = m_org_image.rows - crop_roi.y - 1;

    m_crop_image = m_org_image(crop_roi);

    QString image_size;
    image_size = QString::number(m_crop_image.cols) + "X" + QString::number(m_crop_image.rows);
    ui->ed_crop_size->setText(image_size);

    Display_Image(m_crop_image,mp_crop_qgraphic,ui->view_crop);
}

QPointF Data_Set_Maker::ViewPoint_To_ImagePoint(QPointF _view_point){

    QPointF image_point;

    int view_width = ui->view_original->geometry().width();
    int view_height = ui->view_original->geometry().height();

    double view_h_w_ratio = (double)view_height / (double)view_width;

    int image_width = m_org_image.cols;
    int image_height = m_org_image.rows;

    double image_h_w_ratio = (double)image_height / (double)image_width;

    if(view_h_w_ratio < image_h_w_ratio){
        view_width = (double)((view_width * view_h_w_ratio) / image_h_w_ratio);
    }
    else if(view_h_w_ratio > image_h_w_ratio){
        view_height = (double)((view_height * image_h_w_ratio) / view_h_w_ratio);
    }

    int img_point_x = 0;
    int img_point_y = 0;

    img_point_x = (int)(((double)(_view_point.x()) / (double)(view_width)) * image_width);
    img_point_y = (int)(((double)(_view_point.y()) / (double)(view_height)) * image_height);

    image_point.setX(img_point_x);
    image_point.setY(img_point_y);

    return image_point;
}

bool Data_Set_Maker::Get_Crop_ROI(QPointF _top_left_point,QPointF _bottom_right_point,cv::Rect &_crop_roi){

    int top_left_x = ViewPoint_To_ImagePoint(_top_left_point).x();
    int top_left_y = ViewPoint_To_ImagePoint(_top_left_point).y();

    int bottom_right_x = ViewPoint_To_ImagePoint(_bottom_right_point).x();
    int bottom_right_y = ViewPoint_To_ImagePoint(_bottom_right_point).y();

    int crop_img_width = bottom_right_x - top_left_x;
    int crop_img_height = bottom_right_y - top_left_y;

    if(crop_img_width < 0){//top_left and bottom_right are reversed!
        int left_x = bottom_right_x;
        bottom_right_x = top_left_x;
        top_left_x = left_x;

        crop_img_width = bottom_right_x - top_left_x;
    }
    else if (crop_img_width == 0)
        return false;

    if(crop_img_height < 0){//top_left and bottom_right are reversed!
        int left_y = bottom_right_y;
        bottom_right_y = top_left_y;
        top_left_y = left_y;

        crop_img_height = bottom_right_y - top_left_y;
    }
    else if (crop_img_height == 0)
        return false;

    _crop_roi.x = top_left_x;
    _crop_roi.y = top_left_y;
    _crop_roi.width = crop_img_width;
    _crop_roi.height = crop_img_height;

    return true;
}

bool Data_Set_Maker::Save_Image(QString _folder_name){

    if(_folder_name == "")
        return false;

    if(!fl_load_save_path)
        return false;

    if(ui->ck_save_org_img->isChecked()){
        if(!m_save_image.Save_Image(m_org_image,_folder_name))
            return false;
    }
    else{
        if(!m_save_image.Save_Image(m_crop_image,_folder_name))
            return false;
    }
    return true;
}

QPointF Data_Set_Maker::Get_Valid_Mouse_Point_Valid(QMouseEvent *_mouse_event){

    QPointF offet_point;

    offet_point.setX(0);
    offet_point.setY(FRAME_HEIGHT_OFFSET);

    QPointF point =  _mouse_event->globalPos() - frameGeometry().topLeft() - ui->view_original->frameGeometry().topLeft() - offet_point;

    if(point.x() < 0){
        point.setX(0);
    }
    if(point.x() >= ui->view_original->frameGeometry().width()){
        point.setX(ui->view_original->frameGeometry().width() - 1);
    }
    if(point.y() < 0){
        point.setY(0);
    }
    if(point.y() >= ui->view_original->frameGeometry().height()){
        point.setY(ui->view_original->frameGeometry().height() - 1);
    }

    QString mouse_position = QString::number(point.x()) + "," + QString::number(point.y());
    ui->ed_mouse_position->setText(mouse_position);

    if(fl_load_path){
        QPointF image_point;
        image_point = ViewPoint_To_ImagePoint(point);
        QString image_position = QString::number(image_point.x()) + "," + QString::number(image_point.y());
        ui->ed_image_position->setText(image_position);
    }

    return point;
}


//----------------Keboard Mouse Event----------------//
bool Data_Set_Maker::eventFilter(QObject *_obj, QEvent *_event){

        if( _event->type() == QEvent::MouseMove && (fl_mouse_pressed == true)){
            QMouseEvent *ptr_mouseEvent = static_cast<QMouseEvent*>(_event);
            Get_Valid_Mouse_Point_Valid(ptr_mouseEvent);
        }

        if( _event->type() == QEvent::MouseButtonPress){
            QMouseEvent *ptr_mouseEvent = static_cast<QMouseEvent*>(_event);
            m_mouse_pressed_point = Get_Valid_Mouse_Point_Valid(ptr_mouseEvent);

            fl_mouse_pressed = true;
        }

        if(( _event->type() == QEvent::MouseButtonRelease) && (fl_mouse_pressed == true)){
            QMouseEvent *ptr_mouseEvent = static_cast<QMouseEvent*>(_event);
            m_mouse_released_point = Get_Valid_Mouse_Point_Valid(ptr_mouseEvent);

            Image_Crop(m_mouse_pressed_point,m_mouse_released_point);
            fl_mouse_pressed = false;
        }

    return false;
}

void Data_Set_Maker::keyPressEvent(QKeyEvent *_event){


    if(_event->key() == Qt::Key_R){
        if(ui->ck_save_org_img->isChecked())
            ui->ck_save_org_img->setChecked(false);
        else
            ui->ck_save_org_img->setChecked(true);
    }

    if(_event->key() == Qt::Key_A){
        if(fl_load_path){
            m_current_image_number--;
            Image_Capture();
        }
    }
    if(_event->key() == Qt::Key_D){
        if(fl_load_path){
            m_current_image_number++;
            Image_Capture();
        }
    }

    if(_event->key() == Qt::Key_1){
        if(ui->ck_save_path_1->isEnabled()){
            if(!Save_Image(ui->ed_save_path_1->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_2){
        if(ui->ck_save_path_2->isEnabled()){
            if(!Save_Image(ui->ed_save_path_2->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_3){
        if(ui->ck_save_path_3->isEnabled()){
            if(!Save_Image(ui->ed_save_path_3->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_4){
        if(ui->ck_save_path_4->isEnabled()){
            if(!Save_Image(ui->ed_save_path_4->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_5){
        if(ui->ck_save_path_4->isEnabled()){
            if(!Save_Image(ui->ed_save_path_5->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_6){
        if(ui->ck_save_path_12->isEnabled()){
            if(!Save_Image(ui->ed_save_path_6->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_7){
        if(ui->ck_save_path_13->isEnabled()){
            if(!Save_Image(ui->ed_save_path_7->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_8){
        if(ui->ck_save_path_14->isEnabled()){
            if(!Save_Image(ui->ed_save_path_8->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_9){
        if(ui->ck_save_path_15->isEnabled()){
            if(!Save_Image(ui->ed_save_path_9->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

    if(_event->key() == Qt::Key_0){
        if(ui->ck_save_path_16->isEnabled()){
            if(!Save_Image(ui->ed_save_path_10->text())){
                QMessageBox::information(this, tr("Save Image Error"), tr("Check Image or Path"));
                return;
            }
        }
        else{
            QMessageBox::information(this, tr("Check the class"), tr("Check the class"));
            return;
        }
    }

}
