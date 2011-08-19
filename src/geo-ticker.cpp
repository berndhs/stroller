#include "geo-ticker.h"
#include "geo-test-source.h"
#include <QDebug>
#include <QStringList>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QtGlobal>

QTM_USE_NAMESPACE

namespace geuzen
{
GeoTicker::GeoTicker (QObject *parent)
  :QObject (parent),
   clock (QTime::currentTime()),
   nodataTimer (0),
   mapTimer (0),
   tickInterval (5000),
   mapUpdateInterval (10000),
   source (0),
   isFirstPosition (true),
   boundingBox (QGeoCoordinate (0,0,0),359.9999,179.9999),
   currentMapFile (0),
   mapWidth (100.0),
   mapHeight (100.0)
{
  qDebug () << __PRETTY_FUNCTION__;
  clock.start ();
  qDebug () << "sources " << QGeoPositionInfoSource::availableSources();
  source = QGeoPositionInfoSource::createDefaultSource(this);
  if (!source) {
    source = new GeoTestSource (this);
    tickInterval = 1000; // testing, don't make them wait so long
    mapUpdateInterval = 2000;
  }
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
  mapTimer = new QTimer (this);
  connect (mapTimer, SIGNAL (timeout()), this, SLOT (updateMap()));
  mapTimer->start (mapUpdateInterval);
}

bool
GeoTicker::haveDataSource ()
{
  return source != 0;
}

void
GeoTicker::useTestSource (bool useTest)
{
  if (source) {
    delete source;
  }
  if (useTest) {
    source = new GeoTestSource (this);
  } else {
    source = QGeoPositionInfoSource::createDefaultSource(this);
  }
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
  QTimer::singleShot (500,this,SLOT (updateMap()));
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
GeoTicker::setMapInterval (int msecs)
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  mapUpdateInterval = msecs;
}

int
GeoTicker::mapInterval ()
{ 
  return mapUpdateInterval;
}

void
GeoTicker::setWidthHeight (qreal width, qreal height)
{
  mapWidth = width > 1.0 ? width : 1.0;
  mapHeight = height > 1.0 ? height : 1.0;
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
      boundingBox.setCenter (coord);
      boundingBox.setHeight (1.0/60000.0); // 1/1000 nautical mile, 1.852 meters
      boundingBox.setWidth (1.0/60000.0); 
      isFirstPosition = false;
    } else {
      boundingBox |= QGeoBoundingBox (coord, coord);
    }
  }
  qDebug () << " bb size " << boundingBox.width() 
            << " x " << boundingBox.height();
}

void
GeoTicker::noUpdate ()
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  //processPosition (lastReport);
}

void
GeoTicker::updateMap ()
{
  qDebug () << __PRETTY_FUNCTION__ << " at " << clock.elapsed();
  qDebug () << "  bounding box " << boundingBox.center();
  QString tmpTemplate (QString("%1/%2")
                    .arg (QDir::tempPath())
                    .arg ("geoticker_XXXXXX.svg"));
  QTemporaryFile *newMap = new QTemporaryFile (tmpTemplate, this);
  if (!newMap) {
    return;
  }
  newMap->open ();
  writeSvgHead (newMap);
  writeSvgMap (newMap, boundingBox, path);
  writeSvgTail (newMap);
  QFileInfo info (*newMap);
  QString wholeName = info.absoluteFilePath();
  if (currentMapFile) {
    currentMapFile->close ();
    delete currentMapFile;
  }
  currentMapFile = newMap;
  currentMapFile->close();
  emit mapUpdate (QString ("file://") + wholeName);
  currentMapFile->open ();
  QByteArray bytes = currentMapFile->readAll();
  qDebug () << "temp file " << wholeName;
  qDebug () << bytes;
  currentMapFile->close ();
}

void
GeoTicker::writeSvgHead (QIODevice * device)
{
  if (!device) {
    return;
  }
  device->write ("<?xml version=\"1.0\" standalone=\"no\"?>\n"
                 "<svg>\n");
}

