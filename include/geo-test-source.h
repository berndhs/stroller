#ifndef GEUZEN_GEO_TESTSOURCE_H
#define GEUZEN_GEO_TESTSOURCE_H

#include <QGeoPositionInfoSource>
#include <QTimer>
#include <QFile>

QTM_USE_NAMESPACE

namespace geuzen 
{

class GeoTestSource : public QGeoPositionInfoSource
{
Q_OBJECT
public:

  GeoTestSource(QObject *parent = 0);

  QGeoPositionInfo lastKnownPosition (bool fromSatellitePositioningMethodsOnly = false) const;

  PositioningMethods supportedPositioningMethods () const;
  int minimumUpdateInterval () const;

public slots:

  virtual void startUpdates ();
  virtual void stopUpdates ();

  virtual void requestUpdate (int timeout = 5000);

private slots:

  void readNextPosition ();

private:

  QFile             *dataFile;
  QTimer            *tickTimer;
  QGeoPositionInfo   lastPosition;

};

} // namespace

#endif

