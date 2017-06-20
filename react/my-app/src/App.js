import React, { Component } from 'react';
import './App.css';

const VOL = 'https://www.doc.ic.ac.uk/project/2016/271/g1627123/';
const VOL_IMAGES = VOL.concat('images/');
const idle_cell = VOL_IMAGES.concat('idle_cell.gif');
const big_cell = VOL_IMAGES.concat('big_cell.png')
const logo = VOL_IMAGES.concat('logo.gif');
const small_cell = VOL_IMAGES.concat('small_cell.png');
const arrow = VOL_IMAGES.concat('arrow.png');

var ws;

var width = 20;
var height = 20; //dimensions of the board

var offsetWidth = 0;
var offsetHeight = 0;

const headerHeight = 230;

const players = 10; //determines how colours are split between userID's
var yourUserID = 1;

var DISPLAYMODE = {
  COLOURS: {value: 0, name: "colours"},
  EMOJIS: {value: 1, name: "emojis"}
};

var lastPage = 0;
const INTRO_PAGE = 0;
const GAME_PAGE = 1;
const RULE_PAGE_0 = 2
const RULE_PAGE_1 = 3
const RULE_PAGE_2 = 4
const RULE_PAGE_3 = 5
const RULE_PAGE_4 = 6

const WS_READY = 1;

const UPDATE_FAIL = 1;
const UPDATE_SUCCESS = 2;

const INTERACTIVE_EXAMPLE_GRID_SIZE = 5;

class App extends Component {
  constructor() {
    super();
    this.state = {
      currentPage: INTRO_PAGE,
    }
  }

  setPageTo(page) {
    if (this.state.currentPage < RULE_PAGE_0) {
      lastPage = this.state.currentPage;
    }
    if (page === RULE_PAGE_0 && lastPage === GAME_PAGE) {
      page = RULE_PAGE_1;
    }
    this.setState({currentPage: page});
  }

  render() {
    return (
      <div className="App">
        <div className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
        </div>
        <div style={{position: 'absolute', right:'10px'}}>
          <HelpButton onClick={() => this.setPageTo(RULE_PAGE_0)} currentPage = {this.state.currentPage}/>
        </div>
        <br></br>
        <br></br>
        <div className="App-grid">
          <RulePage0 currentPage = {this.state.currentPage}/>
          <RulePage1 currentPage = {this.state.currentPage} onClick={() => this.setPageTo(RULE_PAGE_2)}/>
          <RulePage2 currentPage = {this.state.currentPage} onClick={() => this.setPageTo(RULE_PAGE_3)}/>
          <RulePage3 currentPage = {this.state.currentPage} onClick={() => this.setPageTo(RULE_PAGE_4)}/>
          <RulePage4 currentPage = {this.state.currentPage} onClick={() => this.setPageTo(lastPage)}/>
          <UserPicker currentPage = {this.state.currentPage} onClick={() => this.setPageTo(GAME_PAGE)}/>
          <InteractiveExample currentPage = {this.state.currentPage}/>
        </div>
      </div>
    );
  }
}

class HelpButton extends Component {
  render() {
    if (this.props.currentPage >= RULE_PAGE_0) {  // hide help button on help pages
      return null;
    }
    return (
      <button onClick = {this.props.onClick} className={'roundButton'}>
      Rules
      </button>
    );
  }
}

class GetButton extends Component {
  render() {
  return (
      <button onClick = {this.props.onClick} className={'roundButton'}>
      Get
      </button>
    );
  }
}


class DisplayModeAdvancer extends Component {
  render() {
    return (
      <button onClick={this.props.onClick} className={'roundButton'}>
      Alternative Display
      </button>
    );
  }
}




function rainbow(n) {
  if (n === 0) { return '#FFFFFF'} //userID 0 is always mapped to white
  n = n * 240 / players; //splits assigned colours based on the number of players
  return 'hsl(' + n + ',100%,50%)';
}


function addNeighbour(neighbours, p1, p2, preBoard) {
  if (p1 < 0 || p1 >= INTERACTIVE_EXAMPLE_GRID_SIZE || p2 < 0 || p2 >= INTERACTIVE_EXAMPLE_GRID_SIZE) {
    return;
  }
  if (preBoard[p1][p2] == 0) {
    return;
  }
  neighbours.push(preBoard[p1][p2]);
}

