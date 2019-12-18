// REQUIRES
// const {dialog} = require('electron').remote;
// const addon = require('../../build/Release/MebParser');
// import mermaid from 'mermaid';
const mermaid = require('mermaid');
mermaid.initialize({startOnLoad: false});

// renderer process
console.log("waiting");

require('electron').ipcRenderer.on('flowchart_data', function (event, analysis_result) {
    console.log(analysis_result);
    document.getElementById('diagram').textContent = analysis_result;
    mermaid.init();
});