/****************************************************************************
** Meta object code from reading C++ file 'bcFormObject.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "Editor/EditorPCH.h"
#include "../../../../Src/BlackCat.Editor/Editor/UI/bcFormObject.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bcFormObject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_black_cat__editor__bc_form_object_t {
    QByteArrayData data[4];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_black_cat__editor__bc_form_object_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_black_cat__editor__bc_form_object_t qt_meta_stringdata_black_cat__editor__bc_form_object = {
    {
QT_MOC_LITERAL(0, 0, 33), // "black_cat::editor::bc_form_ob..."
QT_MOC_LITERAL(1, 34, 16), // "actorUpdateClick"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 16) // "actorDeleteClick"

    },
    "black_cat::editor::bc_form_object\0"
    "actorUpdateClick\0\0actorDeleteClick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_black_cat__editor__bc_form_object[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       3,    1,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void black_cat::editor::bc_form_object::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<bc_form_object *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->actorUpdateClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->actorDeleteClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject black_cat::editor::bc_form_object::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_black_cat__editor__bc_form_object.data,
    qt_meta_data_black_cat__editor__bc_form_object,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *black_cat::editor::bc_form_object::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *black_cat::editor::bc_form_object::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_black_cat__editor__bc_form_object.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int black_cat::editor::bc_form_object::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
