'use strict';

console.log("i was loaded");

// hello.js
const addon = require('../../build/Release/MebParser');
const path = require('path');
const basePath = __dirname;

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

function handleFiles(obj) {
    // const fileList = this.files;
    console.log(obj);
    const fileList = obj.files;
    // const fileListSize = fileList.length;
    // const regexFileCheck = /[a-zA-Z0-9_].log/;

    // if (regexFileCheck.exec(fileList[i].name)) {
    //     console.log("ok");
    // }
    return (fileList.length) ? fileList[0].path : "";
}

const loader = new Loader();

let incoming_table = $('#example');
let file_alert = document.getElementById('file_alert');
let log_path = document.getElementById('log_path');
let testbtn = document.getElementById('testbtn');
let testpath = document.getElementById('testpath');
let trace_analysis = document.getElementById('trace_analysis');

log_path.addEventListener("change", () => {
    file_alert.style.display = "none";
});

testbtn.onclick = () => {
    let file_path = handleFiles(log_path);

    if (file_path === "") {
        file_alert.style.display = "block";
    } else {
        loader.on();
        incoming_table.empty();
        // TODO: set btn disabled

        setTimeout(() => {
            let calls = addon.parseLog(handleFiles(log_path));
            console.log(calls);

            incoming_table.DataTable({
                "destroy": true,
                "data": calls['incoming_calls'],
                "columns": [
                    {"data": "timestamp"},
                    {"data": "call_id"},
                    {"data": "line"},
                    {"data": "actions"}
                ]
            });

            loader.off();
        }, 100);
    }
};

testpath.onclick = () => {
    // console.log(log_path.files);
    console.log(handleFiles(log_path));
};

trace_analysis.onclick = () => {
    // console.log(addon.exportCall("0x80000065", 30182000));
    // console.log(addon.exportCall(30182000, "80000065"));
    console.log(addon.exportCall("80000065", 30182000));
};

// function init() {
//
//     let myNotification = new Notification('Title', {
//         body: 'Lorem Ipsum Dolor Sit Amet'
//     });
//
//
//     myNotification.onclick = () => {
//         console.log('Notification clicked');
//         console.log(addon.parseLog());
//     };
// }
//
// init();

// let start = Date.now();
// let end = Date.now();
// console.log(`seconds elapsed = ${(end - start) / 1000}`);

$(document).ready(function () {
    incoming_table.DataTable();
    // outgoing_table.DataTable();
});