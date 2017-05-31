function refreshCells() {

  // connectSQL()

  document.getElementById("cellTable").rows[0].cells[0].className = "redcell";
  document.getElementById("cellTable").rows[0].cells[1].className = "redcell";
  document.getElementById("cellTable").rows[1].cells[1].className = "redcell";

  document.getElementById("cellTable").rows[3].cells[3].className = "bluecell";
  document.getElementById("cellTable").rows[2].cells[3].className = "bluecell";

  document.getElementById("cellTable").rows[4].cells[4].className = "greencell";

  document.getElementById("cellTable").rows[2].cells[2].className = "emptycell";    // proof of concept to reset cell
  
}

function connectSQL() {

// TODO: completley change

  var connection = new ActiveXObject("ADODB.Connection") ;

  var connectionstring="Data Source=db.doc.ic.ac.uk;Initial Catalog=g1627123_u;User ID=g1627123_u;Password=eyNCD7YSQJ;Provider=SQLOLEDB";

  connection.Open(connectionstring);
  var rs = new ActiveXObject("ADODB.Recordset");

  rs.Open("SELECT * FROM agents.grid", connection);
  rs.MoveFirst
  while(!rs.eof)
  {
    document.write(rs.fields(1));
    rs.movenext;
  }

  rs.close;
  connection.close; 

}