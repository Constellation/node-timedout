var timedout = require('./build/Release/timedout.node');

for (var i = 0; i < 10; ++i) {
  timedout.timedout(function() {
    for (var i = 0; i < 100;++i) { }
  }, 1000);
}

console.log("END");
//timedout.timedout(function() {
//  try {
//    for(var i = 0; i < 1e1000; ++i) { }// console.log(i); }// if (i % 1000000 === 0) console.log(i); }
//    // while (true) { } //console.log("HELLO"); }// if (i % 1000000 === 0) console.log(i); }
//  } catch(e) {
//    console.log("HELLOW", e);
//  }
//  console.log("END!!");
//}, 1000);
