import React, { Component } from 'react';

export default class Grid extends Component{

  constructor(){
    super();
  }

  render(){
    this.origin = this.props.dataset;
    let rowInfo = this.origin.data.reduce((rAcc, row) => {
      return rAcc.concat(Grid.renderRow(row));
    } ,[]);
    return (
      <table>
        {rowInfo}
      </table>
    );
  }

  static renderRow(row){
    let cellInfo = row.reduce((cAcc, cell) => {
      return cAcc.concat(Grid.renderCell(cell));
    },[]);
    return (
      <tr>
        {cellInfo}
      </tr>
    );
  }

  static renderCell(cell){
    return (
      <td>
        {cell.toString()}
      </td>
    );
  }
}
