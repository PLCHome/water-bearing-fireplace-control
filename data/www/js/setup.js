
// URL to get the list of file names from the server
const GETSETUPPARA = `/config/setupPara.json`;
const GETSETUP = `/config/setup.json`;


const SETUPAREA = `
<div id="myArea" style="height:100%; max-width:55em; overflow:auto" class="list-group" >
<div id="setupTab" />
</div>
`;

const SETUPTAB = `
<table id="setuptable" class="w3-table-all w3-hoverable w3-small"/>
`;

const SETUPBARBUTTONS = `
<a class="mySetupSaveButton"><i class="fa-regular fa-floppy-disk"></i> save</a>`;


function buildParas(paras, key, dest) {
    const paraskeys = Object.keys(paras);
    for (let p = 0; p < paraskeys.length; p++) {
        const paraskey = paraskeys[p];
        const para = paras[paraskey];
        let row = $(`<tr><td style="vertical-align: middle; text-align: right; width: 50%" >${para.text}</td></tr>`)
        let edt = `<p>type ${para.type} is unknown!</p>`
        switch (para.type) {
            case "boolean": edt = $(`<input id="${key}.${paraskey}" data-key="${paraskey}" data-type="${para.type}" type="checkbox" class="w3-check configedit"/>`);
                if (typeof para.default !== 'undefined')
                    edt.prop("checked", para.default);
                edt.trigger('change');
                break;
            case "password": edt = $(`<input id="${key}.${paraskey}" data-key="${paraskey}" data-type="${para.type}" type="password" class="w3-input configedit"/>`);
                if (typeof para.default !== 'undefined')
                    edt.val(para.default);
                edt.trigger('change');
                edt.css({ "display": "unset" })
                .css({ "width": "90%" })
                showPw =$(`<button type="button"><i class="fa fa-eye" id="toggleIcon"></i></button>`);
                showPw.css({ "width": "3em" })
                showPw.css({ "float": "right" })
                showPw.data({ "field": `${key}.${paraskey}` })
                showPw.on('click',function (){
                    let field = $(this).data("field");
                    let pw = getByID(field);
                    if (pw.attr('type') === "password") {
                        pw.attr('type',"text");
                        $(this).find(`i`)
                        .removeClass("fa-eye")
                        .addClass("fa-eye-slash");
                    } else {
                        pw.attr('type',"password");
                        $(this).find(`i`)
                        .removeClass("fa-eye-slash")
                        .addClass("fa-eye");
                    }
                });
                ed = edt;
                edt = $(`<div/>`)
                edt.append(showPw);
                edt.append(ed);
                break;
            case "string": edt = $(`<input id="${key}.${paraskey}" data-key="${paraskey}" data-type="${para.type}" type="text" class="w3-input configedit"/>`);
                if (typeof para.default !== 'undefined')
                    edt.val(para.default);
                edt.trigger('change');
                break;
            case "select": edt = $(`<select id="${key}.${paraskey}" data-key="${paraskey}" data-type="${para.valtype}" type="text" class="w3-input configedit"/>`);
                if (typeof para.values !== 'undefined') {
                    valkeys = Object.keys(para.values);
                    for (let v = 0; v < valkeys.length; v++) {
                        edt.append($(`<option/>`).val(valkeys[v]).text(para.values[valkeys[v]]));
                    }
                }
                if (typeof para.default !== 'undefined')
                    edt.find(`option[value="${para.default}"]`).attr('selected', 'selected');
                edt.trigger('change');
                break;
            case "integer": para.step = 1;
            case "number": edt = $(`<input id="${key}.${paraskey}" data-key="${paraskey}" data-type="${para.type}" type="number" class="w3-input configedit"/>`);
                edt.on("change", function () {
                    var curr_val = parseFloat($(this).val());
                    var min = $(this).attr('min');
                    if (typeof min != 'undefined') {
                        if (curr_val < parseFloat(min)) { curr_val = parseFloat(min) }
                    }
                    var max = $(this).attr('max');
                    if (typeof max != 'undefined') {
                        if (curr_val > parseFloat(max)) { curr_val = parseFloat(max) }
                    }
                    function getDecimalPlaces(num) {
                        const str = num.toString();
                        if (str.includes(".")) {
                            return str.split(".")[1].length;
                        }
                        return 0; // Keine Dezimalstellen, wenn kein Punkt
                    }
                    var step = $(this).attr('step');
                    if (typeof step == 'undefined') step = "1"
                    $(this).val(curr_val.toFixed(getDecimalPlaces(step)));
                })
                    .attr("autocomplete", "off")
                    /*.css({ "text-align": "right" })*/;
                if (typeof para.min != 'undefined')
                    edt.attr("min", para.min)
                if (typeof para.max != 'undefined')
                    edt.attr("max", para.max)
                if (typeof para.step != 'undefined')
                    edt.attr("step", para.step)
                else
                    edt.attr("step", "1")
                if (typeof para.default !== 'undefined')
                    edt.val(para.default);
                edt.trigger('change');
                break;
            default:
        }
        row.append($('<td/>').append(edt));
        dest.append(row);
    }
}

