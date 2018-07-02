#ifndef UTIL_H
#define UTIL_H

#include <QMap>
#include <QVariant>

#ifndef InvalidCodePath
#define InvalidCodePath Q_ASSERT(false)
#endif

/**********************************************
 * AUTO-GENERATED FILE, DO NOT MODIFY BY HAND *
 **********************************************/

#define DELETE_AND_NULLIFY(x) \
    do { \
        delete x; \
        x = 0; \
    } while (0)

#define ASSERT_PRESENT(object, key) \
    do { \
        Q_ASSERT(object.contains(key)); \
    } while (0)

#define BEGIN_VALIDATION() \
    QStringList errors; \

#define END_VALIDATION() \
    if (!errors.isEmpty()) \
    { \
        if (err_OUT) \
            *err_OUT = errors; \
        Q_ASSERT(err_OUT->isEmpty()); \
        return StatusCode_ERROR; \
    } \

#define STR_EQUALS(object, field, expected) \
    QString field ## _Converted; \
    do { \
        if (!object.contains(#field)) \
        { \
            QString msg = QString("Missing the following field: %1").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
        QVariant before = object.value(#field); \
\
        if (!before.canConvert(QVariant::String)) \
        { \
            QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'QString'").arg(#field).arg(before.typeName()); \
            errors.append(msg); \
            break; \
        } \
\
        if (!before.convert(QVariant::String)) \
        { \
            QString msg = QString("Field '%1' contains data preventing a conversion to 'QString'").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
        QString actual = before.value<QString>(); \
        if (actual != expected) \
        { \
            QString msg = QString("Field had a bad value. Expected: '%1' Actual: '%2'").arg(expected).arg(actual); \
            errors.append(msg); \
            break; \
        } \
    \
        field ## _Converted = actual; \
    } while (0)

#define IS_STRING(object, field) \
    QString field ## _Converted; \
    do { \
        if (!object.contains(#field)) \
        { \
            QString msg = QString("Missing the following field: %1").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
        QVariant before = object.value(#field); \
\
        if (!before.canConvert(QVariant::String)) \
        { \
            QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'QString'").arg(#field).arg(before.typeName()); \
            errors.append(msg); \
            break; \
        } \
\
        if (!before.convert(QVariant::String)) \
        { \
            QString msg = QString("Field '%1' contains data preventing a conversion to 'QString'").arg(#field); \
            errors.append(msg); \
            break; \
        } \
        field ## _Converted = before.value<QString>(); \
\
    } while (0)

#define IS_INT(object, field) \
    int field ## _Converted; \
    do { \
        if (!object.contains(#field)) \
        { \
            QString msg = QString("Missing the following field: %1").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
        QVariant before = object.value(#field); \
\
        if (!before.canConvert(QVariant::Int)) \
        { \
            QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'int'").arg(#field).arg(before.typeName()); \
            errors.append(msg); \
            break; \
        } \
\
        if (!before.convert(QVariant::Int)) \
        { \
            QString msg = QString("Field '%1' contains data preventing a conversion to 'int'").arg(#field); \
            errors.append(msg); \
            break; \
        } \
        field ## _Converted = before.value<int>(); \
\
    } while (0)

#define IS_UUID(object, field) \
    QUuid field ## _Converted; \
    do { \
        if (!object.contains(#field)) \
        { \
            QString msg = QString("Missing the following field: %1").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
        QVariant before = object.value(#field); \
\
        if (!before.canConvert(QVariant::Uuid)) \
        { \
            QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'QUuid'").arg(#field).arg(before.typeName()); \
            errors.append(msg); \
            break; \
        } \
\
        if (!before.convert(QVariant::Uuid)) \
        { \
            QString msg = QString("Field '%1' contains data preventing a conversion to 'QUuid'").arg(#field); \
            errors.append(msg); \
            break; \
        } \
    \
        field ## _Converted = before.value<QUuid>(); \
        if (field ## _Converted.isNull()) { \
            QString msg = QString("Field '%1' was the null UUID.").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
    } while (0)

#define IS_SLIST(object, field) \
    QStringList field ## _Converted; \
    do { \
        if (!object.contains(#field)) \
        { \
            QString msg = QString("Missing the following field: %1").arg(#field); \
            errors.append(msg); \
            break; \
        } \
\
        QVariant before = object.value(#field); \
\
        if (!before.canConvert(QVariant::StringList)) \
        { \
            QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'QStringList'").arg(#field).arg(before.typeName()); \
            errors.append(msg); \
            break; \
        } \
\
        if (!before.convert(QVariant::StringList)) \
        { \
            QString msg = QString("Field '%1' contains data preventing a conversion to 'QStringList'").arg(#field); \
            errors.append(msg); \
            break; \
        } \
        field ## _Converted = before.value<QStringList>(); \
\
    } while (0)

#define ASSERT_ILIST_1(object, field) ASSERT_ILIST_2(object, field, #field)
#define IS_ILIST_2(object, field, strname) \
    QList<int> field ## _Converted; \
    do { \
        if (!object.contains(strname)) \
        { \
            QString msg = QString("Missing the following field: %1").arg(strname); \
            errors.append(msg); \
            break; \
        } \
\
        QVariant before = object.value(strname); \
\
        if (!before.canConvert(QVariant::List)) \
        { \
            QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'QList'").arg(strname).arg(before.typeName()); \
            errors.append(msg); \
            break; \
        } \
\
        if (!before.convert(QVariant::List)) \
        { \
            QString msg = QString("Field '%1' contains data preventing a conversion to 'QList'").arg(strname); \
            errors.append(msg); \
            break; \
        } \
\
        QVariantList outerConversion = before.value<QVariantList>(); \
        for (QVariant x : outerConversion) \
        { \
            if (!x.canConvert(QVariant::Int)) { \
                QString msg = QString("Field '%1' has type '%2' and cannot be converted to type 'int'").arg(strname).arg(x.typeName()); \
                errors.append(msg); \
                break; \
            } \
            if (!x.convert(QVariant::Int)) { \
                QString msg = QString("Field '%1' contains data preventing a conversion to 'int'").arg(strname); \
                errors.append(msg); \
                break; \
            } \
            field ## _Converted.append(x.value<int>()); \
        } \
\
    } while (0)

#if 0
void
assert_str_equals(const ObjectMap& object, const QString& field, const QString& expected, QStringList* errors_OUT)
{
    if (!object.contains(field))
    {
        QString msg = QString("Missing the following field: %1").arg(field);
        errors_OUT->append(msg);
        return;
    }

    QVariant before = object.value(field);

    if (!before.canConvert(QVariant::String))
    {
        QString msg = QString("Field '%1' cannot be converted to type 'QString'").arg(field);
        errors_OUT->append(msg);
        return;
    }

    if (!before.convert(QVariant::String))
    {
        QString msg = QString("Field '%1' contains data preventing a conversion to 'QString'").arg(field);
        errors_OUT->append(msg);
        return;
    }

    QString actual = before.value<QString>();
    if (actual != expected)
    {
        QString msg = QString("Field had a bad value. Expected: '%1' Actual: '%2'").arg(expected).arg(actual);
        errors_OUT->append(msg);
        return;
    }
} // assert_str_equals

#endif // if 0

enum RunState {
    RUN_NONE,
    RUN_START,
    RUN_INSIDE,
    RUN_END
};

inline RunState
runCategory(bool before, bool current)
{
    RunState result;
    if (!before && !current)
    {
        result = RUN_NONE;
    }
    else if (!before && current)
    {
        result = RUN_START;
    }
    else if (before && current)
    {
        result = RUN_INSIDE;
    }
    else
    {
        result = RUN_END;
    }

    return result;
}

#endif // UTIL_H
