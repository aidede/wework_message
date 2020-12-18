{
    # 'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")"],
    "targets": [
        {
            "target_name": "wework",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": ["src/wework.cc"],
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'CLANG_CXX_LIBRARY': 'libc++',
                'MACOSX_DEPLOYMENT_TARGET': '10.7',
            },
            'msvs_settings': {
                'VCCLCompilerTool': {'ExceptionHandling': 1},
            },
            "include_dirs": [
                "<!(node -p \"require('node-addon-api').include_dir\")",
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'conditions':[
                ['OS=="mac"', {
                    'cflags+': ['-fvisibility=hidden'],
                    'xcode_settings': {
                        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',  # -fvisibility=hidden
                    }
                }]
            ]
        }
    ]
}
