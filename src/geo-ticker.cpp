#include "geo-ticker.h"
#include <QDebug>
#include <QStringList>

QTM_USE_NAMESPACE

namespace geuzen
{
GeoTicker::GeoTicker (QObject *parent)
  :QObject (parent),
   tickInterval (5000),
   source (0)
{
  qDebug () << __PRETTY_FUNCTION__;
  qDebug () << "sources " << QGeoPositionInfoSource::availableSources();
  source = QGeoPositionInfoSource::createDefaultSource(this);
  qDebug () << " source created " << source;
  if (source) {
    bool ok (true);
    ok &= connect (source, SIGNAL (positionUpdated (const QGeoPositionInfo &)),
             this, SLOT (receivePosition(const QGeoPositionInfo &)));
    ok &= connect (source, SIGNAL (updateTimeout()),
             this, SLOT (noUpdate()));
    qDebug () << "   connections " << ok;
    qDebug () << "   mini time " << source->minimumUpdateInterval ();
  }
}

void
GeoTicker::start ()
{
  qDebug () << __PRETTY_FUNCTION__;
  if (source) {
    source->startUpdates ();
    qDebug () << source->lastKnownPosition();
  }
}

void
GeoTicker::setInterval (int msecs, bool doStart)
{
  qDebug () << __PRETTY_FUNCTION__;
  tickInterval = msecs;
  if (source) {
    source->setUpdateInterval (tickInterval);
  }
  if (doStart) {
    start ();
  }
}

int
GeoTicker::interval ()
{
  qDebug () << __PRETTY_FUNCTION__;
  return tickInterval;
}

void
GeoTicker::receivePosition (const QGeoPositionInfo & position)
{
  qDebug () << __PRETTY_FUNCTION__ << position;
  QGeoCoordinate coord = position.coordinate();
  qreal vH = position.attribute (QGeoPositionInfo::GroundSpeed);
  qreal vV = position.attribute (QGeoPositionInfo::VerticalSpeed);
  qreal lat = coord.latitude ();
  qreal lon = coord.longitude ();
  qreal alt = coord.altitude ();
  emit positionUpdate (lat, lon, alt, vH, vV);
}

void
GeoTicker::noUpdate ()
{
  qDebug () << __PRETTY_FUNCTION__;
}


} // namespace
