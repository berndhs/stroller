#include "geo-ticker.h"
#include <QDebug>
#include <QStringList>
#include <QDateTime>

QTM_USE_NAMESPACE

namespace geuzen
{
GeoTicker::GeoTicker (QObject *parent)
  :QObject (parent),
   clock (QTime::currentTime()),
   nodataTimer (0),
   tickInterval (5000),
   source (0),
   isFirstPosition (true)
{
  qDebug () << __PRETTY_FUNCTION__;
  clock.start ();
  qDebug () << "sources " << QGeoPositionInfoSource::availableSources();
  source = QGeoPositionInfoSource::createDefaultSource(this);
  qDebug () << " source created " << source;
  if (source) {
    bool ok (true);
    source->setPreferredPositioningMethods (QGeoPositionInfoSource::AllPositioningMethods);
    ok &= connect (source, SIGNAL (positionUpdated (const QGeoPositionInfo &)),
             this, SLOT (receivePosition(const QGeoPositionInfo &)));
    ok &= connect (source, SIGNAL (updateTimeout()),
             this, SLOT (noUpdate()));
    source->setUpdateInterval (tickInterval);
    qDebug () << "   connections " << ok;
    qDebug () << "   mini time " << source->minimumUpdateInterval ();
    qDebug () << "   methods   " << source->supportedPositioningMethods ();
  }
  nodataTimer = new QTimer (this);
  connect (nodataTimer,SIGNAL (timeout()),this,SLOT (noUpdate()));
  lastReport.setCoordinate (QGeoCoordinate (0.0,0.0,-100.0));
  lastReport.setTimestamp (QDateTime::currentDateTime());
}

void
GeoTicker::start ()
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  if (source) {
    source->startUpdates ();
    nodataTimer->start (2*tickInterval);
    lastReport = source->lastKnownPosition();
    qDebug () << " immediate " << lastReport;
  }
}

void
GeoTicker::stop ()
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  if (source) {
    source->stopUpdates ();
    nodataTimer->stop();
  }
}

void
GeoTicker::setInterval (int msecs, bool doStart)
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  tickInterval = msecs;
  if (source) {
    source->setUpdateInterval (tickInterval);
    nodataTimer->setInterval (2*tickInterval);
  }
  if (doStart) {
    start ();
  }
}

int
GeoTicker::interval ()
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  return tickInterval;
}

void
GeoTicker::receivePosition (const QGeoPositionInfo & position)
{
  processPosition (position);
}

void
GeoTicker::processPosition (const QGeoPositionInfo & position)
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  qDebug () << __PRETTY_FUNCTION__ << position;
  QGeoCoordinate coord = position.coordinate();
  qreal vH = position.attribute (QGeoPositionInfo::GroundSpeed);
  qreal vV = position.attribute (QGeoPositionInfo::VerticalSpeed);
  qreal heading = position.attribute (QGeoPositionInfo::Direction);
  qreal lat = coord.latitude ();
  qreal lon = coord.longitude ();
  qreal alt = coord.altitude ();
  emit positionUpdate (lat, lon, alt, vH, heading, vV);
  if (position.isValid()) {
    path.append (coord);
    if (isFirstPosition) {
      boundingBox = QGeoBoundingBox (coord, coord);
      isFirstPosition = false;
    } else {
      boundingBox |= QGeoBoundingBox (coord, coord);
    }
  }
  qDebug () << " path " << path;
  qDebug () << " bb size " << boundingBox.width() 
            << " x " << boundingBox.height();
}

void
GeoTicker::noUpdate ()
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  //processPosition (lastReport);
}


} // namespace
