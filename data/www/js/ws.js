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

function getReadings() {
    if (websocket)
        websocket.send("getReadings");
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
            html = `<i class="${val ? 'fa-solid':'fa-regular'} fa-lightbulb" onClick="sendData({${path}:${!val}});"></i>`
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
