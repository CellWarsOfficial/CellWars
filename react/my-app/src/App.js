import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

const width = 20;
const height = 20; //dimensions of the board

const players = 10; //determines how colours are split between userID's
const yourUserID = 1;

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

  get() {
    // TODO
    window.alert("Not yet implemented.");
  }

  submit() {
    // TODO
    window.alert("Not yet implemented.");
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
        <p></p>
          <ColourBlindToggler onClick={() => this.toggleColourBlind()}/> &nbsp;
          <GetButton onClick={() => this.get()}/> &nbsp;
          <SubmitButton onClick={() => this.submit()}/> &nbsp;
      </div>
    );
  }
}



class SubmitButton extends Component {
  render() {
  return (
      <button onClick = {this.props.onClick}>
      Submit
      </button>
    );
  }
}

class GetButton extends Component {
  render() {
  return (
      <button onClick = {this.props.onClick}>
      Get
      </button>
    );
  }
}


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



function outsideHomeArea(row, col) {
  return (row < y1Home || row >= y2Home ||
          col < x1Home || col >= x2Home);
}

class Row extends Component {
  renderSquare(userIDturtle, row, col) {
    return (
      <Square
      userID={userIDturtle}
      onClick={() => this.props.onClick(row, col)}
      isColourBlind={this.props.isColourBlind}
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




function fittedExample() { // Generates an example board fitting to the width and height global variables
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