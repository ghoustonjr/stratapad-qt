#ifndef NODESET_ITEM_H
#define NODESET_ITEM_H

#include <QUuid>
#include <QList>

#include "strata_shared.h"

struct nodeset_item
{
    QUuid Id;
    object_type Type;
    QString Hash;
    QString Title;
};

#endif
