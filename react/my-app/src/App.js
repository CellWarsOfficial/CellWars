import React, { Component } from 'react';
import logo from './logo.svg';
import pac_thing from './pac_thing.png';
import './App.css';

const width = 20;
const height = 20; //dimensions of the board

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



class App extends Component {
  constructor() {
    super();
    this.state = {
      displayMode: 0,
      isVisible: false,
    }
  }


  get() {
    // TODO
    window.alert("Not yet implemented.");
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

  handleClick() {
    this.setState({isVisible: true});
  }

  render() {
    return (
      <div className="App">
        <div className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <h2>Cell Wars</h2>
        </div>
        <br></br>
        <br></br>
        <br></br>
        <div className="App-grid">
          <UserPicker isVisible = {this.state.isVisible} onClick={() => this.handleClick()}/>
          <Grid displayMode = {this.state.displayMode} isVisible = {this.state.isVisible}/>
        </div>
        <p></p>
          <DisplayModeAdvancer onClick={() => this.advanceDisplayMode()} isVisible = {this.state.isVisible}/> &nbsp;
          <GetButton onClick={() => this.get()} isVisible = {this.state.isVisible}/> &nbsp;
          <SubmitButton onClick={() => this.submit()} isVisible = {this.state.isVisible}/> &nbsp;
      </div>
    );
  }
}



class SubmitButton extends Component {
  render() {
  if (!this.props.isVisible) {
    return null;
  }
  return (
      <button onClick = {this.props.onClick}>
      Submit
      </button>
    );
  }
}

class GetButton extends Component {
  render() {
  if (!this.props.isVisible) {
    return null;
  }
  return (
      <button onClick = {this.props.onClick}>
      Get
      </button>
    );
  }
}


class DisplayModeAdvancer extends Component {
  render() {
  if (!this.props.isVisible) {
    return null;
  }
    return (
      <button className="display-mode-button" onClick={this.props.onClick}>
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




function addHexColor(c1, c2) {
  var hexStr = (parseInt(c1, 16) + parseInt(c2, 16)).toString(16);
  while (hexStr.length < 5) { hexStr = '0' + hexStr; } // Zero pad.
  return hexStr;
}




function ImgSquare(props) {
  var src = null;

  var border = '1px solid #ddd';
  if (props.inHomeArea) {
    border = '1px solid #200' //Home area cells get a border
  }

  if (props.displayMode === DISPLAYMODE.COLOURS.value) {
    src = pac_thing;
  } else if (props.displayMode === DISPLAYMODE.EMOJIS.value) {
    var hexRet = addHexColor("1f638", (props.userID).toString());
    src = "//cdn.jsdelivr.net/emojione/assets/png/".concat(hexRet).concat(".png");
  }

  return (
    <input
    type="image"
    src={src}
    onClick={props.onClick}
    style={{width:20, height:20, backgroundColor:rainbow(props.userID), border:border}}>
    </input>
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
        temp++;
      }
      if (j < width/2) {
        temp++;
      }
      row.push(temp);
    }
    ret.push(row);
  }
  return ret;
}


class UserPicker extends Component {
  render() {
    if (this.props.isVisible)
    {
     return null;
    }

    return (<div><br></br><h2>Pick your player</h2><br></br><br></br><br></br><table width="100%">
      <tr className = "textcenter">
        <td width = "75" onClick={() => this.handleClick(1)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(1)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(2)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(2)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(3)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(3)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(4)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(4)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(5)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(5)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(6)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(6)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(7)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(7)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(8)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(8)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(9)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(9)}}></img></td>    
        <td width = "75" onClick={() => this.handleClick(10)}><img src={pac_thing} style={{width:100, height:100, backgroundColor:rainbow(10)}}></img></td>    
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
    this.state = {
      board: fittedExample(width, height)
    }
  }

  handleClick(row, col) {
    const board = this.state.board.slice(); //Clones the board
    if (outsideHomeArea(row, col)) {
      window.alert("You may only toggle cells in your home area.");
      return;
    }

    board[row][col] = (board[row][col] === 0) ? yourUserID : 0;
    this.setState({
      board: board
    });
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
    if (!this.props.isVisible) {
      return null;
    }
    var rows = [];
    for (var i = 0; i < height; i++) {
      rows.push(this.renderRow(i));
    }
    return <div>{rows}</div>;
  }
}



export default App;