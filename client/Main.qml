import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Universal
import TCPClient

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

    TCPClient {
        id: tcpClient

        onStatusChanged: status => output.append(addTime(status ? "Connected to the server" : "Disconnected from the host"))
        onReceived: msg => output.append(addTime("Recieved: " + msg))
        onError: err => output.append("Error: ", err)
    }

    Component {
        id: disconnectedActions

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
                    tcpClient.connect("localhost", parseInt(portInput.text));
                }
                background: Rectangle {
                    border.width: 2
                    radius: 5
                    color: "#a6e3a1"
                }
            }
        }
    }

    Component {
        id: connectedActions

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
                onClicked: {
                    tcpClient.disconnect();
                }
                background: Rectangle {
                    border.width: 2
                    radius: 5
                    color: "#f38ba8"
                }
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
                    text: "TCPClient"
                    font.bold: true
                    font.pixelSize: 24
                    color: "#b4befe"
                }

                Text {
                    id: state
                    text: tcpClient.isConnected ? "Connected" : "Disconnected"
                    font.weight: 500
                    font.pixelSize: 20
                    color: tcpClient.isConnected ? "#a6e3a1" : "#f38ba8"
                }
            }

            RowLayout {
                TextField {
                    id: portInput
                    implicitHeight: parent.height
                    implicitWidth: 96
                    font.pixelSize: 16
                    text: "1234"
                    validator: IntValidator {}
                    enabled: !tcpClient.isConnected
                    background: Rectangle {
                        border.width: 2
                        radius: 5
                        color: "#1e1e2e"
                    }
                }
                Loader {
                    sourceComponent: tcpClient.isConnected ? connectedActions : disconnectedActions
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: output
                readOnly: true
                font.pixelSize: 16
                background: Rectangle {
                    border.width: 2
                    radius: 5
                    color: "#1e1e2e"
                }
            }

            Button {
                x: output.width - 40
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
                    radius: 5
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
                enabled: tcpClient.isConnected && input.text != "" ? true : false
                onClicked: {
                    tcpClient.send(input.text);
                    output.append(addTime("Sent: " + input.text));
                    input.clear();
                }
                background: Rectangle {
                    border.width: 2
                    radius: 5
                    opacity: parent.enabled ? 1 : 0.5
                    color: "#89b4fa"
                }
            }
        }
    }

    Component.onCompleted: {
        output.text = addTime("Application started\n- - - - - - -");
    }

    function addTime(msg) {
        var now = new Date();
        var currentTime = ("0" + now.getHours()).slice(-2) + ":" + ("0" + now.getMinutes()).slice(-2) + ":" + ("0" + now.getSeconds()).slice(-2);
        return "[" + currentTime + "] " + msg;
    }
}
