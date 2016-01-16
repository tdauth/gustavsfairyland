#include <QtWidgets/QApplication>
#include "fairytale.h"

#include <QtGlobal>
#include <QtDebug>
 #include <stdio.h>
 #include <stdlib.h>

 void myMessageOutput(QtMsgType type, const QMessageLogContext &c, const QString &msg)
 {
     //in this function, you can write the message to any stream!
     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
 }

int main(int argc, char** argv)
{
	qInstallMessageHandler(myMessageOutput); //install : set the callback
	QApplication app(argc, argv);
	fairytale fairytale;
	fairytale.show();
	return app.exec();
}
