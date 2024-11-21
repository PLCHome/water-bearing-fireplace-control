
// URL to get the list of file names from the server
const GETSETUPPARA = `/config/setupPara.json`;
const GETSETUP = `/config/setup.json`;


const SETUPAREA = `
<div id="myArea" style="height:100%; width:100%; overflow:auto" class="list-group" >
<div id="setupTab" />
</div>
`;

const SETUPTAB = `
<table id="setuptable" class="w3-table-all w3-hoverable"/>
`;



function buildParas(paras, key, dest) {
    const paraskeys = Object.keys(paras);
    for (let p = 0; p < paraskeys.length; p++) {
        const paraskey = paraskeys[p];
        const para = paras[paraskey];
        let row = $(`<tr><td style="vertical-align: middle; text-align: right; width: 50%" >${para.text}</td></tr>`)
        let edt = `<p>type ${para.type} is unknown!</p>`
        switch (para.type) {
            case "boolean": edt = $(`<input id="${key}.${paraskey}" data-type= "${para.type}" type="checkbox" class="w3-check configedit"/>`);
                if (typeof para.default !== 'undefined')
                    edt.prop("checked", para.default);
                edt.trigger('change');
                break;
            case "string": edt = $(`<input id="${key}.${paraskey}" data-type= "${para.type}" type="text" class="w3-input configedit"/>`);
                if (typeof para.default !== 'undefined')
                    edt.val(para.default);
                edt.trigger('change');
                break;
            case "select": edt = $(`<select id="${key}.${paraskey}" data-type= "${para.valtype}" type="text" class="w3-input configedit"/>`);
                if (typeof para.values !== 'undefined') {
                    valkeys = Object.keys(para.values);
                    for (let v = 0; v < valkeys.length; v++) {
                        edt.append($(`<option/>`).val(valkeys[v]).text(para.values[valkeys[v]]));
                    }
                }
                if (typeof para.default !== 'undefined')
                    edt.find(`option[value=${para.default}]`).attr('selected', 'selected');
                edt.trigger('change');
                break;
            case "integer": para.step = 1;
            case "number": edt = $(`<input id="${key}.${paraskey}" data-type= "${para.type}" type="number" class="w3-input configedit"/>`);
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
                    .css({ "text-align": "right" });
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
    let key = `${btn.attr('id')}.${count}`;
    btn.data('count', count + 1)
    let dest = $(`#${sortareaid}`)
    let table = $(`<table id="${key}" style="width:100%" class="w3-table-all w3-hoverable"/>`)
    dest.append($('<div class="w3-light-grey w3-row w3-border w3-round" style="width:100%" />').append(table));
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
    for (let p = 0; p < addablecardkeys.length; p++) {
        let sortareaid = "sortarea" + sortablearray.length;
        bar = $('<div class="" style="width:100%" />');
        const addablecardkey = addablecardkeys[p];
        const addablecard = addablecards[addablecardkey];
        let btn = $(`<a id="${key}.${addablecardkey}" class="w3-button w3-center" style="text-align: center"><i class="fa-solid fa-file-circle-plus"></i>${addablecard.text}</a>`);
        btn.data('params', JSON.stringify(addablecard.paras));
        btn.data('sortareaid', sortareaid);
        btn.data('count', 0);
        btn.on("click", function () {
            addNewCard($(this));
        });
        bar.append(btn);
        container = $(`<div class="card" style="width:100%"/>`);
        container.append(bar);
        let sortarea = $(`<div id="${sortareaid}" class="sortable" style="width:100%"/>`)
        container.append(sortarea);
        dest.append($('<tr/>').append($(`<td colspan="2"/>`).append(container)));
        sortablearray.push(new Sortable(document.getElementById(sortareaid), {
            multiDrag: true, // Enable multi-drag
            selectedClass: 'w3-gray', // The class applied to the selected items
            fallbackTolerance: 3, // So that we can select items on mobile
            animation: 150
        }));
    }
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
                input.find(`option[value=${value}]`).attr('selected', 'selected');
                break
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
                if (carddefaulds)
                    setcarddefaulds(carddefaulds, key);
            }
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

// Function to build the main file management page
function buildPageSetup() {
    // Insert file area HTML into #mySpace
    $("#mySpace").html(SETUPAREA);
    buildSetup();
}

