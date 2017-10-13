const express = require("express");
const cors = require("cors");
const logger = require("morgan")("dev");
const Websocket = require("ws");

const svPort   = process.env.CW_PORT || 3000;
const myAddr   = process.env.CW_MYADDR || "http://localhost";
const hostPath = process.env.CW_SERVPATH || "ws://localhost:7777/database";
const pass     = process.env.CW_SERVPASS || "_";
const path     = "/cells";
const myPath   = myAddr + ":" + svPort + path;

const server = express();

class Database{
  constructor(){
    this.storage = [];
  }

  erase(){
    this.storage = [];
  }

  enrich(data){
    this.storage = this.storage.concat(data);
  }

  select(range){
    return this.storage.filter((elem) => {
      return (elem.x >= range.px1) &&
             (elem.x <= range.px2) &&
             (elem.y >= range.py1) &&
             (elem.y <= range.py2);
    });
  }
}

let serverSelector = 0;
let userSelector = 1;

let databases = [];

function swapRunner(unused){
  let aux = serverSelector;
  serverSelector = userSelector;
  userSelector = aux;
}

function setRunner(data){
  databases[serverSelector].enrich(data);
}

function eraseRunner(unused){
  databases[serverSelector].erase();
}

const runner = {
  SET: setRunner,
  SWAP: swapRunner,
  ERASE: eraseRunner,
}

function sanitize(query){
  let obj = {};
  obj.px1 = parseInt(query.px1);
  obj.px2 = parseInt(query.px2);
  obj.py1 = parseInt(query.py1);
  obj.py2 = parseInt(query.py2);
  if(isNaN(obj.px1 + obj.px2 + obj.py1 + obj.py2)){
    throw {status: 400, data: "Invalid parameters"};
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

function reset_ws(failed)
{
  console.log("Contacting game server");
  ws = new Websocket(hostPath);
  ws.on("error", (err) => {
    if(failed){
      throw err;
    } else {
      console.error(err);
      reset_ws(true);
    };
  });
  ws.on("close", reset_ws);
  ws.on("open", () => {
    ws.send("pass=" + pass + " path=" + myPath);
  });
  ws.on("message", (data) => {
    query = JSON.parse(data);
    console.log("[%d]> %s"
               , Math.floor(new Date().getTime() / 1000)
               , query.action);
    runner[query.action](query.data);
    if(query.action == "SET"){
      setRunner(query.data);
    }
    if(query.action == "SWAP"){
      swapRunner();
    }
    if(query.action == "ERASE"){
      eraseRunner();
    }
  });
}

function init(){
  databases = [new Database(), new Database()];

  reset_ws();

  server
    .use(cors())
    .use(logger)
    .get("/cells", (req, res, next) => {
      try{ // unorthodox code
        let details = sanitize(req.query);
        let cells = databases[userSelector].select(details);
        throw {status: 200, data: cells};
      } catch (response) {
        if(!response.status){
          res.status(500);
        } else {
          res.status(response.status).send(response);
        }
      }
    })
    .listen(svPort, function () {
      console.log("App listening on port " + svPort.toString());
    });
}

init();
