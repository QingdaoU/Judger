{
    "targets": [
    {
        "target_name": "judgerlib",
        "type": "static_library",
        "sources": [
            "../../src/child.c",
            "../../src/killer.c",
            "../../src/logger.c",
            "../../src/runner.c",
            "../../src/rules/c_cpp.c",
            "../../src/rules/c_cpp_file_io.c",
            "../../src/rules/general.c"
        ],
        'include_dirs':[
            "../../src",
            "../../src/rules"
        ]
    },
    {
        "target_name": "judger",
        "sources": [ "judger.cc"],
        'include_dirs':[
            "../../src",
            "../../src/rules"
        ],
        "libraries":[
            "-L/usr/lib","-lpthread","-lseccomp"
        ],

        "dependencies": [
            "judgerlib"
        ]
    }
    ]
}
