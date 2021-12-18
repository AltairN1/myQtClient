#include <QApplication>
#include "myClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    myClient client("localhost", 2324);
    cout << "hello";
    cout << "Nastya" << "\n";
    client.show();
    return app.exec();
}
