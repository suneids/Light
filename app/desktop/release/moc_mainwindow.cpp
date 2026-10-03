/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "updateGreenhouseStatus",
    "",
    "air_temp",
    "air_hum",
    "uint16_t",
    "soilRaw",
    "uint8_t",
    "waterState",
    "sendGreenhouseSingleParam",
    "cmd",
    "param",
    "lightSendPacket",
    "r",
    "g",
    "b",
    "r_scale",
    "g_scale",
    "b_scale",
    "speed",
    "period",
    "brightness",
    "makeLedScenePackets",
    "QWidget*",
    "itemsParent"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[50];
    char stringdata0[11];
    char stringdata1[23];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[8];
    char stringdata5[9];
    char stringdata6[8];
    char stringdata7[8];
    char stringdata8[11];
    char stringdata9[26];
    char stringdata10[4];
    char stringdata11[6];
    char stringdata12[16];
    char stringdata13[2];
    char stringdata14[2];
    char stringdata15[2];
    char stringdata16[8];
    char stringdata17[8];
    char stringdata18[8];
    char stringdata19[6];
    char stringdata20[7];
    char stringdata21[11];
    char stringdata22[20];
    char stringdata23[9];
    char stringdata24[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 22),  // "updateGreenhouseStatus"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 8),  // "air_temp"
        QT_MOC_LITERAL(44, 7),  // "air_hum"
        QT_MOC_LITERAL(52, 8),  // "uint16_t"
        QT_MOC_LITERAL(61, 7),  // "soilRaw"
        QT_MOC_LITERAL(69, 7),  // "uint8_t"
        QT_MOC_LITERAL(77, 10),  // "waterState"
        QT_MOC_LITERAL(88, 25),  // "sendGreenhouseSingleParam"
        QT_MOC_LITERAL(114, 3),  // "cmd"
        QT_MOC_LITERAL(118, 5),  // "param"
        QT_MOC_LITERAL(124, 15),  // "lightSendPacket"
        QT_MOC_LITERAL(140, 1),  // "r"
        QT_MOC_LITERAL(142, 1),  // "g"
        QT_MOC_LITERAL(144, 1),  // "b"
        QT_MOC_LITERAL(146, 7),  // "r_scale"
        QT_MOC_LITERAL(154, 7),  // "g_scale"
        QT_MOC_LITERAL(162, 7),  // "b_scale"
        QT_MOC_LITERAL(170, 5),  // "speed"
        QT_MOC_LITERAL(176, 6),  // "period"
        QT_MOC_LITERAL(183, 10),  // "brightness"
        QT_MOC_LITERAL(194, 19),  // "makeLedScenePackets"
        QT_MOC_LITERAL(214, 8),  // "QWidget*"
        QT_MOC_LITERAL(223, 11)   // "itemsParent"
    },
    "MainWindow",
    "updateGreenhouseStatus",
    "",
    "air_temp",
    "air_hum",
    "uint16_t",
    "soilRaw",
    "uint8_t",
    "waterState",
    "sendGreenhouseSingleParam",
    "cmd",
    "param",
    "lightSendPacket",
    "r",
    "g",
    "b",
    "r_scale",
    "g_scale",
    "b_scale",
    "speed",
    "period",
    "brightness",
    "makeLedScenePackets",
    "QWidget*",
    "itemsParent"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    4,   38,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    2,   47,    2, 0x0a,    6 /* Public */,
      12,   10,   52,    2, 0x0a,    9 /* Public */,
      22,    5,   73,    2, 0x0a,   20 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float, QMetaType::Float, 0x80000000 | 5, 0x80000000 | 7,    3,    4,    6,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5,   10,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, 0x80000000 | 7, 0x80000000 | 5, 0x80000000 | 7,   13,   14,   15,   16,   17,   18,   10,   19,   20,   21,
    QMetaType::Void, 0x80000000 | 23, QMetaType::Int, QMetaType::Int, QMetaType::Int, 0x80000000 | 7,   24,   16,   17,   18,   21,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'updateGreenhouseStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint16_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>,
        // method 'sendGreenhouseSingleParam'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint16_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint16_t, std::false_type>,
        // method 'lightSendPacket'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint16_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>,
        // method 'makeLedScenePackets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateGreenhouseStatus((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint16_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[4]))); break;
        case 1: _t->sendGreenhouseSingleParam((*reinterpret_cast< std::add_pointer_t<uint16_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint16_t>>(_a[2]))); break;
        case 2: _t->lightSendPacket((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[8])),(*reinterpret_cast< std::add_pointer_t<uint16_t>>(_a[9])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[10]))); break;
        case 3: _t->makeLedScenePackets((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[5]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWidget* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(float , float , uint16_t , uint8_t );
            if (_t _q_method = &MainWindow::updateGreenhouseStatus; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::updateGreenhouseStatus(float _t1, float _t2, uint16_t _t3, uint8_t _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