function addNewCard(btn) {
    let paras = JSON.parse(btn.data('params'));
    let sortareaid = btn.data('sortareaid');
    let count = btn.data('count') || 0;
    let btnid = `${btn.attr('id')}`;
    let groupkey = btn.data('groupkey');
    let thiskey = btn.data('thiskey');
    let key = `${groupkey}.${count}.${thiskey}`;
    btn.data('count', count + 1)
    let dest = $(`#${sortareaid}`)
    let table = $(`<table id="${key}" style="width:100%" class="w3-table-all w3-hoverable"/>`)
    dest.append($('<div class="w3-light-grey w3-row w3-border w3-round" style="width:100%" />')
        .attr('data-id', key)
        .data('groupkey', groupkey)
        .data('thiskey', thiskey)
        .attr('data-key', btnid)
        .attr('data-iscard', 1)
        .append(table));
    buildParas(paras, key, table)
    let del = $(`<div class="edit-btn process-del"><i class="fa-solid fa-trash-can"></i></div>`);
    del.addClass("w3-button")
        .css({ "width": "100%" })
        .on("click", function () {
            getByID(key).remove();
        });
    table.append($(`<tr/>`).append($('<td colspan="2"/>').append(del)))
    return table;
}

let sortablearray = [];
function addablecardsBTN(addablecards, key, dest) {
    const addablecardkeys = Object.keys(addablecards);
    let sortareaid = "sortarea" + sortablearray.length;
    bar = $('<div class="" style="width:100%" />');
    for (let p = 0; p < addablecardkeys.length; p++) {
        const addablecardkey = addablecardkeys[p];
        const addablecard = addablecards[addablecardkey];
        let btn = $(`<a id="${key}.${addablecardkey}" class="w3-button w3-center" style="text-align: center"><i class="fa-solid fa-file-circle-plus"></i>${addablecard.text}</a>`);
        btn.data('params', JSON.stringify(addablecard.paras));
        btn.data('groupkey', key);
        btn.data('thiskey', addablecardkey);
        btn.data('sortareaid', sortareaid);
        btn.data('count', 0);
        btn.on("click", function () {
            addNewCard($(this));
        });
        bar.append(btn);
    }
    container = $(`<div class="card" style="width:100%"/>`);
    container.append(bar);
    let sortarea = $(`<div id="${sortareaid}" class="sortable" style="width:100%"/>`)
    container.append(sortarea);
    dest.append($('<tr class="addablecardsbtn"/>').append($(`<td colspan="2"/>`).append(container)));
    sortablearray.push(new Sortable(document.getElementById(sortareaid), {
        multiDrag: true, // Enable multi-drag
        selectedClass: 'w3-gray', // The class applied to the selected items
        fallbackTolerance: 3, // So that we can select items on mobile
        animation: 150
    }));
}

