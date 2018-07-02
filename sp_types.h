#ifndef STRATAPADTYPES_H
#define STRATAPADTYPES_H

#include <QUuid>
#include <QVariant>
#include <QDateTime>

#include "strata_shared.h"

inline QString
SpConv(string S)
{
    return(QString::fromUtf8((const char *)S.Text, (int)S.ByteSize));
}

inline QUuid
SpConv(uuid U)
{
    QByteArray Bytes((const char *)U.Bytes, sizeof(U.Bytes));
    return(QUuid::fromRfc4122(Bytes));
}

inline QString
SpConv(sha256 Hash)
{
    QByteArray Bytes = QByteArray::fromRawData((const char *)Hash.Bytes, sizeof(Hash.Bytes));
    QString Result(Bytes.toHex());

    return(Result);
}

static uuid
ConvertToSpUuid(const QUuid& QtId)
{
    Q_ASSERT(!QtId.isNull());
    uuid Result = {};
    QByteArray BinaryQtId = QtId.toRfc4122();
    Assert(sizeof(Result.Bytes) == BinaryQtId.size());

    //TODO: Think about this some more. I think memcpy_s is a windows function.
    int RC = memcpy_s((void*)&Result.Bytes, sizeof(Result.Bytes), (void*)BinaryQtId.data(), BinaryQtId.size());
    Assert(RC == 0);

    return(Result);
}

static sha256
ConvertToSpHash(const QString& QtHash)
{
    Q_ASSERT(!QtHash.isNull());
    sha256 Result = {};
    QByteArray BinaryQtHash = QByteArray::fromHex(QtHash.toUtf8());
    Assert(sizeof(Result.Bytes) == BinaryQtHash.size());

    //TODO: Think about this some more. I think memcpy_s is a windows function.
    int RC = memcpy_s((void*)&Result.Bytes, sizeof(Result.Bytes), (void*)BinaryQtHash.data(), BinaryQtHash.size());
    Assert(RC == 0);

    return(Result);
}

struct basic_node {
    QString Title;
};

enum status_code {
    StatusCode_ERROR = 0,
    StatusCode_OK
};

#endif // STRATAPADTYPES_H