function crankCell(i, j, preBoard) {
  var neighbours = [];
  for (var x = -1; x <= 1; x++) {
    for (var y = -1; y <= 1; y++) {
      if (x == 0 && y == 0) {
        continue;
      }
      addNeighbour(neighbours, i + x, j + y, preBoard);
    }
  }

  if (neighbours.length < 2) {
    return 0; // death by underpop
  }

  if (neighbours.length > 3) {
    return 0; // death by overpop
  }

  if (neighbours.length == 3 && preBoard[i][j] == 0) {
    if (neighbours.length === new Set(neighbours).size) {
      return 0; // no birth as no parent majority
    }

    if (neighbours[0] == neighbours[1]) {
      return neighbours[0];
    } else {
      return neighbours[2];
    } // birth by parent majority
  }

  return preBoard[i][j]; // stays alive
}

function ImgSquare(props) {
  var src = null;

  var cellColourID = props.userID;
  if (props.boardType == 'postBoard') {
    cellColourID = crankCell(props.row, props.col, props.preBoard);
  }

  if (props.displayMode === DISPLAYMODE.COLOURS.value) {
    src = idle_cell;
  } else if (props.displayMode === DISPLAYMODE.EMOJIS.value) {
    src = VOL_IMAGES.concat('emojis/').concat((Number(cellColourID) + 1000).toString()).concat('.png');
  }

  if (cellColourID === 0) {
    src = small_cell;
  }

  return (
    <div style={{display: 'inline-block'}}>
    <input
    type="image"
    alt="cell"
    src={src}
    onClick={props.onClick}
    style={{width:20, height:20, backgroundColor:rainbow(cellColourID)}}
    className='cell'>
    </input>
    </div>
  );
}



class Row extends Component {
  renderSquare(userIDturtle, row, col) {
    return (
      <ImgSquare
      boardType={this.props.boardType}
      preBoard={this.props.preBoard}
      row={row}
      col={col}
      key={row.toString().concat(col.toString())}
      userID={userIDturtle}
      onClick={() => this.props.onClick(row, col)}
      displayMode={this.props.displayMode}
      />
    );
  }

  render() {
    var squaresInARow = INTERACTIVE_EXAMPLE_GRID_SIZE;
    if (this.props.boardType == 'online') {
      squaresInARow = width;
    }

    var renderedSquares = [];
    for (var i = 0; i < squaresInARow; i++) {
      renderedSquares.push(this.renderSquare(this.props.squares[i], this.props.row, i));
    }
    return (
      <div>{renderedSquares}</div>
    );
  }
}




function emptyGrid(width, height) { // Generates an example board fitting to the width and height global variables
  var ret = [];
  for (var i = 0; i < height; i++) {
    var row = [];
    for (var j = 0; j < width; j++) {
      // TEMP DEBUG
      row.push(0);
      // row.push(i*width + j);
    }
    ret.push(row);
  }
  return ret;
}


class UserPicker extends Component {
  render() {
    if (this.props.currentPage !== INTRO_PAGE && this.props.currentPage !== RULE_PAGE_0)
    {
     return null;
    }

    return (<div><h2>Pick your colour</h2><br></br><table width="100%">
      <tbody>
      <tr className = "textcenter">
        <td onClick={() => this.handleClick(1)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(1)}}></img></td>    
        <td onClick={() => this.handleClick(2)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(2)}}></img></td>    
        <td onClick={() => this.handleClick(3)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(3)}}></img></td>    
        <td onClick={() => this.handleClick(4)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(4)}}></img></td>    
        <td onClick={() => this.handleClick(5)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(5)}}></img></td>    
        <td onClick={() => this.handleClick(6)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(6)}}></img></td>    
        <td onClick={() => this.handleClick(7)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(7)}}></img></td>    
        <td onClick={() => this.handleClick(8)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(8)}}></img></td>    
        <td onClick={() => this.handleClick(9)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(9)}}></img></td>    
        <td onClick={() => this.handleClick(10)}><img alt="big_cell" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(10)}}></img></td>    
      </tr>
      </tbody>
    </table></div>);

  }

  handleClick(uid) {
    yourUserID = uid;
    this.props.onClick();
  }

}

function calculateLocalHighscores(board) {
  var ret = new Array(players + 1);
  ret.fill(0);
  for (var i = 0; i < height; i++) {
    for (var j = 0; j < width; j++) {
      ret[board[i][j]]++;
    }
  }
  return ret;
}




























class InteractiveExample extends Component {
  constructor() {
    super();
    this.state = {
      preBoard: emptyGrid(width, height), // TODO: randomly fill this with ~10 cells of different colour initially
      infobox: '',
      displayMode: 0
    }
  }
  
