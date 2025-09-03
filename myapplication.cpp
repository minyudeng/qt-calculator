#include "myapplication.h"

MyApplication::MyApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{

}

bool MyApplication::notify(QObject *receiver, QEvent *event)
{
    try{
      return QApplication::notify(receiver, event);
    }catch(const std::exception &e){
      qCritical("Exception caught: %s", e.what());
      return false;
    }catch(...){
      qCritical("Unknown exception caught");
      return false;
    }
}
