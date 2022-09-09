import QtQuick 2.0
import QtLocation 5.14
import QtPositioning 5.14

Rectangle {
    visible: true

    signal qmlSignal(double topLatitude, double leftLongitude, double bottomLatitude, double rightLongitude)

    property var rect: "QGeoRectangle()"

    Plugin {
        id: mapPlugin
         name: "esri"
    }

    Map {
        anchors.fill: parent
        id: map
        objectName: "map"
        plugin: mapPlugin
        center: QtPositioning.coordinate(39.06, -94.35) // US
        zoomLevel: 4

        onVisibleRegionChanged: {
            rect = map.visibleRegion.boundingGeoRectangle()
            qmlSignal(rect.topLeft.latitude, rect.topLeft.longitude, rect.bottomRight.latitude, rect.bottomRight.longitude)
        }
    }
}