  advanceDisplayMode() {
    this.setState({
      displayMode: (this.state.displayMode + 1) % 2
    });
  }

  handlePreBoardClick(row, col) {
    /*
    if (row == 0 || row == INTERACTIVE_EXAMPLE_GRID_SIZE || col == 0 || col == INTERACTIVE_EXAMPLE_GRID_SIZE) { // disallows toggling border cells
      return;
    }
    */
    const preBoard = this.state.preBoard.slice(); //Clones the preBoard
    preBoard[row][col] = (this.state.preBoard[row][col] === 0) ? yourUserID : 0; // TODO: need a way of picking your colour for the example, stop using yourUserID
    this.setState({
      preBoard: preBoard
    });
  }

  renderPreRow(row) {
    return (
      <div key={row.toString()}>
        <Row
          boardType={'preBoard'}
          squares={this.state.preBoard[row]}
          row={row}
          onClick={(r, c) => this.handlePreBoardClick(r, c)}
          displayMode={this.state.displayMode}
        />
      </div>
    );
  }

  renderPostRow(row) {
    return (
      <div key={row.toString()}>
        <Row
          boardType={'postBoard'}
          preBoard={this.state.preBoard}
          squares={this.state.preBoard[row]}
          row={row}
          onClick={null}
          displayMode={this.state.displayMode}
        />
      </div>
    );
  } 
  
  render() {
    if (this.props.currentPage !== GAME_PAGE) { // this will change
      return null;
    }
    var preRows = [];
    for (var i = 0; i < INTERACTIVE_EXAMPLE_GRID_SIZE; i++) {
      preRows.push(this.renderPreRow(i));
    }
    var postRows = [];
    for (var j = 0; j < INTERACTIVE_EXAMPLE_GRID_SIZE; j++) {
      postRows.push(this.renderPostRow(j));
    }

    return (
      <div>
        <h1> {this.state.infobox} </h1>
        {preRows}
        <br></br>
        {postRows}
        <DisplayModeAdvancer onClick={() => this.advanceDisplayMode()}/> &nbsp;
      </div>
    );
  }
}













class Grid extends Component {
  constructor() {
    super();
    var localHighscores = new Array(players + 1);
    localHighscores.fill(0);
    this.state = {
      localHighscores: localHighscores,
      displayMode: 0,
      board: emptyGrid(width, height),
      // cache: emptyGrid(width + LOOKAHEAD*2, height + LOOKAHEAD*2),
    }
    var url = "ws".concat(window.location.toString().substring(4));
    ws = new WebSocket(url);
    ws.onopen = function() {
      console.log("Web socket opened : ".concat(url));
      this.get();
    }.bind(this);
    ws.onclose = function() {
      console.log("Web socket closed : ".concat(url));
    }
    ws.onerror = function() {
      console.log("Web socket error");
    }
    ws.onmessage = function (evt)
    {
      width = Math.floor(window.innerWidth / 30);
      height = Math.floor((window.innerHeight - headerHeight) / 30);
      var received_msg = evt.data;
      console.log("Web socket message received: ".concat(received_msg));

      var lines = received_msg.trim().split('\n');

      if ((lines.length - 1) === Number(lines[0])) { // QUERY
        var board = emptyGrid(width, height);
        for (let i = 1; i <= lines[0]; i++) { // filling grid with received information
          var data = lines[i].trim().split(',');
          if (data.length !== 3) {
            console.log("Parse error : unexpected number of commas");
            break;
          }
          var row = data[0] - offsetHeight;
          var col = data[1] - offsetWidth;
          var uid = data[2];

          if (row < 0 || row >= height || col < 0 || col >= width) {
            console.log("Parse error : cell out of bounds");
            break;
          }

          board[row][col] = uid;
        }
        var localHighscores = calculateLocalHighscores(board);
        this.setState({
          localHighscores: localHighscores,
          board: board
        });
        console.log("Parse info : successfully parsed query")
      } else if (lines.length === 1) { // SUBMIT
        switch(Number(lines[0])) {
          case UPDATE_FAIL:
            console.log("Submit request responsed : FAIL");
            window.alert("Submit request responsed : FAIL");
            this.get();
          break;
          case UPDATE_SUCCESS:
            console.log("Submit request responsed : SUCCESS");
            break;
          default: console.log("Parse error : unexpected submit response (value)");
        }
      } else { // ERROR
        console.log("Parse error : unexpected response task");
      }
    }.bind(this);
  }


