#include "mytable.h"

MyTable::MyTable(QWidget *parent)
    : QTableWidget(parent)
{
    initTable();
    tableDataCenter();
}

void MyTable::setHeaderLabels(QStringList labels)
{
    setColumnCount(labels.size());
    setHorizontalHeaderLabels(labels);
}

void MyTable::initTable()
{
    setFocusPolicy(Qt::NoFocus); //设置表格不获取焦点
    setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置表格内容不可编辑
    verticalHeader()->setHidden(true);       //隐藏行号列
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setSelectionMode(QAbstractItemView::NoSelection); //设置不可选中
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); //设置列宽不可调节
    setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中模式为整行
    setSelectionMode(QAbstractItemView::SingleSelection); //设置选中为单选模式
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//平均分配列宽
    setShowGrid(false); //设置不显示格子线
    horizontalHeader()->setSectionsClickable(false);
}

void MyTable::tableDataCenter()
{
    int row = rowCount();
    int column = columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if( item(i, j) )
            {
                item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            }
        }
    }
}
