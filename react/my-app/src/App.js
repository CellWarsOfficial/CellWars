import React, { Component } from 'react';
import './App.css';

const VOL = 'https://www.doc.ic.ac.uk/project/2016/271/g1627123/';
const VOL_IMAGES = VOL.concat('images/');
const idle_cell = VOL_IMAGES.concat('idle_cell.gif');
const big_cell = VOL_IMAGES.concat('big_cell.png')
const logo = VOL_IMAGES.concat('logo.gif');
const small_cell = VOL_IMAGES.concat('small_cell.png');
const arrow = VOL_IMAGES.concat('arrow.png');
const crown = VOL_IMAGES.concat('crown.png');

var ws;

var width = 20;
var height = 20; //dimensions of the board

var offsetWidth = 0;
var offsetHeight = 0;

const headerHeight = 180;

const players = 10; //determines how colours are split between userID's
var yourUserID = 0;

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

const UPDATE_FAIL = 0;
const UPDATE_SUCCESS = 1;

var requests = [];
const QUERY_REQUEST = 1;
const UPDATE_REQUEST = 2;
const PICK_REQUEST = 3;
const SCORE_REQUEST = 4;
const DETAILS_REQUEST = 5;
const CAPITAL_REQUEST = 6;
const INVALID_REQUEST = -1;
const FINISHED_REQUEST = 0;

const CRANKFIN = "CRANKFIN";
const TIMEOUT = "TIMEOUT";
const LOST = "LOST";

var uniqueID = 0;


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
    this.forceUpdate();
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
          <Grid currentPage = {this.state.currentPage}/>
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


function ImgSquare(props) {
  var src = null;
  var user = props.userID;
  var isCapital = false;

  if (user < 0) {
    user = -user;
    isCapital = true;
  }

  if (user === 0) {
    src = small_cell;
  } else if (props.displayMode === DISPLAYMODE.COLOURS.value) {
    if (isCapital) {
      // TODO add capital iamge and set it as src
      src = crown;
    } else {
      src = idle_cell;
    }
  } else if (props.displayMode === DISPLAYMODE.EMOJIS.value) {
    if (isCapital) {
      // TODO add capital iamge and set it as src
      src = crown;
    } else {
      src = VOL_IMAGES.concat('emojis/').concat((Number(user) + 1000).toString()).concat('.png');
    }
  }
  return (
    <div style={{display: 'inline-block'}}>
    <input
    type="image"
    alt="cell"
    src={src}
    onClick={props.onClick}
    style={{width:20, height:20, backgroundColor:rainbow(user)}}
    className='cell'>
    </input>
    </div>
  );
}



class Row extends Component {
  renderSquare(userIDturtle, row, col) {
    return (
      <ImgSquare
      key={row.toString().concat(col.toString())}
      userID={userIDturtle}
      onClick={() => this.props.onClick(row, col)}
      displayMode={this.props.displayMode}
      />
    );
  }

