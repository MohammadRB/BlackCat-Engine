/****************************************************************************
** Meta object code from reading C++ file 'bcWidgetConsole.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "Editor/EditorPCH.h"
#include "../../../../Src/BlackCat.Editor/Editor/Widget/bcWidgetConsole.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bcWidgetConsole.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_black_cat__editor__bc_widget_console_t {
    QByteArrayData data[13];
    char stringdata0[188];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_black_cat__editor__bc_widget_console_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_black_cat__editor__bc_widget_console_t qt_meta_stringdata_black_cat__editor__bc_widget_console = {
    {
QT_MOC_LITERAL(0, 0, 36), // "black_cat::editor::bc_widget_..."
QT_MOC_LITERAL(1, 37, 13), // "executeScript"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 8), // "p_string"
QT_MOC_LITERAL(4, 61, 16), // "p_output_console"
QT_MOC_LITERAL(5, 78, 14), // "_returnPressed"
QT_MOC_LITERAL(6, 93, 12), // "_infoToggled"
QT_MOC_LITERAL(7, 106, 8), // "p_toggle"
QT_MOC_LITERAL(8, 115, 13), // "_debugToggled"
QT_MOC_LITERAL(9, 129, 15), // "_warningToggled"
QT_MOC_LITERAL(10, 145, 13), // "_errorToggled"
QT_MOC_LITERAL(11, 159, 14), // "_scriptToggled"
QT_MOC_LITERAL(12, 174, 13) // "_clearClicked"

    },
    "black_cat::editor::bc_widget_console\0"
    "executeScript\0\0p_string\0p_output_console\0"
    "_returnPressed\0_infoToggled\0p_toggle\0"
    "_debugToggled\0_warningToggled\0"
    "_errorToggled\0_scriptToggled\0_clearClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_black_cat__editor__bc_widget_console[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   59,    2, 0x08 /* Private */,
       6,    1,   60,    2, 0x08 /* Private */,
       8,    1,   63,    2, 0x08 /* Private */,
       9,    1,   66,    2, 0x08 /* Private */,
      10,    1,   69,    2, 0x08 /* Private */,
      11,    1,   72,    2, 0x08 /* Private */,
      12,    0,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,

       0        // eod
};

void black_cat::editor::bc_widget_console::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<bc_widget_console *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->executeScript((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->_returnPressed(); break;
        case 2: _t->_infoToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->_debugToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->_warningToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->_errorToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->_scriptToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->_clearClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (bc_widget_console::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&bc_widget_console::executeScript)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject black_cat::editor::bc_widget_console::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_black_cat__editor__bc_widget_console.data,
    qt_meta_data_black_cat__editor__bc_widget_console,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *black_cat::editor::bc_widget_console::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *black_cat::editor::bc_widget_console::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_black_cat__editor__bc_widget_console.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int black_cat::editor::bc_widget_console::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void black_cat::editor::bc_widget_console::executeScript(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
