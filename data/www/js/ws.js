var gateway = `${(window.location.protocol == "https:" ? "wss" : "ws")}://${window.location.hostname}:${window.location.port}/ws`;
if (gateway.indexOf('localhost') != -1) {
  gateway = gateway.replace('localhost', '192.168.2.178');
}
var websocket = null;

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
  if (typeof data === 'string') {
    if (websocket)
      websocket.send(data);
  } else {
    if (websocket)
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
  let ele = $(`[data-update='${path}']`)
  if (ele && ele.hasClass) {
    // Prüfen, ob bereits ein Eingabefeld offen ist
    if (ele.find(".input-overlay").length > 0) {
      return; // Wenn ja, nicht überschreiben
    }
    let html = "";
    if (ele.hasClass("div100")) {
      html = `<i onClick="showInput('${path}', ${val || 0})">${((val || 0) / 100).toFixed(2)}</i>`;
    } else if (ele.hasClass("lightbulb")) {
      html = `<i class="${val ? 'fa-solid' : 'fa-regular'} fa-lightbulb" onClick="sendData({${path}:${!val}});"></i>`
    } else if (ele.hasClass("digit2")) {
      html = val.toString().padStart(2, '0');
    } else if (ele.hasClass("digit4")) {
      html = val.toString().padStart(4, '0');
    } else {
      html = val;
    }
    ele.html(html);
  }
}

function showInput(path, val) {
  let ele = $(`[data-update='${path}']`);
  if (!ele) return;

  let inputHtml = `
    <div class="input-overlay">
      <input type="number" id="input-${path}" value="${(val / 100).toFixed(2)}">
      <button onClick="confirmInput('${path}')">OK</button>
      <button onClick="cancelInput('${path}', ${val})">Abbrechen</button>
    </div>
  `;
  ele.html(inputHtml);
}

function confirmInput(path) {
  let inputVal = parseFloat($(`#input-${path}`).val());
  let newValue = Math.round(inputVal * 100); // Umrechnung für dein Datenformat
  let ele = $(`[data-update='${path}']`);
  ele.find(".input-overlay").remove();
  sendData({ [path]: newValue });
}

function cancelInput(path, originalVal) {
  let ele = $(`[data-update='${path}']`);
  ele.find(".input-overlay").remove();
  setValue(path, originalVal);
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
  console.log(typeof myObj);
  if (myObj) change('', myObj);
}

function uploadString(filename, fileContent, cb) {
  // Remove leading slash (if present) from the filename
  if (filename.startsWith("/")) {
    filename = filename.substring(1); // Removes the first character (the slash)
  }

  // Create a Blob from the text content
  var strblob = new Blob([fileContent], { type: 'text/plain' });

  // Create FormData and append the file and other fields
  var formdata = new FormData();
  formdata.append("file", strblob, filename);
  formdata.append("field-1", "field-1-data"); // Example of another field

  // Perform the AJAX request
  $.ajax({
    url: "/upload",
    type: "POST",
    data: formdata,
    processData: false, // Prevents automatic processing of FormData
    contentType: false, // Prevents setting the Content-Type header (important for FormData)
    success: function (result) {
      console.log("Upload successful:", result); // Success message
      if (typeof cb === "function") {
        cb(); // Call the callback, if defined
      }
    },
    error: function (xhr, status, error) {
      // Detailed error handling
      console.log("Error during upload:", error);
      console.log("Status:", status);
      console.log("XHR Status:", xhr.status);
      console.log("XHR Response:", xhr.responseText);
    }
  });
}

function uploadFile(file, filename, cb) {
  // Remove leading slash (if present) from the filename
  if (filename.startsWith("/")) {
    filename = filename.substring(1); // Removes the first character (the slash)
  }

  if (!file) {
    console.log("No file selected.");
    return;
  }

  // Create FormData and append the file and other fields
  var formdata = new FormData();
  formdata.append("file", file, filename);
  formdata.append("field-1", "field-1-data"); // Example of another field

  // Perform the AJAX request
  $.ajax({
    url: "/upload",
    type: "POST",
    data: formdata,
    processData: false, // Prevents automatic processing of FormData
    contentType: false, // Prevents setting the Content-Type header (important for FormData)
    success: function (result) {
      console.log("Upload successful:"); // Success message
      if (typeof cb === "function") {
        cb(); // Call the callback, if defined
      }
    },
    error: function (xhr, status, error) {
      // Detailed error handling
      console.log("Error during upload:", error);
      console.log("Status:", status);
      console.log("XHR Status:", xhr.status);
      console.log("XHR Response:", xhr.responseText);
    }
  });
}

function getFileNameFromUrl(url) {
  // Extracts the filename from the URL
  return url.substring(url.lastIndexOf('/') + 1);
}

// Function to download a file
function downloadFile(fileUrl, fileName) {
  if (!fileName) {
    fileName = getFileNameFromUrl(fileUrl);
  }

  // Create an invisible <a> tag
  var a = document.createElement('a');
  a.href = '/download?filename=' + encodeURIComponent(fileUrl);  // Set the file URL
  a.download = fileName;  // Set the filename to be downloaded

  // Append the <a> tag to the DOM
  document.body.appendChild(a);

  // Simulate a click on the download link
  a.click();

  // Remove the <a> tag after the click to keep the DOM clean
  document.body.removeChild(a);
}

// Function to download a file
function downloadFileAsString(filename, cb) {
  // Construct the URL for the /download route with the filename as a query parameter
  var fileUrl = '/download?filename=' + encodeURIComponent(filename);

  // Load the file content via AJAX
  $.ajax({
    url: fileUrl, // The /download route
    method: 'GET',
    dataType: 'text', // Expect plain text content
    success: cb,
    error: function (xhr, status, error) {
      alert("Error loading file: " + error);  // Handle error
    }
  });
}

function deleteFile(filename, Callback) {
  cb = Callback;
  $.ajax({
    url: '/delete-file',           // The URL to be called
    type: 'GET',                   // HTTP method (GET)
    data: { filename: filename },  // The filename as a query parameter
    success: function (response) {
      // Success: The response from the server is processed here
      console.log("Successfully deleted: " + response);
      if (typeof cb === "function") {
        cb(); // Call the callback, if defined
      }
    },
    error: function (xhr, status, error) {
      // Error handling in case something goes wrong
      console.error("Error during file deletion: " + error);
      alert("Error deleting the file.");
    }
  });
}
