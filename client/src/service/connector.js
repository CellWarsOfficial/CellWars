const logger = console;

parseCWProto(wsMessage){
  let aux;
  aux = wsMessage.split(":", 1).toString();
  let seq = parseInt(aux);
  let raw = wsMessage.cut(aux.length + 1).trim();
  if((seq > 0) && (seq <= 100)){
    return { sequence: seq, incoming: false, answer: false, data: raw};
  } else { // server issued method
    let method = raw.split(" ", 1).toString().toUpperCase();
    if((method === "CRANKFIN") || 
       (method === "TIMEOUT") || 
       (method === "LOST")){
      return {sequence: seq, incoming: true, answer: false, data: method};
    }
    console.warn("unrecognised method:", method);
    return {sequence: seq, incoming: true, answer: false, data: method};
  }
}

export default class Connector{
  constructor(address){
    this.address = address;
    this.contextList = [];
    this.listeners = [];
    this.ws = undefined;
    this.errCount = 0;
    this.nextSeq = 0;
    this.initWS();
  }

  getSeq(){
    nextSeq = (nextSeq % 100) + 1;
    return nextSeq;
  }

  initWS(){
    this.ws = new WebSocket(address);
    this.ws.onmessage = this.handleMessage.bind(this);
    this.ws.onclose = this.handleClose.bind(this);
    this.ws.onerror = this.handleError.bind(this);
  }

  manualRetry(){
    this.errCount = 1;
    this.initWS();
  }

  findContext(id){
    if(contextList[id] === undefined){
      console.warn("unassigned sequence:", id);
    }
    return contextList[id];
  }

  notifyListeners(method, data){
    this.listeners
      .filter((entry) => {return entry.method === method;})
      .forEach((entry) => {entry.callback(data);});
  }

  handleMessage(event){
    let details = parseCWProto(event.data);
    if(details.incoming){
      this.notifyListeners(details.method, details.data);
    } else {
      let context = this.findContext(details.sequence);
      context.callback(details.data, context);
    }
  }

  handleClose(){
    this.errCount += 1;
    if(this.errCount < 3){
      initWS();
    } else {
      this.handleError(new Error("Maintainance"));
    }
  }

  handleError(errData){
    logger.error(errData);
  }

  on(event, callback){
    this.listeners.push({method: event, callback:callback});
  }

  /* Promise based returns */
  getDb(){
    let that = this;
    return new Promise((resolve, reject) => {
      let seq = this.getSeq();
      contextList[seq] = {callback: (data) => {
        let aux = data.split(" ");
        if(aux[0]){
          resolve(aux[1]);
        } else {
          reject();
        }
      }};
      that.send(seq.toString + ": DATABASE");
    });
  }

  /* Promise based returns */
  getScore(){
    let that = this;
    return new Promise((resolve, reject) => {
      let seq = this.getSeq();
      contextList[seq] = {callback: (data) => {
        let aux = data.split(" ");
        if(aux[0]){
          resolve(aux[1]);
        } else {
          reject();
        }
      }};
      that.send(seq.toString + ": DATABASE");
    });
  }
}
