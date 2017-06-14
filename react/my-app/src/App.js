import React, { Component } from 'react';
import logo from './logo.svg';
import pac_thing from './images/pac_thing.png';
import './App.css';

var ws;

var width = 20;
var height = 20; //dimensions of the board
const headerHeight = 230;

const players = 10; //determines how colours are split between userID's
var yourUserID = 1;

const x1Home = width/2 - 2;
const x2Home = width/2 + 2;
const y1Home = height/2 - 2;
const y2Home = height/2 + 2; //location of the editable home region

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


class App extends Component {
  constructor() {
    super();
    this.state = {
      displayMode: 0,
      currentPage: INTRO_PAGE,
    }
  }


  get() {
    // TODO
    window.alert("not yet implemented");
  }

  submit() {
    // TODO
    window.alert("Not yet implemented.");
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
          <h1>Cell Wars</h1>
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
          <GetButton onClick={() => this.get()} currentPage = {this.state.currentPage}/> &nbsp;
          <SubmitButton onClick={() => this.submit()} currentPage = {this.state.currentPage}/> &nbsp;
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

class SubmitButton extends Component {
  render() {
  if (this.props.currentPage !== GAME_PAGE) {
    return null;
  }
  return (
      <button onClick = {this.props.onClick} className={'roundButton'}>
      Submit
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
function Square(props) {
  var label = '';
  if (props.isColourBlind) {
    label = props.userID; //Write the userIDs on squares when colourblind mode is on
  }
  var border = '';
  if (props.inHomeArea) {
    border = '1px solid #200' //Home area cells get a border
  }

  return (
    <button
      className="square"
      onClick={props.onClick}
      style={{backgroundColor:rainbow(props.userID), border:border}}>
      {label}
    </button>
  );
}
*/




function ImgSquare(props) {
  var src = null;


  if (props.displayMode === DISPLAYMODE.COLOURS.value) {
    src = pac_thing;
  } else if (props.displayMode === DISPLAYMODE.EMOJIS.value) {
    src = 'https://www.doc.ic.ac.uk/project/2016/271/g1627123/images/emojis/'.concat((props.userID).toString()).concat('.png');
  }

  if (props.userID === 0) {
    src = pac_thing;
  }
  var style = {display: 'inline-block'};
  if (props.inHomeArea) {
    style = {backgroundColor: 'red', display: 'inline-block'};
  }

  return (
    <div style={style}>
    <input
    type="image"
    alt="cell"
    src={src}
    onClick={props.onClick}
    style={{width:20, height:20, backgroundColor:rainbow(props.userID), border:'1px solid #ddd'}}
    className='cell'>
    </input>
    </div>
    );

}




function outsideHomeArea(row, col) {
  return (row < y1Home || row >= y2Home ||
          col < x1Home || col >= x2Home);
}

class Row extends Component {
  renderSquare(userIDturtle, row, col) {
    return (
      <ImgSquare
      userID={userIDturtle}
      onClick={() => this.props.onClick(row, col)}
      displayMode={this.props.displayMode}
      inHomeArea={!outsideHomeArea(row, col)}
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




function fittedExample(width, height) { // Generates an example board fitting to the width and height global variables
  var ret = [];
  for (var i = 0; i < height; i++) {
    var row = [];
    for (var j = 0; j < width; j++) {
      var temp = 0;
      if (i < height/2) {
        temp+=3;
      }
      if (j < width/2) {
        temp+=5;
      }
      row.push(temp);
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
      <tr className = "textcenter">
        <td width = "75" onClick={() => this.handleClick(1)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(1), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(2)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(2), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(3)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(3), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(4)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(4), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(5)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(5), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(6)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(6), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(7)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(7), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(8)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(8), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(9)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(9), cursor:'pointer'}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(10)}><img alt="pacman" src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(10), cursor:'pointer'}}></img></td>    
      </tr>
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
    this.updateDimensions();
    this.state = {
      board: fittedExample(width, height)
    }

    var url = "ws".concat(window.location.toString().substring(4));
    ws = new WebSocket(url);
    ws.onopen = function() {
      console.log("web socket opened : ".concate(url));
    }
    ws.onclose = function() {
      console.log("web socket closed : ".concat(url));
    }
    ws.onerror = function() {
      console.log("web socket error");
    }
    ws.onmessage = function (evt)
    {
      // TODO: update board here, this should be triggered upon sending a query
      var received_msg = evt.data;
      console.log("web socket message received: ".concat(received_msg));
    }

  }

  handleClick(row, col) {
    if (outsideHomeArea(row, col)) {
      window.alert("You may only toggle cells in your home area.");
      return;
    }

    const board = this.state.board.slice(); //Clones the board

    board[row][col] = (board[row][col] === 0) ? yourUserID : 0;
    this.setState({
      board: board
    });

    ws.send("UPDATE px=".concat(row.toString())
      .concat(" py=")
      .concat(col.toString())
      .concat(" t=")
      .concat(board[row][col].toString()));
    ws.close();
  }

  renderRow(row) {
    return (
      <div>
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
    return <div>{rows}</div>;
  }

  componentDidMount() {
    window.addEventListener("resize", () => {this.updateDimensions(); this.updateBoard(); this.forceUpdate()});
  }

  updateDimensions() {
    width = Math.floor(window.innerWidth / 50);
    height = Math.floor((window.innerHeight - headerHeight) / 50);
  }

  updateBoard() {
    this.setState({
      board: fittedExample(width, height)
    });
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
    return (
    <div onClick={this.props.onClick}>
    <h1>Cell Law I</h1>
    <h2>Any live cell with fewer than two live neighbours will die, as if caused by underpopulation.</h2>
    </div>
    );
  }
}

class RulePage2 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_2) {
      return null;
    }
    return (
    <div onClick={this.props.onClick}>
    <h1>Cell Law II</h1>
    <h2>Any live cell with two or three live neighbours lives on to the next generation.</h2>
    </div>
    );
  }
}

class RulePage3 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_3) {
      return null;
    }
    return (
    <div onClick={this.props.onClick}>
    <h1>Cell Law III</h1>
    <h2>Any dead cell with exactly 3 live neighbours and a majority cell type can be identified
        among them, the cell will be reborn as the same type as the majority.</h2>
    </div>
    );
  }
}

class RulePage4 extends Component {
  render() {
    if (this.props.currentPage !== RULE_PAGE_4) {
      return null;
    }
    return (
    <div onClick={this.props.onClick}>
    <h1>Cell Law IV</h1>
    <h2>Any live cell with greater than three live neighbours dies, as if caused by overpopulation.</h2>
    </div>
    );
  }
}

export default App;