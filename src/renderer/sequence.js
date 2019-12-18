// REQUIRES
// const {dialog} = require('electron').remote;
// const addon = require('../../build/Release/MebParser');

// renderer process
console.log("waiting");

require('electron').ipcRenderer.on('flowchart_data', function (event, analysis_result) {
    // console.log(analysis_result);

    // let diag = Diagram.parse(analysis_result);
    // let options = {
    //     theme: 'simple'
    // };
    //
    // diag.drawSVG('diagram', options);
    var diagram = Diagram.parse("A->B: Message");
    diagram.drawSVG("diagram", {theme: 'simple'});
});