var run = require("../");

var res = run("ping 127.0.0.1", 1000);
console.log(res);

res = run("ls -l");
console.log(res);

res = run("ls", "test");
console.log(res);

res = run("test/test");
console.log(res);

res = run("gcc --version");
console.log(res);
