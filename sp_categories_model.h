#ifndef SP_CATEGORIES_MODEL_H
#define SP_CATEGORIES_MODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "sp_db_manager.h"

enum CategoryType {
    CategoryType_Expl,
    CategoryType_Obs,
    CategoryType_Tag
};

struct Category {
    CategoryType type;
    QString title;
    QList<QString> data;
};

class sp_categories_model : public QAbstractItemModel
{
public:
    explicit sp_categories_model(QObject *parent=0);
    ~sp_categories_model();
};

#endif // SP_CATEGORIES_MODEL_H