  render() {
    var renderedSquares = [];
    for (var i = 0; i < width; i++) {
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
      row.push(0);
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
    requestColor(yourUserID);
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

function generateUniqueHeader(requestType) {
  var header = (Math.floor((Math.random() * 100) + 1)); 
  var notUnique = true;
  while (notUnique) {
    header = (Math.floor((Math.random() * 100) + 1));
    notUnique = false;
    for (let i = 0; i < requests.length; i++) {
      if (getHeader(requests[i]) === header) {
        if (getType(requests[i]) === (FINISHED_REQUEST)) {
          setType(requests[i], requestType);
          return header;
        }
        notUnique = true;
        break;
      }
    }
  }
  var newRequest = [header, requestType];
  requests.push(newRequest);
  return header;
}

function getHeader(entry) {
  return entry[0];
}

function getType(entry) {
  return entry[1];
}

function setType(entry, type) {
  entry[1] = (type);
}

function findType(header) {
  for (let i = 0; i < requests.length; i++) {
    if (getHeader(requests[i]) === (header)) {
      return getType(requests[i]);
    }
  }
  return INVALID_REQUEST;
}

function completeHeader(header) {
  for (let i = 0; i < requests.length; i++) {
    if (getHeader(requests[i]) === (header)) {
      setType(requests[i], FINISHED_REQUEST);
      return true;
    }
  }
  return false;
}

function send(request) {
  console.log("WS_SEND : '".concat(request).concat("'"));
  if (ws.readyState !== WS_READY) {
    console.log("ABORT: Websocket is not ready!");
    ws.close();
  } else {
    ws.send(request);
  }
}

function requestColor(yourUserID) {
  console.log("Requesting color ".concat(yourUserID));
  var header = generateUniqueHeader(PICK_REQUEST);
  var pickRequest = header.toString()
                    .concat(": PICK t=")
                    .concat(yourUserID)
  send(pickRequest);
}

class Grid extends Component {
  shouldComponentUpdate(nextProps, nextState) {
    return false;
  }

  decrementTime() {
    var newTime = Math.max(this.state.timeLeft - 1, 0);
    this.setState({
      timeLeft: newTime,
    });
    this.forceUpdate();
   }

  componentDidMount() {
    var intervalId = setInterval(() => {this.decrementTime()}, 1000);
    this.setState({intervalId: intervalId});
    window.addEventListener("resize", () => {this.get()});
  }

  constructor() {
    super();
    var localHighscores = new Array(players + 1);
    localHighscores.fill(0);
    this.state = {
      localHighscores: localHighscores,
      displayMode: 0,
      board: emptyGrid(width, height),
      timePerRound: 10,
      timeLeft: -1,
    }
    var url = "ws".concat(window.location.toString().substring(4));
    // url = "ws://89.122.28.235:7777/"; // DEBUG
    ws = new WebSocket(url);
    ws.onopen = function() {
      console.log("Web socket opened : ".concat(url));
    };
    ws.onclose = function() {
      console.log("Web socket closed : ".concat(url));
    }
    ws.onerror = function() {
      console.log("Web socket error");
    }
    ws.onmessage = function (evt)
    {
      var received_msg = evt.data;

      console.log("WS_RECEIVE : '".concat(received_msg).concat("'"));
      var split_msg = received_msg.trim().split(':');

      if (split_msg.length !== 2) {
        console.log("Parse error : Invalid number of ':'");
      }

      var header = Number(split_msg[0].trim());
      var data = split_msg[1].trim();

      var lines = data.match(/\S+/g) || [];

      if (header > 0 && header <= 100) {
        var type = findType(header);
        switch (type) {
          case UPDATE_REQUEST: this.parseUpdate(lines); break;
          case SCORE_REQUEST: this.parseScore(lines); break;
          case PICK_REQUEST: this.parsePick(lines); break;
          case QUERY_REQUEST: this.parseQuery(lines); break;
          case DETAILS_REQUEST: this.parseDetails(lines); break;
          case CAPITAL_REQUEST: this.parseCapital(lines); break;
          case INVALID_REQUEST: console.log("Parse error : Header not found"); break;
          default: console.log("Parse error : Invalid header type (".concat(header).concat(" - ").concat(type).concat(")"));
        }
        if (!completeHeader(header)) {
          console.log("Fatal error: cannot find header which we just had!");
        }
      } else if (header > 100 && header <= 200) {
        if (lines.length !== 1) {
          console.log("Parse error(101 - 200): unexpected number of lines");
        }
        switch (lines[0]) {
          case CRANKFIN: this.respondCrankFin(header); break;
          case TIMEOUT: this.respondTimeout(header); break;
          case LOST: this.respondLost(header); break;
          default: console.log("Parse error(101-200): unknown method");
        }
      } else {
        console.log("Error : Header out of bounds!");
      }


    }.bind(this);
  }

  restartTimer() {
    this.setState({
      timeLeft: this.state.timePerRound,
    });
    this.forceUpdate();
  }


  respondCrankFin(header) {
    console.log("Responding to CRANKFIN");
    this.respondOne(header);
    this.restartTimer();
    this.get();
  }

  respondTimeout(header) {
    console.log("Responding to TIMEOUT");
    this.respondOne(header);
  }

  respondLost(header) {
    console.log("USER HAS LOST!");
    ws.close();
    var userResponse = window.confirm("You have lost!\n Do you wish to refresh?");
    if (userResponse) {
      window.location.reload();
    }
  }

  respondOne(header) {
    if (uniqueID === 0)
      return;
    var response = header.toString().concat(": 1");
    send(response);
  }

  parseUpdate(lines) {
    if (lines.length !== 1) {
      console.log("ParseUpdate: unexpected length");
      return;
    }
    switch (Number(lines[0])) {
      case UPDATE_FAIL:
        console.log("ParseUpdate : response FAIL");
        this.get();
        break;

      case UPDATE_SUCCESS:
        console.log("ParseUpdate : response SUCCESS");
        break;

      default:
        console.log("ParseUpdate: unexpected response value");
    }
  }

  parseDetails(lines) {
    if (Number(lines[0]) === 0) {
      console.log("ParseDetails: game is ending..");
      return;
    } else if (Number(lines[0] < 0)) {
      console.log("ParseDetails: unexpected initial number (".concat(lines[0]).concat(")"));
      return;
    }
    for (let i = 1; i < lines.length; i++) {
      var data = lines[i].split("=");
      if (data.length !== 2) {
        console.log("ParseDetails: unexpected number of '='s");
        return;
      }
      switch (data[0]) {
        case "gtc": this.parseDetailsGtc(data[1]); break;
        case "wait": this.parseDetailsWait(data[1]); break;
        default: console.log("ParseDetails: unexpected param (".concat(lines[i]).concat(")")); return;
      }
    }
    console.log("ParseDetails: successfully parsed details");
  }

  parseDetailsGtc(value) {
    console.log("ParseDetailsGtc : Currently ignoring(".concat(value).concat(")"));
  }

  parseDetailsWait(value) {
    this.updateTimePerRound(Number(value));
  }

  updateTimePerRound(time) {
    this.setState({
      timePerRound: time,
    });
  }

  fetchDetails() {
    if (uniqueID === 0)
      return;

    var header = generateUniqueHeader(DETAILS_REQUEST);

    var detailsRequest = header.toString().concat(": DETAILS");

    send(detailsRequest);
  }

  parseScore(lines) {
    // TODO
    console.log("ParseScore: currently ignoring score response");
  }

  parsePick(lines) {
    if (Number(lines[0]) === 0) {
      console.log("ParsePick : uid failed");
    }
    uniqueID = Number(lines[0]);
    console.log("ParsePick : uid received ".concat(lines[0]));
    this.get();
    this.fetchDetails();
  }

  parseQuery(lines) {
    width = Math.floor(window.innerWidth / 30);
    height = Math.floor((window.innerHeight - headerHeight) / 30);

    var numberOfLines = Number(lines[0]);
    if (numberOfLines !== ((lines.length - 1)/3)) {
      console.log("ParseQuery: number of lines do not match up");
      return;
    }

    var board = emptyGrid(width, height);

    for (let i = 0; i < numberOfLines; i++) {
      var row = lines[i*3 + 1] - offsetHeight;
      var col = lines[i*3 + 2] - offsetWidth;
      var uid = lines[i*3 + 3];

      if (row < 0 || row >= height || col < 0 || col >= width) {
        console.log("ParseQuery : cell out of bounds");
        return;
      }
      board[row][col] = uid;
    }

    this.updateHighScores(board);
    this.requestCapitals();

    console.log("ParseQuery: successfully parsed query");
  }

  parseCapital(lines) {
    var numberOfLines = Number(lines[0]);
    if (numberOfLines !== ((lines.length - 1)/ 3)) {
      console.log("ParseCapital: number of lines do not match up");
      return;
    }

    const board = this.state.board;

    for (let i = 0; i < numberOfLines; i++) {
      var row = lines[i*3 + 1] - offsetHeight;
      var col = lines[i*3 + 2] - offsetWidth;
      var uid = lines[i*3 + 3];

      if (row < 0 || row >= height || col < 0 || col >= width) {
        continue;
      }

      board[row][col] = -uid;
    }

    this.forceUpdate();
    console.log("ParseCapital: successfully parsed query");
  }

  updateHighScores(board) {
    var localHighscores = calculateLocalHighscores(board);
    this.setState({
      localHighscores: localHighscores,
      board: board
    });
  }

  submit(board, row, col) {
    if (uniqueID === 0)
      return;
    var header = generateUniqueHeader(UPDATE_REQUEST);
    var updateRequest = header.toString()
                .concat(": UPDATE px=")
                .concat(row + offsetHeight)
                .concat(" py=")
                .concat(col + offsetWidth)
                .concat(" t=")
                .concat(board[row][col]);
    send(updateRequest);
  }


  handleClick(row, col) {
    const board = this.state.board.slice(); //Clones the board

    board[row][col] = (board[row][col] === 0) ? yourUserID : 0;
    this.setState({
      board: board
    });
    this.forceUpdate();
    this.submit(board, row, col);
  }

  renderRow(row) {
    return (
      <div key={row.toString()}>
        <Row
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

  renderTime() {
    return (
      <div key={this.state.timeLeft.toString()} className='time'>
      Time Left : {this.state.timeLeft.toString()}
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

    var time = this.renderTime();

    return (
      <div>
      {time}
      {scoresC}
      {rows}
      <MoveLeft onClick={() => this.get()}/>
      <MoveDown onClick={() => this.get()}/>
      <MoveUp onClick={() => this.get()}/>
      <MoveRight onClick={() => this.get()}/>
      <MoveReset onClick={() => this.get()}/> &nbsp;
      <DisplayModeAdvancer onClick={() => this.advanceDisplayMode()}/> &nbsp;
      <GetButton onClick={() => this.get()}/> &nbsp;
      </div>);
  }

  advanceDisplayMode() {
    this.setState({
      displayMode: (this.state.displayMode + 1) % 2
    });
    this.forceUpdate();
  }

  requestCapitals() {
    if (uniqueID === 0)
      return;

    var header = generateUniqueHeader(CAPITAL_REQUEST);
    var capitalRequest = header.toString()
                        .concat(": CAPITAL");
    send(capitalRequest);
  }

  get() {
    if (uniqueID === 0)
      return;

    var header = generateUniqueHeader(QUERY_REQUEST);

    var width = Math.floor(window.innerWidth / 30);
    var height = Math.floor((window.innerHeight - headerHeight) / 30);
    var px1 = offsetHeight;
    var py1 = offsetWidth;
    var px2 = height - 1 + offsetHeight;
    var py2 = width - 1 + offsetWidth;



    var queryRequest = header.toString()
                .concat(": QUERY px1=")
                .concat(px1)
                .concat(" py1=")
                .concat(py1)
                .concat(" px2=")
                .concat(px2)
                .concat(" py2=")
                .concat(py2);
    send(queryRequest);
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