void
GeoTicker::writeSvgTail (QIODevice * device)
{
  if (!device) {
    return;
  }
  device->write ("\n</svg>\n");
}

void
GeoTicker::writeSvgMap (QIODevice * device,
                    const QGeoBoundingBox & mapBox,
                    const QList <QGeoCoordinate> & mapPath)
{
  if (!device) {
    return;
  }
  qDebug () << " bottom left " << mapBox.bottomLeft();
  qDebug () << " top right " << mapBox.topRight();
  QGeoCoordinate botLeft = mapBox.bottomLeft();
  QGeoCoordinate topRight = mapBox.topRight();
  minLat = botLeft.latitude();
  qreal maxLat = topRight.latitude();
  minLon = botLeft.longitude();
  qreal maxLon = topRight.longitude();

  rangeLat = maxLat - minLat;
  rangeLon = maxLon - minLon, rangeLon;
  if (rangeLat > rangeLon) {
    rangeLon = rangeLat;
  } else {
    rangeLat = rangeLon;
  }
  qreal boxHeight = botLeft.distanceTo (QGeoCoordinate (
                                          topRight.latitude(),
                                          minLon));  
  latScale = 1.0;
  lonScale = 1.0;
  #if 0
  qreal boxWidth (0);
  if (minLat > 0.0) {
    // all points in northern hemisphere - widest at bottom
    boxWidth = botLeft.distanceTo (QGeoCoordinate (minLat, maxLon));
  } else if (maxLat < 0.0) {
    // all points in southern hemisphere - widest at top
    boxWidth = topRight.distanceTo (QGeoCoordinate (maxLat, minLon));
  } else {
    // bottom in south, top in north hemisphere - widest at equator
    boxWidth = QGeoCoordinate (0,minLon).distanceTo
                    (QGeoCoordinate (0,maxLon));
  }    

  if (boxWidth < 1.0) { boxWidth = 1.0; }   // meters
  if (boxHeight < 1.0) { boxHeight = 1.0; } // meters
  // this scaling is wrong
  if (boxWidth > boxHeight) {
    // make width (longitude) differences look bigger than they are
    lonScale *= boxWidth/boxHeight;
  } else if (boxWidth < boxHeight) {
    // make height (latitude) differences look bigger than they are
    latScale *= boxHeight/boxWidth;
  }
  #endif
  device->write (
    "<rect x=\"0\" y=\"0\" width=\"1000\" height=\"1000\" "
    "style=\"fill:black;stroke:green;stroke-width:1\" "
    "/>\n");
  device->write (QString (
    "<circle cx=\"%1\" cy=\"%2\" r=\"20\" "
    "style=\"fill:none;stroke:red;stroke-width:2\" "
    "/>\n").arg(mappedLon(mapBox.center().longitude()))
           .arg(mappedLat(mapBox.center().latitude()))
      .toAscii());
  
  qreal dist (0.0);
  if (!path.isEmpty()) {
    QString pathString;
    for (int i=0; i<path.count(); i++) {
      if (i > 1) {
        dist += mapPath.at(i).distanceTo (mapPath.at(i-1));
      }
      qDebug () << " path element " << i << mapPath.at(i) << " dist " << dist;
      pathString += QString (" %1,%2 ")
                   .arg (mappedLon (mapPath.at(i).longitude()))
                   .arg (mappedLat (mapPath.at(i).latitude()));
    }
    device->write (QString (
      "<polyline \n"
      "  style=\"fill:none; stroke:green; stroke-width:2\" \n"
      "  points=\"%1\"\n"
      "/>\n")  
        .arg (pathString)
       .toAscii());
  }
}

int
GeoTicker::mappedLon (qreal rawLon)
{
  if (rangeLon < 0.00001) {
    return 1;
  }
  return qRound (1000.0 * lonScale * (rawLon - minLon) / rangeLon);
}

int
GeoTicker::mappedLat (qreal rawLat)
{
  if (rangeLat < 0.00001) {
    return 1;
  }
  return 1000 - qRound (1000.0 * latScale * (rawLat - minLat) / rangeLat);
}


} // namespace
