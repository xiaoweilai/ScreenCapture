/****************************************************************************
** Meta object code from reading C++ file 'screencap.h'
**
** Created: Sun Apr 26 00:19:40 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../screencap.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'screencap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ScreenCap[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      34,   10,   10,   10, 0x05,
      56,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      78,   10,   10,   10, 0x08,
     111,  107,   10,   10, 0x08,
     133,   10,   10,   10, 0x08,
     161,  154,  150,   10, 0x0a,
     187,  154,  150,   10, 0x0a,
     224,  212,   10,   10, 0x0a,
     277,  268,   10,   10, 0x0a,
     306,   10,   10,   10, 0x0a,
     320,   10,   10,   10, 0x0a,
     332,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ScreenCap[] = {
    "ScreenCap\0\0emitCtrlPthreadStart()\0"
    "emitCtrlPthreadStop()\0emitCtrlPthreadQuit()\0"
    "on_pushButtonStart_clicked()\0str\0"
    "LineTextTips(QString)\0StopActionSets()\0"
    "int\0ipaddr\0CheckIPAddrValid(QString)\0"
    "WithNetworkInit(QString)\0socketError\0"
    "displayNetErr(QAbstractSocket::SocketError)\0"
    "numBytes\0updateClientProgress(qint64)\0"
    "NetSendData()\0TimerSets()\0MergeMessage()\0"
};

void ScreenCap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ScreenCap *_t = static_cast<ScreenCap *>(_o);
        switch (_id) {
        case 0: _t->emitCtrlPthreadStart(); break;
        case 1: _t->emitCtrlPthreadStop(); break;
        case 2: _t->emitCtrlPthreadQuit(); break;
        case 3: _t->on_pushButtonStart_clicked(); break;
        case 4: _t->LineTextTips((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->StopActionSets(); break;
        case 6: { int _r = _t->CheckIPAddrValid((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: { int _r = _t->WithNetworkInit((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: _t->displayNetErr((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: _t->updateClientProgress((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 10: _t->NetSendData(); break;
        case 11: _t->TimerSets(); break;
        case 12: _t->MergeMessage(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ScreenCap::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ScreenCap::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ScreenCap,
      qt_meta_data_ScreenCap, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ScreenCap::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ScreenCap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ScreenCap::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScreenCap))
        return static_cast<void*>(const_cast< ScreenCap*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ScreenCap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ScreenCap::emitCtrlPthreadStart()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ScreenCap::emitCtrlPthreadStop()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void ScreenCap::emitCtrlPthreadQuit()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
