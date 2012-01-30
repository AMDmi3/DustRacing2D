// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "mapbase.hpp"

#include "tracktilebase.hpp"
#include "trackdatabase.hpp"

#include <QPoint>
#include <QPointF>

MapBase::MapBase(TrackDataBase * trackData, unsigned int cols, unsigned int rows)
: m_trackData(trackData)
, m_cols(cols)
, m_rows(rows)
, m_map(rows, TrackTileRow(m_cols, nullptr))
{
//    // Create an empty map
//    for (unsigned int j = 0; j < m_rows; j++)
//    {
//        TrackTileRow row(m_cols, nullptr);
//        m_map.push_back(row);
//    }
}

unsigned int MapBase::cols() const
{
    return m_cols;
}

unsigned int MapBase::rows() const
{
    return m_rows;
}

bool MapBase::setTile(unsigned int x, unsigned int y, TrackTileBase * pTile)
{
    if (x >= m_cols || y >= m_rows)
        return false;

    m_map[y][x] = pTile;

    return true;
}

TrackTileBase * MapBase::getTile(unsigned int x, unsigned int y) const
{
    if (x >= m_cols || y >= m_rows)
        return nullptr;

    return m_map[y][x];
}

MapBase::~MapBase()
{
}