function loadTable() {

  var cols = 60;
  var rows = 40;

  var myTableDiv = document.getElementById("dynamicTable");

  // Creating the table
   
  var table = document.createElement("TABLE");
  table.id = "cellTable";
  table.width = "100%";

  var tableBody = document.createElement("TBODY");
  table.appendChild(tableBody);

  // Filling the table

  for (var j = 0; j < rows; j++) {
    var tr = document.createElement("TR");
    tr.classList.add("textcenter");
    tableBody.appendChild(tr);

    for (var i = 0; i < cols; i++) {
      var td = document.createElement("TD");
      td.width = "75";
      td.appendChild(document.createTextNode(""));
      tr.appendChild(td);
    }
  }

  myTableDiv.appendChild(table);

}