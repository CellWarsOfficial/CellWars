import React, { Component } from 'react';
import {allColors} from './playertypes';

const cellImg = 'images/idle_cell.gif';

export default class Grid extends Component{

  render(){
    let dataset = this.props.dataset;
    let callback = this.props.onClick;
    let region = this.props.region;
    let cellRenderer = Grid.cellRenderers[this.props.display];
    let renderedRows = [];
    let reducedDS = dataset.filter((cell) => {
      return (cell.x >= region.x1) && (cell.x <= region.x2) &&
             (cell.y >= region.y1) && (cell.y <= region.y2);
    });
    for(let row = region.x1; row <= region.x2; row++){
      let renderedCells = [];
      let rowDS = reducedDS.filter((cell) => {
        return cell.x === row;
      });
      for(let col = region.y1; col <= region.y2; col++){
        let nextCell = rowDS.find((cell) => {
          return cell.y === col;
        });
        if(nextCell === undefined){
          nextCell = {x:row, y:col, t:0};
        }
        let customCallback = (event) => {
          return callback(nextCell, event);
        };
        renderedCells.push(cellRenderer(nextCell, customCallback));
      }
      renderedRows.push(Grid.renderRow(renderedCells))
    }
    return (
      <table>
        {renderedRows}
      </table>
    );
  }

  static renderRow(content){
    return (
      <tr>
        {content}
      </tr>
    );
  }

  static renderCellStatic(cell, callback){
    return (
      <td className='cellContainer'>
        <input
          type="button"
          alt="cell"
          value={("00" + cell.t.toString(16)).substr(-2)}
          onClick={callback}
          style={{backgroundColor:allColors[cell.t].toString(),color:allColors[cell.t].contrast().toString()}}
          className='cell'
        >
        </input>
      </td>
    );
  }

  static renderCellImage(cell, callback){
    return (
      <td className='cellContainer'>
        <input
          type="image"
          alt="cell"
          src={cellImg}
          onClick={callback}
          style={{backgroundColor:allColors[cell.t].toString()}}
          className='cell'
        >
        </input>
      </td>
    );
  }

  static cellRenderers = [
    Grid.renderCellStatic,
    Grid.renderCellImage
  ]
}
