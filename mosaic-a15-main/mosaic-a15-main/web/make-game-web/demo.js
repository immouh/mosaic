Module.onRuntimeInitialized = () => { 
    start();

}
let col2str = [" ", "□", "■"];
let num2str = 
    ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"];

var g;
var canvas = document.getElementById('mycanvas');
const EMPTY = 0;
const WHITE = 1;
const BLACK = 2;
const UNCONSTRAINED = -1;
const FULL = 0;
const ORTHO = 1;
const FULL_EXCLUDE = 2;
const ORTHO_EXCLUDE = 3;
const ERROR=0;
const UNSATISFIED=1;
const SATISFIED=2;

const TRANSPARENTC='rgba(0, 0, 0, 0)';
const WHITEC='white';
const BLACKC='black'
const REDC='red'
const GREENC='green'

let const_color= [REDC,BLACKC,GREENC,WHITEC]
const_color;
let case_color=[TRANSPARENTC,WHITEC,BLACKC]
case_color;


window.addEventListener('load', windowLoad);              // window load
canvas.addEventListener('click', function(event){canvasLeftClick(event)});        // left click event


function square2str(n, c) {
    if (n == UNCONSTRAINED)
        return col2str[c];
    else
        return num2str[n];
}

function drawCanvas() {
    var ctx = canvas.getContext('2d');
    var width = canvas.width;
    var height = canvas.height;


    ctx.fillStyle = 'lightgreen';


    ctx.fillRect(0, 0, canvas.width, canvas.height);
}

function drawGame(g){
    var ctx = canvas.getContext('2d');
    var width = canvas.width;
    var height = canvas.height;

    // clear canvas
    ctx.clearRect(0, 0, width, height);
    ctx.beginPath();
    drawCanvas();
    // draw some lines
    ctx.save();
    ctx.strokeStyle = 'black';
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    for (var row = 0; row <= nb_rows; row++) {
        ctx.moveTo(0,row*(height/nb_rows));
        ctx.lineTo(width,row*(height/nb_rows));
    } 
    for (var col = 0; col <= nb_cols; col++) {
        ctx.moveTo(col*(width/nb_cols),0);
        ctx.lineTo(col*(width/nb_cols),height);
    }
    ctx.stroke();
    ctx.restore();
    ctx.save();
    var fontsize = Math.min((width/nb_cols),(height/nb_rows));
    ctx.font = fontsize.toString() + 'px Roboto';
    ctx.textBaseline = 'middle';
    ctx.textAlign = 'center';
    for (var row = 0; row < nb_rows; row++) {
        for (var col = 0; col < nb_cols; col++) {
            var n = Module._get_constraint(g, row, col);
            var c = Module._get_color(g, row, col);
            var s=Module._get_status(g,row,col);
            var text = square2str(n, c);
            //FILL RECT
            ctx.fillStyle = case_color[c]
            ctx.fillRect(col * (width / nb_cols)+1, row * (height / nb_rows)+1, (width / nb_cols)-1, (height / nb_rows)-1);
            //Draw constraint
            if(n!=-1){
            ctx.fillStyle=(s==1)&&(c==BLACK) ? const_color[s+2] : const_color[s];
            ctx.fillText(text, col * (width / nb_cols) + (width / nb_cols) / 2, row * (height / nb_rows) + (height / nb_rows) / 2);
            }  
        }
    }   
    ctx.restore();
}  

function canvasLeftClick(event){
    var rect = canvas.getBoundingClientRect();
    var x = event.clientX - rect.left;
    var y = event.clientY - rect.top;
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    var width = canvas.width;
    var height = canvas.height;
    var col = x/(width/nb_cols);
    var row = y/(height/nb_rows);
    var c = Module._get_color(g, row, col);
    c = (c + 2)%3;
    Module._play_move(g,row,col,c);
    drawGame(g);
    if(Module._won(g)){
        alert("Congratulations!");
    } 

} 
function printGame(g) {
    var text = "";
    var nb_rows = Module._nb_rows(g);
    var nb_cols = Module._nb_cols(g);
    for (var row = 0; row < nb_rows; row++) {
        for (var col = 0; col < nb_cols; col++) {
            var n = Module._get_constraint(g, row, col);
            var c = Module._get_color(g, row, col);
            var status = Module._get_status(g, row, col);
            text += square2str(n, c);
        }
        text += "\n";
    }

    // put this text in <div> element with ID 'result'
    var elm = document.getElementById('result');
    elm.innerHTML = text;
}

    
    //BUTTONS
    const solveBUTTON = document.getElementById('solve');
    solveBUTTON.addEventListener('click', function() {
     var solved=Module._solve(g);
     drawGame(g);
     if(Module._won(g)){
        alert("This is a solution of this grid");
     }
     else{
        alert("This grid had no solution in the first place!");
     } 
    });
    const undoBUTTON = document.getElementById('undo');
    undoBUTTON.addEventListener('click', function() {
        Module._undo(g);
        drawGame(g);
    });
    const restartBUTTON = document.getElementById('restart');
    restartBUTTON.addEventListener('click', function() {
     Module._restart(g);
     drawGame(g);
    });
    const redoBUTTON = document.getElementById('redo');
    redoBUTTON.addEventListener('click', function() {
     Module._redo(g);
     drawGame(g);
    });
    const newBUTTON = document.getElementById('newgame');
    newBUTTON.addEventListener('click', function() {
        random();
    });
function random(){
    let listrow = document.getElementById('randomrow');
    let row = listrow.value;
    let listcol = document.getElementById('randomcol');
    let col= listcol.value;
    
    let list_neigh=document.getElementById('randomneigh');
    let neigh=list_neigh.value;
    let list_wrapping=document.getElementById('randomwrapping');
    let wrapping=list_wrapping.value;
     Module._delete(g);
     g=Module._new_random(row, col, wrapping, neigh, 0.3, 0.3);
     drawGame(g);
}

function windowLoad() {
    console.log("window load");
}

function start() {
    console.log("call start routine");
    g = Module._new_default();
    //var g = Module._new_random(6, 6, false, FULL, 0.6, 0.5);
    //Module._play_move(g, 0, 0, WHITE);
    drawGame(g);
    //printGame(g);
}

