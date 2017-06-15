import React, { Component } from 'react';
import logo from './images/logo.gif';
import mouse_over from './images/mouse_over.gif';
import idle_cell from './images/idle_cell.gif';
import big_cell from './images/big_cell.png';
import arrow from './images/arrow.png';
import './App.css';

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


class App extends Component {
  constructor() {
    super();
    this.state = {
      displayMode: 0,
      currentPage: INTRO_PAGE,
    }
  }

  advanceDisplayMode() {
    this.setState({
      displayMode: (this.state.displayMode + 1) % 2
    });
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
          <Grid displayMode = {this.state.displayMode} currentPage = {this.state.currentPage}/>
        </div>
        <p></p>
          <DisplayModeAdvancer onClick={() => this.advanceDisplayMode()} currentPage = {this.state.currentPage}/> &nbsp;
          <GetButton onClick={() => get()} currentPage = {this.state.currentPage}/> &nbsp;
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
      Help
      </button>
    );
  }
}

class GetButton extends Component {
  render() {
  if (this.props.currentPage !== GAME_PAGE) {
    return null;
  }
  return (
      <button onClick = {this.props.onClick} className={'roundButton'}>
      Get
      </button>
    );
  }
}


class DisplayModeAdvancer extends Component {
  render() {
  if (this.props.currentPage !== GAME_PAGE) {
    return null;
  }
    return (
      <button onClick={this.props.onClick} className={'roundButton'}>
      Advance Display
      </button>
    );
  }
}




function rainbow(n) {
  if (n === 0) { return '#FFFFFF'} //userID 0 is always mapped to white
  n = n * 240 / players; //splits assigned colours based on the number of players
  return 'hsl(' + n + ',100%,50%)';
}



/*
function ImgSquare(props) {
  var src = null;

  if (props.displayMode === DISPLAYMODE.COLOURS.value) {
    src = idle_cell;
  } else if (props.displayMode === DISPLAYMODE.EMOJIS.value) {
    src = 'https://www.doc.ic.ac.uk/project/2016/271/g1627123/images/emojis/'.concat((props.userID).toString()).concat('.png');
  }

  if (props.userID === 0) {
    src = idle_cell;
  }

  return (
    <div style={{display: 'inline-block'}}>
    <input
    type="image"
    alt="cell"
    src={src}
    onClick={props.onClick}
    style={{width:20, height:20, backgroundColor:rainbow(props.userID)}}
    className='cell'>
    </input>
    </div>
    );
}
*/

class ImgSquare extends Component {
  constructor(props) {
    super(props);
    this.state = {
      src: idle_cell
    }
    this.handleMouseOver = this.handleMouseOver.bind(this);
    this.handleMouseOut = this.handleMouseOut.bind(this);
    this.goState();
  }

  goState() {
    var src = null;
    if (this.props.displayMode === DISPLAYMODE.COLOURS.value) {
      src = idle_cell;
    } else if (this.props.displayMode === DISPLAYMODE.EMOJIS.value) {
      src = 'https://www.doc.ic.ac.uk/project/2016/271/g1627123/images/emojis/'.concat((this.props.userID).toString()).concat('.png');
    }
    this.state = {
      src: src
    }
  }

  handleMouseOver() {
    if (this.props.displayMode === DISPLAYMODE.COLOURS.value) {
      window.alert("something");
      this.setState({
        src: mouse_over
      });
    }
  }

  handleMouseOut() {
    this.goState();
  }

  render() {
    return (
      <div style={{display: 'inline-block'}}>
      <input
      type="image"
      alt="cell"
      src={this.state.src}
      onClick={this.props.onClick}
      onMouseOver={this.handleMouseOver}
      onMouseOut={this.handleMouseOut}
      style={{width:20, height:20, backgroundColor:rainbow(this.props.userID)}}
      className='cell'>
      </input>
      </div>
    );
  }
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
        <td onClick={() => this.handleClick(1)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(1)}}></img></td>    
        <td onClick={() => this.handleClick(2)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(2)}}></img></td>    
        <td onClick={() => this.handleClick(3)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(3)}}></img></td>    
        <td onClick={() => this.handleClick(4)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(4)}}></img></td>    
        <td onClick={() => this.handleClick(5)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(5)}}></img></td>    
        <td onClick={() => this.handleClick(6)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(6)}}></img></td>    
        <td onClick={() => this.handleClick(7)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(7)}}></img></td>    
        <td onClick={() => this.handleClick(8)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(8)}}></img></td>    
        <td onClick={() => this.handleClick(9)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(9)}}></img></td>    
        <td onClick={() => this.handleClick(10)}><img alt="pacman" src={big_cell} className={'tableImage'} style={{backgroundColor:rainbow(10)}}></img></td>    
      </tr>
      </tbody>
    </table></div>);

  }

  handleClick(uid) {
    yourUserID = uid;
    this.props.onClick();
  }

}

class Grid extends Component {
  constructor() {
    super();
    this.state = {
      board: emptyGrid(width, height)
    }
    var url = "ws".concat(window.location.toString().substring(4));
    // url = "ws://89.122.28.235:7777/" // temp url used for local debugging
    ws = new WebSocket(url);
    ws.onopen = function() {
      console.log("Web socket opened : ".concat(url));
      get();
    }
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
        this.setState({
          board: board
        });
        console.log("Parse info : successfully parsed query")
      } else if (lines.length === 1) { // SUBMIT
        switch(Number(lines[0])) {
          case UPDATE_FAIL:
            console.log("Submit request responsed : FAIL");
            window.alert("Submit request responsed : FAIL");
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
          squares={this.state.board[row]}
          row={row}
          onClick={(r, c) => this.handleClick(r, c)}
          displayMode={this.props.displayMode}
        />
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

    return (
      <div>
      {rows}
      <MoveLeft/>
      <MoveDown/>
      <MoveUp/>
      <MoveRight/>
      <MoveReset/>
      </div>);
  }

  componentDidMount() {
    window.addEventListener("resize", () => {get()});
  }
}

function get() {
  var width = Math.floor(window.innerWidth / 30);
  var height = Math.floor((window.innerHeight - headerHeight) / 30);
  var queryRequest = "QUERY px1="
              .concat(offsetHeight)
              .concat(" py1=")
              .concat(offsetWidth)
              .concat(" px2=")
              .concat(height - 1 + offsetHeight)
              .concat(" py2=")
              .concat(width - 1 + offsetWidth)
  console.log("Sending query : ".concat(queryRequest));

  if (ws.readyState !== WS_READY) {
    console.log("ABORT: Websocket is not ready!");
  } else {
    ws.send(queryRequest);
  }
}

class MoveRight extends Component {
  render() {
    var char = '>';
    return(
      <button onClick = {this.handleClick} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetWidth += 1;
    get();
  }
}

class MoveLeft extends Component {
  render() {
    var char = '<';
    return(
      <button onClick = {this.handleClick} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetWidth -= 1;
    get();
  }
}

class MoveUp extends Component {
  render() {
    var char = '^';
    return(
      <button onClick = {this.handleClick} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetHeight -= 1;
    get();
  }
}

class MoveDown extends Component {
  render() {
    var char = 'v';
    return(
      <button onClick = {this.handleClick} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetHeight += 1;
    get();
  }
}

class MoveReset extends Component {
  render() {
    var char = 'Re-Centre';
    return(
      <button onClick = {this.handleClick} className={'moveButton'}>
      {char}
      </button>
    );
  }

  handleClick() {
    offsetHeight = 0;
    offsetWidth = 0;
    get();
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
