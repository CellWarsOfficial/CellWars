import React, { Component } from 'react';
import Grid from '../data/grid';

export default class PlayPage extends Component {

  static identifier = "PlayPage";

  constructor(props) {
    super(props);
    this.root = props.root;
    this.root.connector.on("CRANKFIN", this.reloader.bind(this))
    this.state = {render: 0, px: 0, py: 0, width:20, height:20};
    this.identifier = PlayPage.identifier;
    this.gridRef = undefined;
    this.cells = [];
    this.magic = 0;
    this.reloader();
  }

  getRange(){
    return {
      x1: this.state.px - Math.floor(this.state.height / 2),
      x2: this.state.px + Math.ceil (this.state.height / 2) - 1,
      y1: this.state.py - Math.floor(this.state.width  / 2),
      y2: this.state.py + Math.ceil (this.state.width  / 2) - 1
    };
  }

  toggleCell(cell) {
    let newColor = cell.t ? 0 : this.root.color;
    this.root.connector.update(
      cell.x,
      cell.y,
      newColor,
      (response) => {
        if(response.accept !== "0"){
          cell.t = newColor;
          if(cell.t && this.cells.find((trial) => {
            return trial.x === cell.x && trial.y === cell.y;
          }) <= 0){
            this.cells.push(cell);
          }
          this.gridRef.setState({update: this.magic++});
        }
      }
    );
  }

  reloader(){
    if(this.state.render){
      this.root.database.getCells(this.getRange(), (cells) => {
        this.cells.length = 0;
        [].push.apply(this.cells, cells.data); // i'm just as confused, but https://stackoverflow.com/questions/23486687/short-way-to-replace-content-of-an-array
        this.gridRef.setState({update: this.magic++});
      });
    }
  }

  render() {
    if(this.state.render){
      let region = this.getRange();
      let callback = this.toggleCell.bind(this);
      return(
        <Grid
          dataset={this.cells}
          region={region}
          onClick={callback}
          ref={(r) => {this.gridRef = r;}}
          display="0"
        />
      );
    }
    return null;
  }
}
