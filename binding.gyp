{
  "targets": [
    {
      "target_name": "MebParser",
      'include_dirs': [
        'addons/common_headers',
      ],
      "sources": [
        'addons/common_headers/FlowCreator.cpp',
        'addons/parser/meb/BSTNode.cpp',
        'addons/parser/meb/RepetitionNinja.cpp',
        'addons/parser/meb/ParserModule.cpp'
      ]
    }
  ]
}