  handleClick(row, col) {
    const board = this.state.board.slice(); //Clones the board

    board[row][col] = (board[row][col] === 0) ? yourUserID : 0;
    this.setState({
      board: board
    });

    var updateRequest = "UPDATE px="
                .concat(row + offsetHeight)
                .concat(" py=")
                .concat(col + offsetWidth)
                .concat(" t=")
                .concat(board[row][col]);
    console.log("Sending query : ".concat(updateRequest));

    if (ws.readyState !== WS_READY) {
      console.log("ABORT: Websocket is not ready!");
    } else {
      ws.send(updateRequest);
    }
  }

  renderRow(row) {
    return (
      <div key={row.toString()}>
        <Row
          boardType={'online'}
          squares={this.state.board[row]}
          row={row}
          onClick={(r, c) => this.handleClick(r, c)}
          displayMode={this.state.displayMode}
        />
      </div>
    );
  }

  renderScore(highscorePosition, player, score) {
    if (player === 0) { return; }
    if (score === 0 && player !== yourUserID) { return; }

    var src = null;

    if (this.state.displayMode === DISPLAYMODE.COLOURS.value) {
      src = idle_cell;
    } else if (this.state.displayMode === DISPLAYMODE.EMOJIS.value) {
      src = VOL_IMAGES.concat('emojis/').concat((Number(player) + 1000).toString()).concat('.png');
    }

    if (player === 0) {
      src = small_cell;
    }

    var border = '0px solid white';
    if (player === yourUserID) {
      border = '1px solid grey';
    }
    var opacity = 1 - highscorePosition * 0.8 / players;


  return (
    <div key={player.toString()} style={{display: 'inline-block'}}>
    <input 
    type="image"
    alt="cell"
    src={src}
    style={{width:20, height:20, backgroundColor:rainbow(player), border: border, opacity: opacity}}
    className='cell'>
    </input>
    <h6  style={{color: rainbow(player), opacity: opacity}}>
    {':'.concat(score.toString())}
    </h6>
    </div>
  );

  }

  render() {
    if (this.props.currentPage !== GAME_PAGE) {
      return null;
    }
    var rows = [];
    for (var i = 0; i < height; i++) {
      rows.push(this.renderRow(i));
    }

    // clones the array of scores, on each iteration it finds the biggest player and score in the array, prints them and sets the clone score to 0
    var biggestScore = 0;
    var biggestPlayer = 0;
    var scoresC = [];
    var localHighscoresClone = this.state.localHighscores.slice(0); //= [50, 45, 40, 35, 30, 25, 20, 15, 10, 0]; // for testing
    for (var z = 0; z < players + 1; z++) {
      for (var y = 0; y < players + 1; y++) {
        if (localHighscoresClone[y] >= biggestScore) {
          biggestScore = localHighscoresClone[y];
          biggestPlayer = y;
        }
      }
      scoresC.push(this.renderScore(z, biggestPlayer, biggestScore));
      localHighscoresClone[biggestPlayer] = -1;
      biggestScore = 0;
      biggestPlayer = -1;
    }

    return (
      <div>
      {scoresC}
      {rows}
      <MoveLeft onClick={() => this.get()}/>
      <MoveDown onClick={() => this.get()}/>
      <MoveUp onClick={() => this.get()}/>
      <MoveRight onClick={() => this.get()}/>
      <MoveReset onClick={() => this.get()}/>
      <DisplayModeAdvancer onClick={() => this.advanceDisplayMode()}/> &nbsp;
      <GetButton onClick={() => this.get()}/> &nbsp;
      </div>);
  }

  componentDidMount() {
    window.addEventListener("resize", () => {this.get()});
  }

  advanceDisplayMode() {
    this.setState({
      displayMode: (this.state.displayMode + 1) % 2
    });
  }

  get() {
    var width = Math.floor(window.innerWidth / 30);
    var height = Math.floor((window.innerHeight - headerHeight) / 30);
    var px1 = offsetHeight;
    var py1 = offsetWidth;
    var px2 = height - 1 + offsetHeight;
    var py2 = width - 1 + offsetWidth;

/*    if (px1 > last_px1 && py1 > last_py1 && px2 < last_px2 && py2 < last_py2) {
      // update board

    } else {*/
      var queryRequest = "QUERY px1="
                  .concat(px1)
                  .concat(" py1=")
                  .concat(py1)
                  .concat(" px2=")
                  .concat(px2)
                  .concat(" py2=")
                  .concat(py2);
      console.log("Sending query : ".concat(queryRequest));

      if (ws.readyState !== WS_READY) {
        console.log("ABORT: Websocket is not ready!");
      } else {
        ws.send(queryRequest);
      }
    // }

  }
}


