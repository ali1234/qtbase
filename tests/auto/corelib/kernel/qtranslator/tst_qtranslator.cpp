/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QWidget>
#include <qtranslator.h>
#include <qfile.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QTranslator : public QWidget
{
    Q_OBJECT

public:
    tst_QTranslator();
protected:
    bool event(QEvent *event);
private slots:
    void initTestCase();

    void load();
    void load2();
    void threadLoad();
    void testLanguageChange();
    void plural();
    void translate_qm_file_generated_with_msgfmt();
    void loadFromResource();
    void loadDirectory();

private:
    int languageChangeEventCounter;
};

tst_QTranslator::tst_QTranslator()
    : languageChangeEventCounter(0)
{
    show();
    hide();
}

void tst_QTranslator::initTestCase()
{
    // chdir into the directory containing our testdata,
    // to make the code simpler (load testdata via relative paths)
    QString testdata_dir = QFileInfo(QFINDTESTDATA("hellotr_la.qm")).absolutePath();
    QVERIFY2(QDir::setCurrent(testdata_dir), qPrintable("Could not chdir to " + testdata_dir));
}

bool tst_QTranslator::event(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        ++languageChangeEventCounter;
    return QWidget::event(event);
}

void tst_QTranslator::load()
{

    QTranslator tor( 0 );
    tor.load("hellotr_la");
    QVERIFY(!tor.isEmpty());
    QCOMPARE(tor.translate("QPushButton", "Hello world!"), QString::fromLatin1("Hallo Welt!"));
}

void tst_QTranslator::load2()
{
    QTranslator tor( 0 );
    QFile file("hellotr_la.qm");
    file.open(QFile::ReadOnly);
    QByteArray data = file.readAll();
    tor.load((const uchar *)data.constData(), data.length());
    QVERIFY(!tor.isEmpty());
    QCOMPARE(tor.translate("QPushButton", "Hello world!"), QString::fromLatin1("Hallo Welt!"));
}

class TranslatorThread : public QThread
{
    void run() {
        QTranslator tor( 0 );
        tor.load("hellotr_la");

        if (tor.isEmpty())
            qFatal("Could not load translation");
        if (tor.translate("QPushButton", "Hello world!") !=  QString::fromLatin1("Hallo Welt!"))
            qFatal("Test string was not translated correctlys");
    }
};


void tst_QTranslator::threadLoad()
{
    TranslatorThread thread;
    thread.start();
    QVERIFY(thread.wait(10 * 1000));
}

void tst_QTranslator::testLanguageChange()
{
    languageChangeEventCounter = 0;

    QTranslator *tor = new QTranslator;
    tor->load("hellotr_la.qm");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 0);

    tor->load("doesn't exist, same as clearing");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 0);

    tor->load("hellotr_la.qm");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 0);

    qApp->installTranslator(tor);
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 1);

    tor->load("doesn't exist, same as clearing");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 2);

    tor->load("hellotr_la.qm");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 3);

    qApp->removeTranslator(tor);
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 4);

    tor->load("doesn't exist, same as clearing");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 4);

    qApp->installTranslator(tor);
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 4);

    tor->load("hellotr_la.qm");
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 5);

    delete tor;
    tor = 0;
    qApp->sendPostedEvents();
    qApp->sendPostedEvents();
    QCOMPARE(languageChangeEventCounter, 6);
}


void tst_QTranslator::plural()
{

    QTranslator tor( 0 );
    tor.load("hellotr_la");
    QVERIFY(!tor.isEmpty());
    QCoreApplication::installTranslator(&tor);
    QCoreApplication::Encoding e = QCoreApplication::UnicodeUTF8;
    QCOMPARE(QCoreApplication::translate("QPushButton", "Hello %n world(s)!", 0, e, 0), QString::fromLatin1("Hallo 0 Welten!"));
    QCOMPARE(QCoreApplication::translate("QPushButton", "Hello %n world(s)!", 0, e, 1), QString::fromLatin1("Hallo 1 Welt!"));
    QCOMPARE(QCoreApplication::translate("QPushButton", "Hello %n world(s)!", 0, e, 2), QString::fromLatin1("Hallo 2 Welten!"));
}

void tst_QTranslator::translate_qm_file_generated_with_msgfmt()
{
    QTranslator translator;
    translator.load("msgfmt_from_po");
    qApp->installTranslator(&translator);

    QCOMPARE(QCoreApplication::translate("", "Intro"), QLatin1String("Einleitung"));
    // The file is converted from a po file, thus it does not have any context info.
    // The following should then not be translated
    QCOMPARE(QCoreApplication::translate("contekst", "Intro"), QLatin1String("Intro"));
    QCOMPARE(QCoreApplication::translate("contekst", "Intro\0\0"), QLatin1String("Intro"));
    QCOMPARE(QCoreApplication::translate("contekst", "Intro\0x"), QLatin1String("Intro"));
    QCOMPARE(QCoreApplication::translate("", "Intro\0\0"), QLatin1String("Einleitung"));
    QCOMPARE(QCoreApplication::translate("", "Intro\0x"), QLatin1String("Einleitung"));

    qApp->removeTranslator(&translator);
}

void tst_QTranslator::loadFromResource()
{
    QTranslator tor;
    tor.load(":/tst_qtranslator/hellotr_la.qm");
    QVERIFY(!tor.isEmpty());
    QCOMPARE(tor.translate("QPushButton", "Hello world!"), QString::fromLatin1("Hallo Welt!"));
}

void tst_QTranslator::loadDirectory()
{
    QString current_base = QDir::current().dirName();
    QVERIFY(QFileInfo("../" + current_base).isDir());

    QTranslator tor;
    tor.load(current_base, "..");
    QVERIFY(tor.isEmpty());
}

QTEST_MAIN(tst_QTranslator)
#include "tst_qtranslator.moc"
