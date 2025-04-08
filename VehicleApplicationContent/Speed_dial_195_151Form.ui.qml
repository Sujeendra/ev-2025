import QtQuick
import QtQuick.Timeline
import QtQuick.Studio.Components

Item {
    id: speed_dial_195_151
    width: 290
    height: 330

    property alias kphDisplay: kph_number_195_91.text
    property alias kplDisplay: kpl_number_195_93.text
    property alias kphFrame: kphTimeline.currentFrame
    Image {
        id: speed_dial_195_151Asset
        x: 89
        y: 114
        width: 102
        height: 102
        source: "images/speed_dial_195_151.png"
    }
    Item {
        id: speed_numbers_195_116
        x: -143
        y: 0
        Text {
            id: kph_195_95
            x: 202
            y: 256
            color: "#FFFFFF"
            text: "0"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_96
            x: 174
            y: 238
            color: "#FFFFFF"
            text: "10"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_97
            x: 153
            y: 210
            color: "#FFFFFF"
            text: "20"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_98
            x: 140
            y: 164
            color: "#FFFFFF"
            text: "30"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_99
            x: 145
            y: 127
            color: "#FFFFFF"
            text: "40"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_100
            x: 154
            y: 95
            color: "#FFFFFF"
            text: "50"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_101
            x: 171
            y: 70
            color: "#CCCCCC"
            text: "60"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_102
            x: 198
            y: 45
            color: "#FFFFFF"
            text: "70"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_103
            x: 237
            y: 28
            color: "#FFFFFF"
            text: "80"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_104
            x: 270
            y: 22
            color: "#FFFFFF"
            text: "90"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_105
            x: 304
            y: 28
            color: "#FFFFFF"
            text: "100"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_106
            x: 340
            y: 42
            color: "#FFFFFF"
            text: "110"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_107
            x: 369
            y: 62
            color: "#FFFFFF"
            text: "120"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_108
            x: 386
            y: 85
            color: "#FFFFFF"
            text: "130"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_109
            x: 395
            y: 114
            color: "#FFFFFF"
            text: "140"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_110
            x: 402
            y: 152
            color: "#FFFFFF"
            text: "150"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_111
            x: 402
            y: 181
            color: "#FFFFFF"
            text: "160"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_112
            x: 395
            y: 210
            color: "#FFFFFF"
            text: "170"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_113
            x: 377
            y: 235
            color: "#FFFFFF"
            text: "180"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_114
            x: 358
            y: 254
            color: "#FFFFFF"
            text: "190"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }

        Text {
            id: kph_195_115
            x: 321
            y: 274
            color: "#FFFFFF"
            text: "200"
            font.weight: Font.Thin
            font.pixelSize: 19
            font.family: "Exo 2"
        }
    }
    Item {
        id: speedometer_dots_195_141
        x: -80
        y: 36
        CustomDot {
            id: dot_0_195_140
            x: 154
            y: 209
        }

        CustomDot {
            id: dot_10_195_125
            x: 134
            y: 188
        }

        CustomDot {
            id: dot_20_195_129
            x: 119
            y: 163
        }

        CustomDot {
            id: dot_30_195_132
            x: 113
            y: 131
        }

        CustomDot {
            id: dot_40_195_121
            x: 117
            y: 102
        }

        CustomDot {
            id: dot_50_195_135
            x: 123
            y: 80
        }

        CustomDot {
            id: dot_60_195_138
            x: 138
            y: 60
        }

        CustomDot {
            id: dot_70_195_123
            x: 157
            y: 41
            width: 10
            height: 8
        }

        CustomDot {
            id: dot_80_195_127
            x: 187
            y: 27
        }

        CustomDot {
            id: dot_90_195_131
            x: 213
            y: 21
        }

        CustomDot {
            id: dot_100_195_120
            x: 243
            y: 27
        }

        CustomDot {
            id: dot_110_195_134
            x: 265
            y: 218
        }

        CustomDot {
            id: dot_120_195_139
            x: 267
            y: 36
        }

        CustomDot {
            id: dot_130_195_126
            x: 288
            y: 52
        }

        CustomDot {
            id: dot_140_195_130
            x: 305
            y: 71
        }

        CustomDot {
            id: dot_150_195_133
            x: 315
            y: 92
        }

        CustomDot {
            id: dot_160_195_122
            x: 321
            y: 120
        }

        CustomDot {
            id: dot_170_195_136
            x: 315
            y: 147
        }

        CustomDot {
            id: dot_180_195_137
            x: 309
            y: 171
        }

        CustomDot {
            id: dot_190_195_124
            x: 298
            y: 190
        }

        CustomDot {
            id: dot_200_195_128
            x: 282
            y: 205
        }
    }
    Image {
        id: sppedometer_outer_ring_195_86
        x: 23
        y: 49
        width: 235
        height: 219
        source: "images/sppedometer_outer_ring_195_86.png"
    }
    Image {
        id: speedometer_track_bg_195_87
        x: 49
        y: 77
        width: 182
        height: 165
        source: "images/speedometer_track_bg_195_87.png"
    }
    Image {
        id: speedometer_needle_ring_195_90
        x: 84
        y: 109
        width: 113
        height: 113
        source: "images/speedometer_needle_ring_195_90.png"
    }
    Text {
        id: kpl_number_195_93
        x: 118
        y: 166
        width: 46
        height: 23
        color: "#FFFFFF"
        text: "0"
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 20
        font.family: "Cherry"
    }

    Text {
        id: kpl_readout_195_118
        x: 113
        y: 195
        color: "#FFFFFF"
        text: "Dch. Pwr in kW"
        font.weight: Font.ExtraLight
        font.pixelSize: 8
        font.family: "IBM Plex Mono"
    }

    Text {
        id: kph_number_195_91
        x: 118
        y: 137
        width: 44
        height: 32
        color: "#FFFFFF"
        text: "0"
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 24
        font.family: "Cherry"
    }

    Text {
        id: kph_readout_195_117
        x: 106
        y: 129
        color: "#FFFFFF"
        text: "Ch. Power in kW"
        font.weight: Font.ExtraLight
        font.pixelSize: 9
        font.family: "IBM Plex Mono"
    }
    ArcItem {
        id: arc
        x: 49
        y: 74
        width: 182
        height: 182
        fillColor: "#00ffffff"
        rotation: -90
        strokeColor: "#5ca8ba"
        end: 239.7
        begin: -61.3
        strokeWidth: 2
    }
    Item {
        id: item1
        x: 102
        y: 121
        width: 80
        height: 80
        rotation: -212.876

        Image {
            id: speedometer_needle_195_142
            x: 83
            y: -5
            width: 45
            height: 14
            source: "images/speedometer_needle_195_142.png"
        }
    }
    Timeline {
        id: kphTimeline
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
        endFrame: 200

        KeyframeGroup {
            target: item1
            property: "rotation"

            Keyframe {
                frame: 0
                value: -209.5
            }

            Keyframe {
                frame: 200
                value: 92.5
            }
        }

        KeyframeGroup {
            target: arc
            property: "end"

            Keyframe {
                frame: 0
                value: -61.3
            }

            Keyframe {
                frame: 200
                value: 240.2
            }

            Keyframe {
                frame: 10
                value: -46.45
            }

            Keyframe {
                frame: 30
                value: -16.13
            }

            Keyframe {
                frame: 40
                value: -0.78
            }

            Keyframe {
                frame: 50
                value: 14.55
            }

            Keyframe {
                frame: 70
                value: 44.77
            }

            Keyframe {
                frame: 80
                value: 59.96
            }

            Keyframe {
                frame: 190
                value: 225.12
            }
        }

        KeyframeGroup {
            target: kph_195_95
            property: "scale"

            Keyframe {
                frame: 0
                value: 1.2
            }

            Keyframe {
                frame: 2
                value: 1.2
            }

            Keyframe {
                frame: 4
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_95
            property: "color"

            Keyframe {
                frame: 0
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 2
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 4
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_96
            property: "scale"
            Keyframe {
                frame: 9
                value: 1.2
            }

            Keyframe {
                frame: 11
                value: 1.2
            }

            Keyframe {
                frame: 7
                value: 1
            }

            Keyframe {
                frame: 13
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_96
            property: "color"
            Keyframe {
                frame: 7
                value: "#ffffff"
            }

            Keyframe {
                frame: 9
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 11
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 13
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_97
            property: "scale"
            Keyframe {
                frame: 19
                value: 1.2
            }

            Keyframe {
                frame: 21
                value: 1.2
            }

            Keyframe {
                frame: 17
                value: 1
            }

            Keyframe {
                frame: 23
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_97
            property: "color"
            Keyframe {
                frame: 17
                value: "#ffffff"
            }

            Keyframe {
                frame: 19
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 21
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 23
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_98
            property: "scale"
            Keyframe {
                frame: 29
                value: 1.2
            }

            Keyframe {
                frame: 31
                value: 1.2
            }

            Keyframe {
                frame: 27
                value: 1
            }

            Keyframe {
                frame: 33
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_98
            property: "color"
            Keyframe {
                frame: 27
                value: "#ffffff"
            }

            Keyframe {
                frame: 29
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 31
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 33
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_99
            property: "scale"
            Keyframe {
                frame: 39
                value: 1.2
            }

            Keyframe {
                frame: 41
                value: 1.2
            }

            Keyframe {
                frame: 37
                value: 1
            }

            Keyframe {
                frame: 43
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_99
            property: "color"
            Keyframe {
                frame: 37
                value: "#ffffff"
            }

            Keyframe {
                frame: 39
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 41
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 43
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_100
            property: "scale"
            Keyframe {
                frame: 49
                value: 1.2
            }

            Keyframe {
                frame: 51
                value: 1.2
            }

            Keyframe {
                frame: 47
                value: 1
            }

            Keyframe {
                frame: 53
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_100
            property: "color"
            Keyframe {
                frame: 47
                value: "#ffffff"
            }

            Keyframe {
                frame: 49
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 51
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 53
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_101
            property: "scale"
            Keyframe {
                frame: 59
                value: 1.2
            }

            Keyframe {
                frame: 61
                value: 1.2
            }

            Keyframe {
                frame: 57
                value: 1
            }

            Keyframe {
                frame: 63
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_101
            property: "color"
            Keyframe {
                frame: 57
                value: "#ffffff"
            }

            Keyframe {
                frame: 59
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 61
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 63
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_102
            property: "scale"
            Keyframe {
                frame: 68
                value: 1.2
            }

            Keyframe {
                frame: 70
                value: 1.2
            }

            Keyframe {
                frame: 66
                value: 1
            }

            Keyframe {
                frame: 72
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_102
            property: "color"
            Keyframe {
                frame: 66
                value: "#ffffff"
            }

            Keyframe {
                frame: 68
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 70
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 72
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_103
            property: "scale"
            Keyframe {
                frame: 79
                value: 1.2
            }

            Keyframe {
                frame: 81
                value: 1.2
            }

            Keyframe {
                frame: 77
                value: 1
            }

            Keyframe {
                frame: 83
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_103
            property: "color"
            Keyframe {
                frame: 77
                value: "#ffffff"
            }

            Keyframe {
                frame: 79
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 81
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 83
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_104
            property: "scale"
            Keyframe {
                frame: 89
                value: 1.2
            }

            Keyframe {
                frame: 91
                value: 1.2
            }

            Keyframe {
                frame: 87
                value: 1
            }

            Keyframe {
                frame: 93
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_104
            property: "color"
            Keyframe {
                frame: 87
                value: "#ffffff"
            }

            Keyframe {
                frame: 89
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 91
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 93
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_105
            property: "scale"
            Keyframe {
                frame: 99
                value: 1.2
            }

            Keyframe {
                frame: 101
                value: 1.2
            }

            Keyframe {
                frame: 97
                value: 1
            }

            Keyframe {
                frame: 103
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_105
            property: "color"
            Keyframe {
                frame: 97
                value: "#ffffff"
            }

            Keyframe {
                frame: 99
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 101
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 103
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_106
            property: "scale"
            Keyframe {
                frame: 109
                value: 1.2
            }

            Keyframe {
                frame: 111
                value: 1.2
            }

            Keyframe {
                frame: 107
                value: 1
            }

            Keyframe {
                frame: 113
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_106
            property: "color"
            Keyframe {
                frame: 107
                value: "#ffffff"
            }

            Keyframe {
                frame: 109
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 111
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 113
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_107
            property: "scale"
            Keyframe {
                frame: 119
                value: 1.2
            }

            Keyframe {
                frame: 121
                value: 1.2
            }

            Keyframe {
                frame: 117
                value: 1
            }

            Keyframe {
                frame: 123
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_107
            property: "color"
            Keyframe {
                frame: 117
                value: "#ffffff"
            }

            Keyframe {
                frame: 119
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 121
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 123
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_108
            property: "scale"
            Keyframe {
                frame: 129
                value: 1.2
            }

            Keyframe {
                frame: 131
                value: 1.2
            }

            Keyframe {
                frame: 127
                value: 1
            }

            Keyframe {
                frame: 133
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_108
            property: "color"
            Keyframe {
                frame: 127
                value: "#ffffff"
            }

            Keyframe {
                frame: 129
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 131
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 133
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_109
            property: "scale"
            Keyframe {
                frame: 139
                value: 1.2
            }

            Keyframe {
                frame: 141
                value: 1.2
            }

            Keyframe {
                frame: 137
                value: 1
            }

            Keyframe {
                frame: 143
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_109
            property: "color"
            Keyframe {
                frame: 137
                value: "#ffffff"
            }

            Keyframe {
                frame: 139
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 141
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 143
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_110
            property: "scale"
            Keyframe {
                frame: 149
                value: 1.2
            }

            Keyframe {
                frame: 151
                value: 1.2
            }

            Keyframe {
                frame: 147
                value: 1
            }

            Keyframe {
                frame: 153
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_110
            property: "color"
            Keyframe {
                frame: 147
                value: "#ffffff"
            }

            Keyframe {
                frame: 149
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 151
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 153
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_111
            property: "scale"
            Keyframe {
                frame: 159
                value: 1.2
            }

            Keyframe {
                frame: 161
                value: 1.2
            }

            Keyframe {
                frame: 157
                value: 1
            }

            Keyframe {
                frame: 163
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_111
            property: "color"
            Keyframe {
                frame: 157
                value: "#ffffff"
            }

            Keyframe {
                frame: 159
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 161
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 163
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_112
            property: "scale"
            Keyframe {
                frame: 169
                value: 1.2
            }

            Keyframe {
                frame: 171
                value: 1.2
            }

            Keyframe {
                frame: 167
                value: 1
            }

            Keyframe {
                frame: 173
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_112
            property: "color"
            Keyframe {
                frame: 167
                value: "#ffffff"
            }

            Keyframe {
                frame: 169
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 171
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 173
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_113
            property: "scale"
            Keyframe {
                frame: 179
                value: 1.2
            }

            Keyframe {
                frame: 181
                value: 1.2
            }

            Keyframe {
                frame: 177
                value: 1
            }

            Keyframe {
                frame: 183
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_113
            property: "color"
            Keyframe {
                frame: 177
                value: "#ffffff"
            }

            Keyframe {
                frame: 179
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 181
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 183
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_114
            property: "scale"
            Keyframe {
                frame: 189
                value: 1.2
            }

            Keyframe {
                frame: 191
                value: 1.2
            }

            Keyframe {
                frame: 187
                value: 1
            }

            Keyframe {
                frame: 193
                value: 1
            }
        }

        KeyframeGroup {
            target: kph_195_114
            property: "color"
            Keyframe {
                frame: 187
                value: "#ffffff"
            }

            Keyframe {
                frame: 189
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 191
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 193
                value: "#ffffff"
            }
        }

        KeyframeGroup {
            target: kph_195_115
            property: "scale"
            Keyframe {
                frame: 196
                value: 1
            }

            Keyframe {
                frame: 198
                value: 1.2
            }

            Keyframe {
                frame: 200
                value: 1.2
            }
        }

        KeyframeGroup {
            target: kph_195_115
            property: "color"
            Keyframe {
                frame: 196
                value: "#ffffff"
            }

            Keyframe {
                frame: 198
                value: "#5ca8ba"
            }

            Keyframe {
                frame: 200
                value: "#5ca8ba"
            }
        }
    }
}
