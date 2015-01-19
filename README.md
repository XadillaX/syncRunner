Sync Runner
==========

To execute a binary executable file and return its standard output synchronization. (node.js)

> Now **syncRunner** is supporting [Nodejs](http://nodejs.org/) `v0.11.*` and
> [iojs](http://iojs.org/) `v1.*`!

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

### v0.1.0

2015/1/19

+ Support Nodejs v0.11.\* and iojs v1.\*.

### v0.0.5

2014/5/26

+ MacOS & Linux support added. (Be careful with Linux, may have Zombie if time exceeded. I'm seeking the solution. MacOS will be OK)

### v0.0.1

2014/4/27

+ Windows only
