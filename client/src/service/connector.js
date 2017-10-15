const logger = console;

/* Cell wars protocol in a nutshell:
 * Q: "%s: %m %f*"
 * R: "%s: %a %f*"
 * f: "%d=%v*"
 * Q = query
 * R = response
 * s = sequence number, client issued have 1..100, server issued have 101..200
 * m = method, string in capital letters
 * f = field containing no whitespace
 * d = data identifier
 * v = value identifier
 */

function varParse(data){
  let ans = {};
  data.split(" ").forEach((field) => {
    if(field === ""){
      return;
    }
    let info = field.split("=")
    ans[info[0].trim()] = info[1].trim();
  });
  return ans;
}

function parseCWProto(wsMessage){
  console.log(wsMessage);
  let seqStr = wsMessage.split(":", 1).toString();
  let raw = wsMessage.slice(seqStr.length + 1).trim();
  let seq = parseInt(seqStr, 10);
  let incoming = undefined;
  let method = undefined;
  let status = undefined;
  let data = {};
  if((seq > 0) && (seq <= 100)){
    incoming = false;
    let statusStr = raw.split(" ", 1).toString()
    status = parseInt(statusStr, 10);
    data = varParse(raw.slice(statusStr.length).trim());
  } else { // server issued method
    incoming = true;
    method = raw.split(" ", 1).toString().toUpperCase();
    data = varParse(raw.slice(method.length).trim());
  }
  return {
    sequence: seq,
    method: method,
    status: status,
    incoming: incoming,
    answer: false,
    data: data
  };
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

  send(stuff){
    if(this.ws.readyState !== this.ws.OPEN){
      throw new Error("Websocket not opened");
    }
    this.ws.send(stuff);
  }

  reset(){
    this.contextList = [];
    this.listeners = [];
    this.ws = undefined;
    this.errCount = 0;
    this.nextSeq = 0;
    this.initWS();
  }

  getSeq(){
    this.nextSeq = (this.nextSeq % 100) + 1;
    return this.nextSeq;
  }

  initWS(){
    this.ws = new WebSocket(this.address);
    this.ws.onmessage = this.handleMessage.bind(this);
    this.ws.onclose = this.handleClose.bind(this);
    this.ws.onerror = this.handleError.bind(this);
  }

  manualRetry(){
    this.errCount = 1;
    this.initWS();
  }

  findContext(id){
    let context = this.contextList[id]
    if(context === undefined){
      console.warn("unassigned sequence:", id);
    }
    this.contextList[id] = undefined;
    return context;
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
      context["_cwCall"](details.data, context);
    }
  }

  handleClose(){
    this.errCount += 1;
    if(this.errCount < 3){
      this.initWS();
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

  getDB(callback, context = {}){
    context["_cwCall"] = callback;
    let seq = this.getSeq();
    this.contextList[seq] = context;
    this.send(seq.toString() + ": DATABASE");
  }

  getScore(playerType, callback, context = {}){
    context["_cwCall"] = callback;
    let seq = this.getSeq();
    this.contextList[seq] = context;
    this.send(seq.toString() + ": SCORE t=" + playerType.toString());
  }

  selectType(playerType, callback, context = {}){
    context["_cwCall"] = callback;
    let seq = this.getSeq();
    this.contextList[seq] = context;
    this.send(seq.toString() + ": PICK t=" + playerType.toString());
  }

  update(px, py, t, callback, context = {}){
    context["_cwCall"] = callback;
    let seq = this.getSeq();
    this.contextList[seq] = context;
    this.send(seq.toString() + ": UPDATE px=" + px.toString() + 
                                     " py=" + py.toString() + 
                                     " t=" + t.toString()
    );
  }

  fetchDetails(callback, context = {}){
    context["_cwCall"] = callback;
    let seq = this.getSeq();
    this.contextList[seq] = context;
    this.send(seq.toString() + ": DETAILS");
  }
}
