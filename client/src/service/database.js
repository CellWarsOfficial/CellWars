export default class Database{
  constructor(address){
    this.address = address;
    if(!(this.use("HEAD", "?px1=0&px2=0&py1=0&py2=0"))){
      return undefined;
    };
  }

  getCells(range, callback){
    let opt = "?px1=" + range.x1 +
              "&px2=" + range.x2 +
              "&py1=" + range.y1 +
              "&py2=" + range.y2;
    this.use("GET", opt, (data) => {
      callback(JSON.parse(data));
    });
  }

  use(method, options, callback = false){
    let xhr = new XMLHttpRequest();
    let wait = false;
    if(callback){
      wait = true;
      xhr.onreadystatechange = () => {
        if(xhr.readyState === 4){
          if(Math.floor(xhr.status / 100) === 2){
            callback(xhr.response);
          } else {
            callback(false);
          }
        }
      };
    }
    xhr.open(method, this.address + options, wait);
    xhr.setRequestHeader('Cache-Control', 'no-cache');
    xhr.send();
    return xhr.response;
  }
}
