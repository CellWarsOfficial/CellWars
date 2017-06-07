import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

const width = 20;
const height = 20; //dimensions of the board

const players = 10; //determines how colours are split between userID's

const x1Home = width/2 - 2;
const x2Home = width/2 + 2;
const y1Home = height/2 - 2;
const y2Home = height/2 + 2; //location of the editable home region




class App extends Component {
  constructor() {
    super();
    this.state = {
      isColourBlind: false
    }
  }

  toggleColourBlind() {
    this.setState({
      isColourBlind: !this.state.isColourBlind
    });
  }

  render() {
    return (
      <div className="App">
        <div className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <h2>Cell Wars</h2>
        </div>
        <div className="App-grid">
          <Grid isColourBlind = {this.state.isColourBlind}/>
        </div>
        <div className="Colour-blind-toggler">
          <ColourBlindToggler onClick={() => this.toggleColourBlind()}/>
        </div>
      </div>
    );
  }
}

//<ColourBlindToggler onClick={() => this.toggleColourBlind()}/>


class ColourBlindToggler extends Component {
  render() {
    return (
      <button className="colour-blind-button" onClick={this.props.onClick}>
      Colour Blind
      </button>
    );
  }
}




function rainbow(n) {
    if (n === 0) { return '#FFFFFF'} //userID 0 is always mapped to white
    n = n * 240 / players; //splits assigned colours based on the number of players
    return 'hsl(' + n + ',100%,50%)';
}

function Square(props) {
  var label = null;
  if (props.isColourBlind) {
    label = props.userID; //Write the userIDs on squares when colourblind mode is on
  }
  var borderz = '1px solid #999'
  if (props.inHomeArea) {
    borderz = '1px solid #200'
  }

  return (
    <button
      className="square"
      onClick={props.onClick}
      style={{backgroundColor:rainbow(props.userID), border:borderz}}>
      {label}
    </button>
  );
}




class Row extends Component {
  renderSquare(userIDturtle, r, c) {
    const inHomeArea = !outsideHomeArea(r, c);
    return (
      <Square
      userID={userIDturtle}
      onClick={() => this.props.onClick(r, c)}
      isColourBlind={this.props.isColourBlind}
      inHomeArea={inHomeArea}
      />
    );
  }

  render() {
    var formatSquares = [];
    for (var i = 0; i < width; i++) {
      formatSquares.push(this.renderSquare(this.props.squares[i], this.props.theRow, i));
    }
    return (
      <div>{formatSquares}</div>
    );
  }
}




function fittedExample() {
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

function outsideHomeArea(row, col) {
  return (row < y1Home || row >= y2Home ||
          col < x1Home || col >= x2Home);
}

class Grid extends Component {
  constructor() {
    super();
    this.state = {
      board: fittedExample()
    }
  }

  handleClick(clickedRow, clickedCol) {
    const board = this.state.board.slice();
    if (outsideHomeArea(clickedRow, clickedCol)) {
      window.alert("You may only toggle cells in your home area.");
      return;
    }

    var clickedCell = board[clickedRow][clickedCol];
    if (clickedCell === 0) {
      clickedCell = 1;
    } else {
      clickedCell = 0;
    }
    board[clickedRow][clickedCol] = clickedCell;
    this.setState({
      board: board
    });
  }

  renderRow(r) {
    return (
      <div>
        <Row
          squares={this.state.board[r]}
          theRow={r}
          onClick={(r, c) => this.handleClick(r, c)}
          isColourBlind={this.props.isColourBlind}
        />
      </div>
    );
  }

  render() {
    var rows = [];
    for (var i = 0; i < height; i++) {
      rows.push(this.renderRow(i));
    }
    return <div>{rows}</div>;
  }
}




export default App;