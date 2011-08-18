#ifndef GEUZEN_GEOTICKER_H
#define GEUZEN_GEOTICKER_H

#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QGeoBoundingBox>
#include <QGeoCoordinate>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QTemporaryFile>
#include <QIODevice>

QTM_USE_NAMESPACE

namespace geuzen
{

class GeoTicker: public QObject
{
Q_OBJECT
public:

  GeoTicker (QObject *parent=0);

  Q_INVOKABLE void start ();
  Q_INVOKABLE void stop ();
  Q_INVOKABLE void setInterval (int msecs, bool start=false);
  Q_PROPERTY(int interval READ interval)
  Q_PROPERTY(int mapInterval READ mapInterval WRITE setMapInterval)

  int interval ();

  int mapInterval ();
  void setMapInterval (int msecs);

private slots:

  void receivePosition (const QGeoPositionInfo & position);
  void noUpdate ();
  void updateMap ();

signals:

  void positionUpdate (qreal posLat, qreal posLon, qreal posAlt,
                       qreal velHorizontal, qreal heading, qreal velVertical);
  void mapUpdate (const QString & fileName);

private:

  void processPosition (const QGeoPositionInfo & position);
  void writeSvgHead (QIODevice * device);
  void writeSvgMap (QIODevice * device,
                    const QGeoBoundingBox & mapBox,
                    const QList <QGeoCoordinate> & mapPath);
  void writeSvgTail (QIODevice * device);
  int  mappedLon (qreal rawLon);
  int  mappedLat (qreal rawLat);

  QTime                    clock;
  QTimer                  *nodataTimer;
  QTimer                  *mapTimer;
  int                      tickInterval;
  int                      mapUpdateInterval;
  QGeoPositionInfoSource  *source;
  bool                     isFirstPosition;
  QGeoBoundingBox          boundingBox;
  QGeoPositionInfo         lastReport;
  QList <QGeoCoordinate>   path;
  QTemporaryFile          *currentMapFile;

  qreal   minLat;
  qreal   rangeLat;
  qreal   minLon;
  qreal   rangeLon;

};

}
#endif // GEUZEN_GEOTICKER_H