class MoveRight extends Component {
  render() {
    var char = '>';
    return(
      <button onClick = {() => this.handleClick()} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetWidth += Math.floor(width/4);
    this.props.onClick();
  }
}

class MoveLeft extends Component {
  render() {
    var char = '<';
    return(
      <button onClick = {() => this.handleClick()} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetWidth -= Math.floor(width/4);
    this.props.onClick();
  }
}

class MoveUp extends Component {
  render() {
    var char = '^';
    return(
      <button onClick = {() => this.handleClick()} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetHeight -= Math.floor(height/4);
    this.props.onClick();
  }
}

class MoveDown extends Component {
  render() {
    var char = 'v';
    return(
      <button onClick = {() => this.handleClick()} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetHeight += Math.floor(height/4);
    this.props.onClick();
  }
}

class MoveReset extends Component {
  render() {
    var char = 'Re-Centre';
    return(
      <button onClick = {() => this.handleClick()} className={'roundButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetHeight = 0;
    offsetWidth = 0;
    this.props.onClick();
  }
}

class RulePage0 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_0) {
      return null;
    }
    return (
    <div>
    <h1>To play the game you must first pick your colour which your cells will be representing!</h1>
    </div>
    );
  }
}

class RulePage1 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_1) {
      return null;
    }
    var divHeight = ((window.innerHeight - headerHeight)*.8).toString().concat('px');
    return (
    <div onClick={this.props.onClick} style={{height: divHeight}}>
    <h1>Cell Law I</h1>
    <h2>Any live cell with fewer than two live neighbours will die, as if caused by underpopulation.</h2>
    <div className="fullExample">
    <div className="gridExample left">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(1)}}></img></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(1)}}></img></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    <div className="arrow"><img alt="arrow" src={arrow} style={{width: '100px', height: '240px'}}></img></div>
    <div className="gridExample right">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    </div>
    </div>
    );
  }
}

class RulePage2 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_2) {
      return null;
    }
    var divHeight = ((window.innerHeight - headerHeight)*.8).toString().concat('px');
    return (
    <div onClick={this.props.onClick} style={{height: divHeight}}>
    <h1>Cell Law II</h1>
    <h2>Any live cell with two or three live neighbours lives on to the next generation.</h2>
    <div className="fullExample">
    <div className="gridExample left">
      <table>
      <tbody>
      <tr>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(10)}}></img></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(10)}}></img></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(10)}}></img></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(10)}}></img></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    <div className="arrow"><img alt="arrow" src={arrow} style={{width: '100px', height: '240px'}}></img></div>
    <div className="gridExample right">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(10)}}></img></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    </div>
    </div>
    );
  }
}

class RulePage3 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_3) {
      return null;
    }
    var divHeight = ((window.innerHeight - headerHeight)*.8).toString().concat('px');
    return (
    <div onClick={this.props.onClick} style={{height: divHeight}}>
    <h1>Cell Law III</h1>
    <h2>Any dead cell with exactly 3 live neighbours and a majority cell type can be identified
        among them, the cell will be reborn as the same type as the majority.</h2>
    <div className="fullExample">
    <div className="gridExample left">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(1)}}></img></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(10)}}></img></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(1)}}></img></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    <div className="arrow"><img alt="arrow" src={arrow} style={{width: '100px', height: '240px'}}></img></div>
    <div className="gridExample right">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(1)}}></img></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    </div>
    </div>
    );
  }
}

class RulePage4 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_4) {
      return null;
    }
    var divHeight = ((window.innerHeight - headerHeight)*.8).toString().concat('px');
    return (
    <div onClick={this.props.onClick} style={{height: divHeight}}>
    <h1>Cell Law IV</h1>
    <h2>Any live cell with greater than three live neighbours dies, as if caused by overpopulation.</h2>
    <div className="fullExample">
    <div className="gridExample left">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(6)}}></img></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(6)}}></img></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(6)}}></img></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(6)}}></img></td>
        <td></td>
        <td><img alt="pacman" src={idle_cell} style={{width: '30px', height: '30px', backgroundColor:rainbow(6)}}></img></td>
      </tr>
      </tbody>
      </table>
    </div>
    <div className="arrow"><img alt="arrow" src={arrow} style={{width: '100px', height: '240px'}}></img></div>
    <div className="gridExample right">
      <table>
      <tbody>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
      </tbody>
      </table>
    </div>
    </div>
    </div>
    );
  }
}

export default App;
