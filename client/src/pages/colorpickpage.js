import React, { Component } from 'react';
import playerTypes from '../data/playertypes';
import Grid from '../data/grid';
/*class UserPicker extends Component {
  render() {
    if (this.props.currentPage !== INTRO_PAGE)
    {
     return null;
    }

    var cols = [];
    for (let i = 0; i < Math.floor(players / cellPerRow); i++) {
      cols.push(<GenCol key={"r=".concat(i*cellPerRow)} start={i*cellPerRow} onClick={(a) => this.handleClick(a)} />);
    }

    return (<div><h2>Pick your colour</h2><br></br><table className="table">
      <tbody>
      {cols}
      </tbody>
    </table>
    <br></br>
    <br></br>
    <h5>Based on the cellular automaton, <a href="https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life">Conway's Game of Life - https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life</a></h5>
    </div>);

  }

  handleClick(uid) {
    yourUserID = uid;
    requestColor(yourUserID);
    this.props.onClick();
  }

}*/

export default class ColorPickPage extends Component {

  static identifier = "ColorPickPage";

  constructor() {
    super();
    console.log("con called");
    this.identifier = ColorPickPage.identifier;
  }

  render() {
    if(this.props.root.shouldIRender(this)){
      return(
        <Grid dataset = {{data:[[7,7,7],[7,7,7]]}}/>
      );
    }
    return null;
  }
}
