/****************************************************************************
** Meta object code from reading C++ file 'led_strip.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../led_strip.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'led_strip.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSLedStripENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSLedStripENDCLASS = QtMocHelpers::stringData(
    "LedStrip",
    "lightPreparePacket",
    "",
    "r",
    "g",
    "b",
    "r_scale",
    "g_scale",
    "b_scale",
    "cmd",
    "uint8_t",
    "speed",
    "uint16_t",
    "period",
    "brightness",
    "prepareLedScenePackets",
    "QWidget*",
    "itemsParent"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSLedStripENDCLASS_t {
    uint offsetsAndSizes[36];
    char stringdata0[9];
    char stringdata1[19];
    char stringdata2[1];
    char stringdata3[2];
    char stringdata4[2];
    char stringdata5[2];
    char stringdata6[8];
    char stringdata7[8];
    char stringdata8[8];
    char stringdata9[4];
    char stringdata10[8];
    char stringdata11[6];
    char stringdata12[9];
    char stringdata13[7];
    char stringdata14[11];
    char stringdata15[23];
    char stringdata16[9];
    char stringdata17[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSLedStripENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSLedStripENDCLASS_t qt_meta_stringdata_CLASSLedStripENDCLASS = {
    {
        QT_MOC_LITERAL(0, 8),  // "LedStrip"
        QT_MOC_LITERAL(9, 18),  // "lightPreparePacket"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 1),  // "r"
        QT_MOC_LITERAL(31, 1),  // "g"
        QT_MOC_LITERAL(33, 1),  // "b"
        QT_MOC_LITERAL(35, 7),  // "r_scale"
        QT_MOC_LITERAL(43, 7),  // "g_scale"
        QT_MOC_LITERAL(51, 7),  // "b_scale"
        QT_MOC_LITERAL(59, 3),  // "cmd"
        QT_MOC_LITERAL(63, 7),  // "uint8_t"
        QT_MOC_LITERAL(71, 5),  // "speed"
        QT_MOC_LITERAL(77, 8),  // "uint16_t"
        QT_MOC_LITERAL(86, 6),  // "period"
        QT_MOC_LITERAL(93, 10),  // "brightness"
        QT_MOC_LITERAL(104, 22),  // "prepareLedScenePackets"
        QT_MOC_LITERAL(127, 8),  // "QWidget*"
        QT_MOC_LITERAL(136, 11)   // "itemsParent"
    },
    "LedStrip",
    "lightPreparePacket",
    "",
    "r",
    "g",
    "b",
    "r_scale",
    "g_scale",
    "b_scale",
    "cmd",
    "uint8_t",
    "speed",
    "uint16_t",
    "period",
    "brightness",
    "prepareLedScenePackets",
    "QWidget*",
    "itemsParent"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLedStripENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,   10,   26,    2, 0x06,    1 /* Public */,
      15,    5,   47,    2, 0x06,   12 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, 0x80000000 | 10, 0x80000000 | 12, 0x80000000 | 10,    3,    4,    5,    6,    7,    8,    9,   11,   13,   14,
    QMetaType::Void, 0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int, 0x80000000 | 10,   17,    6,    7,    8,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject LedStrip::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSLedStripENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLedStripENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLedStripENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LedStrip, std::true_type>,
        // method 'lightPreparePacket'
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
        // method 'prepareLedScenePackets'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWidget *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>
    >,
    nullptr
} };

void LedStrip::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LedStrip *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->lightPreparePacket((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[8])),(*reinterpret_cast< std::add_pointer_t<uint16_t>>(_a[9])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[10]))); break;
        case 1: _t->prepareLedScenePackets((*reinterpret_cast< std::add_pointer_t<QWidget*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[5]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
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
            using _t = void (LedStrip::*)(int , int , int , int , int , int , int , uint8_t , uint16_t , uint8_t );
            if (_t _q_method = &LedStrip::lightPreparePacket; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LedStrip::*)(QWidget * , int , int , int , uint8_t );
            if (_t _q_method = &LedStrip::prepareLedScenePackets; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *LedStrip::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LedStrip::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLedStripENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LedStrip::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void LedStrip::lightPreparePacket(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, uint8_t _t8, uint16_t _t9, uint8_t _t10)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t9))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t10))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LedStrip::prepareLedScenePackets(QWidget * _t1, int _t2, int _t3, int _t4, uint8_t _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
