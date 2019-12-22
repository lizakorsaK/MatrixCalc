#include "window.h"
#include "ui_window.h"


Window::Window(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);

    this->setFixedSize(1100,430);


}

Window::~Window()
{
    delete ui;
}

void Window::on_rows_spinBox_valueChanged(int arg1)
{
    this->ui->firstMatrix->setRowCount(arg1);
    this->ui->firstMatrix->update();
}

void Window::on_column_spinbox_valueChanged(int arg1)
{
    this->ui->firstMatrix->setColumnCount(arg1);
    this->ui->firstMatrix->update();
}

void Window::on_pushButton_scalarmul_clicked()
{
    if (!isFirstMatrixValid()) return;

    bool isValidScalar = false;
    double scalarValue = this->ui->scalar_value->text().toDouble(&isValidScalar);
    if (!isValidScalar)
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введено некоректное значение"), QMessageBox::Ok);
        return;
    }

    for (int rowIndex = 0; rowIndex < this->ui->firstMatrix->rowCount(); ++rowIndex)
        for (int columnIndex = 0; columnIndex < this->ui->firstMatrix->columnCount(); ++columnIndex)
        {
            double previousValueInCell = this->ui->firstMatrix->item(rowIndex,columnIndex)->text().toDouble();
            double newValue = previousValueInCell * scalarValue;
            this->ui->firstMatrix->item(rowIndex,columnIndex)->setText(QString::number(newValue));
        }
}

bool Window::isFirstMatrixValid()
{
    bool result = true;

    for (int rowIndex = 0; rowIndex < this->ui->firstMatrix->rowCount(); ++rowIndex)
        for (int columnIndex = 0; columnIndex < this->ui->firstMatrix->columnCount(); ++columnIndex)
        {
            if (this->ui->firstMatrix->item(rowIndex,columnIndex) == nullptr)
            {
                QMessageBox::warning(this, tr("Ошибка"), tr("Пустое поле в первой матрице"), QMessageBox::Ok);
                return false;
            }
            else
            {
                this->ui->firstMatrix->item(rowIndex,columnIndex)->text().toDouble(&result);
                if (result == false)
                    QMessageBox::warning(this, tr("Ошибка"), tr("Введено некоректное значение в первой матрице"), QMessageBox::Ok);
            }
        }

    return result;
}

bool Window::isSecondMatrixValid()
{
    bool result = true;

    for (int rowIndex = 0; rowIndex < this->ui->secondMatrix->rowCount(); ++rowIndex)
        for (int columnIndex = 0; columnIndex < this->ui->secondMatrix->columnCount(); ++columnIndex)
        {
            if (this->ui->secondMatrix->item(rowIndex,columnIndex) == nullptr)
            {
                QMessageBox::warning(this, tr("Ошибка"), tr("Пустое поле во второй матрице"), QMessageBox::Ok);
                return false;
            }
            else
            {
                this->ui->secondMatrix->item(rowIndex,columnIndex)->text().toDouble(&result);
                if (result == false)
                    QMessageBox::warning(this, tr("Ошибка"), tr("Введено некоректное значение во втророй матрице"), QMessageBox::Ok);
            }
        }

    return result;
}

bool Window::isSame()
{
    bool result = ui->firstMatrix->rowCount() == ui->secondMatrix->rowCount() && ui->firstMatrix->columnCount() == ui->secondMatrix->columnCount();
    if (result == false) QMessageBox::warning(this, tr("Ошибка"), tr("Размеры матриц не совпадают"), QMessageBox::Ok);
    return result;
}

void Window::on_pushButton_inver_clicked()
{
    if (!isFirstMatrixValid()) return;
    DMatrix m(ui->firstMatrix);
    try {
        m = m.inverse();
    } catch (const std::exception& any) {
        QMessageBox::warning(this, tr("Ошибка"), any.what());
    }

    m.install(ui->firstMatrix);
}

void Window::on_pushButton_det_clicked()
{
    if (!isFirstMatrixValid()) return;
    if (this->ui->firstMatrix->rowCount() != this->ui->firstMatrix->columnCount())
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введённая матрица не квадратная"), QMessageBox::Ok);
        return;
    }

     DMatrix m(ui->firstMatrix);
     QMessageBox::information(this, "Значение детерминанта", QString("Детерминант равен ") + QString::number(m.det()));
}

void Window::on_pushButton_transpose_clicked()
{
    if (!isFirstMatrixValid()) return;
    if (this->ui->firstMatrix->rowCount() != this->ui->firstMatrix->columnCount())
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введённая матрица не квадратная"), QMessageBox::Ok);
        return;
    }

     DMatrix m(ui->firstMatrix);
     m = m.transpose();
     m.install(ui->firstMatrix);
}

void Window::on_rows_spinBox_4_valueChanged(int arg1)
{
    ui->secondMatrix->setRowCount(arg1);
    ui->secondMatrix->update();
}

void Window::on_column_spinbox_4_valueChanged(int arg1)
{
    ui->secondMatrix->setColumnCount(arg1);
    ui->secondMatrix->update();
}



void Window::on_pushButton_addMat_clicked()
{
    if(!isFirstMatrixValid() or !isSecondMatrixValid() or !isSame()) return;

    DMatrix first(ui->firstMatrix);
    DMatrix second(ui->secondMatrix);
    first = first + second;
    first.install(ui->firstMatrix);
}

void Window::on_pushButton_subMat_clicked()
{
    if(!isFirstMatrixValid() or !isSecondMatrixValid() or !isSame()) return;

    DMatrix first(ui->firstMatrix);
    DMatrix second(ui->secondMatrix);
    first = first - second;
    first.install(ui->firstMatrix);
}

void Window::on_pushButton_swapMat_clicked()
{
    if(!isFirstMatrixValid() or !isSecondMatrixValid()) return;

    // сохранение матриц
    DMatrix first(ui->firstMatrix);
    DMatrix second(ui->secondMatrix);

    // смена бегунков
    int tmp = ui->rows_spinBox->value();
    ui->rows_spinBox->setValue(ui->rows_spinBox_4->value());
    ui->rows_spinBox_4->setValue(tmp);
    tmp = ui->column_spinbox->value();
    ui->column_spinbox->setValue(ui->column_spinbox_4->value());
    ui->column_spinbox_4->setValue(tmp);

    // смена размеров
    ui->firstMatrix->setRowCount(ui->rows_spinBox->value());
    ui->firstMatrix->setColumnCount(ui->column_spinbox->value());
    ui->secondMatrix->setRowCount(ui->rows_spinBox_4->value());
    ui->secondMatrix->setColumnCount(ui->column_spinbox_4->value());

    // ждём обновления виджета
    qApp->processEvents();

    // установка матриц
    first.install(ui->secondMatrix);
    second.install(ui->firstMatrix);
}

void Window::on_pushButton_mulMat_clicked()
{
    if(!isFirstMatrixValid() or !isSecondMatrixValid()) return;
    try {
        DMatrix first(ui->firstMatrix);
        DMatrix second(ui->secondMatrix);
        DMatrix result = first*second;

        ui->firstMatrix->setColumnCount(result.getW());
        ui->firstMatrix->setRowCount(result.getH());

        ui->firstMatrix->update();
        qApp->processEvents();

        result.install(ui->firstMatrix);

    } catch (const std::exception& any) {
        QMessageBox::critical(this, "Ошибка", QString(any.what()));
    }
}
