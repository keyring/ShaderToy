import QtQuick 2.5
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import ShaderToyQML 1.0
import CodeEditorQML 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: document.documentTitle
    color: "black"  // dont be 'white', or the gl render will be hide

    Action {
        id: openAction
        text: qsTr("Open")
        shortcut: "ctrl+o"
        onTriggered: {
            fileDialog.selectExisting = true
            fileDialog.open()
        }
    }

    Action {
        id: saveAction
        text: qsTr("Save")
        shortcut: "ctrl+s"
        onTriggered:{
            if(document.isUntitled){
                fileDialog.selectExisting = false
                fileDialog.open()
            }else{
                document.save()
            }
        }
    }

    Action {
        id: saveAsAction
        text: qsTr("Save as...")
        shortcut: "ctrl+shift+s"
        onTriggered: {
            fileDialog.selectExisting = false
            fileDialog.open()
        }
    }

    Action {
        id: quitAction
        text: qsTr("Quit")
        shortcut: "ctrl+q"
        onTriggered: Qt.quit()
    }
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem { action: openAction }
            MenuItem { action: saveAction }
            MenuItem { action: saveAsAction }
            MenuItem { action: quitAction }
        }
        Menu {
            title: qsTr("Edit")
            MenuItem {
                text: qsTr("vs")
            }
            MenuItem {
                text: qsTr("fs")
                onTriggered: Qt.quit()
            }
        }
    }

//    statusBar: StatusBar {
//        RowLayout {
//            anchors.fill: parent
//            Label { text: qsTr("Read Only") }
//        }
//    }


    TextArea {

        Accessible.name: "document"
        id: textArea
        width: parent.width/2
        height: parent.height
        baseUrl: "qrc:/"
        text: document.text
        font.pointSize: 12
        font.family: "monospace"
        wrapMode: TextEdit.WordWrap
        Component.onCompleted: forceActiveFocus()

        style: TextAreaStyle {
            textColor: "#fff"
            selectionColor: "steelblue"
            selectedTextColor: "#eee"
            backgroundColor: "#3a4055"
        }
    }

    CodeEditor {
        id: document
        target: textArea
        Component.onCompleted: document.fileUrl = ""
    }

    FileDialog{
        id: fileDialog
        modality: Qt.NonModal
        selectMultiple: false
        selectFolder: false
        showFocusHighlight: true
        nameFilters: ["GLSL Files (*.glsl *.GLSL *.st)", "All Files (*.*)"]
        onAccepted: {
            if (fileDialog.selectExisting)
                document.fileUrl = fileUrl
            else
                document.saveAs(fileUrl)
        }
    }

    Item {
        width: parent.width/2
        height: parent.height
        anchors.right: parent.right

        Playground {
            id: playground
            NumberAnimation on t {
                loops: Animation.Infinite
                running: true
                from: 0
                to: 1
                duration: 1000
            }
            source: document.text
        }

    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: playground.compilelog
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }

}