function getByID(id) {
    if (id[0] !== '#') { id = "#" + id }
    id = id.replaceAll(`\\.`, `.`).replaceAll(`.`, `\\.`);
    return $(id);
}

function setSetupValue(input, value) {
    if (typeof input == 'string') {
        input = getByID(input);
    }
    if (input.length > 0) {
        let type = input.data('type');
        switch (type) {
            case "boolean":
                input.prop("checked", value);
                break
            case "sstring":
            case "snumber":
            case "sinteger":
                input.find(`option[value="${value}"]`).attr('selected', 'selected');
                break
            case "password":
            case "string":
            case "number":
            case "integer":
            default:
                input.val(value);
                input.trigger('change')

        }
    }
}
function getSetupValue(input) {
    let value;
    if (typeof input == 'string') {
        input = getByID(input);
    }
    if (input.length > 0) {
        let type = input.data('type');
        switch (type) {
            case "boolean":
                value = input.prop("checked");
                break
            case "sstring":
            case "snumber":
            case "sinteger":
            case "password":
            case "string":
            case "number":
            case "integer":
            default:
                value = input.val();
        }
        switch (type) {
            case "boolean":
                value = value ? true : false;
                break
            case "snumber":
            case "number":
                value = parseFloat(value);
                break
            case "sinteger":
            case "integer":
                value = parseInt(value);
                break
            case "password":
            case "sstring":
            case "string":
            default:
                value = String(value);

        }
    }
    return value;
}

function setcarddefaulds(carddefaulds, key) {
    for (let p = 0; p < carddefaulds.length; p++) {
        let carddefauld = carddefaulds[p];
        let btn = getByID(`${key}\\.${carddefauld.card}`);
        let dest = addNewCard(btn);
        let desid = dest.attr('id');
        const valkeys = Object.keys(carddefauld);
        for (let i = 0; i < valkeys.length; i++) {
            let para = valkeys[i];
            let input = getByID(`${desid}.${para}`)
            setSetupValue(input, carddefauld[para]);
            console.log(getSetupValue(input));
        }
    }
}

function loadSetupData() {
    $.getJSON(GETSETUP, function (data) {
        function traverseObject(obj, prefix = '', callback) {
            Object.keys(obj).forEach((key) => {
                const value = obj[key];
                const path = prefix ? `${prefix}.${key}` : key; // Erstelle den vollständigen Pfad

                if (typeof value === 'object' && value !== null && !Array.isArray(value)) {
                    // Rekursion für verschachtelte Objekte
                    traverseObject(value, path, callback);
                } else if (Array.isArray(value)) {
                    // Durchlaufe Arrays und füge den Index zum Pfad hinzu
                    value.forEach((item, index) => {
                        const card = item.card;
                        let btn = getByID(`${key}.${card}`);
                        let dest = addNewCard(btn);
                        let desid = dest.attr('id');
                        const valkeys = Object.keys(item);
                        for (let i = 0; i < valkeys.length; i++) {
                            let para = valkeys[i];
                            let input = getByID(`${desid}.${para}`)
                            setSetupValue(input, item[para]);
                            console.log(getSetupValue(input));
                        }
                        //const arrayPath = `${path}.${index}`;
                        //if (typeof item === 'object' && item !== null) {
                        //    traverseObject(item, arrayPath, callback);
                        //} else {
                        //    callback(arrayPath, item); // Einfacher Wert im Array
                        //}
                    });
                } else {
                    // Einfacher Schlüssel-Wert, Callback aufrufen
                    callback(path, value);
                }
            });
        }
        traverseObject(data, '', setSetupValue);

    }).fail(function (jqxhr, textStatus, error) {
        console.log("Fehler beim Laden der JSON-Daten:", textStatus, error);
    });

}

