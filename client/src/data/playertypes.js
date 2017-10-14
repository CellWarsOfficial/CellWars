export default class ColorInfo{

  static rainbow(n){
    var b = 255 - (n % 4) * 64;
    var g = 255 - ((n / 4) % 4) * 64;
    var r = 255 - ((n / 16) % 4)* 64;
    return new ColorInfo(r, g, b);
  }

  constructor(r, g, b){
    this.r = r;
    this.g = g;
    this.b = b;
  };

  toString(){
    return 'rgb(' + this.r + ',' + this.g + ',' + this.b + ')';
  }
}

function generateTypes(){
  let i = 0;
  let ret = [];
  while (i < 256){
    ret[i] = ColorInfo.rainbow(i);
    i++;
  }
  return ret;
}

const allColors = generateTypes();
let playableColours = allColors.slice()
playableColours[0] = false;

export {ColorInfo, playableColours, allColors};
