# libKitsuneJinja2

## Description

This is a small jinja2-converter to merge a jinja2-template together with a json-like data-structure into a filled text file. It doesn't support the full power of jinja2 at the moment, but provides the following three basic-operations:

- replacements
- if-conditions
- for-each loops

## Common Information aboud my projects

Here some common information about my projects and my code-styling. It's not complete and a bit short. I will write a styling-guide for my projects, where I will write this a bit longer with more explanation, why I make it like this.

### About my kitsune libraries

1. All my libraries beginning with `libKitsune`, because I needed a naming to identify my own libraries and I decided to use `Kitsune` as name, because Kitsunemimi are moe. ;)

2. The namespace of the code within the libraries is identically to the name of the library. So for example all content of `libKitsuneCommon` has the namespace `Kitsune::Common`.

3. If you only want to use the library, beside the binary you only ne the public methods and variables in the header-files, which are located in the `include`-directory of each `libKitsune`-repo. I try my best to make these as small and self-explaining, as possible. 

### About my repositories in general

1. I programming only on Linux. Sorry, but I have no time for supporting Windows and Mac.

2. All my projects are mirrored to gitlab, where I have still my gitlab-ci runner.

3. In each project-repository is an `build.sh`-script to make it easier for you to build the project by yourself. It download and link all required git-repositories in the correct version and link it all. This script is also used by the gitlab-ci-runner and so it is everytime up-to-date.

4. All my projects have unit-tests, which are executed by the ci-runner in each commit. Most of the functions are tested. Primary some negative-tests are missing. I try to avoid mocks in the tests as much as possible. Even tcp-socket-tests using the localhost for real data-transfer-tests.

5. For the naming of variables, methods and classes, I use camel case and for the names of files and directories, I use snake case.

6. About my comments:

	- Many comments in my code seems to be a bit useless, but I like to use them for optical separation. I also write them, when I think I finished the programming task and while writing the comments I recheck the code under the comment again and often I find improvements or bugs while this.

	- At first I planned to use doxygen comment-style for methods and files, but I think I will change this, because while writing the current state of the code, I don't like it to write big comments for each simple self-explaining method.

	- I don't like it to write much comments into header-files. More exactly I absolutly hate it, when I have to search the provided functions of a header between a massive amount of comments. Header-files are for me primary to have an overview of all provided functions, which I want to see as fast as possible. Comments of functions, parameters and return-values in my code are only written into the source-files. So when something is unclear for a specific method, then look into the source-file. If the comment there are not helpful for you, then please write me a mail or an issue to fix this. 

7. I try to avoid templates and macros as much as possible. I don't dislike them, but I only use them, when I don't have another solution. 


## Build

### Requirements

paket | version
--- | ---
g++ | 6.3.0
qt5-qmake | 5.7.1
bison | 3.0.4
flex | 2.6.1 

This are the version I have installed under Debian Stable via apt. Some older or newer version should work as well. I write my projects with the Qt-creator, but without Qt itself. Thats why qmake is required to build my projects.

IMPORTANT: All my projects are only tested on Linux. 

#### Kitsune-repositories

Repository-Name | Version-Tag | Download-Path
--- | --- | ---
libKitsuneCommon | 0.5.3 |  https://github.com/tobiasanker/libKitsuneCommon.git
libKitsuneJson | 0.5.0 |  https://github.com/tobiasanker/libKitsuneJson.git

### build library

In all of my repositories you will find a `build.sh`. You only have to run this script. It doesn't required sudo, because you have to install required tool via apt, for example, by yourself. But if other projects from me are required, it download them from github and build them in the correct version too. This script is also use by the ci-pipeline, so its tested with every commit.

Before running the build-script:

```bash
.
└── libKitsuneJinja2
    ├── build.sh
    └── ...
```

After running the build-script:

```bash
.
├── build
│   ├── libKitsuneCommon
│   │   └── ...
│   ├── libKitsuneJson
│   │   └── ...
│   └── libKitsuneJinja2
│       └── ...
│
├── libKitsuneCommon
│   └── ...
├── libKitsuneJson
│   └── ...
├── libKitsuneJinja2
│   ├── build.sh
│   └── ...
│
└── result
    ├── include
    │   ├── libKitsuneCommon
    │   │   └── ...
    │   ├── libKitsunejson
    │   │   └── ...
    │   └── libKitsuneJinja2
    │       └── ...
    │
    ├── libKitsuneCommon.so -> libKitsuneCommon.so.0.5.3
    ├── libKitsuneCommon.so.0 -> libKitsuneCommon.so.0.5.3
    ├── libKitsuneCommon.so.0.5 -> libKitsuneCommon.so.0.5.3
    ├── libKitsuneCommon.so.0.5.3
    │
    ├── libKitsuneJson.so -> libKitsuneJson.so.0.5.0
    ├── libKitsuneJson.so.0 -> libKitsuneJson.so.0.5.0
    ├── libKitsuneJson.so.0.5 -> libKitsuneJson.so.0.5.0
    ├── libKitsuneJson.so.0.5.0
    │
    ├── libKitsuneJinja2.so -> libKitsuneJinja2.so.0.2.0
    ├── libKitsuneJinja2.so.0 -> libKitsuneJinja2.so.0.2.0
    ├── libKitsuneJinja2.so.0.2 -> libKitsuneJinja2.so.0.2.0
    └── libKitsuneJinja2.so.0.2.0
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
#include <jinja2_converter.h>

std::string testString("this is a {{ item.sub_item }}");

Jinja2Converter converter;
std::pair<std::string, bool> result;

result = converter.convert(testString, m_testJson->toMap());
// result.first = "this is a test-string"

```

### if-conditions

Generic form: `{% if <JSON_PATH> is <COMPARE_VALUE> %} ... {% else %} ... {% endif %}`

```cpp
#include <jinja2_converter.h>

std::string testString("this is "
	                   "{% if item2.sub_item2 is 42 %}"
	                   " a "
	                   "{% else %}"
	                   " no "
	                   "{% endif %} test-string");

Jinja2Converter converter;
std::pair<std::string, bool> result;

result = converter.convert(testString, m_testJson->toMap());
// result.first = "this is a test-string"
```

### for-each loop

Generic form: `{% for <TEMP_VAR> in <JSON_PATH> %} ... {{ <TEMP_VAR>.<JSON_PATH> }} ... {% endfor %}`

```cpp
#include <jinja2_converter.h>

std::string testString("this is"
                       "{% for value in loop %}"
                       " a "
                       "{{ value.x }}"
                       "{% endfor %}");
    
Jinja2Converter converter;
std::pair<std::string, bool> result;

result = converter.convert(testString, m_testJson->toMap());
// result.first = "this is a test1 a test2 a test3"

```

## Contributing

Please give me as many inputs as possible: Bugs, bad code style, bad documentation and so on.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
