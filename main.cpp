// SPDX-License-Identifier: GPL-2.0-only
#include <QApplication>
#include <QCommandLineParser>
#include "log.h"
#include "conf.h"
#include "panel.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("tint");
    QApplication::setApplicationVersion("0.0.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("A panel of beauty");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption outputOption(QStringList() << "o" << "output", "Output to use", "output");
    parser.addOption(outputOption);

    QCommandLineOption configOption(QStringList() << "c" << "config", "Config file to use",
                                    "filename");
    parser.addOption(configOption);

    parser.process(app);

    QString filename = parser.value(configOption);
    if (filename.isEmpty()) {
        filename = qgetenv("HOME") + "/.config/tint/tintrc";
    }

    info("read config file '{}'", filename.toStdString());
    confInit(filename);
    confSetOutput(parser.value(outputOption));

    Panel panel;
    panel.show();
    return app.exec();
}
