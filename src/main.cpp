#include <QApplication>
#include <QInputDialog>
#include <QDir>
#include "Game.h"
#include <QFontDatabase>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    
    int fontId = QFontDatabase::addApplicationFont(":/fonts/PressStart2P.ttf");
    if (fontId < 0) {
        qDebug() << "Warning: PressStart2P font not found in resources.";
    }

    
    bool ok;
    QString text = QInputDialog::getText(nullptr, "Ad Astra Login",
                                         "Enter Pilot Name:", QLineEdit::Normal,
                                         "PLAYER", &ok);
    if (!ok || text.isEmpty()) {
        text = "PILOT";
    }

    Game g;
    g.setPlayerName(text);
    
    
    return app.exec();
}