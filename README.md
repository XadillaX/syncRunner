Sync Runner
==========

To execute a binary executable file and return its standard output synchronization. (node.js)

> Now **syncRunner** is supporting [Nodejs](http://nodejs.org/) `v0.11.*` and
> [iojs](http://iojs.org/) `v1.*` and [Nodejs](http://nodejs.org/) `v4.*`!

Installation
----------

```shell
$ npm install sync-runner
```

Usage
----------

```javascript
var run = require("sync-runner");
var result = run("command", "current_work_directory", max_million_seconds);
console.log(result);
```

> `current_work_directory` and `max_million_seconds` are optional. You can pass only `current_work_directory` or `max_million_seconds`, or neither.

Change Log
----------

### v0.1.4

2015/9/16

+ Update NAN to v2.0.9 to support Nodejs v4.0.0.

### v0.1.3

2015/4/26

+ Fix a buffer bug under linux (especially unser CentOS).

### v0.1.2

2015/4/24

+ Fix a compilation bug under windows.

### v0.1.0

2015/1/19

+ Support Nodejs v0.11.\* and iojs v1.\*.

### v0.0.5

2014/5/26

+ MacOS & Linux support added. (Be careful with Linux, may have Zombie if time exceeded. I'm seeking the solution. MacOS will be OK)

### v0.0.1

2014/4/27

+ Windows only
