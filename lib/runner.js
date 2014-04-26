!(function() {
    var _runner = require("../build/Release/exec_addon");

    /**
     * To run a executable file.
     *
     * @param command
     * @param [cwd] current work directory
     * @param [max_million_second] max wait ms
     * @return string
     */
    exports.run = function(command, cwd, max_million_second) {
        var res = _runner.exec(command, cwd, max_million_second);
        return res;
    };
})();
