import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

const width = 20;
const height = 20;
const players = 10; //determines colour generation

const x1Home = width/2 - 2;
const x2Home = width/2 + 2;
const y1Home = height/2 - 2;
const y2Home = height/2 + 2;

class App extends Component {
  render() {
    return (
      <div className="App">
        <div className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <h2>Cell Wars</h2>
        </div>
        <div className="App-grid">
          <Grid/>
        </div>
      </div>
    );
  }
}

function rainbow(n) {
    if (n === 0) { return '#FFFFFF'}
    n = n * 240 / players;
    return 'hsl(' + n + ',100%,50%)';
}

function ColourBlindSquare(props) {
  return (
    <button className="square" onClick={props.onClick} style={{backgroundColor:rainbow(props.userID)}}>
      {props.userID}
    </button>
  );
}

function Square(props) {
  return (
    <button className="square" onClick={props.onClick} style={{backgroundColor:rainbow(props.userID)}}>
    </button>
  );
}

class Row extends Component {
  renderSquare(userIDturtle, r, c) {
    return (
      <Square
      userID={userIDturtle}
      onClick={() => this.props.onClick(r, c)}
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

class Grid extends Component {
  constructor() {
    super();
    this.state = {
      board: fittedExample()
    }
  }

  handleClick(clickedRow, clickedCol) { // Would prefer to only copy and set the one peice that i have to rather than the whole thing but i dont know how
    var temp = this.state.board;

    if (clickedRow < y1Home || clickedRow >= y2Home ||
        clickedCol < x1Home || clickedCol >= x2Home) {
      window.alert("You may only toggle cells in your home area.");
      return;
    }

    if (temp[clickedRow][clickedCol] === 0) {
      temp[clickedRow][clickedCol] = 1;
    } else {
      temp[clickedRow][clickedCol] = 0;
    }
    this.setState({
      board:temp
    });
  }

  renderRow(r) {
    return (
      <div>
        <Row
          squares={this.state.board[r]}
          theRow={r}
          onClick={(r, c) => this.handleClick(r, c)}
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