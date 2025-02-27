#include "mainwindow.h"

#include <QApplication>
#include <QTime>

extern "C" {
#include "dtls_debug.h"
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "DBG"}, {QtInfoMsg, "INF"}, {QtWarningMsg, "WRN"}, {QtCriticalMsg, "CRI"}, {QtFatalMsg, "FAT"}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    /*
        if (logToFile) {
            QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
            QFile outFile(logFilePath);
            outFile.open(QIODevice::WriteOnly | QIODevice::Append);
            QTextStream ts(&outFile);
            ts << txt << endl;
            outFile.close();
        } else {
        */
    //        fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);

    fprintf(stderr, "%s\t%s\t%s\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData());
    fflush(stderr);
    //}

    //if (type == QtFatalMsg)
    //    abort();



}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret =  a.exec();
    return ret;
}

extern "C" void viFailureLog(char *msg) {
    qDebug() << "viFailureLog" <<  msg;
}

extern "C" void viDebugLog(char *msg) {
    qDebug() << "viDebugLog" <<  msg;
}



/*
 * Caution. If DTLS_CONSTRAINED_STACK is set, then the same mutex will get
 * locked in dtls_log() and/or dtls_dsrv_hexdump_log() so these functions
 * cannot call each other.  Furthermore, if log_handler() calls dsrv_log() /
 * dtls_dsrv_hexdump_log() there will be a recursive lookup.
 */
extern "C" void dsrv_log(log_t level, const char *format, ...) {
    va_list ap;
    size_t len;
    char message[DTLS_DEBUG_BUF_SIZE];

/*
    if (maxlog < (int) level)
        return;
*/
    va_start(ap, format);
    len = vsnprintf( message, sizeof(message), format, ap);
    va_end(ap);
    if (len + 1 > sizeof(message)) {
        /* 6 is needed as trailing 0 byte space needed */
        snprintf(&message[sizeof(message)-6], 6, " ...\n");
    }
    qDebug() << "dsrv_log" <<  message;

}
