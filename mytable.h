#ifndef MYTABLE_H
#define MYTABLE_H

#include <QObject>
#include <QTableWidget>
#include <QHeaderView>

class MyTable : public QTableWidget
{
    Q_OBJECT
public:
    MyTable(QWidget *parent = nullptr);
    // 设置表格头部
    void setHeaderLabels(QStringList labels);
    // 表格数据居中
    void tableDataCenter();
private:
    // 初始化表格样式
    void initTable();
};

#endif // MYTABLE_H
