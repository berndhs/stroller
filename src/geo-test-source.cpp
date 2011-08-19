#include "geo-test-source.h"
#include <QStringList>
#include <QDebug>

namespace geuzen
{

GeoTestSource::GeoTestSource (QObject *parent)
  : QGeoPositionInfoSource (parent),
    dataFile (new QFile(this)),
    tickTimer (new QTimer(this))
{
  qDebug () << __PRETTY_FUNCTION__;
  connect (tickTimer, SIGNAL(timeout()), this, SLOT(readNextPosition()));

  dataFile->setFileName ("pathelements.txt");
  if (!dataFile->open(QIODevice::ReadOnly))
    qWarning() << "Error: cannot open source file" << dataFile->fileName();
}

QGeoPositionInfo 
GeoTestSource::lastKnownPosition (bool fromSatellitePositioningMethodsOnly) const
{
  qDebug () << __PRETTY_FUNCTION__;
  Q_UNUSED (fromSatellitePositioningMethodsOnly)
  return lastPosition;
}

GeoTestSource::PositioningMethods 
GeoTestSource::supportedPositioningMethods () const
{
  qDebug () << __PRETTY_FUNCTION__;
  return AllPositioningMethods;
}

int 
GeoTestSource::minimumUpdateInterval () const
{
  qDebug () << __PRETTY_FUNCTION__;
  return 500;
}

void 
GeoTestSource::startUpdates ()
{
  qDebug () << __PRETTY_FUNCTION__;
  int interval = updateInterval ();
  if (interval < minimumUpdateInterval ())
    interval = minimumUpdateInterval ();

  tickTimer->start (interval);
}

void 
GeoTestSource::stopUpdates ()
{
  qDebug () << __PRETTY_FUNCTION__;
  tickTimer->stop ();
}

void 
GeoTestSource::requestUpdate (int timeout)
{
  Q_UNUSED (timeout)
  qDebug () << __PRETTY_FUNCTION__;
  if (dataFile->canReadLine ())
    readNextPosition ();
  else
    emit updateTimeout ();
}

void 
GeoTestSource::readNextPosition ()
{
  qDebug () << __PRETTY_FUNCTION__;
  QString line (dataFile->readLine());
  if (!line.isEmpty()) {
    QStringList data = line.split(',');
    double latitude;
    double longitude;
    double altitude;
    QDateTime timestamp = QDateTime::currentDateTime ();
    latitude = data.at(0).toDouble();
    longitude = data.at(1).toDouble();
    altitude = data.at(2).toDouble();

    QGeoCoordinate coordinate (latitude, longitude, altitude);
    QGeoPositionInfo info (coordinate, timestamp);
    if (info.isValid()) {
      lastPosition = info;
      emit positionUpdated(info);
    }
  }
}

} // namespace
