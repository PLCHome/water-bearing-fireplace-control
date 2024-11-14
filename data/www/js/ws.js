var gateway = `${(window.location.protocol == "https:" ? "wss" : "ws")}://${window.location.hostname}/ws`;
if (gateway.indexOf('localhost') != -1) {
    gateway = gateway.replace('localhost', '192.168.2.178');
}
var websocket;
// Init web socket when the page loads
$(window).on('load', onload);

function onload(event) {
    initWebSocket();
}

var toGetReadings;
function getReadings() {
    clearTimeout(toGetReadings);
    toGetReadings = setTimeout(function () {
        if (websocket)
            websocket.send("getReadings");
    }, 1000)
}

function sendData(data) {
    if (typeof data === 'String') {
        websocket.send(data);
    } else {
        websocket.send(JSON.stringify(data));
    }
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function setValue(path, val) {
    //let ele = $('#'+ path)
    let ele = $(`[id='${path}']`)
    if (ele && ele.hasClass) {
        let html = "";
        if (ele.hasClass("div100")) {
            html = ((val || 0) / 100).toFixed(2);
        } else if (ele.hasClass("lightbulb")) {
            html = `<i class="${val ? 'fa-solid' : 'fa-regular'} fa-lightbulb" onClick="sendData({${path}:${!val}});"></i>`
        } else {
            html = val;
        }
        ele.html(html);
    }
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
    console.log(event);
    var myObj = JSON.parse(event.data);
    function change(path, myObj) {
        var keys = Object.keys(myObj);
        for (var i = 0; i < keys.length; i++) {
            let key = keys[i];
            if (typeof myObj[key] === 'object') {
                change(path + key + (Array.isArray(myObj[key]) ? '' : '_'), myObj[key]);
            } else {
                setValue(path + key, myObj[key]);
            }
        }
    }
    change('', myObj);
}

function uploadString(filename, fileContent) {
    // Entferne führenden Slash (falls vorhanden) vom Dateinamen
    if (filename.startsWith("/")) {
        filename = filename.substring(1); // Entfernt das erste Zeichen (den Slash)
    }

    // Blob aus dem Textinhalt erstellen
    var strblob = new Blob([fileContent], { type: 'text/plain' });

    // FormData erstellen und die Datei sowie andere Felder anhängen
    var formdata = new FormData();
    formdata.append("file", strblob, filename);
    formdata.append("field-1", "field-1-data"); // Beispiel für ein weiteres Feld

    // AJAX-Request ausführen
    $.ajax({
        url: "/upload",
        type: "POST",
        data: formdata,
        processData: false, // verhindert die automatische Verarbeitung von FormData
        contentType: false, // verhindert das Setzen des Content-Type Headers (wichtig für FormData)
        success: function (result) {
            console.log("Upload erfolgreich:", result); // Erfolgsmeldung
        },
        error: function (xhr, status, error) {
            // Detaillierte Fehlerbehandlung
            console.log("Fehler beim Upload:", error);
            console.log("Status:", status);
            console.log("XHR-Status:", xhr.status);
            console.log("XHR-Antwort:", xhr.responseText);
        }
    });
}

function getFileNameFromUrl(url) {
    // Extrahiert den Dateinamen aus der URL
    return url.substring(url.lastIndexOf('/') + 1);
}

// Funktion zum Herunterladen einer Datei
function downloadFile(fileUrl, fileName) {
    // Wenn der Dateiname nicht angegeben wurde, versuche, ihn aus der URL zu extrahieren
    fileUrl = fileUrl.replace('/www/','/');

    if (!fileName) {
        fileName = getFileNameFromUrl(fileUrl);
    }

    // Erstelle ein unsichtbares <a>-Tag
    var a = document.createElement('a');
    a.href = fileUrl;  // Setze die URL der Datei
    a.download = fileName;  // Setze den Namen der Datei, die heruntergeladen wird

    // Füge das <a>-Tag zum DOM hinzu
    document.body.appendChild(a);

    // Simuliere den Klick auf den Download-Link
    a.click();

    // Entferne das <a>-Tag nach dem Klick, um den DOM sauber zu halten
    document.body.removeChild(a);
}

function deleteFile(filename, Callback) {
    cb = Callback;
    $.ajax({
        url: '/delete-file',           // Die URL, die aufgerufen werden soll
        type: 'GET',                   // HTTP-Methode (GET)
        data: { filename: filename },  // Der Dateiname als Query-Parameter
        success: function(response) {
            // Erfolg: Die Antwort vom Server wird hier verarbeitet
            console.log("Erfolgreich gelöscht: " + response);
            alert("Erfolgreich gelöscht: " + response);
        },
        error: function(xhr, status, error) {
            // Fehlerbehandlung, falls etwas schief geht
            console.error("Fehler beim Löschen der Datei: " + error);
            alert("Fehler beim Löschen der Datei.");
        }
    });
}
