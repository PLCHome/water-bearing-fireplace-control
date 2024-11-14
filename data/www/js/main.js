var temperaturNames = [];
for (let i = 0; i < 24; i++) {
  temperaturNames[i] = `Measurement ${i + 1}`;
}
var outputNames = [];
for (let i = 0; i < 12; i++) {
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
  for (let i = 0; i < 12; i++) {
    body += `<tr><td id="name_relay${i}">${outputNames[i]}</td><td style="text-align: right;" id="relay${i}" class="lightbulb"> &nbsp; </td></tr>`;
  }
  body += '</table></div>';
  $("#mySpace").html(body);

  // $.getJSON("temperatures.json", function (data) {
  //   //console.log(data);
  //   if (data.name && Array.isArray(data.name) &&
  //     data.value && Array.isArray(data.value)) {
  //     $.each(data.name, function (i, item) {
  //       $("#name"+i).text(item);
  //       $("#value"+i).text(((data.value[i] || 0) / 100).toFixed(2));
  //     })
  //   }
  // });
}

function buildPage(anchor) {
  meineFunktion();
}

const fileRow = `<tr>
<td class="path">&nbsp;</td>
<td class="type">&nbsp;</td>
<td class="size">&nbsp;</td>
<td class="colbtn">
<a class="filebtn upload" ><i class="fa-solid fa-upload" aria-hidden="true"></i></a>
<a class="filebtn download" ><i class="fa-solid fa-download" aria-hidden="true"></i></a>
<a class="filebtn delete" ><i class="fa-solid fa-trash-arrow-up" aria-hidden="true"></i></a>
</td>
</tr>
`;
const fileHead = `<tr>
<th>file</th>
<th>type</th>
<th class="size">size</th>
<th>&nbsp;</th>
</tr>
`;
//<label for="fileInput">selecht file</label
const filetab=`<div id="loading">Lade Dateien...</div>
<div id="myArea" style="height:100%; width:100%; overflow:auto"" class="list-group" >
<table id="fileupload" class="w3-table-all w3-hoverable">
<tr>
  <td >
    <input style="width:100%" type="file" id="fileInput"/> 
  </td>
  <td style="width:5em" >
    <label for="filename">filename</label>
  </td>
  <td >
    <input style="width:100%" type="text" id="filename" />
  </td>
  <td class="colbtn">
  <a class="filebtn upload" ><i class="fa-solid fa-upload" aria-hidden="true"></i></a>
  </td>
</tr>
</table>
<table id="filetable" class="w3-table-all w3-hoverable"/>
</div>`;

const GETFILENAMES = `/list?dir=/`;
function buildPageFiles() {
  $("#mySpace").html(filetab);
  $("#filetable").append(fileHead);
  $("#fileInput").on('change',function () {
    $("#filename").val(this.files[0].name);
  });
  $(".filebtn")
  .addClass('w3-button w3-hover-black w3-round-large')
  .css({ "padding-top": "0px", "padding-bottom": "0px", "padding-right": "4px", "padding-left": "4px" });

  // Fehlerbehandlung für Netzwerkprobleme, Zeitüberschreitungen und andere Anomalien
  $.ajax({
    url: GETFILENAMES,
    dataType: 'json',
    timeout: 5000, // Timeout nach 5 Sekunden
    success: function (data) {
      // Ladeanzeige entfernen, wenn die Daten erfolgreich geladen wurden
      $("#loading").remove();

      // Prüfen, ob die Daten ein Array sind
      if (Array.isArray(data)) {
        if (data.length === 0) {
          // Keine Dateien gefunden
          $("#filetable").append('<tr><td colspan="4">Keine Dateien gefunden.</td></tr>');
        } else {
          // Wenn Daten vorhanden sind, füge sie der Tabelle hinzu
          for (var i = 0; i < data.length; i++) {
            let row = $(fileRow);
            keys = Object.keys(data[i]);
            for (var k = 0; k < keys.length; k++) {
              row.find(`.${keys[k]}`).text(data[i][keys[k]]);
            }
            row.data("row", i);
            row.find('.download')
              .data("path", data[i].path)
              .on('click', function () {
                // Hole die URL der Datei aus dem data-Attribut
                var fileUrl = $(this).data('path');
                // Prüfen, ob die URL vorhanden ist
                if (fileUrl) {
                  // Ruf die downloadFile-Funktion auf
                  downloadFile(fileUrl);
                } else {
                  // Fehlermeldung, wenn keine URL vorhanden ist
                  console.error("Fehler: Keine gültige Datei-URL.");
                }
              });

            row.find('.delete')
              .data("path", data[i].path)
              .on('click', function () {
                // Hole die URL der Datei aus dem data-Attribut
                var fileUrl = $(this).data('path');
                // Prüfen, ob die URL vorhanden ist
                if (fileUrl) {
                  // Ruf die downloadFile-Funktion auf
                  deleteFile(fileUrl);
                } else {
                  // Fehlermeldung, wenn keine URL vorhanden ist
                  console.error("Fehler: Keine gültige Datei-URL.");
                }
              });
            $("#filetable").append(row);
          }
        }

        // Styling
        $(".colbtn")
          .css({ "width": "7em" });
        $(".size")
          .css({ "text-align": "right" });
        $(".filebtn")
          .addClass('w3-button w3-hover-black w3-round-large')
          .css({ "padding-top": "0px", "padding-bottom": "0px", "padding-right": "4px", "padding-left": "4px" });
      } else {
        // Fehler: Die Antwort ist kein Array
        $("#filetable").append('<tr><td colspan="4">Fehler: Ungültige Datenstruktur erhalten.</td></tr>');
        console.error("Die geladenen Daten sind kein Array.");
      }
    },
    error: function (jqxhr, textStatus, error) {
      // Ladeanzeige entfernen, Fehlernachricht anzeigen
      $("#loading").remove();

      if (textStatus === "timeout") {
        // Timeout-Fehler
        $("#filetable").append('<tr><td colspan="4">Fehler: Zeitüberschreitung beim Laden der Daten.</td></tr>');
        console.error("Fehler: Zeitüberschreitung beim Laden der Daten.");
      } else {
        // Andere Fehler (z.B. Netzwerkprobleme)
        $("#filetable").append(`<tr><td colspan="4">Fehler: ${textStatus} - ${error}</td></tr>`);
        console.error(`Fehler beim Laden der JSON-Daten: ${textStatus}, ${error}`);
      }
    }
  });
}

function getAnchor() {
  var currentUrl = document.URL,
    urlParts = currentUrl.split('#');

  return (urlParts.length > 1) ? urlParts[1] : null;
}


function doPage(e) {
  var anchor = getAnchor();
  console.log(anchor);
  $("#mySpace").empty();
  $("#myAddon").empty();
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
  getReadings();
}

$(window).on('hashchange', doPage);
$(window).on('load', doPage);
