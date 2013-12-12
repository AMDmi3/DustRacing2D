// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "inputhandler.hpp"

#include <map>
#include <QString>

class Track;

//! Singleton settings class that wraps the use of QSettings.
class Settings
{
public:

    //! Constructor.
    Settings();

    static Settings & instance();

    void saveLapRecord(const Track & track, int msecs);
    int loadLapRecord(const Track & track) const;
    void resetLapRecords();

    void saveRaceRecord(const Track & track, int msecs, int lapCount);
    int loadRaceRecord(const Track & track, int lapCount) const;
    void resetRaceRecords();

    void saveBestPos(const Track & track, int pos, int lapCount);
    int loadBestPos(const Track & track, int lapCount) const;
    void resetBestPos();

    void saveTrackUnlockStatus(const Track & track, int lapCount);
    bool loadTrackUnlockStatus(const Track & track, int lapCount) const;
    void resetTrackUnlockStatuses();

    void saveResolution(int hRes, int vRes, bool nativeResolution, bool fullScreen);
    void loadResolution(int & hRes, int & vRes, bool & nativeResolution, bool & fullScreen);

    void saveKeyMapping(int player, InputHandler::InputAction action, int key);
    int loadKeyMapping(int player, InputHandler::InputAction action);

    void saveValue(QString key, int value);
    int loadValue(QString key, int defaultValue = 0);

    static QString fpsKey();
    static QString lapCountKey();
    static QString vsyncKey();

private:

    QString combineActionAndPlayer(int player, InputHandler::InputAction action);

    static Settings * m_instance;
    std::map<InputHandler::InputAction, QString> m_actionToStringMap;
};

#endif // SETTINGS_HPP
