{
  "targets": [
    {
      "target_name": "exec_addon",
      "sources": [
        "src/run.cc",
        "src/platform/popen_plus/popen_plus.cc"
      ],
      "conditions": [
        ['OS=="win"', {
          "defines": [ "_WIN32" ]
        }]
      ]
    }
  ]
}
