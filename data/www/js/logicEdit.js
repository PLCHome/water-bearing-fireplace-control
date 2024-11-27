

const PT_TEMP = 0;
const PT_TEMPT = 1;
const PT_LOGIC = 2;
const PT_OUT = 3;
const PT_MIXER = 4;

const POINTSFILENAME = "/config/points.json";

var sortable;

const LOGCSELECT = {
  "0": "AND",
  "1": "OR",
  "2": "AND NOT",
  "3": "OR NOT"
};
const INVERSSELECT = {
  "0": "SET",
  "1": "SET NOT"
};

const BARBUTTONS = `
<a data-type="${PT_TEMP}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> temperature value</a>
<a data-type="${PT_TEMPT}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> temperature to another</a>
<a data-type="${PT_LOGIC}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> logic</a>
<a data-type="${PT_OUT}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> output</a>
<a data-type="${PT_MIXER}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> mixer</a>
<a class="mySaveButton"><i class="fa-regular fa-floppy-disk"></i> save</a>`;

const SORTAREA = '<div id="mySortArea" style="height:100%; width:100%; overflow:auto"" class="list-group" ></div>';


const BARTEMPERATURE = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name-%id%" class="name name_input""/>
      <label for="name-%id%">name</label>
    </div>
    <div class="edit-item">
      <select id="tpos-%id%" class="tpos tempselect" ></select>
      <label for="tpos-%id%">temperature measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-sitem">
      <input id="toff-%id%" class="toff 2dec_input"/>
      <label for="toff-%id%">temperature off</label>
    </div>
    <div class="edit-sitem">
      <input id="ton-%id%" class="ton 2dec_input"/>
      <label for="ton-%id%">temperature on</label>
    </div>
  </div>
</div>
</form>`;

const BAR2TEMPERATURE = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name-%id%" class="name name_input""/>
      <label for="name-%id%">name</label>
    </div>
    <div class="edit-item">
      <select id="tpos-%id%" class="tpos tempselect" ></select>
      <label for="tpos-%id%">temperature measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <select id="tpos2-%id%" class="tpos2 tempselect" ></select>
      <label for="tpos2-%id%">hysteresis measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-sitem">
      <input id="t2minus-%id%" class="t2minus 2dec_input"/>
      <label for="t2minus-%id%">hysteresis off</label>
    </div>
    <div class="edit-sitem">
      <input id="t2plus-%id%" class="t2plus 2dec_input"/>
      <label for="t2plus-%id%">hysteresis on</label>
    </div>
  </div>
</div>
</form>`;

const BARLOGIC = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name-%id%" class="name name_input""/>
      <label for="name-%id%">name</label>
    </div>
    <div class="edit-item">
      <select id="ida-%id%" class="ida processelect" ></select>
      <label for="ida-%id%">process a <a class="value">&nbsp;</a></label>
    </div>
    <div class="edit-sitem">
      <select id="logic-%id%" class="logic logcselect" ></select>
      <label for="logic-%id%">logic</label>
    </div>
    <div class="edit-item">
      <select id="idb-%id%" class="idb processelect" ></select>
      <label for="idb-%id%">process b <a class="value">&nbsp;</a></label>
    </div>
  </div>
</div>
</form>`;

const BAROUTPUT = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name-%id%" class="name name_input""/>
      <label for="name-%id%">name</label>
    </div>
    <div class="edit-item">
      <select id="ida-%id%" class="ida processelect" ></select>
      <label for="ida-%id%">process <a class="value">&nbsp;</a></label>
    </div>
    <div class="edit-sitem">
      <select id="op-%id%" class="op inversselect" ></select>
      <label for="op-%id%">logic</label>
    </div>
    <div class="edit-item">
      <select id="opos-%id%" class="opos outselect" ></select>
      <label for="opos-%id%">output <a class="value lightbulb">&nbsp;</a></label>
    </div>
  </div>
</div>
</form>`;

