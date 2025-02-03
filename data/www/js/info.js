const description = {
  "uptime": "Total up time of ESP (maximum 47 days)",
  "temp": "ESP CPU temperature",
  "reason": "Reason last start",
  "heapsize": "Total heap memory available",
  "freeheap": "Free heap memory available",
  "maxallocheap": "Largest block of allocatable heap memory",
  "sketchsize": "Size of the current firmware",
  "freesketchspace": "Free space for a new firmware",
  "flashchipsize": "Total flash memory size",
  "flashchipmode": "Flash chip operating mode",
  "flashchipspeed": "Flash chip speed (Hz)",
  "spiffstotalbytes": "Total SPIFFS storage",
  "spiffsusedbytes": "Used SPIFFS storage",
  "chipcores": "Number of CPU cores",
  "chipmodel": "Chip model",
  "chiprevision": "Chip revision",
  "cpufreqmhz": "CPU frequency (MHz)",
  "cyclecount": "CPU cycles since reset",
  "efusemac": "Unique MAC address",
  "accesspointActive": "Wi-Fi access point active",
  "wifiaptime": "Wi-Fi access point uptime (miliseconds)",
  "lanactive": "LAN interface active",
  "lanhostname": "LAN hostname",
  "lanip": "LAN IP",
  "lanipv6": "LAN IP v6",
  "wifiactive": "Wi-Fi active",
  "wifimode": "Wi-Fi operating mode",
  "wifihostname": "Wi-Fi hostname",
  "wifiip": "Wi-Fi IP",
  "wifiipv6": "Wi-Fi IP v6",
  "freeheapsize": "free heap memory (freeRTOS)",
  "minimumeverfreeheapsize": "lowest recorded free heap memory (freeRTOS)",
  "ds18b20ids": "DS18B20 sensor ids found on the bus."
};

function foyesno(val) {
  return val ? "yes" : "no";
}
function fonumber(val) {
  const number = parseInt(val, 10);
  return number.toLocaleString('de-DE');
}

function formatMacAddress(mac) {
  return mac.replace(/(.{2})(?=.)/g, '$1:');
}

function fouptime(val) {
  return val.d.toString().padStart(2, '0') + 'd ' +
    val.h.toString().padStart(2, '0') + ':' +
    val.m.toString().padStart(2, '0') + ':' +
    val.s.toString().padStart(2, '0')
}


function formatWiFIMode(val) {
  const modes = {
    "0": "NULL",
    "1": "STA",
    "2": "AP",
    "3": "AP & STA",
    "4": "MAX"
  }
  return "WiFi mode " + modes[val] ? modes[val] : val;
}

function formatReason(val) {
  const modes = {
    "0": "Reset reason can not be determined",
    "1": "Reset due to power-on event",
    "2": "Reset by external pin (not applicable for ESP32)",
    "3": "Software reset via esp_restart",
    "4": "Software reset due to exception/panic",
    "5": "Reset (software or hardware) due to interrupt watchdog",
    "6": "Reset due to task watchdog",
    "7": "Reset due to other watchdogs",
    "8": "Reset after exiting deep sleep mode",
    "9": "Brownout reset (software or hardware)",
    "10": "Reset over SDIO"
  }
  return modes[val] ? modes[val] : val;
}


const infoFormats = {
  "uptime": fouptime,
  "heapsize": fonumber,
  "freeheap": fonumber,
  "maxallocheap": fonumber,
  "sketchsize": fonumber,
  "freesketchspace": fonumber,
  "flashchipsize": fonumber,
  "flashchipmode": null,
  "flashchipspeed": fonumber,
  "spiffstotalbytes": fonumber,
  "spiffsusedbytes": fonumber,
  "chipcores": fonumber,
  "chipmodel": null,
  "chiprevision": fonumber,
  "cpufreqmhz": fonumber,
  "cyclecount": fonumber,
  "efusemac": formatMacAddress,
  "accesspointActive": foyesno,
  "wifiaptime": fonumber,
  "lanactive": foyesno,
  "lanhostname": null,
  "wifiactive": foyesno,
  "wifimode": formatWiFIMode,
  "wifihostname": null,
  "freeheapsize": fonumber,
  "minimumeverfreeheapsize": fonumber,
  "ds18b20ids": null,
  "reason": formatReason
}

const GETINFO = "/sysinfo";

const INFOAREA = `
<div id="myArea" style="height:100%; max-width:45em; overflow:auto" class="list-group" >
<div id="infoTab" />
</div>
`;

const INFOTAB = `
<table id="setuptable" class="w3-table-all w3-hoverable w3-small"/>
`;

function infoBuild() {
  $("#mySpace").html(INFOAREA);
  $("#infoTab").html('<div id="loading">Loading files...</div>');
  $.getJSON(GETINFO, {_: new Date().getTime()}, function (data) {
    let tab = $(INFOTAB);
    tab.append($(`<tr />`)
    .append($(`<td colspan=2  style="vertical-align: middle; text-align: right"/>`).html(`<button type="button" onClick="infoBuild()" ><i class="fa fa-refresh" id="refresh"></i> Refresh</button>`))
  );

    Object.keys(data).forEach((key) => {
      let text = description[key] ? description[key] : key;
      let dat = infoFormats[key] ? infoFormats[key](data[key]) : data[key];
      tab.append($(`<tr/>`)
        .append($(`<td style="vertical-align: middle; text-align: right"/>`).text(text))
        .append($(`<td style="vertical-align: middle; text-align: right"/>`).text(dat))
      );

    })

    $("#infoTab").html(tab);
  }).fail(function (jqxhr, textStatus, error) {
    console.log("Fehler beim Laden der JSON-Daten", textStatus, error);
  });
}
