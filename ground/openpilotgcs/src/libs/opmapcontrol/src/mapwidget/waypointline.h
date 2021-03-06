/**
 ******************************************************************************
 *
 * @file       waypointline.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2012.
 * @brief      A graphicsItem representing a line connecting 2 waypoints
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   OPMapWidget
 * @{
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#ifndef WAYPOINTLINE_H
#define WAYPOINTLINE_H
#include <QGraphicsItem>
#include <QPainter>
#include <QLabel>
#include "../internals/pointlatlng.h"
#include "mapgraphicitem.h"
#include "waypointitem.h"
#include <QObject>
#include <QPoint>

namespace mapcontrol {
class WayPointLine : public QObject, public QGraphicsLineItem {
    Q_OBJECT Q_INTERFACES(QGraphicsItem)
public:
    enum { Type = UserType + 8 };
    WayPointLine(WayPointItem *from, WayPointItem *to, MapGraphicItem *map, QColor color = Qt::green, bool dashed = false, int width = -1);
    WayPointLine(HomeItem *from, WayPointItem *to, MapGraphicItem *map, QColor color = Qt::green, bool dashed = false, int width = -1);
    int type() const;
    QPainterPath shape() const;
    void setColor(const QColor &color)
    {
        myColor = color;
    }
private:
    QGraphicsItem *source;
    QGraphicsItem *destination;
    MapGraphicItem *my_map;
    QPolygonF arrowHead;
    QColor myColor;
    bool dashed;
    int lineWidth;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public slots:
    void refreshLocations();
    void waypointdeleted();
    void setOpacitySlot(qreal opacity);
};
}
#endif // WAYPOINTLINE_H
