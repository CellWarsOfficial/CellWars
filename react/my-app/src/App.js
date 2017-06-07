import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

const width = 10;
const height = 10;

class App extends Component {
  render() {
    return (
      <div className="App">
        <div className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <h2>Welcome to React</h2>
        </div>
        <p className="App-intro">
          To get started, edit <code>src/App.js</code> and save to reload.
        </p>
        <div className="App-grid">
          <Grid/>
        </div>
      </div>
    );
  }
}

function Square(props) {
  return (
    <button className="square" onClick={props.onClick}>
      {props.userID}
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

var example = [
  [0, 0, 0, 0, 0, 1, 1, 1, 1, 1],
  [0, 0, 0, 0, 0, 1, 1, 1, 1, 1],
  [0, 0, 0, 0, 0, 1, 1, 1, 1, 1],
  [0, 0, 0, 0, 0, 1, 1, 1, 1, 1],
  [0, 0, 0, 0, 0, 1, 1, 1, 1, 1],
  [2, 2, 2, 2, 2, 0, 0, 0, 0, 0],
  [2, 2, 2, 2, 2, 0, 0, 0, 0, 0],
  [2, 2, 2, 2, 2, 0, 0, 0, 0, 0],
  [2, 2, 2, 2, 2, 0, 0, 0, 0, 0],
  [2, 2, 2, 2, 2, 0, 0, 0, 0, 0]
];

class Grid extends Component {
  constructor() {
    super();
    this.state = {
      board: example
    }
  }

  handleClick(clickedRow, clickedCol) { // Would prefer to only copy and set the one peice that i have to rather than the whole thing but i dont know how
    var temp = this.state.board;
    temp[clickedRow][clickedCol] = 22;
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
