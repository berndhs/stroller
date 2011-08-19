#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QIcon>
#include <QFont>
#include <QSystemDeviceInfo>
#include <QDesktopWidget>
#include <QStringList>
#include <QDebug>
#include <qdeclarative.h>

#include "geo-ticker.h"
#include "orientation.h"
#include "deliberate.h"
#include "version.h"


QTM_USE_NAMESPACE

using namespace deliberate;

void
separateOptions (QStringList & arguments, QStringList & options)
{
  options.clear();
  QStringList realArgs;
  for (auto ait=arguments.begin(); ait!=arguments.end(); ait++) {
    if (ait->startsWith('-')) {
      options.append (*ait);
    } else {
      realArgs.append (*ait);
    }
  }
  arguments = realArgs;
}

void
findBadOptions (const QStringList & goodOptions,
                const QStringList & presentOptions,
                      QStringList & badOptions)
{
  badOptions.clear ();
  for (auto oit=presentOptions.begin(); oit!=presentOptions.end(); oit++) {
    if (!goodOptions.contains (*oit)) {
      badOptions.append (*oit);
    }
  }
}

int
main (int argc, char *argv[])
{
  QCoreApplication::setOrganizationName ("BerndStramm");
  QCoreApplication::setOrganizationDomain ("moui.sourceforge.net");
  QCoreApplication::setApplicationName ("stroller");
  deliberate::ProgramVersion pv ("Stroller");
  QCoreApplication::setApplicationVersion (pv.ShortVersion());
  
  QApplication app (argc, argv);
  
  QSettings  settings;
  deliberate::InitSettings ();
  deliberate::SetSettings (settings);
  settings.setValue ("program",pv.MyName());
  
  QStringList arguments = app.arguments();
  QStringList options;
  QStringList knownOptions;
  
  knownOptions << "--help" << "-h"
               << "--version" << "-v"
               << "--test" << "-T"
               << "--desktop" << "-D";
               
  separateOptions (arguments, options);

  QSystemDeviceInfo sdi;

  QString imsi = sdi.imsi();
  QString imei = sdi.imei();
  bool isPhone (!(imsi.isEmpty() || imei.isEmpty()));
  bool isTesting (false);
  
  if (options.contains (QString("--desktop")) || options.contains("-D")) {
    isPhone = false;
  }
  if (options.contains (QString("--test")) || options.contains("-T")) {
    isTesting = true;
  }
  if (options.contains (QString("--version")) || options.contains ("-v")) {
    pv.CLIVersion ();
    exit (0);
  }
  if (options.contains (QString("--help")) || options.contains("-h")) {
    StdOut() << QObject::tr("Options Understood: ") << endl 
             << knownOptions << endl;
    exit (0);
  }
  QStringList badOptions;
  findBadOptions (knownOptions, options, badOptions);
  if (!badOptions.isEmpty()) {
    StdErr () << QObject::tr("Unknown Options: ") << endl 
              << badOptions << endl;
    StdErr().flush();
    exit (1);
  }

  qDebug () << " is phone ? " << isPhone;

  QDeclarativeView  * view = new QDeclarativeView;
  QDeclarativeEngine * engine = view->engine();
  QDeclarativeContext * context = view->rootContext();

  if (context) {
    context->setContextProperty ("isProbablyPhone", QVariant(isPhone));
    context->setContextProperty ("isTesting",QVariant(isTesting));
  }

  if (isPhone) {
    QFont appFont = app.font();
    appFont.setPointSizeF (appFont.pointSizeF() * 2);
    app.setFont (appFont);
  }

  const char geoUri[] = "moui.geuzen.geo.static";
  const char utilUri[] = "moui.geuzen.utils.static";

  qmlRegisterType<geuzen::GeoTicker>(geoUri, 1, 0,
                 "GeuzenGeoTicker");
  qmlRegisterType<geuzen::OrientationWatcher>(utilUri, 1, 0,
                 "GeuzenOrientation");

  view->setWindowIcon (QIcon (":/icons/icon.png"));
  view->setSource (QUrl ("qrc:stroller.qml"));
  if (isPhone) {
    view->setGeometry (app.desktop()->screenGeometry());
    view->showFullScreen ();
  } else {
    view->setGeometry (0,0,500,500);
  }
  qDebug () << " view set to size " << view->size();
  view->setResizeMode (QDeclarativeView::SizeRootObjectToView);
  QObject::connect (engine, SIGNAL (quit()),&app, SLOT(quit()));
  view->show ();
  int ok = app.exec ();
  return ok;
}
