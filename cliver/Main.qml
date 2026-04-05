import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Universal
import TCPServer

Window {
    id: window
    width: 640
    height: 400
    visible: true
    title: qsTr("Hello World")
    color: "#11111b"

    Universal.theme: Universal.Dark
    Universal.background: "#1e1e2e"
    Universal.foreground: "#cdd6f4"
    Universal.accent: "#b4befe"

    TCPServer {
        id: tcpServer

        onServerStatusChanged: status => output.append(addTime(status ? "Server started" : "Server stopped"))
        onClientStatusChanged: status => output.append(addTime(status ? "Client connected" : "Client disconnected"))
        onReceived: msg => output.append(addTime("Recieved: " + msg))
        onError: err => output.append(addTime("Error: " + err))
    }

    Component {
        id: stopBtnComp

        Button {
            id: stopBtn
            topPadding: 8
            leftPadding: 16
            rightPadding: 16
            bottomPadding: 8
            font.pixelSize: 16
            Universal.foreground: "#11111b"
            text: "Stop"
            onClicked: {
                tcpServer.stop();
            }
            background: Rectangle {
                border.width: 2
                radius: 8
                color: "#f38ba8"
            }
        }
    }

    Component {
        id: stoppedActions

        RowLayout {
            Button {
                id: connectBtn
                topPadding: 8
                leftPadding: 16
                rightPadding: 16
                bottomPadding: 8
                font.pixelSize: 16
                Universal.foreground: "#11111b"
                text: "Connect"
                onClicked: {
                    tcpServer.connect("localhost", parseInt(port.text));
                }
                background: Rectangle {
                    border.width: 2
                    radius: 8
                    color: "#a6e3a1"
                }
            }

            Button {
                id: startBtn
                topPadding: 8
                leftPadding: 16
                rightPadding: 16
                bottomPadding: 8
                font.pixelSize: 16
                Universal.foreground: "#11111b"
                text: "Start"
                onClicked: {
                    tcpServer.start("localhost", parseInt(port.text));
                }
                background: Rectangle {
                    border.width: 2
                    radius: 8
                    color: "#a6e3a1"
                }
            }
        }
    }

    Component {
        id: startedActions

        RowLayout {
            Button {
                id: disconnectBtn
                topPadding: 8
                leftPadding: 16
                rightPadding: 16
                bottomPadding: 8
                font.pixelSize: 16
                Universal.foreground: "#11111b"
                text: "Disconnect"
                enabled: tcpServer.isConnected
                onClicked: {
                    tcpServer.disconnect();
                }
                background: Rectangle {
                    border.width: 2
                    radius: 8
                    opacity: tcpServer.isConnected ? 1 : 0.5
                    color: "#f38ba8"
                }
            }

            Loader {
                sourceComponent: tcpServer.isListening ? stopBtnComp : undefined
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        anchors.margins: 8

        FlexboxLayout {
            Layout.fillHeight: false
            justifyContent: FlexboxLayout.JustifySpaceBetween
            alignItems: FlexboxLayout.AlignCenter

            RowLayout {
                Text {
                    id: appName
                    text: "CliVer"
                    font.bold: true
                    font.pixelSize: 24
                    color: "#b4befe"
                    leftPadding: 8
                }

                Text {
                    id: state
                    text: tcpServer.isConnected ? "Connected" : tcpServer.isListening ? "Listening" : "Disconnected"
                    font.weight: 500
                    font.pixelSize: 20
                    color: tcpServer.isConnected ? "#a6e3a1" : tcpServer.isListening ? "#b4befe" : "#f38ba8"
                }
            }

            RowLayout {
                TextField {
                    id: port
                    implicitHeight: parent.height
                    implicitWidth: 76
                    font.pixelSize: 16
                    text: "1234"
                    validator: IntValidator {}
                    enabled: !tcpServer.isListening && !tcpServer.isConnected
                    background: Rectangle {
                        border.width: 2
                        radius: 8
                        color: "#1e1e2e"
                    }
                }

                Loader {
                    sourceComponent: tcpServer.isListening || tcpServer.isConnected ? startedActions : stoppedActions
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent

                TextArea {
                    id: output
                    readOnly: true
                    font.pixelSize: 16
                    background: Rectangle {
                        border.width: 2
                        radius: 8
                        color: "#1e1e2e"
                    }
                }
            }

            Button {
                x: parent.width - 40
                y: 8
                icon.color: "#f38ba8"
                icon.source: "qrc:/bin.png"
                onClicked: output.clear()
                background: Rectangle {
                    color: "transparent"
                }
            }
        }

        RowLayout {
            TextField {
                id: input
                implicitHeight: sendBtn.height
                Layout.fillWidth: true
                font.pixelSize: 16
                placeholderText: qsTr("Enter something")
                background: Rectangle {
                    border.width: 2
                    radius: 8
                    color: "#1e1e2e"
                }
            }

            Button {
                id: sendBtn
                topPadding: 8
                leftPadding: 16
                rightPadding: 16
                bottomPadding: 8
                font.pixelSize: 16
                Universal.foreground: "#11111b"
                text: "Send"
                enabled: tcpServer.isConnected && input.text != "" ? true : false
                onClicked: {
                    tcpServer.send(input.text);
                    output.append(addTime("Sent: " + input.text));
                    input.clear();
                }
                background: Rectangle {
                    border.width: 2
                    radius: 8
                    opacity: parent.enabled ? 1 : 0.5
                    color: "#89b4fa"
                }
            }
        }
    }

    Component.onCompleted: {
        output.text = addTime("Application started");
    }

    function addTime(msg) {
        var now = new Date();
        var currentTime = ("0" + now.getHours()).slice(-2) + ":" + ("0" + now.getMinutes()).slice(-2) + ":" + ("0" + now.getSeconds()).slice(-2);
        return "[" + currentTime + "] " + msg;
    }
    function getTime() {
        var now = new Date();
        return ("0" + now.getHours()).slice(-2) + ":" + ("0" + now.getMinutes()).slice(-2) + ":" + ("0" + now.getSeconds()).slice(-2);
    }
}
