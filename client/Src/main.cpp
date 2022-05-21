#include "../Include/mainwindow.h"
#include "../Include/login.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Login l;
    l.show();
    QSettings set("C:/ProjectDB/Agar.IQ/settings", QSettings::IniFormat);
    bool type = set.value("type").toBool();
    if (type){
        l.hide();
    }

    return a.exec();
}
