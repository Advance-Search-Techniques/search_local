{
  "targets": [
    {
      "target_name": "search",
      "sources": [ "src/index.cc","src/interface.cc","src/register.c",
      "src/sqlite3.c","src/tokenizer.c","src/tool.c"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "defines": [
        "WIN32_LEAN_AND_MEAN"
      ]
    }
  ]
}