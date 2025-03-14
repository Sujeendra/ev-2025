
import QtQuick
import QtQuick.Timeline
import QtQuick.Studio.Components
import QtQuick.Studio.Effects

Item {
    id: fuel_dial_195_43
    width: 230
    height: 330
    visible: true
    clip: false
    property alias rangeDisplay: _kilometers_195_11.text
    property alias litersDisplay: _liters_195_10.text
    property alias fuelFrame: fuelTimeline.currentFrame


    Item {
        id: item2
        x: 8
        y: 20
        width: 96
        height: 131

        Image {
            id: fuel_dial_195_43Asset
            x: 0
            y: 80
            width: 100
            height: 111
            source: "images/fuel_dial_195_43.png"
        }
        Item {
            id: kilometers_left_195_192
            x: 71
            y: 250
            Text {
                id: _kilometers_195_11
                x: -40
                y: -114
                width: 40
                height: 25
                color: "#FFFFFF"
                text: "0"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 20
                font.family: "Cherry"
            }

            Text {
                id: kilometers_195_12
                x: -40
                y: -83
                width: 54
                height: 11
                color: "#FFFFFF"
                text: "VOLT (x10)"
                font.weight: Font.ExtraLight
                font.pixelSize: 9
                font.family: "IBM Plex Mono"
            }
        }
        Item {
            id: liters_left_195_194
            x: 96
            y: 144
            Text {
                id: _liters_195_10
                x: -61
                y: -35
                width: 33
                height: 22
                color: "#FFFFFF"
                text: "0"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 20
                font.family: "Cherry"
            }

            Text {
                id: liters_195_13
                x: -59
                y: -47
                width: 31
                height: 11
                color: "#FFFFFF"
                text: "Current"
                font.weight: Font.ExtraLight
                font.pixelSize: 9
                font.family: "IBM Plex Mono"
            }
        }
        Item {
            id: fuel_dots_195_190
            x: 62
            y: 31

            CustomDot {
                id: fuel_dot_1
                x: 52
                y: -14
            }

            CustomDot {
                id: fuel_dot_2
                x: 76
                y: 8
            }

            CustomDot {
                id: fuel_dot_3
                x: 96
                y: 33
            }

            CustomDot {
                id: fuel_dot_4
                x: 108
                y: 62
            }

            CustomDot {
                id: fuel_dot_5
                x: 112
                y: 93
            }

            CustomDot {
                id: fuel_dot_6
                x: 108
                y: 128
            }

            CustomDot {
                id: fuel_dot_7
                x: 97
                y: 154
            }

            CustomDot {
                id: fuel_dot_8
                x: 80
                y: 182
            }

            CustomDot {
                id: fuel_dot_9
                x: 57
                y: 205
            }

            CustomDot {
                id: fuel_dot_10
                x: 25
                y: 221
            }

            CustomDot {
                id: fuel_dot_11
                x: -16
                y: 230
            }

            CustomDot {
                id: fuel_dot_12
                x: -57
                y: 224
            }
        }
        Item {
            id: fuel_numbers_195_42
            x: 44
            y: 0
            Text {
                id: item_195_30
                x: -44
                y: 279
                color: "#FFFFFF"
                text: "0"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_31
                x: -12
                y: 285
                color: "#FFFFFF"
                text: "10"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_32
                x: 37
                y: 275
                color: "#FFFFFF"
                text: "20"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_33
                x: 84
                y: 254
                width: 20.725
                color: "#FFFFFF"
                text: "30"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_34
                x: 116
                y: 222
                width: 21.725
                height: 23
                color: "#FFFFFF"
                text: "40"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_35
                x: 139
                y: 185
                width: 21.725
                color: "#FFFFFF"
                text: "50"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_36
                x: 150
                y: 149
                width: 21.725
                color: "#FFFFFF"
                text: "60"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_37
                x: 150
                y: 111
                color: "#FFFFFF"
                text: "70"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_38
                x: 145
                y: 76
                color: "#FFFFFF"
                text: "80"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_39
                x: 139
                y: 46
                color: "#FFFFFF"
                text: "90"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_40
                x: 115
                y: 17
                color: "#FFFFFF"
                text: "100"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }

            Text {
                id: item_195_41
                x: 83
                y: -9
                color: "#FFFFFF"
                text: "110"
                font.weight: Font.Thin
                font.pixelSize: 19
                font.family: "Exo 2"
            }
        }
        Image {
            id: arc_background_track_195_6
            x: 8
            y: 31
            width: 154
            height: 219
            source: "images/arc_background_track_195_6.png"
        }
        ArcItem {
            id: arc
            x: -68
            y: 28
            width: 240
            height: 200
            rotation: -90
            fillColor: "#00ffffff"
            end: 288.4
            begin: 273.2
            strokeWidth: 40
        }

        Item {
            id: item1
            x: 11
            y: 91
            width: 90
            height: 90
            rotation: 39.303

            Image {
                id: fuel_needle_195_27
                x: 42
                y: 102
                width: 20
                height: 57
                source: "images/fuel_needle_195_27.png"
            }
        }
        GlowItem {
            id: glow
            x: -28
            y: 60
            width: 147
            height: 165
            color: "#e24141"
            radius: 1.7
            samples: 32
            spread: 0.2

            PieItem {
                id: pie
                x: 28
                y: 16
                width: 104
                height: 122
                fillColor: "#00ffffff"
                strokeWidth: 4.5
                end: 360
            }
        }
        Image {
            id: outside_white_arch_195_28
            x: -7
            y: 0
            width: 202
            height: 284
            source: "images/outside_white_arch_195_28.png"
        }


    }
    Timeline {
        id: fuelTimeline
        animations: [
            TimelineAnimation {
                id: timelineAnimation
                property: "currentFrame"
                loops: 1
                duration: 1000
                from: 0
                to: 1000
                running: false
            }
        ]
        enabled: true
        startFrame: 0
        endFrame: 1100

        KeyframeGroup {
            target: item1
            property: "rotation"

            Keyframe {
                frame: 0
                value: 32.5
            }

            Keyframe {
                frame: 1100
                value: -141.3
            }
        }

        KeyframeGroup {
            target: arc
            property: "begin"

            Keyframe {
                frame: 0
                value: 288.4
            }

            Keyframe {
                frame: 1100
                value: 118.8
            }

            Keyframe {
                frame: 100
                value: 273.2
            }

            Keyframe {
                frame: 500
                value: 211.68
            }

            Keyframe {
                frame: 600
                value: 196.42
            }

            Keyframe {
                frame: 700
                value: 181.12
            }

            Keyframe {
                frame: 800
                value: 165.76
            }

            Keyframe {
                frame: 900
                value: 150.34
            }

            Keyframe {
                frame: 1000
                value: 134.82
            }
        }

        KeyframeGroup {
            target: arc
            property: "strokeColor"

            Keyframe {
                frame: 0
                value: "#e24141"
            }

            Keyframe {
                frame: 299
                value: "#e24141"
            }

            Keyframe {
                frame: 300
                value: "#df8c49"
            }

            Keyframe {
                frame: 599
                value: "#df8c49"
            }

            Keyframe {
                frame: 600
                value: "#3eee6b"
            }

            Keyframe {
                frame: 1100
                value: "#3eee6b"
            }
        }

        KeyframeGroup {
            target: pie
            property: "strokeColor"

            Keyframe {
                frame: 0
                value: "#e24141"
            }

            Keyframe {
                frame: 299
                value: "#e24141"
            }

            Keyframe {
                frame: 300
                value: "#df8c49"
            }

            Keyframe {
                frame: 599
                value: "#df8c49"
            }

            Keyframe {
                frame: 600
                value: "#3eee6b"
            }

            Keyframe {
                frame: 1100
                value: "#3eee6b"
            }
        }

        KeyframeGroup {
            target: item_195_30
            property: "scale"

            Keyframe {
                frame: 0
                value: 1.2
            }

            Keyframe {
                frame: 10
                value: 1.2
            }

            Keyframe {
                frame: 20
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_30
            property: "color"

            Keyframe {
                frame: 0
                value: "#e24141"
            }

            Keyframe {
                frame: 10
                value: "#e24141"
            }

            Keyframe {
                frame: 20
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_31
            property: "scale"

            Keyframe {
                frame: 95
                value: 1.2
            }

            Keyframe {
                frame: 105
                value: 1.2
            }

            Keyframe {
                frame: 115
                value: 1
            }

            Keyframe {
                frame: 85
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_31
            property: "color"
            Keyframe {
                frame: 85
                value: "#ffffff"
            }

            Keyframe {
                frame: 95
                value: "#e24141"
            }

            Keyframe {
                frame: 105
                value: "#e24141"
            }

            Keyframe {
                frame: 115
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_32
            property: "scale"
            Keyframe {
                frame: 195
                value: 1.2
            }

            Keyframe {
                frame: 205
                value: 1.2
            }

            Keyframe {
                frame: 215
                value: 1
            }

            Keyframe {
                frame: 185
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_32
            property: "color"
            Keyframe {
                frame: 185
                value: "#ffffff"
            }

            Keyframe {
                frame: 195
                value: "#e24141"
            }

            Keyframe {
                frame: 205
                value: "#e24141"
            }

            Keyframe {
                frame: 215
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_33
            property: "scale"
            Keyframe {
                frame: 295
                value: 1.2
            }

            Keyframe {
                frame: 305
                value: 1.2
            }

            Keyframe {
                frame: 315
                value: 1
            }

            Keyframe {
                frame: 285
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_33
            property: "color"
            Keyframe {
                frame: 285
                value: "#ffffff"
            }

            Keyframe {
                frame: 295
                value: "#df8c49"
            }

            Keyframe {
                frame: 305
                value: "#df8c49"
            }

            Keyframe {
                frame: 315
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_34
            property: "scale"
            Keyframe {
                frame: 396
                value: 1.2
            }

            Keyframe {
                frame: 406
                value: 1.2
            }

            Keyframe {
                frame: 416
                value: 1
            }

            Keyframe {
                frame: 386
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_34
            property: "color"
            Keyframe {
                frame: 386
                value: "#ffffff"
            }

            Keyframe {
                frame: 396
                value: "#df8c49"
            }

            Keyframe {
                frame: 406
                value: "#df8c49"
            }

            Keyframe {
                frame: 416
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_35
            property: "scale"
            Keyframe {
                frame: 495
                value: 1.2
            }

            Keyframe {
                frame: 505
                value: 1.2
            }

            Keyframe {
                frame: 515
                value: 1
            }

            Keyframe {
                frame: 485
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_35
            property: "color"
            Keyframe {
                frame: 485
                value: "#ffffff"
            }

            Keyframe {
                frame: 495
                value: "#df8c49"
            }

            Keyframe {
                frame: 505
                value: "#df8c49"
            }

            Keyframe {
                frame: 515
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_36
            property: "scale"
            Keyframe {
                frame: 596
                value: 1.2
            }

            Keyframe {
                frame: 606
                value: 1.2
            }

            Keyframe {
                frame: 616
                value: 1
            }

            Keyframe {
                frame: 586
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_36
            property: "color"
            Keyframe {
                frame: 586
                value: "#ffffff"
            }

            Keyframe {
                frame: 596
                value: "#3eee6b"
            }

            Keyframe {
                frame: 606
                value: "#3eee6b"
            }

            Keyframe {
                frame: 616
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_37
            property: "scale"
            Keyframe {
                frame: 697
                value: 1.2
            }

            Keyframe {
                frame: 707
                value: 1.2
            }

            Keyframe {
                frame: 717
                value: 1
            }

            Keyframe {
                frame: 687
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_37
            property: "color"
            Keyframe {
                frame: 687
                value: "#ffffff"
            }

            Keyframe {
                frame: 697
                value: "#3eee6b"
            }

            Keyframe {
                frame: 707
                value: "#3eee6b"
            }

            Keyframe {
                frame: 717
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_38
            property: "scale"
            Keyframe {
                frame: 795
                value: 1.2
            }

            Keyframe {
                frame: 805
                value: 1.2
            }

            Keyframe {
                frame: 815
                value: 1
            }

            Keyframe {
                frame: 785
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_38
            property: "color"
            Keyframe {
                frame: 785
                value: "#ffffff"
            }

            Keyframe {
                frame: 795
                value: "#3eee6b"
            }

            Keyframe {
                frame: 805
                value: "#3eee6b"
            }

            Keyframe {
                frame: 815
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_39
            property: "scale"
            Keyframe {
                frame: 896
                value: 1.2
            }

            Keyframe {
                frame: 906
                value: 1.2
            }

            Keyframe {
                frame: 916
                value: 1
            }

            Keyframe {
                frame: 886
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_39
            property: "color"
            Keyframe {
                frame: 886
                value: "#ffffff"
            }

            Keyframe {
                frame: 896
                value: "#3eee6b"
            }

            Keyframe {
                frame: 906
                value: "#3eee6b"
            }

            Keyframe {
                frame: 916
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_40
            property: "scale"
            Keyframe {
                frame: 995
                value: 1.2
            }

            Keyframe {
                frame: 1005
                value: 1.2
            }

            Keyframe {
                frame: 1015
                value: 1
            }

            Keyframe {
                frame: 985
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_40
            property: "color"
            Keyframe {
                frame: 985
                value: "#ffffff"
            }

            Keyframe {
                frame: 995
                value: "#3eee6b"
            }

            Keyframe {
                frame: 1005
                value: "#3eee6b"
            }

            Keyframe {
                frame: 1015
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: item_195_41
            property: "scale"

            Keyframe {
                frame: 1100
                value: 1.2
            }

            Keyframe {
                frame: 1090
                value: 1.198
            }

            Keyframe {
                frame: 1080
                value: 1
            }
        }

        KeyframeGroup {
            target: item_195_41
            property: "color"
            Keyframe {
                frame: 1080
                value: "#ffffff"
            }

            Keyframe {
                frame: 1090
                value: "#3eee6b"
            }

            Keyframe {
                frame: 1100
                value: "#3eee6b"
            }
        }

        KeyframeGroup {
            target: glow
            property: "radius"
            Keyframe {
                frame: 0
                value: 11.7
            }

            Keyframe {
                frame: 20
                value: 0
            }

            Keyframe {
                frame: 40
                value: 11.7
            }

            Keyframe {
                frame: 60
                value: 0
            }

            Keyframe {
                frame: 80
                value: 11.7
            }

            Keyframe {
                frame: 100
                value: 0
            }

            Keyframe {
                frame: 120
                value: 11.7
            }

            Keyframe {
                frame: 140
                value: 0
            }

            Keyframe {
                frame: 160
                value: 11.7
            }

            Keyframe {
                frame: 180
                value: 0
            }

            Keyframe {
                frame: 200
                value: 11.7
            }

            Keyframe {
                frame: 220
                value: 0
            }

            Keyframe {
                frame: 240
                value: 11.7
            }

            Keyframe {
                frame: 260
                value: 0
            }

            Keyframe {
                frame: 280
                value: 11.7
            }

            Keyframe {
                frame: 300
                value: 0
            }
        }

        KeyframeGroup {
            target: glow
            property: "spread"
            Keyframe {
                frame: 0
                value: 0.7
            }

            Keyframe {
                frame: 20
                value: 0
            }

            Keyframe {
                frame: 40
                value: 0.7
            }

            Keyframe {
                frame: 60
                value: 0
            }

            Keyframe {
                frame: 80
                value: 0.7
            }

            Keyframe {
                frame: 100
                value: 0
            }

            Keyframe {
                frame: 120
                value: 0.7
            }

            Keyframe {
                frame: 140
                value: 0
            }

            Keyframe {
                frame: 160
                value: 0.7
            }

            Keyframe {
                frame: 180
                value: 0
            }

            Keyframe {
                frame: 200
                value: 0.7
            }

            Keyframe {
                frame: 220
                value: 0
            }

            Keyframe {
                frame: 240
                value: 0.7
            }

            Keyframe {
                frame: 260
                value: 0
            }

            Keyframe {
                frame: 280
                value: 0.7
            }

            Keyframe {
                frame: 300
                value: 0
            }
        }
    }

}
