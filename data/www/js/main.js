var temperaturNames = [];
var outputNames = [];
var inputNames = [];

var namesok = false;

function getNames(cb) {
  $.getJSON("/count", {_: new Date().getTime()} , function (data) {
    for (let i = 0; i < data.data.tmp; i++) {
      temperaturNames[i] = `Measurement ${i + 1}`;
    }
    for (let i = 0; i < data.data.out; i++) {
      outputNames[i] = `Relays ${i + 1}`;
    }
    for (let i = 0; i < data.data.in; i++) {
      inputNames[i] = `Eingang ${i + 1}`;
    }

    if (typeof cb == 'function') {
      cb()
    }

  }).fail(function (jqxhr, textStatus, error) {
    console.log("Fehler beim Laden der JSON-Daten", textStatus, error);
    if (typeof cb == 'function') {
      cb()
    }
  });
}




function meineFunktion() {
  getNames(function () {
    let body = '<div style="height:100%; width:100%; overflow:auto"><table class="w3-table-all w3-hoverable"><tr><th> Bezeichnung </th><th> Wert </th></tr>';
    for (let i = 0; i < temperaturNames.length; i++) {
      body += `<tr><td data-update="name_tempholdingreg${i}">${temperaturNames[i]}</td><td style="text-align: right;" ><a data-update="tempholdingreg${i}" class="div100">&nbsp;</a> <i data-update="noreadtemp${i}" class="processind">&nbsp;</i></td></tr>`;
    }
    for (let i = 0; i < inputNames.length; i++) {
      body += `<tr><td data-update="name_inputintern${i}">${inputNames[i]}</td><td style="text-align: right;" data-update="inputintern${i}"> &nbsp; </td></tr>`;
    }
    for (let i = 0; i < outputNames.length; i++) {
      body += `<tr><td data-update="name_relay${i}">${outputNames[i]}</td><td style="text-align: right;" data-update="relays${i}" class="lightbulb"> &nbsp; </td></tr>`;
    }
    body += '</table></div>';
    $("#mySpace").html(body);
  });
}

function buildPage(anchor) {
  meineFunktion();
}


function getAnchor() {
  var currentUrl = document.URL,
    urlParts = currentUrl.split('#');

  return (urlParts.length > 1) ? urlParts[1] : null;
}

function updateBuild() {
  $("#mySpace").html(
    `<h1 class="w3-container w3-gray">OTA Update</h1>
    <form class="w3-container" method="POST" action="/update" enctype="multipart/form-data">
      <input class="w3-input" type="file" name="update">
      <input class="w3-input" type="submit" value="Upload">
    </form>
    <i data-update="update"> &nbsp; </i>`
  );
}

let Pages = {
  "": { "func": function () { } },
  "process": { "func": buildPageProcess },
  "files": { "func": buildPageFiles },
  "setup": { "func": buildPageSetup },
  "info": { "func": infoBuild },
  "update": { "func": updateBuild }
};

function doPage(e) {
  getNames(function () {
    var anchor = getAnchor();
    console.log(anchor);
    $("#mySpace").empty();
    $("#myAddon").empty();
    $("#myExtension").html(`&nbsp;`);
    if (Pages[anchor] && Pages[anchor].func) {
      Pages[anchor].func();
    } else {
      switch (anchor) {
        case "": {
          break;
        }
        case "process": {
          buildPageProcess();
          break;
        }
        case "files": {
          buildPageFiles();
          break;
        }
        default:
          buildPage(anchor);
      }
    }
    getReadings();
  })
}