const BARMIXER = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name-%id%" class="name name_input""/>
      <label for="name-%id%">name</label>
    </div>
    <div class="edit-item">
      <select id="idon-%id%" class="idon processelect" ></select>
      <label for="idon-%id%">process on<a class="value">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <select id="tpos-%id%" class="tpos tempselect" ></select>
      <label for="tpos-%id%">temperature measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-sitem">
      <input id="chkint-%id%" class="chkint uint_input"/>
      <label for="chkint-%id%">inetrvall (s)</label>
    </div>
    <div class="edit-sitem">
      <input id="imptime-%id%" class="imptime uint_input"/>
      <label for="imptime-%id%">impulse time (s)</label>
    </div>
    <div class="edit-sitem">
      <input id="impmax-%id%" class="impmax uint_input"/>
      <label for="impmax-%id%">impulse to open</label>
    </div>
    <div class="edit-sitem">
      <input id="ttemp-%id%" class="ttemp 2dec_input"/>
      <label for="ttemp-%id%">target temperature</label>
    </div>
    <div class="edit-sitem">
      <input id="hyst-%id%" class="hyst 2dec_input"/>
      <label for="hyst-%id%">hysterresis</label>
    </div>
    <div class="edit-sitem">
      <input id="delta-%id%" class="delta 2dec_input"/>
      <label for="delta-%id%">max delta</label>
    </div>
    <div class="edit-item">
      <select id="opclose-%id%" class="opclose outselect" ></select>
      <label for="opclose-%id%">output close <a class="value lightbulb">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <select id="opopen-%id%" class="opopen outselect" ></select>
      <label for="opopen-%id%">output open <a class="value lightbulb">&nbsp;</a></label>
    </div>
  </div>
