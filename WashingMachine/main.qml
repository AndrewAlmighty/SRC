import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

Window
{
    id: mainWindow
    visible: true
    minimumWidth: 640
    minimumHeight: 250
    title: qsTr("Washing machine")

    Item
    {
        anchors.fill: parent

        GridLayout
        {
            id: statusLayout
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 20
            width: 200

            columns: 2
            rows: 2

            Label { text: "Status: " }
            Label { id: machineStatus; text: GuiManager.machineStatus }
            Label { text: "Lock status:" }
            Label { id: lockStatus; text: GuiManager.lockStatus }
        }

        ButtonGroup { id: laundryMode; onCheckedButtonChanged: GuiManager.setmode(laundryMode.checkedButton.text) }

        Flow
        {
            id: modesLayout
            anchors.top: statusLayout.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: details.top
            anchors.margins: 30
            width: parent.width - 20

            RadioButton { text: "Idle"; checked: true; ButtonGroup.group: laundryMode }
            RadioButton { text: "Fast laundry - 30°C"; ButtonGroup.group: laundryMode }
            RadioButton { text: "Everyday laundry - 40°C"; ButtonGroup.group: laundryMode }
            RadioButton { text: "Cotton - 60°C"; ButtonGroup.group: laundryMode }
        }

        RowLayout
        {
            id: details
            anchors.bottom: laundryButton.top
            anchors.left: parent.left
            anchors.margins: 20
            width: parent.width / 3
            visible: GuiManager.laundry

            Label { text: "T: " }
            Label { id: temp; text: GuiManager.temp + "°C" }
            Label { text: "Motor Speed:" }
            Label { id: motor; text: GuiManager.speed + "rpm" }
        }

        Button
        {
            id: laundryButton
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 30
            text: "Start Laundry"
            enabled: !GuiManager.laundry
            onClicked: GuiManager.setLaundry(true)
        }
    }
}
