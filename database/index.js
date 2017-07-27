const express = require("express");
const cors = require("cors");
const sequelize = require("sequelize");
const morgan = require("morgan");
const cfgLoader = require("config.json");

const cfg = cfgLoader('./config.json')
const svPort = process.env.PORT || cfg.svDefPort;

const server = express();

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
  return obj;
}

function init(){
  server
    .use(cors())
    .use(morgan('dev'))
    .get("/cells", (req, res, next) => {
      try{
        let details = sanitize(req.query);
        res.status(200).send("Not implemented just yet");
      } catch (err) {
        res.status(400).send(err);
      }
    })
    .listen(svPort, function () {
      console.log('Example app listening on port ' + svPort.toString() + '!');
    });
}

init();
