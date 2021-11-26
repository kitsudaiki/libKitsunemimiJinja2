# libKitsunemimiJinja2

![Gitlab pipeline status](https://img.shields.io/gitlab/pipeline/kitsudaiki/libKitsunemimiJinja2?label=build%20and%20test&style=flat-square)
![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/kitsudaiki/libKitsunemimiJinja2?label=version&style=flat-square)
![GitHub](https://img.shields.io/github/license/kitsudaiki/libKitsunemimiJinja2?style=flat-square)
![C++Version](https://img.shields.io/badge/c%2B%2B-17-blue?style=flat-square)
![Platform](https://img.shields.io/badge/platform-Linux--x64-lightgrey?style=flat-square)

## Description

This is a small jinja2-converter to merge a jinja2-template together with a json-like data-structure into a filled text file. It doesn't support the full power of jinja2 at the moment, but provides the following three basic-operations:

- replacements
- if-conditions
- for-each loops

## Build

### Requirements

name | repository | version | task
--- | --- | --- | ---
g++ | g++ | >= 8.0 | Compiler for the C++ code.
make | make | >= 4.0 | process the make-file, which is created by qmake to build the programm with g++
qmake | qt5-qmake | >= 5.0 | This package provides the tool qmake, which is similar to cmake and create the make-file for compilation.
FLEX | flex | >= 2.6 | Build the lexer-code for all used parser.
GNU Bison | bison | 3.x | Build the parser-code together with the lexer-code.

Installation on Ubuntu/Debian:

```bash
sudo apt-get install g++ make qt5-qmake flex bison
```

IMPORTANT: All my projects are only tested on Linux. 

### Kitsunemimi-repositories

Repository-Name | Version-Tag | Download-Path
--- | --- | ---
libKitsunemimiCommon | v0.24.0 |  https://github.com/kitsudaiki/libKitsunemimiCommon.git
libKitsunemimiJson | v0.11.2 |  https://github.com/kitsudaiki/libKitsunemimiJson.git

HINT: These Kitsunemimi-Libraries will be downloaded and build automatically with the build-script below.

### build library

In all of my repositories you will find a `build.sh`. You only have to run this script. It doesn't required sudo, because you have to install required tool via apt, for example, by yourself. But if other projects from me are required, it download them from github and build them in the correct version too. This script is also use by the ci-pipeline, so its tested with every commit.


Run the following commands:

```
git clone https://github.com/kitsudaiki/libKitsunemimiJinja2.git
cd libKitsunemimiJinja2
./build.sh
cd ../result
```

It create automatic a `build` and `result` directory in the directory, where you have cloned the project. At first it build all into the `build`-directory and after all build-steps are finished, it copy the include directory from the cloned repository and the build library into the `result`-directory. So you have all in one single place.

Tested on Debian and Ubuntu. If you use Centos, Arch, etc and the build-script fails on your machine, then please write me a mail and I will try to fix the script.

## Usage

For the examples for the functionallity of the library the following json will used as example:

```
{
	"item":
    { 
        "sub_item": "test-string"
    },
    "item2":
    { 
        "sub_item2": 42
        },
    "loop":
    [ 
        {
            "x": "test1" 
        }, 
        {
            "x": "test2" 
        }, 
        {
            "x": "test3" 
        }
    ]
}
```

### replacements

Generic form: `{{ <JSON_PATH> }}`

```cpp
#include <libKitsunemimiJinja2/jinja2_converter.h>

std::string testString("this is a {{ item.sub_item }}");
#include <libKitsunemimiCommon/logger.h>

Jinja2Converter converter;
std::pair<bool, std::string> result;
ErrorContainer error;

result = converter.convert(testString, m_testJson->toMap(), error);
// result.second = "this is a test-string"

```

### if-conditions

Generic form: `{% if <JSON_PATH> is <COMPARE_VALUE> %} ... {% else %} ... {% endif %}`

```cpp
#include <libKitsunemimiJinja2/jinja2_converter.h>
#include <libKitsunemimiCommon/logger.h>

std::string testString("this is "
	                   "{% if item2.sub_item2 is 42 %}"
	                   " a "
	                   "{% else %}"
	                   " no "
	                   "{% endif %} test-string");

Jinja2Converter converter;
std::pair<bool, std::string> result;
ErrorContainer error;

result = converter.convert(testString, m_testJson->toMap(), error);
// result.second = "this is a test-string"
```

### for-each loop

Generic form: `{% for <TEMP_VAR> in <JSON_PATH> %} ... {{ <TEMP_VAR>.<JSON_PATH> }} ... {% endfor %}`

```cpp
#include <libKitsunemimiJinja2/jinja2_converter.h>
#include <libKitsunemimiCommon/logger.h>

std::string testString("this is"
                       "{% for value in loop %}"
                       " a "
                       "{{ value.x }}"
                       "{% endfor %}");
    
Jinja2Converter converter;
std::pair<bool, std::string> result;
ErrorContainer error;

result = converter.convert(testString, m_testJson->toMap(), error);
// result.second = "this is a test1 a test2 a test3"

```

## Contributing

Please give me as many inputs as possible: Bugs, bad code style, bad documentation and so on.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
