/****************************************************************************
** Meta object code from reading C++ file 'capthread.h'
**
** Created: Sun Apr 26 00:19:41 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../capthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'capthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CapThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      22,   10,   10,   10, 0x0a,
      39,   10,   10,   10, 0x0a,
      55,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CapThread[] = {
    "CapThread\0\0capFrame()\0SetStartThread()\0"
    "SetStopThread()\0SetQuitThread()\0"
};

void CapThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CapThread *_t = static_cast<CapThread *>(_o);
        switch (_id) {
        case 0: _t->capFrame(); break;
        case 1: _t->SetStartThread(); break;
        case 2: _t->SetStopThread(); break;
        case 3: _t->SetQuitThread(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CapThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CapThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CapThread,
      qt_meta_data_CapThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CapThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CapThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CapThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CapThread))
        return static_cast<void*>(const_cast< CapThread*>(this));
    return QThread::qt_metacast(_clname);
}

int CapThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
