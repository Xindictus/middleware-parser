'use strict';

// REQUIRES
const {dialog} = require('electron').remote;
const path = require('path');
const basePath = __dirname;
const addon = require('../../build/Release/MebParser');

// CLASSES
class Loader {
    constructor() {
        this.container = document.getElementsByClassName("container")[0];
        this.overlay = document.getElementsByClassName("overlay")[0];
        this.loader = document.getElementById("loader");
        this.getLoader = function () {
            return "../static/img/loaders/loader" + (Math.ceil(Math.random() * 100) % 3 + 1) + ".gif";
        }
    }

    on() {
        this.container.classList.add("blur-filter");
        this.overlay.style.display = 'block';
        this.loader.style.background = "url('" + this.getLoader() + "') center center no-repeat";
    }

    off() {
        this.overlay.style.display = 'none';
        this.container.classList.remove("blur-filter");
    }
}

// FUNCTIONS
function handleFiles(obj) {
    const fileList = obj.files;

    return (fileList.length) ? fileList[0].path : "";
}

function toggle_btn_disable(btn, flag) {
    if (flag) {
        btn.disabled = false;
        btn.classList.remove('disabled', 'disable-cursor');
    } else {
        btn.disabled = true;
        btn.classList.add('disabled', 'disable-cursor');
    }
}

function exportToFile() {
    let path = modal_output_path.value;

    if (path !== "") {
        addon.exportCall(modal_call_id.value, parseInt(modal_cursor_pos.value), handleFiles(log_path), path);

        new Notification('Exporting Completed', {
            body: 'Call-ID: ' + modal_call_id.value + ' was exported successfully to ' + path
        });
    }
}

function prepareExportToFile() {
    // Get call-id to export (slice HEX part out of the call-id)
    modal_call_id.value = this.dataset.callId.slice(2);
    // Get cursor position for faster export
    modal_cursor_pos.value = this.dataset.cursorPos;
    // Open modal
    export_modal.modal();
}

function exportBtnListener() {
    Array.from(document.querySelectorAll('.export-call')).forEach(link => {
        link.addEventListener('click', prepareExportToFile);
    });
}

// DEFINES
const loader = new Loader();
const incoming_table = $('#incoming_table');
const outgoing_table = $('#outgoing_table');
const file_alert = document.getElementById('file_alert');
const log_path = document.getElementById('log_path');
const btn_parse = document.getElementById('btn_parse');
const trace_analysis = document.getElementById('trace_analysis');
const modal_call_id = document.getElementById('modal_call_id');
const modal_cursor_pos = document.getElementById('modal_cursor_pos');
const modal_output_path = document.getElementById('modal_output_path');
const modal_btn_export = document.getElementById('modal_btn_export');
const modal_output_div = document.getElementById('modal_output_div');
const export_modal = $('#exportModal');

// EVENT LISTENERS
log_path.addEventListener("change", () => {
    file_alert.style.display = "none";

    toggle_btn_disable(btn_parse, handleFiles(log_path));
});

// ON CLICK LISTENERS
modal_btn_export.onclick = () => {
    // loaders and such
    loader.on();
    // TODO: start exporting
    setTimeout(() => {
        exportToFile();
        loader.off();
        export_modal.modal('toggle');
    }, 100);
    // Close modal
};

modal_output_div.onclick = () => {
    dialog.showSaveDialog({
        title: "Choose where to save exported trace",
        buttonLabel: "Okay",
        defaultPath: modal_call_id.value + "_export.log"
    }).then(result => {
        modal_output_path.value = result.filePath;

        if (!result.canceled) {
            toggle_btn_disable(modal_btn_export, 1);
        } else {
            toggle_btn_disable(modal_btn_export, 0);
        }
    }).catch(err => {
        console.log(err);
        toggle_btn_disable(modal_btn_export, 0);
    });
};

btn_parse.onclick = () => {
    let file_path = handleFiles(log_path);
    // TODO: remove fixed path, currently only for test
    // file_path = "C:/Users/A745268/test/12.log";

    if (file_path === "") {
        file_alert.style.display = "block";
    } else {
        loader.on();
        incoming_table.empty();
        outgoing_table.empty();
        // TODO: set btn disabled

        setTimeout(() => {
            let calls = addon.parseLog(file_path);

            let test = incoming_table.DataTable({
                "destroy": true,
                "data": calls['incoming_calls'],
                "columns": [
                    {"data": "timestamp"},
                    {"data": "call_id"},
                    {"data": "line"},
                    {"data": "actions"}
                ]
            });

            outgoing_table.DataTable({
                "destroy": true,
                "data": calls['outgoing_calls'],
                "columns": [
                    {"data": "timestamp"},
                    {"data": "call_id"},
                    {"data": "line"},
                    {"data": "actions"}
                ]
            });

            // Refresh listeners on pagination, ordering and search
            incoming_table.on('draw.dt', exportBtnListener);
            outgoing_table.on('draw.dt', exportBtnListener);
            // TODO: restrict refresh of btn listeners per table event

            exportBtnListener();

            loader.off();

            new Notification('Parsing Completed', {
                body: 'Incoming calls: ' + calls['incoming_calls'].length
                    + '\nOutgoing calls: ' + calls['outgoing_calls'].length
            });
        }, 100);
    }
};

trace_analysis.onclick = () => {
    // let analysis_result = addon.analyzeCall("C:/Users/konst/PhpstormProjects/test/test.log");
    let analysis_result = addon.analyzeCall(handleFiles(log_path));

    if (analysis_result !== null) {
        analysis_result.path = handleFiles(log_path);

        const {BrowserWindow} = require('electron').remote;
        const {format} = require('url');
        let win = new BrowserWindow({
            width: 800,
            height: 600,
            webPreferences: {
                nodeIntegration: true
            }
        });
        // win.webContents.openDevTools();
        win.loadURL(format({
            pathname: path.join(__dirname, 'sequence.html'),
            protocol: 'file:',
            slashes: true
        }));

        win.webContents.on('did-finish-load', () => {
            win.webContents.send('flowchart_data', analysis_result);
            win.show();
        });

        // TODO: null on close

        // setTimeout(() => {
        // }, 2000);
    }
};

// let start = Date.now();
// let end = Date.now();
// console.log(`seconds elapsed = ${(end - start) / 1000}`);

$(document).ready(function () {
    // DataTable initialization
    incoming_table.DataTable();
    outgoing_table.DataTable();
});