function buildSetup() {
    $("#setupTab").html('<div id="loading">Loading config...</div>');

    // Handle network errors, timeouts, and other issues
    $.ajax({
        url: GETSETUPPARA,
        dataType: 'json',
        timeout: 5000, // Timeout set to 5 seconds
        success: function (data) {
            // Remove loading indicator once data is loaded
            $("#setupTab").html(SETUPTAB);
            sortablearray = [];
            const keys = Object.keys(data);
            for (let i = 0; i < keys.length; i++) {
                const key = keys[i];
                const table = $("#setuptable");
                table.append(`<tr><th colspan="2">${data[key].text}</th></tr>`);
                const paras = data[key].paras;
                if (paras)
                    buildParas(paras, key, table);
                const addablecards = data[key].addablecards;
                if (addablecards)
                    addablecardsBTN(addablecards, key, table);
                const carddefaulds = data[key].carddefaulds;
                //if (carddefaulds)
                    //setcarddefaulds(carddefaulds, key);
            }
            let button = $(SETUPBARBUTTONS)
            $("#myAddon").append(button);
            $(".mySetupSaveButton")
                .addClass("w3-button")
                .off("click")
                .on("click", function () {
                    saveSetupJSON();
                });
            loadSetupData();
        },
        error: function (jqxhr, textStatus, error) {
            // Remove loading indicator and show error message
            $("#loading").remove();

            if (textStatus === "timeout") {
                // Timeout error message
                $("#setupTab").html(`<div id="loading">Error: Timeout while loading data.</div>`);
                console.error("Error: Timeout while loading data.");
            } else {
                // General error message for other issues (e.g., network errors)
                $("#setupTab").html(`<div id="loading">Error: ${textStatus} - ${error}</div>`);
                console.error(`Error loading JSON data: ${textStatus}, ${error}`);
            }
        }
    });
}

function setNestedValue(obj, path, value) {
    const keys = path.split('.'); // Pfad in Schlüssel aufteilen
    let current = obj; // Referenz auf das Objekt
    let last = obj; // Referenz auf das Objekt

    keys.forEach((key, index) => {
        if (index === keys.length - 1) {
            current[key] = value;
        } else {
            const isNumber = !isNaN(Number(keys[index + 1]));
            // Stelle sicher, dass Zwischenschlüssel existieren und Objekte sind
            if (!(key in current)) {
                if (isNumber) {
                    current[key] = [];
                } else {
                    current[key] = {};
                }
            }
            current = current[key]; // Weiter zum nächsten Objekt
        }
    });
}

function saveSetupJSON() {

    /*
    sortablearray.forEach(function (sortable, index) {
        let array = sortable.toArray();
        for (let i = 0; i < array.length; i++) {
            let card = $(`[data-id="${array[i]}"]`)
            let newid = `${card.data('groupkey')}.${i}.${card.data('thiskey')}`;
            if (newid !== array[i]) {
                card.find(`[data-key]`).each(function () {
                    $(this).attr('id', `${newid}.${$(this).data('key')}`)
                })
            }
        }
    });
    */
    obj = {};
    $("[data-type]").each(function () {
        let a=$(this).parents("[data-iscard]");
        if (a.length == 0) {
            let id = $(this).attr("id");
            setNestedValue(obj, id, getSetupValue($(this)));
        }
    })
    sortablearray.forEach(function (sortable, index) {
        let array = sortable.toArray();
        for (let i = 0; i < array.length; i++) {
            let card = $(`[data-id="${array[i]}"]`)
            let newid = `${card.data('groupkey')}.${i}`;
            setNestedValue(obj, `${newid}.card`, card.data('thiskey'));
            card.find(`[data-key]`).each(function () {
                setNestedValue(obj, `${newid}.${$(this).data('key')}`, getSetupValue($(this)));
            })
        }
    });

    let json = JSON.stringify(obj);
    console.log(json);
    uploadString(GETSETUP, json, function () {
        //--sendData('reloadPoints');
    });
}

// Function to build the main file management page
function buildPageSetup() {
    // Insert file area HTML into #mySpace
    $("#mySpace").html(SETUPAREA);
    buildSetup();
}

