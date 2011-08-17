#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QIcon>
#include <QFont>
#include <QSystemDeviceInfo>
#include <QDesktopWidget>
#include <QDebug>
#include <qdeclarative.h>

#include "geo-ticker.h"
#include "orientation.h"


QTM_USE_NAMESPACE

int
main (int argc, char *argv[])
{
  QApplication app (argc, argv);

  QSystemDeviceInfo sdi;

  QString imsi = sdi.imsi();
  QString imei = sdi.imei();
  bool isPhone (!(imsi.isEmpty() || imei.isEmpty()));

  qDebug () << " is phone ? " << isPhone;

  QDeclarativeView  * view = new QDeclarativeView;
  QDeclarativeEngine * engine = view->engine();
  QDeclarativeContext * context = view->rootContext();

  if (context) {
    context->setContextProperty ("isProbablyPhone", QVariant(isPhone));
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
    view->setGeometry (0,0,500,300);
  }
  qDebug () << " view set to size " << view->size();
  view->setResizeMode (QDeclarativeView::SizeRootObjectToView);
  QObject::connect (engine, SIGNAL (quit()),&app, SLOT(quit()));
  view->show ();
  int ok = app.exec ();
  return 0;
}
