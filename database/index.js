const express = require("express");
const cors = require("cors");
const sequelize = require("sequelize");
const morgan = require("morgan");
const cfgLoader = require("config.json");

const cfg = cfgLoader("./config.json")
const svPort = process.env.PORT || cfg.svDefPort;
const dbName = cfg.dbName;
const dbUser = cfg.dbUser;
const dbPass = cfg.dbPass;
const dbDialect = cfg.dbType;

const server = express();
const db = new sequelize(
  dbName,
  dbUser,
  dbPass,{
  dialect: dbDialect
});

const Cell = db.define("cell", {
  x:{
    type: sequelize.NUMERIC
  },
  y:{
    type: sequelize.NUMERIC
  },
  type:{
    type: sequelize.NUMERIC
  }
});

function sanitize(query){
  let obj = {};
  if(isNaN(query.px1)){
    throw "px1 is not a number";
  } else {
    obj.px1 = query.px1;
  }
  if(isNaN(query.px2)){
    throw "px2 is not a number";
  } else {
    obj.px2 = query.px2;
  }
  if(isNaN(query.py1)){
    throw "py1 is not a number";
  } else {
    obj.py1 = query.py1;
  }
  if(isNaN(query.py2)){
    throw "py2 is not a number";
  } else {
    obj.py2 = query.py2;
  }
  if(obj.px1 > obj.px2){
    var aux = obj.px1;
    obj.px1 = obj.px2;
    obj.px2 = aux;
  }
  if(obj.py1 > obj.py2){
    var aux = obj.py1;
    obj.py1 = obj.py2;
    obj.py2 = aux;
  }
  return obj;
}

function init(){
  db.authenticate()
  .then(() => {
    console.log("Database connected");
  })
  .catch((err) => {
    console.log(err);
  });

  Cell.sync({force: true}).then (() =>{
    console.log("created table");
  });

  server
    .use(cors())
    .use(morgan("dev"))
    .get("/cells", (req, res, next) => {
      try{
        let details = sanitize(req.query);
        Cell.findAll({ 
          where: {
            x: {
              $gte: details.px1,
              $lte: details.px2
            },
            y: {
              $gte: details.py1,
              $lte: details.py2
            }
          } 
        })
        .then((cellData) => {
          res.status(200).send(cellData);
        });
      } catch (err) {
        res.status(400).send(err);
      }
    })
    .listen(svPort, function () {
      console.log("Example app listening on port " + svPort.toString() + "!");
    });
}

init();
