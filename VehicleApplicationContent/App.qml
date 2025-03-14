// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import VehicleApplication




Window {
    width: mainScreen.width
    height: mainScreen.height

    visible: true
    title: "VehicleApplication"
    QtObject {
        id: gData
        property var signalIndexMap: new Map()
        Component.onCompleted: {
            signalValueMap.signalValueList.forEach(function(signal, index) {
                       signalIndexMap[signal.key] = index;
            });
        }

    }
    MainForm {
        id: mainScreen

    }

}

