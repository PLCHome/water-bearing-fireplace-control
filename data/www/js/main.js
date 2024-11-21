var temperaturNames = [];
for (let i = 0; i < 24; i++) {
  temperaturNames[i] = `Measurement ${i + 1}`;
}
var outputNames = [];
for (let i = 0; i < 13; i++) {
  outputNames[i] = `Relays ${i + 1}`;
}
var inputNames = [];
for (let i = 0; i < 8; i++) {
  inputNames[i] = `Eingang ${i + 1}`;
}

function meineFunktion() {
  let body = '<div style="height:100%; width:100%; overflow:auto"><table class="w3-table-all w3-hoverable"><tr><th> Bezeichnung </th><th> Wert </th></tr>';
  for (let i = 0; i < 24; i++) {
    body += `<tr><td id="name_tempholdingreg${i}">${temperaturNames[i]}</td><td style="text-align: right;" id="tempholdingreg${i}" class="div100"> &nbsp; </td></tr>`;
  }
  for (let i = 0; i < 8; i++) {
    body += `<tr><td id="name_inputintern${i}">${inputNames[i]}</td><td style="text-align: right;" id="inputintern${i}"> &nbsp; </td></tr>`;
  }
  for (let i = 0; i < 13; i++) {
    body += `<tr><td id="name_relay${i}">${outputNames[i]}</td><td style="text-align: right;" id="relays${i}" class="lightbulb"> &nbsp; </td></tr>`;
  }
  body += '</table></div>';
  $("#mySpace").html(body);
}

function buildPage(anchor) {
  meineFunktion();
}


function getAnchor() {
  var currentUrl = document.URL,
    urlParts = currentUrl.split('#');

  return (urlParts.length > 1) ? urlParts[1] : null;
}

let Pages={
  "": {"func": function (){}},
  "process": {"func": buildPageProcess},
  "files" : {"func": buildPageFiles},
  "setup" : {"func": buildPageSetup}
};

function doPage(e) {
  var anchor = getAnchor();
  console.log(anchor);
  $("#mySpace").empty();
  $("#myAddon").empty();
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
}

$(window).on('hashchange', doPage);
$(window).on('load', doPage);
