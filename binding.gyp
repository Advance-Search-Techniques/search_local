{
  "targets": [
    {
      "target_name": "search",
      "sources": [ "src/index.cc","src/search.cc","src/" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}