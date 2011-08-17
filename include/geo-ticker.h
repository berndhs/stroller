#ifndef GEUZEN_GEOTICKER_H
#define GEUZEN_GEOTICKER_H

#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QGeoBoundingBox>

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

  int interval ();

private slots:

  void receivePosition (const QGeoPositionInfo & position);
  void noUpdate ();

signals:

  void positionUpdate (qreal posLat, qreal posLon, qreal posAlt,
                       qreal velHorizontal, qreal heading, qreal velVertical);

private:

  int                      tickInterval;
  QGeoPositionInfoSource  *source;
  QGeoBoundingBox          boundingBox;

};

}
#endif // GEUZEN_GEOTICKER_H
