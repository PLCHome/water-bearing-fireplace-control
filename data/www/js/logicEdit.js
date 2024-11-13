

const PT_TEMP = 0;
const PT_TEMPT = 1;
const PT_LOGIC = 2;
const PT_OUT = 3;


var sortable;

const barTemperature = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name" class="name_input""/>
      <label for="name">name</label>
    </div>
    <div class="edit-item">
      <select id="tpos" class="tempselect" ></select>
      <label for="tpos">temperature measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <input id="ton" class="2dec_input"/>
      <label for="ton">temperature on</label>
    </div>
    <div class="edit-item">
      <input id="toff" class="2dec_input"/>
      <label for="toff">temperature off</label>
    </div>
  </div>
</div>
</form>`;

const bar2Temperature = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name" class="name_input""/>
      <label for="name">name</label>
    </div>
    <div class="edit-item">
      <select id="tpos" class="tempselect" ></select>
      <label for="tpos">temperature measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <select id="tpos2"" class="tempselect" ></select>
      <label for="tpos2"">hysteresis measurement <a class="value div100">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <input id="t2plus" class="2dec_input"/>
      <label for="t2plus">hysteresis on</label>
    </div>
    <div class="edit-item">
      <input id="t2minus" class="2dec_input"/>
      <label for="t2minus">hysteresis off</label>
    </div>
  </div>
</div>
</form>`;

const barLogic = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name" class="name_input""/>
      <label for="name">name</label>
    </div>
    <div class="edit-item">
      <select id="ida" class="processelect" ></select>
      <label for="ida">process a <a class="value">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <select id="logic" class="logcselect" ></select>
      <label for="logic">logic</label>
    </div>
    <div class="edit-item">
      <select id="idb" class="processelect" ></select>
      <label for="idb">process b <a class="value">&nbsp;</a></label>
    </div>
  </div>
</div>
</form>`;

const barOutput = `<form>
<div class="edit-btn process-mov">
  <i class="fa-solid fa-up-down"></i>
</div>
<div class="edit-btn process-del">
  <i class="fa-solid fa-trash-can"></i>
</div>
<div class="edit-cont">
  <div class="edit-bar">
    <div class="edit-item">
      <input id="name" class="name_input""/>
      <label for="name">name</label>
    </div>
    <div class="edit-item">
      <select id="ida" class="processelect" ></select>
      <label for="ida">process <a class="value">&nbsp;</a></label>
    </div>
    <div class="edit-item">
      <select id="opos" class="inversselect" ></select>
      <label for="opos">logic</label>
    </div>
    <div class="edit-item">
      <select id="op" class="outselect" ></select>
      <label for="op">output <a class="value">&nbsp;</a></label>
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
    .addClass("w3-input");
  $(".name_input")
    .attr("type", "text")
    .attr("autocomplete", "off")
    .addClass("w3-input")
    .off("change")
    .on("change", function () {
      makeProcessSelect();
    });
  $(".tempselect")
    .addClass("w3-select");
  $("label")
    .css({ "font-size": "0.8em" });
  $(".processelect")
    .addClass("w3-select");
  $(".logcselect")
    .addClass("w3-select");
  $(".outselect")
    .addClass("w3-select");
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
      let type = $(this).attr("type");
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

  $(".tempselect")
  .off("change")
  .on("change", function () {
    var sel = $(this).find('option:selected').val();
    if (sel!="") {
      $(this).parent().find('.value').prop("id", "tempholdingreg"+sel).html("");
      getReadings();
    }
  });
}

function makeLogicSelect() {
  updateSelect("logcselect", {
    "0": "AND",
    "1": "OR",
    "2": "AND NOT",
    "3": "OR NOT"
  });
}

function makeInversSelect() {
  updateSelect("inversselect", {
    "0": "SET",
    "1": "SET NOT"
  });
}

function makeProcessSelect() {
  var order = sortable.toArray();
  var vals = {};
  for (let i = 0; i < order.length; i++) {
    vals[order[i]] = $(`[data-id='${order[i]}'] #name`).val();
  }
  updateSelect("processelect", vals)
}

function updateSelect(calss, vals) {
  $(`.${calss}`).each(function (index) {
    var sel = $(this).find('option:selected').val();
    var html = `<option value="" disabled selected>Choose your option</option>`;
    var key = Object.keys(vals);
    for (let i = 0; i < key.length; i++) {
      html += `<option value="${key[i]}">${vals[i]}</option>`;
    }
    $(this).html(html);
    $(this).find(`[value="${sel}"]`).attr('selected', true);
  });
}

