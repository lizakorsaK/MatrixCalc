#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMessageBox>


#include "simple_matrix.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

private slots:
    void on_rows_spinBox_valueChanged(int arg1);

    void on_column_spinbox_valueChanged(int arg1);

    void on_pushButton_scalarmul_clicked();

    void on_pushButton_inver_clicked();

    void on_pushButton_det_clicked();

    void on_pushButton_transpose_clicked();

    void on_rows_spinBox_4_valueChanged(int arg1);

    void on_column_spinbox_4_valueChanged(int arg1);

    void on_pushButton_swapMat_clicked();

    void on_pushButton_addMat_clicked();

    void on_pushButton_subMat_clicked();

    void on_pushButton_mulMat_clicked();

private:
    Ui::Window *ui;

    bool isFirstMatrixValid();
    bool isSecondMatrixValid();
    bool isSame();
};
#endif // WINDOW_H