</div>
</form>`;

function setAtt() {
  $(".2dec_input")
    .off("change")
    .on("change", function () {
      var curr_val = parseFloat($(this).val());
      if (curr_val > 99.99) { curr_val = 99.99 }
      else
        if (curr_val < -99.99) { curr_val = -99.99 }
      $(this).val(curr_val.toFixed(2));
    })
    .attr("type", "number")
    .attr("step", "0.01")
    .attr("min", "-99.99")
    .attr("max", "99.99")
    .attr("autocomplete", "off")
    .addClass("w3-input")
    .css({ "text-align": "right" });
  $(".uint_input")
    .off("change")
    .on("change", function () {
      var curr_val = parseFloat($(this).val());
      if (curr_val > 99999) { curr_val = 99999 }
      else
        if (curr_val < 0) { curr_val = 0 }
      $(this).val(curr_val.toFixed(0));
    })
    .attr("type", "number")
    .attr("step", "1")
    .attr("min", "0")
    .attr("max", "99999")
    .attr("autocomplete", "off")
    .addClass("w3-input")
    .css({ "text-align": "right" });
  $(".name_input")
    .attr("type", "text")
    .attr("autocomplete", "off")
    .addClass("w3-input")
    .off("change")
    .on("change", function () {
      makeProcessSelect();
    });
  $(".tempselect")
    .addClass("w3-select")
    .off("change")
    .on("change", function () {
      var sel = $(this).find('option:selected').val();
      if (sel != "") {
        //data-update
        //$(this).parent().find('.value').prop("id", "tempholdingreg" + sel).html("");
        $(this).parent().find('.value').attr("data-update", "tempholdingreg" + sel).html("");
        getReadings();
      }
    });
  $("label")
    .css({ "font-size": "0.8em" });
  $(".processelect")
    .addClass("w3-select");
  $(".logcselect")
    .addClass("w3-select");
  $(".outselect")
    .addClass("w3-select")
    .off("change")
    .on("change", function () {
      var sel = $(this).find('option:selected').val();
      if (sel != "") {
        $(this).parent().find('.value').attr("data-update", "relays" + sel).html("");
        getReadings();
      }
    });

    //
  $(".inversselect")
    .addClass("w3-select");
  $(".list-group-item")
    .addClass("w3-light-grey")
    .addClass("w3-hover-blue-gray")
    .addClass("w3-row")
    .addClass("w3-border w3-round");
  $(".edit-bar")
    .addClass("w3-bar");
  $(".edit-cont")
    .addClass("w3-rest")
    .addClass("w3-container");
  $(".edit-item")
    .addClass("w3-bar-item")
    .css({ "width": "15em" });
  $(".edit-sitem")
    .addClass("w3-bar-item")
    .css({ "min-width": "7em" });
  $(".btn-item")
    .addClass("w3-bar-item")
  $(".process-del")
    .addClass("w3-button")
    .addClass("w3-right")
    .addClass("w3-col")
    .addClass("w3-container")
    .css({ "width": "1em" })
    .off("click")
    .on("click", function () {
      this.closest('.list-group-item').remove();
      makeProcessSelect();
    });
  $(".process-mov")
    .addClass("w3-button")
    .addClass("w3-left")
    .addClass("w3-container")
    .addClass("w3-col")
    .css({ "width": "1em" });
  $(".myAddButton")
    .addClass("w3-button")
    .off("click")
    .on("click", function () {
      let type = $(this).attr("data-type");
      createNewRow(type)
    });
  $(".mySaveButton")
    .addClass("w3-button")
    .off("click")
    .on("click", function () {
      saveJSON();
    });
  makeProcessSelect();
  updateSelect("outselect", outputNames);
  updateSelect("tempselect", temperaturNames);
  makeLogicSelect();
  makeInversSelect();

}

function makeLogicSelect() {
  updateSelect("logcselect", LOGCSELECT);
}

function makeInversSelect() {
  updateSelect("inversselect", INVERSSELECT);
}

function makeProcessSelect() {
  var order = sortable.toArray();
  var vals = {};
  for (let i = 0; i < order.length; i++) {
    vals[order[i]] = $(`[data-id='${order[i]}'] .name`).val();
  }
  updateSelect("processelect", vals)
}

function updateSelect(calss, vals) {
  $(`.${calss}`).each(function (index) {
    var sel = $(this).find('option:selected').val();
    var html = `<option value="" disabled selected>choose</option>`;
    var key = Object.keys(vals);
    for (let i = 0; i < key.length; i++) {
      html += `<option value="${key[i]}">${vals[i]}</option>`;
    }
    $(this).html(html);
    $(this).find(`[value="${sel}"]`).attr('selected', true);
  });
}

function createNewRow(type) {
  var val = { "type": parseInt(type, 10), "id": 0 };
  var order = sortable.toArray();
  while (order.includes(String(val.id))) val.id++;
  switch (val.type) {
    case PT_TEMP: val.ton = 2200; val.toff = 2300; break;
    case PT_TEMPT: val.t2plus = 100; val.t2minus = -100; break;
    case PT_LOGIC: break;
    case PT_OUT: break;
    case PT_MIXER: val.chkint = 10; val.imptime = 2; val.ttemp = 30; val.impmax = 30; val.hyst = 100;  val.delta = 30; break;
  }
  val.name = `Process ${val.id + 1}`;
  createRow(val)
}

function createRow(val) {
  body = $(`<div class="list-group-item"/>`);
  body.attr('data-id',val.id);
  body.attr('data-type',val.type);
  switch (parseInt(val.type, 10)) {
    case PT_TEMP: body.html(BARTEMPERATURE.replaceAll(`%id%`, `${val.id}`)); break;
    case PT_TEMPT: body.html(BAR2TEMPERATURE.replaceAll(`%id%`, `${val.id}`)); break;
    case PT_LOGIC: body.html(BARLOGIC.replaceAll(`%id%`, `${val.id}`)); break;
    case PT_OUT: body.html(BAROUTPUT.replaceAll(`%id%`, `${val.id}`)); break;
    case PT_MIXER: body.html(BARMIXER.replaceAll(`%id%`, `${val.id}`)); break;
  }
  $("#mySortArea").append(body);
  setAtt();
  $(`[data-id='${val.id}'] .name`).val(val.name);
  $(`[data-id='${val.id}'] .name`).trigger('change');
  $(`[data-id='${val.id}'] .ton`).val((parseFloat(val.ton)/100).toFixed(2));
  $(`[data-id='${val.id}'] .toff`).val((parseFloat(val.toff)/100).toFixed(2));
  $(`[data-id='${val.id}'] .t2plus`).val((parseFloat(val.t2plus)/100).toFixed(2));
  $(`[data-id='${val.id}'] .t2minus`).val((parseFloat(val.t2minus)/100).toFixed(2));
  $(`[data-id='${val.id}'] .hyst`).val((parseFloat(val.hyst)/100).toFixed(2));
  $(`[data-id='${val.id}'] .delta`).val((parseFloat(val.delta)/100).toFixed(2));
  $(`[data-id='${val.id}'] .chkint`).val(val.chkint);
  $(`[data-id='${val.id}'] .imptime`).val(val.imptime);
  $(`[data-id='${val.id}'] .impmax`).val(val.impmax);
  $(`[data-id='${val.id}'] .ttemp`).val((parseFloat(val.ttemp)/100).toFixed(2));
    
  function setOption(id) {
    var ele = $(`[data-id='${val.id}'] .${id} option`);
    ele.filter(`[value=""]`)
      .attr('selected', true)
    ele.filter(`[value="${val[id]}"]`)
      .attr('selected', true)
    ele.trigger('change');
  }
  setOption('tpos')
  setOption('tpos2')
  setOption('logic')
  setOption('ida')
  setOption('idb')
  setOption('idon')
  setOption('opos')
  setOption('op')
  setOption('opclose')
  setOption('opopen')



}

function buildPageProcess() {
  $("#mySpace").html(SORTAREA);
  sortable = new Sortable(mySortArea, {
    multiDrag: true, // Enable multi-drag
    selectedClass: 'w3-gray', // The class applied to the selected items
    fallbackTolerance: 3, // So that we can select items on mobile
    animation: 150
  });
  $("#myAddon").append(BARBUTTONS);
  setAtt();
  loadJSON();
}

function loadJSON() {
  
  $.getJSON(POINTSFILENAME, function (data) {
    // Prüfen, ob die Daten ein Array sind
    if (Array.isArray(data)) {
      // Jedes Element im Array an die Callback-Funktion übergeben
      for (var i = 0; i < data.length; i++) {
        createRow(data[i]);
      }
    } else {
      console.log("Die geladenen Daten sind kein Array.");
    }
  }).fail(function (jqxhr, textStatus, error) {
    console.log("Fehler beim Laden der JSON-Daten:", textStatus, error);
  });
}



function saveJSON() {
  var json = buildJSON();
  console.log(json);
  uploadString(POINTSFILENAME, json, function (){
    sendData('reloadPoints');
  });
}

function buildJSON() {
  var order = sortable.toArray();
  var vals = [];
  for (let i = 0; i < order.length; i++) {
    let ele = $(`[data-id='${order[i]}']`);
    vals[i] = {
      "id": parseFloat(order[i]),
      "type": parseFloat(ele.attr('data-type'))
    }

    function setValues(name, func) {
      ele.find(`.${name}`).each(function () {
        vals[i][name] = $(this).val()
        vals[i][name] = func(vals[i][name])
      });
    }

    function valString(val){
      return val.toString();
    }
    function valInt(val){
      return parseInt(val,10);
    }
    function valInt100(val){
      return parseFloat(val,10)*100;
    }

    setValues('name', valString)
    setValues('ton', valInt100)
    setValues('toff', valInt100)
    setValues('t2plus', valInt100)
    setValues('t2minus', valInt100)
    setValues('tpos', valInt)
    setValues('tpos2', valInt)
    setValues('logic', valInt)
    setValues('ida', valInt)
    setValues('idb', valInt)
    setValues('opos', valInt)
    setValues('op', valInt)
    setValues('idon', valInt)
    setValues('chkint', valInt)
    setValues('imptime', valInt)
    setValues('impmax', valInt)
    setValues('ttemp', valInt100)
    setValues('hyst', valInt100)
    setValues('delta', valInt100)
    setValues('opclose', valInt)
    setValues('opopen', valInt)
    
  }
  return JSON.stringify(vals);
}