function createNewRow(type) {
  var val = {};
  var order = sortable.toArray();
  let row = 0;
  while (order.includes(String(row)) || row > 100) row++;
  switch (parseInt(type, 10)) {
    case PT_TEMP: val = { ton: 22, toff: 23 }; break;
    case PT_TEMPT: val = { t2plus: 1, t2minus: -1, }; break;
    case PT_LOGIC: val = {}; break;
    case PT_OUT: val = {}; break;
  }
  val.name = `Process ${row + 1}`;
  createRow(type, row, val)
}

function createRow(type, row, val) {
  body = `<div data-id="${row}" type="${type}" class="list-group-item">`;
  switch (parseInt(type, 10)) {
    case PT_TEMP: body += barTemperature; break;
    case PT_TEMPT: body += bar2Temperature; break;
    case PT_LOGIC: body += barLogic; break;
    case PT_OUT: body += barOutput; break;
  }
  body += `</div>`;
  $("#mySortArea").append(body);
  setAtt();
  $(`[data-id='${row}'] #name`).val(val.name);
  $(`[data-id='${row}'] #name`).trigger('change');
  $(`[data-id='${row}'] #ton`).val(parseFloat(val.ton).toFixed(2));
  $(`[data-id='${row}'] #toff`).val(parseFloat(val.toff).toFixed(2));
  $(`[data-id='${row}'] #t2plus`).val(parseFloat(val.t2plus).toFixed(2));
  $(`[data-id='${row}'] #t2minus`).val(parseFloat(val.t2minus).toFixed(2));
  function setOption(id) {
    $(`[data-id='${row}'] #${id} option`)
      .filter(`[value=""]`)
      .attr('selected', true)
    $(`[data-id='${row}'] #${id} option`)
      .filter(`[value="${val[id]}"]`)
      .attr('selected', true)
  }
  setOption('tpos')
  setOption('tpos2')
  setOption('logic')
  setOption('ida')
  setOption('idb')
  setOption('opos')
  setOption('op')
}

function buildPageProcess() {
  let body = '<div id="mySortArea" style="height:100%; width:100%; overflow:auto"" class="list-group" ></div>';
  $("#mySpace").html(body);
  sortable = new Sortable(mySortArea, {
    multiDrag: true, // Enable multi-drag
    selectedClass: 'w3-gray', // The class applied to the selected items
    fallbackTolerance: 3, // So that we can select items on mobile
    animation: 150
  });
  body = `<a type="${PT_TEMP}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> temperature value</a>`;
  body += `<a type="${PT_TEMPT}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> temperature to another</a>`;
  body += `<a type="${PT_LOGIC}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> logic</a>`;
  body += `<a type="${PT_OUT}" class="myAddButton"><i class="fa-solid fa-file-circle-plus"></i> output</a>`;
  body += `<a class="mySaveButton"><i class="fa-regular fa-floppy-disk"></i> save</a>`;
  $("#myAddon").append(body);
  setAtt();
  loadJSON();
}

function loadJSON() {
  let json = `[{"id":0,"type":0,"name":"Test 1","ton":22,"toff":23,"tpos":0},
  {"id":1,"type":1,"name":"Test 2","t2plus":1,"t2minus":-1,"tpos":1,"tpos2":2},
  {"id":2,"type":2,"name":"Test 3","logic":3,"ida":0,"idb":1},
  {"id":3,"type":3,"name":"Test 4","ida":2,"opos":1,"op":0}]`


  var vals = JSON.parse(json);
  for (var i = 0; i < vals.length; i++) {
    createRow(vals[i].type, i, vals[i])
  }
}

function saveJSON() {
  console.log(buildJSON());
}

function buildJSON() {
  var order = sortable.toArray();
  var vals = [];
  for (let i = 0; i < order.length; i++) {
    let ele = $(`[data-id='${order[i]}']`);
    vals[i] = {
      "id": parseFloat(order[i]),
      "type": parseFloat(ele.attr('type'))
    }

    function setValues(name, string) {
      ele.find(`#${name}`).each(function () {
        vals[i][name] = $(this).val()
        if (!string) {
          vals[i][name] = parseFloat(vals[i][name])
        }
      });
    }
    setValues('name', true)
    setValues('ton', false)
    setValues('toff', false)
    setValues('t2plus', false)
    setValues('t2minus', false)
    setValues('tpos', false)
    setValues('tpos2', false)
    setValues('logic', false)
    setValues('ida', false)
    setValues('idb', false)
    setValues('opos', false)
    setValues('op', false)
  }
  return JSON.stringify(vals);
}


