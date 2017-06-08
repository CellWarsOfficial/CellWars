import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';

class Game extends React.Component {
  constructor() {
    super();
    this.state = {
      data: [{
        row: Array(60).fill(null),
      }],
    };
    this.initTable();
  }

  initTable() {
    for (let i = 0; i < 40; i++) {
      const data = this.state.data;
      const row = data[0].row.slice();
      this.state = ({
        data: data.concat([{
          row: row
        }]),
      });
    }
    this.state = ({
      data: this.state.data,
      player: "bluecell",
    });
    return;
  }

  updateTable() {
    // ADD CELL INFO HERE WITH BASH SCRIPT
    const data = this.state.data;
    data[3].row[4] = "redcell";
    data[13].row[14] = "greencell";
    data[33].row[24] = "bluecell";
    return;
  }

  render() {
    this.updateTable();
    let rows = this.state.data.map(r =>
    {
      return <Row value = {r.row} player = {this.state.player} />
    })


    return <table width = "100%">
        <tbody> {
          rows
        }
        </tbody>
      </table>  }

}

const Row = (props) => {
  let cols = props.value.map(c =>
  {return <Col value = {c} player = {props.player}/>})

  return (
    <tr className = "textcenter">
    <tbody> {
      cols
    }
    </tbody>
    </tr>
    );
}

const Col = (props) => {
  return (
      <td width = "75" className={props.value}>
      </td>
    );
}


// ========================================

ReactDOM.render(
  <Game />,
  document.getElementById('root')
);
