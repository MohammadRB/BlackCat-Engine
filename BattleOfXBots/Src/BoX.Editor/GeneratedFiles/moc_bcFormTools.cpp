/****************************************************************************
** Meta object code from reading C++ file 'bcFormTools.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "Editor/EditorPCH.h"
#include "../../../../Src/BlackCat.Editor/Editor/UI/bcFormTools.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bcFormTools.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_black_cat__editor__bc_form_tools_t {
    QByteArrayData data[16];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_black_cat__editor__bc_form_tools_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_black_cat__editor__bc_form_tools_t qt_meta_stringdata_black_cat__editor__bc_form_tools = {
    {
QT_MOC_LITERAL(0, 0, 32), // "black_cat::editor::bc_form_tools"
QT_MOC_LITERAL(1, 33, 19), // "objectSelectToggled"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 9), // "p_toggled"
QT_MOC_LITERAL(4, 64, 19), // "objectInsertToggled"
QT_MOC_LITERAL(5, 84, 20), // "terrainHeightToggled"
QT_MOC_LITERAL(6, 105, 20), // "terrainSmoothToggled"
QT_MOC_LITERAL(7, 126, 22), // "terrainMaterialToggled"
QT_MOC_LITERAL(8, 149, 28), // "terrainMaterialSmoothToggled"
QT_MOC_LITERAL(9, 178, 19), // "decalPainterToggled"
QT_MOC_LITERAL(10, 198, 18), // "decalSelectToggled"
QT_MOC_LITERAL(11, 217, 12), // "mousePressed"
QT_MOC_LITERAL(12, 230, 12), // "QMouseEvent*"
QT_MOC_LITERAL(13, 243, 7), // "p_event"
QT_MOC_LITERAL(14, 251, 13), // "mouseReleased"
QT_MOC_LITERAL(15, 265, 10) // "mouseMoved"

    },
    "black_cat::editor::bc_form_tools\0"
    "objectSelectToggled\0\0p_toggled\0"
    "objectInsertToggled\0terrainHeightToggled\0"
    "terrainSmoothToggled\0terrainMaterialToggled\0"
    "terrainMaterialSmoothToggled\0"
    "decalPainterToggled\0decalSelectToggled\0"
    "mousePressed\0QMouseEvent*\0p_event\0"
    "mouseReleased\0mouseMoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_black_cat__editor__bc_form_tools[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x08 /* Private */,
       4,    1,   72,    2, 0x08 /* Private */,
       5,    1,   75,    2, 0x08 /* Private */,
       6,    1,   78,    2, 0x08 /* Private */,
       7,    1,   81,    2, 0x08 /* Private */,
       8,    1,   84,    2, 0x08 /* Private */,
       9,    1,   87,    2, 0x08 /* Private */,
      10,    1,   90,    2, 0x08 /* Private */,
      11,    1,   93,    2, 0x08 /* Private */,
      14,    1,   96,    2, 0x08 /* Private */,
      15,    1,   99,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

void black_cat::editor::bc_form_tools::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<bc_form_tools *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->objectSelectToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->objectInsertToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->terrainHeightToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->terrainSmoothToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->terrainMaterialToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->terrainMaterialSmoothToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->decalPainterToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->decalSelectToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->mousePressed((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 9: _t->mouseReleased((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 10: _t->mouseMoved((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject black_cat::editor::bc_form_tools::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_black_cat__editor__bc_form_tools.data,
    qt_meta_data_black_cat__editor__bc_form_tools,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *black_cat::editor::bc_form_tools::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *black_cat::editor::bc_form_tools::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_black_cat__editor__bc_form_tools.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int black_cat::editor::bc_form_tools::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
