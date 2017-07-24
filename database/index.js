const express = require("express");
const cors = require("cors");
const sequelize = require("sequelize");
const server = express();

function init(){
  server.use(cors());
  console.log(server);
  express.Router().get("/cells", (req, res, next) => {
    res.status(500).send("Not implemented just yet");
  });
}

init();