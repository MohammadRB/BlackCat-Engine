/****************************************************************************
** Meta object code from reading C++ file 'bcFormMainToolBar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "Editor/EditorPCH.h"
#include "../../../../Src/BlackCat.Editor/Editor/UI/bcFormMainToolBar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bcFormMainToolBar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_black_cat__editor__bc_form_main_tool_bar_t {
    QByteArrayData data[5];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_black_cat__editor__bc_form_main_tool_bar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_black_cat__editor__bc_form_main_tool_bar_t qt_meta_stringdata_black_cat__editor__bc_form_main_tool_bar = {
    {
QT_MOC_LITERAL(0, 0, 40), // "black_cat::editor::bc_form_ma..."
QT_MOC_LITERAL(1, 41, 13), // "gameModeClick"
QT_MOC_LITERAL(2, 55, 0), // ""
QT_MOC_LITERAL(3, 56, 14), // "pauseModeClick"
QT_MOC_LITERAL(4, 71, 15) // "editorModeClick"

    },
    "black_cat::editor::bc_form_main_tool_bar\0"
    "gameModeClick\0\0pauseModeClick\0"
    "editorModeClick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_black_cat__editor__bc_form_main_tool_bar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08 /* Private */,
       3,    1,   32,    2, 0x08 /* Private */,
       4,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void black_cat::editor::bc_form_main_tool_bar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<bc_form_main_tool_bar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->gameModeClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->pauseModeClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->editorModeClick((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject black_cat::editor::bc_form_main_tool_bar::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_black_cat__editor__bc_form_main_tool_bar.data,
    qt_meta_data_black_cat__editor__bc_form_main_tool_bar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *black_cat::editor::bc_form_main_tool_bar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *black_cat::editor::bc_form_main_tool_bar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_black_cat__editor__bc_form_main_tool_bar.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int black_cat::editor::bc_form_main_tool_bar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
