function loadTable() {

  var gridSize = 10;

  var myTableDiv = document.getElementById("dynamicTable");

  // Creating the table
   
  var table = document.createElement("TABLE");
  table.id = "cellTable";
  table.width = "100%";

  var tableBody = document.createElement("TBODY");
  table.appendChild(tableBody);

  // Filling the table

  for (var j = 0; j < gridSize; j++) {
    var tr = document.createElement("TR");
    tr.classList.add("textcenter");
    tableBody.appendChild(tr);

    for (var i = 0; i < gridSize; i++) {
      var td = document.createElement("TD");
      td.width = "75";
      td.appendChild(document.createTextNode("Z"));
      tr.appendChild(td);
    }
  }

  myTableDiv.appendChild(table);

}