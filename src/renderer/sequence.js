// REQUIRES
// const {dialog} = require('electron').remote;
const mermaid = require('mermaid');
const Chart = require('chart.js');
const addon = require('../../build/Release/MebParser');

let svg = undefined;
const pre = $('#preview_trace');
const previewLinesNo = document.getElementById('previewLineNo');
const colors = $('.color-square');
const clearSelectedColor = document.getElementById('clear_selected_color');
const clearAllColors = document.getElementById('clear_all_colors');
const ctx = document.getElementById('myChart').getContext('2d');


// (boxMargin - buttomMarginAdj) <= 10 in order to keep full boxes of actors
let sequence_config = {
    startOnLoad: false,
    sequence: {
        diagramMarginX: 50,
        diagramMarginY: 10,
        boxTextMargin: 5,
        // noteMargin: 10,
        messageMargin: 75,
        actorMargin: 100,
        mirrorActors: true,
        boxMargin: 50,
        bottomMarginAdj: 50,
        showSequenceNumbers: true,
        // Width of actor boxes
        // width: 150,
        // Height of actor boxes
        // height: 65,
        // actorFontSize: 14,
        // actorFontFamily: '"Open-Sans", "sans-serif"',
    }
};

mermaid.initialize(sequence_config);

// Adds an exists function to the jQuery object selectors
$.fn.exists = function () {
    return this.length !== 0;
};

// Return number with as many zero characters needed
function fixZeroDigit(num, max) {
    let final_num = '';
    let diff = max.toString().length - num.toString().length;

    for (let i = 0; i < diff; ++i) {
        final_num += '0';
    }

    final_num += num;

    return final_num;
}

// Changes font/color/background of selected node
function toggleNodeFocus(obj, index_id) {
    let rect_id = 'rect_' + index_id;

    if ($('#' + rect_id).exists()) {
        svg.find('#' + rect_id).remove();
    } else {
        let x1 = $("line", obj).attr('x1');
        let x2 = $("line", obj).attr('x2');
        let y = $("text", obj).first().attr('y');
        let color = $('.selected-color').css('background-color');

        let rect = document.createElementNS("http://www.w3.org/2000/svg", "rect");
        // rect.setAttribute('x', "0");
        // rect.setAttribute('width', "100%");
        rect.setAttribute('id', rect_id);
        rect.setAttribute('class', 'xin-rect');
        rect.setAttribute('x', String(Math.min(x1, x2)));
        rect.setAttribute('y', String(y - 20));
        rect.setAttribute('width', String(Math.abs(x1 - x2)));
        rect.setAttribute('height', "60");
        rect.setAttribute('fill', color);

        svg.prepend(rect);
    }

    $(obj).toggleClass('node-focus');
}

// Async receive of data from MainBrowserWindow
require('electron').ipcRenderer.on('flowchart_data', function (event, analysis_result) {

    // Set the flow of the svg to its respective element
    document.getElementById('diagram').textContent = analysis_result['flow_sequence'];

    // Initialize SVG
    console.time("Mermaid");
    mermaid.init();
    console.timeEnd("Mermaid");

    // Get SVG
    svg = $('svg');

    // selects all elements from diagram all apart actors
    let graph_elements = $("g:has(line:not([id])), g:has(rect.note)");

    // Make nodes clickable/hoverable
    graph_elements.addClass('clickable-node');
    graph_elements.addClass('hoverable-node');

    // Focus nodes with Ctrl + click
    graph_elements.on('click', function (evt) {
        if (evt.ctrlKey) {
            toggleNodeFocus($(this), graph_elements.index(this));
        }
    });

    // Return trace lines on double click
    graph_elements.on('dblclick', function () {
        let index_of_clicked = graph_elements.index(this);
        let trace_line = analysis_result['line_sequence'][index_of_clicked];

        // TODO: Get analysis.path
        let trace_preview = addon.previewTraceLines(trace_line, parseInt(previewLinesNo.value));

        // let pre = document.getElementById('preview_trace');
        pre.empty();

        // Returns the max number of the line from the traces returned
        let max_line = Math.max.apply(Math, trace_preview.map(function (o) {
            return o.line;
        }));

        // Build the preformatted trace along with the line numbering from the original file
        for (let i = 0; i < trace_preview.length; ++i) {
            pre.append(
                '<span class="line-number">' + fixZeroDigit(trace_preview[i].line, max_line) + '</span>' +
                ((trace_preview[i].line === trace_line) ? '<span class="selected">' : '<span>') +
                trace_preview[i].trace +
                '</span>' +
                '<span class="cl"></span>'
            );
        }

        // Show modal
        $('#previewModal').modal();
    });

    // Selection of text on the preview is automatically copied to the clipboard
    pre.on('mouseup', () => {
        // Copy to clipboard
        document.execCommand('copy');

        // Remove selection after mouseup
        if (window.getSelection) {
            window.getSelection().removeAllRanges();
        } else if (document.selection) {
            document.selection.empty();
        }
    });

    // Radio color buttons
    colors.on('click', function () {
        colors.removeClass('selected-color');
        $(this).addClass('selected-color');
    });

    // Un-focuses the nodes that were focused using the selected color
    clearSelectedColor.addEventListener('click', () => {
        let color = $('.selected-color').css('background-color');
        // TODO: Dangerous, could possibly delete lines/nodes with
        //       similar color. Change to a better selector
        svg.find('[fill="' + color + '"]').remove();
    });

    // Removes
    clearAllColors.addEventListener('click', () => {
        svg.find('.xin-rect').remove();
    });

    let bar_data = addon.getTraceStatistics();

    let keys = Object.keys(bar_data);
    let values = Object.values(bar_data);

    new Chart(ctx, {
        type: 'bar',
        data: {
            labels: keys,
            datasets: [{
                label: '# of bytes logged',
                data: values,
                backgroundColor: [
                    'rgba(255, 99, 132, 0.2)',
                    'rgba(54, 162, 235, 0.2)',
                    'rgba(255, 206, 86, 0.2)',
                    'rgba(75, 192, 192, 0.2)',
                    'rgba(153, 102, 255, 0.2)',
                    'rgba(255, 159, 64, 0.2)'
                ],
                borderColor: [
                    'rgba(255, 99, 132, 1)',
                    'rgba(54, 162, 235, 1)',
                    'rgba(255, 206, 86, 1)',
                    'rgba(75, 192, 192, 1)',
                    'rgba(153, 102, 255, 1)',
                    'rgba(255, 159, 64, 1)'
                ],
                borderWidth: 1
            }]
        },
        options: {
            scales: {
                yAxes: [{
                    ticks: {
                        beginAtZero: true
                    }
                }]
            }
        }
    });
});

