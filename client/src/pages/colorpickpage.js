import React, { Component } from 'react';
import Grid from '../data/grid';

export default class ColorPickPage extends Component {

  static identifier = "ColorPickPage";

  constructor(props) {
    super(props);
    this.root = props.root;
    this.state = {render: 0};
    this.identifier = ColorPickPage.identifier;
    this.available = [];
    this.region = {x1:1, x2:15, y1:1, y2:17}
    let col = 0;
    for(let i = this.region.x1; i <= this.region.x2; i++){
      for(let j = this.region.y1; j <= this.region.y2; j++){
        col++
        this.available.push({x:i, y:j, t:col});
      }
    }
  }

  render() {
    if(this.state.render){
      let callback = (cell) => {
        this.root.setColor(cell.t);
      };
      return(
        <Grid
          dataset={this.available}
          region={this.region}
          onClick={callback}
          display="0"
        />
      );
    }
    return null;
  }
}
