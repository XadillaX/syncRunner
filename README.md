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
